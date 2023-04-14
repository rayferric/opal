from manim import *
import math

ARRAY = [-1, 3, -7, 9, 1, 10, 1]
SORTED_ARRAY_Y = 3


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
    
    def anim_swap_with(self, other):
        anims = self.anim_move_to(other.get_center())
        anims += other.anim_move_to(self.get_center())
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
    def __init__(self, array):
        self.numbers = [Number(value=number) for number in array]

        for i, number in enumerate(self.numbers):
            number.shift(i * RIGHT * 1.5)

        offset = -self.get_center()

        for number in self.numbers:
            number.shift(offset)

        self.last_index = len(self.numbers) - 1
        self.sorted_array_build_x = self.numbers[-1].get_center()[0]

    def anim_create(self):
        return sum([number.anim_create() for number in self.numbers], [])

    def get_center(self):
        min_x = min([number.get_center()[0] for number in self.numbers])
        max_x = max([number.get_center()[0] for number in self.numbers])

        min_y = min([number.get_center()[1] for number in self.numbers])
        max_y = max([number.get_center()[1] for number in self.numbers])

        return (min_x + max_x) / 2 * RIGHT + (min_y + max_y) / 2 * UP

    def anim_morph_to_heap(self):
        height = math.floor(math.log(len(self.numbers), 2)) + 1
        visual_height = height * 1.5 - 1.5

        # distance between two nodes at the same level
        spacing = 2 ** height - 1
        
        anims = []

        for level in range(height):
            node_count = 2 ** level
            y = level * 1.5
            y -= visual_height / 2
            y *= DOWN
            row_width = (node_count - 1) * spacing + node_count - 1

            for i in range(node_count):
                index = (node_count - 1) + i
                x = i * (spacing + 1)
                x -= row_width / 2
                x *= RIGHT

                anims += self.numbers[index].anim_move_to(x + y)

            spacing = (spacing + 1) // 2 - 1

        return anims
    
    def anim_swap(self, i, j):
        self.numbers[i], self.numbers[j] = self.numbers[j], self.numbers[i]
        return self.numbers[i].anim_swap_with(self.numbers[j])
    
    # Move last element outside of the heap.
    # Place it down right below the heap to progressively build a sorted array.
    def anim_remove_last(self):
        last = self.numbers[self.last_index]
        self.last_index -= 1
        anims = last.anim_move_to(self.sorted_array_build_x * RIGHT + SORTED_ARRAY_Y * DOWN)
        self.sorted_array_build_x -= 1.5
        return anims
    
    def anim_center_result(self):
        return sum([number.anim_shift(self.get_center()[1] * DOWN) for number in self.numbers], [])
    
    def anim_focus_node_with_children(self, index):
        anims = self.numbers[index].anim_set_color(YELLOW)

        if index * 2 + 1 <= self.last_index:
            anims += self.numbers[index * 2 + 1].anim_set_color(RED)
        
        if index * 2 + 2 <= self.last_index:
            anims += self.numbers[index * 2 + 2].anim_set_color(RED)

        anims += sum([number.anim_set_color(BLUE) for i, number in enumerate(self.numbers) if i != index and i != index * 2 + 1 and i != index * 2 + 2], [])

        return anims
    
    def anim_focus_largest(self, index):
        return self.numbers[index].anim_set_color(GREEN)
    
    def anim_remove_focus(self):
        return sum([number.anim_set_color(BLUE) for number in self.numbers], [])

class HeapSort(MovingCameraScene):
    def construct(self):
        title = Text("Heap Sort", font_size=50)
        self.array = Array(array=ARRAY)

        self.play(Write(title))
        self.wait(1)
        self.play(FadeOut(title))
        self.wait(1)

        self.play(*self.array.anim_create())
        self.play(*self.array.anim_morph_to_heap(), run_time=2)

        self.heap_sort(ARRAY.copy())

        self.play(*self.array.anim_center_result())

        self.wait(1)

    def heapify(self, array, size, root):
        if root > size // 2 - 1:
            return

        self.play(*self.array.anim_focus_node_with_children(root))

        left = root * 2 + 1
        right = root * 2 + 2

        # Find the largest element of all three.
        largest = root
        if left < size and array[left] > array[largest]:
            largest = left
        if right < size and array[right] > array[largest]:
            largest = right

        self.play(*self.array.anim_focus_largest(largest))
        
        # If the root is not the largest, swap it with the largest and fix up the
        # new heap.
        if largest != root:
            array[root], array[largest] = array[largest], array[root]
            # root now points to the larger value and largest points to the smaller
            # value (the child).

            self.play(*self.array.anim_swap(root, largest))

            # Heapify the sub-heap starting from the child.
            self.heapify(array, size, largest)

    def heap_sort(self, array):
        size = len(array)

        # Build the max-heap.
        # array[(size / 2) - 1] is the last non-leaf node.
        for root in range(size // 2 - 1, -1, -1):
            self.heapify(array, size, root)
        
        while size > 1:
            self.play(*self.array.anim_remove_focus())

            # Reduce the size of the heap by one.
            size -= 1

            # Move the largest element (root) outside of the heap.
            array[0], array[size] = array[size], array[0]

            self.play(*self.array.anim_swap(0, size))
            self.play(*self.array.anim_remove_last())

            # Fix up the heap.
            self.heapify(array, size, 0)

        self.play(*self.array.anim_remove_last())