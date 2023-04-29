from manim import *


class Number:
    def __init__(self, value=0, other=None):
        if other == None:
            self.circle = Circle(radius=0.5, color=BLUE)
            self.text = Text(str(value), font_size=30, color=WHITE)
            self.value = value
        else:
            self.circle = other.circle.copy()
            self.text = other.text.copy()
            self.value = other.value

    def anim_create(self):
        return [Create(self.circle), Write(self.text)]

    def anim_fade_in(self):
        return [FadeIn(self.circle), FadeIn(self.text)]

    def anim_fade_out(self):
        return [FadeOut(self.circle), FadeOut(self.text)]

    def anim_set_color(self, color):
        return [self.circle.animate.set_color(color)]

    def anim_move_to(self, position):
        return [
            self.circle.animate.move_to(position),
            self.text.animate.move_to(position),
        ]

    def anim_shift(self, direction):
        return [
            self.circle.animate.shift(direction),
            self.text.animate.shift(direction),
        ]

    def anim_set_value(self, value):
        self.value = value

        new_text = Text(str(value), font_size=30, color=WHITE)
        new_text.move_to(self.text)

        old_text = self.text
        self.text = new_text

        return [
            ReplacementTransform(
                old_text,
                new_text,
            )
        ]

    def anim_set_opacity(self, opacity):
        circle_opacity = self.circle.get_stroke_opacity()
        return [
            UpdateFromAlphaFunc(
                self.circle,
                lambda mob, alpha: mob.set_stroke(
                    opacity=lerp(circle_opacity, opacity, alpha)
                ),
            ),
        ] + self.anim_set_value_opacity(opacity)

    def anim_set_value_opacity(self, opacity):
        text_opacity = 1
        return [
            UpdateFromAlphaFunc(
                self.text,
                lambda mob, alpha: mob.set_opacity(
                    lerp(text_opacity, opacity, alpha)
                ),
            ),
        ]

    def anim_morph_to(self, other):
        anims = other.anim_set_value(self.get_value())
        anims += self.anim_move_to(other.get_center())
        anims += self.anim_set_opacity(0)
        return anims

    def shift(self, direction):
        self.circle.shift(direction)
        self.text.shift(direction)

    def move_to(self, position):
        self.circle.move_to(position)
        self.text.move_to(position)

    def copy(self):
        return Number(other=self)

    def get_center(self):
        return self.circle.get_center()

    def get_value(self):
        return self.value

    def set_color(self, color):
        self.circle.set_color(color)

    def anim_swap_with(self, other):
        anims = self.anim_move_to(other.get_center())
        anims += other.anim_move_to(self.get_center())
        return anims

    def anim_swap_value_with(self, other):
        anims = [self.text.animate.move_to(other.get_center())]
        anims += [other.text.animate.move_to(self.get_center())]
        self.text, other.text = other.text, self.text
        self.value, other.value = other.value, self.value
        return anims

    def set_value_opacity(self, opacity):
        self.text.set_opacity(opacity)


class Node:
    def __init__(self, value):
        self.parent = None
        self.left = None
        self.right = None
        self.value = value

        self.number = Number(value)
        self.parent_connection = None

    def anim_create(self):
        anims = self.number.anim_create()

        if self.parent_connection != None:
            anims += [Create(self.parent_connection)]

        if self.left != None:
            anims += self.left.anim_create()

        if self.right != None:
            anims += self.right.anim_create()

        return anims

    def set_left(self, child):
        self.left = child
        child.parent = self

        # Move child below parent.
        child.number.move_to(self.number.get_center())
        child.number.shift(1.5 * DOWN + 1.5 * LEFT)

        # Create connection.
        # Number radius is 0.5.
        child_to_parent = self.number.get_center() - child.number.get_center()
        center_point = child.number.get_center() + child_to_parent / 2
        child_to_parent *= (
            np.linalg.norm(child_to_parent) - 0.5 - 0.5
        ) / np.linalg.norm(child_to_parent)
        self.left.parent_connection = Line(
            center_point - child_to_parent / 2,
            center_point + child_to_parent / 2,
            color=WHITE,
        )

    def set_right(self, child):
        self.right = child
        child.parent = self

        # Move child below parent.
        child.number.move_to(self.number.get_center())
        child.number.shift(1.5 * DOWN + 1.5 * RIGHT)

        # Create connection.
        # Number radius is 0.5.
        child_to_parent = self.number.get_center() - child.number.get_center()
        center_point = child.number.get_center() + child_to_parent / 2
        child_to_parent *= (
            np.linalg.norm(child_to_parent) - 0.5 - 0.5
        ) / np.linalg.norm(child_to_parent)
        self.right.parent_connection = Line(
            center_point - child_to_parent / 2,
            center_point + child_to_parent / 2,
            color=WHITE,
        )

    def anim_center_camera(self, scene):
        # Move the tree to center.

        y_min = float("inf")
        y_max = float("-inf")

        def find_height(node):
            nonlocal y_min, y_max

            if node == None:
                return

            y_min = min(y_min, node.number.get_center()[1])
            y_max = max(y_max, node.number.get_center()[1])

            find_height(node.left)
            find_height(node.right)

        find_height(self)
        y_center = (y_min + y_max) / 2
        shift = (y_center - scene.camera.frame.get_center()[1]) * UP
        # shift = (y_center - 0) * DOWN

        # def apply_shift(node):
        #     if node == None:
        #         return []

        #     anims = node.number.anim_shift(shift)

        #     if node.parent_connection != None:
        #         anims += [node.parent_connection.animate.shift(shift)]

        #     anims += apply_shift(node.left)
        #     anims += apply_shift(node.right)

        #     return anims

        # return apply_shift(self)

        return ([scene.camera.frame.animate.shift(shift)], shift)

    def anim_highlight_left(self):
        anims = self.number.anim_set_color(BLUE)

        if self.left != None:
            anims += self.left.number.anim_set_color(GREEN)

        return anims

    def anim_highlight_right(self):
        anims = self.number.anim_set_color(BLUE)

        if self.right != None:
            anims += self.right.number.anim_set_color(GREEN)

        return anims

    def anim_delete(self):
        anims = self.number.anim_fade_out()

        # Fade out children connections.
        if self.left != None:
            anims += [FadeOut(self.left.parent_connection)]
            self.left.parent = None
        if self.right != None:
            anims += [FadeOut(self.right.parent_connection)]
            self.right.parent = None

        # Fade out parent connection.
        if self.parent_connection != None:
            anims += [FadeOut(self.parent_connection)]

        if self.parent.left == self:
            self.parent.left = None
        elif self.parent.right == self:
            self.parent.right = None

        return anims

    def anim_set_left(self, child):
        self.left = child
        child.parent = self

        # Move child below parent.
        new_child_pos = self.number.get_center() + 1.5 * DOWN + 1.5 * LEFT
        anims = child.number.anim_move_to(new_child_pos)

        # Create connection.
        # Number radius is 0.5.
        child_to_parent = self.number.get_center() - new_child_pos
        center_point = new_child_pos + child_to_parent / 2
        child_to_parent *= (
            np.linalg.norm(child_to_parent) - 0.5 - 0.5
        ) / np.linalg.norm(child_to_parent)
        self.left.parent_connection = Line(
            center_point - child_to_parent / 2,
            center_point + child_to_parent / 2,
            color=WHITE,
        )

        anims += [Create(self.left.parent_connection)]
        return anims

    def anim_set_right(self, child):
        self.right = child
        child.parent = self

        # Move child below parent.
        new_child_pos = self.number.get_center() + 1.5 * DOWN + 1.5 * RIGHT
        anims = child.number.anim_move_to(new_child_pos)

        # Create connection.
        # Number radius is 0.5.
        child_to_parent = self.number.get_center() - new_child_pos
        center_point = new_child_pos + child_to_parent / 2
        child_to_parent *= (
            np.linalg.norm(child_to_parent) - 0.5 - 0.5
        ) / np.linalg.norm(child_to_parent)
        self.right.parent_connection = Line(
            center_point - child_to_parent / 2,
            center_point + child_to_parent / 2,
            color=WHITE,
        )

        anims += [Create(self.right.parent_connection)]
        return anims

    def next_in_order(self):
        # Try to go to the right.
        if self.right != None:
            node = self.right

            # Go to the left as far as possible.
            while node.left != None:
                node = node.left

            return node

        # Go up until we can go to the right.
        node = self
        while node.parent != None:
            if node.parent.left == node:
                node = node.parent
                return node

            node = node.parent

        return None

    def find_traversal_points(self):
        points = []

        points.append((self.number.get_center() + LEFT, -1))

        if self.left != None:
            points += self.left.find_traversal_points()

        points.append((self.number.get_center() + DOWN, 0))

        if self.right != None:
            points += self.right.find_traversal_points()

        points.append((self.number.get_center() + RIGHT, 1))

        return points


class BST(MovingCameraScene):
    def construct(self):
        self.root = None

        title = Text("Binary Search Tree", font_size=50)

        self.play(Write(title))
        self.wait(1)
        self.play(FadeOut(title))
        self.wait(1)

        self.anim_insert(5)
        self.anim_insert(7)
        self.anim_insert(1)
        self.anim_insert(9)
        self.anim_insert(3)
        self.anim_insert(0)

        self.anim_find(9)
        node = self.anim_find(5)
        self.anim_erase(node)
        node = self.anim_find(3)
        self.anim_erase(node)

        self.anim_insert(8)
        self.anim_insert(10)

        self.anim_pre_order()
        self.anim_in_order()
        self.anim_post_order()

        self.wait(1)

    def anim_fade_in(self):
        return []

    def anim_fade_out(self):
        return []

    def anim_insert(self, x: int):
        subtitle = Text("Inserting " + str(x), font_size=30)
        subtitle.move_to(self.camera.frame.get_center()[1] * UP + 3 * UP)

        self.play(Write(subtitle))

        if self.root == None:
            self.root = Node(x)
            self.play(*self.root.anim_create())
            self.play(FadeOut(subtitle))
            return

        node = self.root
        self.play(*node.number.anim_set_color(GREEN))

        while True:
            if x < node.value:
                highlight_anim = node.anim_highlight_left()

                if node.left == None:
                    node.set_left(Node(x))
                    center_anim = self.root.anim_center_camera(self)
                    anims = [
                        *highlight_anim,
                        *node.left.anim_create(),
                        *center_anim[0],
                        subtitle.animate.shift(center_anim[1]),
                    ]
                    self.play(*anims)
                else:
                    self.play(*highlight_anim)

                node = node.left
            elif node.value < x:
                highlight_anim = node.anim_highlight_right()

                if node.right == None:
                    node.set_right(Node(x))
                    center_anim = self.root.anim_center_camera(self)
                    anims = [
                        *highlight_anim,
                        *node.right.anim_create(),
                        *center_anim[0],
                        subtitle.animate.shift(center_anim[1]),
                    ]
                    self.play(*anims)
                else:
                    self.play(*highlight_anim)

                node = node.right
            else:
                break

        self.play(FadeOut(subtitle))

    def anim_find(self, x: int):
        subtitle = Text("Finding " + str(x), font_size=30)
        subtitle.move_to(self.camera.frame.get_center()[1] * UP + 3 * UP)

        self.play(Write(subtitle))

        node = self.root
        self.play(*node.number.anim_set_color(GREEN))

        while True:
            if x < node.value:
                highlight_anim = node.anim_highlight_left()
                self.play(*highlight_anim)

                if node.left == None:
                    break

                node = node.left
            elif node.value < x:
                highlight_anim = node.anim_highlight_right()
                self.play(*highlight_anim)

                if node.right == None:
                    break

                node = node.right
            else:
                break

        self.play(*node.number.anim_set_color(RED))
        self.play(FadeOut(subtitle), *node.number.anim_set_color(BLUE))

        return node

    def anim_erase(self, node: Node, display_title: bool = True):
        if display_title:
            subtitle = Text("Erasing " + str(node.value), font_size=30)
            subtitle.move_to(self.camera.frame.get_center()[1] * UP + 3 * UP)
            self.play(Write(subtitle))

        # If the node has no children, just delete it.
        if node.left == None and node.right == None:
            self.play(*node.anim_delete())

        # Else if the node has only one child, replace it with that child.
        elif node.left == None:
            # If there's no left child, replace the node with the right child.
            self.play(
                *node.anim_delete(), *node.parent.anim_set_right(node.right)
            )
        elif node.right == None:
            # If there's no right child, replace the node with the left child.
            node.left.parent = node.parent
            self.play(
                *node.anim_delete(), *node.parent.anim_set_left(node.left)
            )

        # Finally, if there's two children, replace this node with the next
        # in-order node.
        else:
            # Find the next in-order node.
            next_in_order = node.next_in_order()

            # Move the next in-order node's value to this node.
            # Delete the next in-order node.
            self.play(*node.number.anim_swap_value_with(next_in_order.number))
            self.anim_erase(next_in_order, display_title=False)

        if display_title:
            self.play(FadeOut(subtitle))

    def anim_pre_order(self):
        subtitle = Text("Pre-Order Traversal", font_size=30)
        subtitle.move_to(self.camera.frame.get_center()[1] * UP + 3 * UP)
        self.play(Write(subtitle))

        traversal_points = self.root.find_traversal_points()
        curve = VMobject()
        curve.set_points_smoothly([point[0] for point in traversal_points])
        curve = DashedVMobject(curve, num_dashes=50)

        run_time = 5
        anims = []
        circles = []
        for i, point in enumerate(traversal_points):
            if point[1] == -1:
                pre_order_point = point[0] + 0.5 * RIGHT
                circle = Circle(radius=0.1, color=RED)
                circle.set_fill(RED, opacity=0.5)
                circle.move_to(pre_order_point)
                delay = run_time * (i / len(traversal_points))
                delay *= 0.003
                anims.append(
                    Succession(Wait(delay), Create(circle, run_time=0.005))
                )
                circles.append(circle)

        self.play(Create(curve), AnimationGroup(*anims), run_time=run_time)

        self.play(FadeOut(subtitle), FadeOut(curve), FadeOut(Group(*circles)))

    def anim_in_order(self):
        subtitle = Text("In-Order Traversal", font_size=30)
        subtitle.move_to(self.camera.frame.get_center()[1] * UP + 3 * UP)
        self.play(Write(subtitle))

        traversal_points = self.root.find_traversal_points()
        curve = VMobject()
        curve.set_points_smoothly([point[0] for point in traversal_points])
        curve = DashedVMobject(curve, num_dashes=50)

        run_time = 5
        anims = []
        circles = []
        for i, point in enumerate(traversal_points):
            if point[1] == 0:
                in_order_point = point[0] + 0.5 * UP
                circle = Circle(radius=0.1, color=YELLOW)
                circle.set_fill(YELLOW, opacity=0.5)
                circle.move_to(in_order_point)
                delay = run_time * (i / len(traversal_points))
                delay *= 0.003
                anims.append(
                    Succession(Wait(delay), Create(circle, run_time=0.005))
                )
                circles.append(circle)

        self.play(Create(curve), AnimationGroup(*anims), run_time=run_time)

        self.play(FadeOut(subtitle), FadeOut(curve), FadeOut(Group(*circles)))

    def anim_post_order(self):
        subtitle = Text("Post-Order Traversal", font_size=30)
        subtitle.move_to(self.camera.frame.get_center()[1] * UP + 3 * UP)
        self.play(Write(subtitle))

        traversal_points = self.root.find_traversal_points()
        curve = VMobject()
        curve.set_points_smoothly([point[0] for point in traversal_points])
        curve = DashedVMobject(curve, num_dashes=50)

        run_time = 5
        anims = []
        circles = []
        for i, point in enumerate(traversal_points):
            if point[1] == 1:
                post_order_point = point[0] + 0.5 * LEFT
                circle = Circle(radius=0.1, color=GREEN)
                circle.set_fill(GREEN, opacity=0.5)
                circle.move_to(post_order_point)
                delay = run_time * (i / len(traversal_points))
                delay *= 0.01
                anims.append(
                    Succession(Wait(delay), Create(circle, run_time=0.005))
                )
                circles.append(circle)

        self.play(Create(curve), AnimationGroup(*anims), run_time=run_time)

        self.play(FadeOut(subtitle), FadeOut(curve), FadeOut(Group(*circles)))
