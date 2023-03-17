from manim import *

ARRAY = [-1, 3, -7, 9, 1, 10, 1]
SHIFT_DOWN = 1.5 * DOWN


def lerp(a, b, alpha):
    return a + alpha * (b - a)


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

    def anim_set_color(self, color):
        return [self.circle.animate.set_color(color)]

    def anim_move_to(self, position):
        return [
            self.circle.animate.move_to(position),
            self.text.animate.move_to(position),
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

    def copy(self):
        return Number(other=self)

    def get_center(self):
        return self.circle.get_center()

    def get_value(self):
        return self.value
    
    def set_color(self, color):
        self.circle.set_color(color)


class Array:
    def __init__(self, array=None, numbers=None):
        if numbers == None:
            self.numbers = [Number(value=number) for number in array]

            for i, number in enumerate(self.numbers):
                number.shift(i * RIGHT * 1.5)

            offset = -self.get_center()

            for number in self.numbers:
                number.shift(offset)
        else:
            self.numbers = numbers

        left_branch = None
        right_child = None

    def anim_create(self):
        return sum([number.anim_create() for number in self.numbers], [])
    
    def anim_fade_in(self):
        return sum([number.anim_fade_in() for number in self.numbers], [])

    def get_center(self):
        return sum([number.get_center() for number in self.numbers]) / len(
            self.numbers
        )
    
    def anim_fork_branch(self, scene, is_right):
        split_index = len(self.numbers) // 2

        numbers = [n.copy() for n in (self.numbers[split_index:] if is_right else self.numbers[:split_index])]
        for number in numbers:
            number.set_color(RED if is_right else GREEN)

        branch = Array(numbers=numbers)

        # Fade in branch
        anims = branch.anim_fade_in()

        # Move branch down
        anims += sum(
            [
                number.anim_move_to(number.get_center() + SHIFT_DOWN)
                for number in branch.numbers
            ],
            [],
        )

        if is_right:
            self.right_branch = branch
        else:
            self.left_branch = branch

        # Move camera to branch
        if not is_right:
            anims += [scene.camera.frame.animate.shift(SHIFT_DOWN * 0.5)]

        return anims
    
    def anim_clear_values(self):
        return sum([number.anim_set_value_opacity(0) for number in self.numbers], [])
    
    def anim_merge_branch(self, scene, i_from, i_to, is_right):
        anims = []

        if i_to == len(self.numbers) - 1:
            anims += [scene.camera.frame.animate.shift(SHIFT_DOWN * -0.5)]

        if is_right:
            anims += self.right_branch.numbers[i_from].anim_morph_to(self.numbers[i_to])
        else:
            anims += self.left_branch.numbers[i_from].anim_morph_to(self.numbers[i_to])

        return anims

def merge(scene, array, raw_array, l, m, u):
    scene.play(*array.anim_clear_values())
    
    buffer = []

    i = l
    j = m

    while i < m and j < u:
        if raw_array[i] <= raw_array[j]:
            scene.play(*array.anim_merge_branch(scene, i - l, len(buffer), False))
            buffer.append(raw_array[i])
            i += 1
        else:
            scene.play(*array.anim_merge_branch(scene, j - m, len(buffer), True))
            buffer.append(raw_array[j])
            j += 1
    
    while i < m:
        scene.play(*array.anim_merge_branch(scene, i - l, len(buffer), False))
        buffer.append(raw_array[i])
        i += 1

    while j < u:
        scene.play(*array.anim_merge_branch(scene, j - m, len(buffer), True))
        buffer.append(raw_array[j])
        j += 1

    for i in range(l, u):
        raw_array[i] = buffer[i - l]

def merge_sort(scene, array, raw_array, l, u):
    if u - l > 1:
        m = (l + u) // 2

        scene.play(*array.anim_fork_branch(scene, False))
        merge_sort(scene, array.left_branch, raw_array, l, m)

        scene.play(*array.anim_fork_branch(scene, True))
        merge_sort(scene, array.right_branch, raw_array, m, u)

        merge(scene, array, raw_array, l, m, u)

class InsertionSort(MovingCameraScene):
    def construct(self):
        title = Text("Merge Sort", font_size=50)

        array = Array(array=ARRAY)
        raw_array = ARRAY

        self.play(Write(title))
        self.wait(1)
        self.play(FadeOut(title))
        self.wait(1)

        self.play(*array.anim_create())

        merge_sort(self, array, raw_array, 0, len(raw_array))

        self.wait(1)
