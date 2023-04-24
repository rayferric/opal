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

    def anim_fade_out(self):
        return [FadeOut(self.circle), FadeOut(self.text)]

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

        self.compared_number = None
        self.comparison_sign = None
        self.left_iterator = None
        self.right_iterator = None
        self.unsorted_begin = 0
        self.separators = []

    def anim_create(self):
        return sum([number.anim_create() for number in self.numbers], [])

    def anim_fade_in(self):
        return sum([number.anim_fade_in() for number in self.numbers], [])

    def get_center(self):
        return sum([number.get_center() for number in self.numbers]) / len(
            self.numbers
        )

    def anim_highlight_pivot(self, pivot):
        # Copy the pivot element, color it and fade it in.
        self.pivot = self.numbers[pivot].copy()
        self.pivot.set_color(PINK)
        return self.pivot.anim_fade_in()

    def anim_focus_pivot(self):
        # Move the pivot element to the center of and below the array.
        # Offset it a little bit to the right to make room for comparison sign and a second number on the left.
        return self.pivot.anim_move_to(self.get_center() + SHIFT_DOWN + RIGHT)

    def anim_pick_for_comparison(self, index, is_left_iterator):
        # This method manages the two iterators in Hoare partitioning.
        # Highlight the number in green/red.
        # Simultaneously remove previous lef/right highlight.
        # Simultaneously spawn and fade in a duplicate of this number next to the pivot.
        # Simultaneously spawn the right comparison sign between the two.
        # The comparison sign must be green if the iterator will advance further.
        # Otherwise it shall be red.

        # Highlight the number in yellow.
        if index in [self.left_iterator, self.right_iterator]:
            anims = self.numbers[index].anim_set_color(YELLOW)
        else:
            anims = self.numbers[index].anim_set_color(
                GREEN if is_left_iterator else RED
            )

        # Simultaneously remove previous left/right highlight.
        if is_left_iterator:
            if index > self.unsorted_begin:
                if (index - 1) == self.right_iterator:
                    anims += self.numbers[index - 1].anim_set_color(RED)
                else:
                    anims += self.numbers[index - 1].anim_set_color(BLUE)
        elif index < len(self.numbers) - 1:
            if (index + 1) == self.left_iterator:
                anims += self.numbers[index + 1].anim_set_color(GREEN)
            else:
                anims += self.numbers[index + 1].anim_set_color(BLUE)

        # Simultaneously spawn and fade in a duplicate of this number next to the pivot.
        if self.compared_number:
            anims += self.compared_number.anim_set_value(
                self.numbers[index].get_value()
            )
            anims += self.compared_number.anim_set_color(
                GREEN if is_left_iterator else RED
            )
        else:
            self.compared_number = self.numbers[index].copy()
            self.compared_number.set_color(GREEN if is_left_iterator else RED)
            self.compared_number.move_to(self.pivot.get_center() + LEFT * 2)
            anims += self.compared_number.anim_fade_in()

        # Simultaneously spawn the right comparison sign between the two.
        # The comparison sign must be green if the iterator will advance further.
        # Otherwise it shall be red.
        old_comparison_sign = self.comparison_sign
        if is_left_iterator:
            if self.compared_number.get_value() >= self.pivot.get_value():
                self.comparison_sign = Tex(
                    "$\\boldsymbol{\\ge}$", font_size=30, color=RED
                )
            else:
                self.comparison_sign = Tex(
                    "$\\boldsymbol{<}$", font_size=30, color=GREEN
                )
        else:
            if self.compared_number.get_value() <= self.pivot.get_value():
                self.comparison_sign = Tex(
                    "$\\boldsymbol{\\le}$", font_size=30, color=RED
                )
            else:
                self.comparison_sign = Tex(
                    "$\\boldsymbol{>}$", font_size=30, color=GREEN
                )

        self.comparison_sign.move_to(self.pivot.get_center() + LEFT)

        if old_comparison_sign:
            anims += [
                ReplacementTransform(
                    old_comparison_sign,
                    self.comparison_sign,
                )
            ]
        else:
            anims += [FadeIn(self.comparison_sign)]

        if is_left_iterator:
            self.left_iterator = index
        else:
            self.right_iterator = index

        return anims

    def anim_swap(self, i, j):
        # self.numbers[i], self.numbers[j] = self.numbers[j], self.numbers[i]
        return self.numbers[i].anim_swap_value_with(self.numbers[j])

    def anim_finish_partitioning(self):
        # Morph right iterator into a vertical separation line to the right of it.
        # This line separates the array into two parts.
        # Simultaneously remove the comparison below the array and clear the colors of the numbers.

        # Make a copy of the right iterator circle.
        circle = self.numbers[self.right_iterator].circle.copy()

        # Morph right iterator into a vertical separation line to the right of it.
        self.separators.append(
            Line(
                self.numbers[self.right_iterator].get_center()
                + RIGHT * 0.75
                + DOWN * 0.5,
                self.numbers[self.right_iterator].get_center()
                + RIGHT * 0.75
                + UP * 0.5,
                color=RED,
            )
        )
        anims = [
            ReplacementTransform(
                circle,
                self.separators[-1],
            )
        ]

        # Clear the colors of the numbers.
        anims += sum(
            [
                self.numbers[i].anim_set_color(BLUE)
                for i in range(self.unsorted_begin, len(self.numbers))
            ],
            [],
        )

        # Remove comparison sign.
        anims += [FadeOut(self.comparison_sign)]

        # Remove compared number.
        anims += self.compared_number.anim_fade_out()

        # Remove pivot.
        anims += self.pivot.anim_fade_out()

        self.compared_number = None
        self.comparison_sign = None
        self.left_iterator = None
        self.right_iterator = None

        return anims

    def anim_mark_one_sorted(self):
        anims = self.numbers[self.unsorted_begin].anim_set_color(WHITE)
        self.unsorted_begin += 1
        return anims

    def anim_remove_separators(self):
        # Remove separators.
        anims = [FadeOut(separator) for separator in self.separators]

        # Color all numbers to blue.
        anims += sum(
            [number.anim_set_color(BLUE) for number in self.numbers],
            [],
        )

        return anims


class QuickSort(MovingCameraScene):
    def construct(self):
        title = Text("Quick Sort", font_size=50)
        subtitle = Text("(Hoare Partitioning)", font_size=20)
        subtitle.shift(DOWN)
        subtitle.set_color(GRAY)

        self.array = Array(array=ARRAY)
        self.raw_array = ARRAY

        self.play(Write(title))
        self.play(Write(subtitle))
        self.wait(1)
        self.play(FadeOut(title), FadeOut(subtitle))
        self.wait(1)

        self.play(*self.array.anim_create())

        self.quick_sort(0, len(self.raw_array))

        self.play(*self.array.anim_remove_separators())

        self.wait(1)

    def quick_sort(self, begin: int, end: int):
        size = end - begin

        if size <= 1:
            self.play(*self.array.anim_mark_one_sorted())
            return

        pivot = self.partition_hoare(begin=begin, end=end)

        self.quick_sort(begin=begin, end=pivot)
        self.quick_sort(begin=pivot, end=end)

    def partition_hoare(self, begin: int, end: int):
        pivot = self.raw_array[begin]
        self.play(*self.array.anim_highlight_pivot(begin))
        self.play(*self.array.anim_focus_pivot())

        left = begin - 1
        right = end

        while True:
            left += 1
            self.play(*self.array.anim_pick_for_comparison(left, True))
            while self.raw_array[left] < pivot:
                left += 1
                self.play(*self.array.anim_pick_for_comparison(left, True))

            right -= 1
            self.play(*self.array.anim_pick_for_comparison(right, False))
            while self.raw_array[right] > pivot:
                right -= 1
                self.play(*self.array.anim_pick_for_comparison(right, False))

            if right <= left:
                self.play(*self.array.anim_finish_partitioning())
                return right + 1

            self.raw_array[left], self.raw_array[right] = (
                self.raw_array[right],
                self.raw_array[left],
            )
            self.play(*self.array.anim_swap(left, right))
