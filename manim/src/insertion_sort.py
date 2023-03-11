from manim import *

ARRAY = [7, 11, -1, -2, 9, 11, 0, -1]

COMPARED_NUMBER_STAGE_POSITION = (-1, -2, 0)
FOCUSED_NUMBER_STAGE_POSITION = (1, -2, 0)
COMPARISON_SIGN_POSITION = (0, -2, 0)


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
        text_opacity = 1
        return [
            UpdateFromAlphaFunc(
                self.circle,
                lambda mob, alpha: mob.set_stroke(
                    opacity=lerp(circle_opacity, opacity, alpha)
                ),
            ),
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


class Array:
    def __init__(self, array):
        self.numbers = [Number(value=number) for number in array]
        for i, number in enumerate(self.numbers):
            number.shift(i * RIGHT * 1.5)
        offset = -self.get_center()
        for number in self.numbers:
            number.shift(offset)
        self.focused = None
        self.focused_index = None
        self.compared = None
        self.compared_index = None

    def anim_create(self):
        return sum([number.anim_create() for number in self.numbers], [])

    def anim_focus(self, index):
        anims = self.numbers[index].anim_set_color(GREEN)

        if self.focused_index != None:
            anims += self.numbers[self.focused_index].anim_set_color(BLUE)
        if self.compared_index != None:
            anims += self.numbers[self.compared_index].anim_set_color(BLUE)

        self.focused = self.numbers[index].copy()
        self.focused_index = index

        anims += self.focused.anim_fade_in()
        anims += self.focused.anim_set_color(GREEN)

        return anims

    def anim_stage_focused(self):
        return self.focused.anim_move_to(FOCUSED_NUMBER_STAGE_POSITION)

    def anim_compare(self, index):
        anims = self.numbers[index].anim_set_color(RED)

        if self.compared_index != None:
            anims += self.numbers[self.compared_index].anim_set_color(BLUE)

        self.compared = self.numbers[index].copy()
        self.compared_index = index

        anims += self.compared.anim_fade_in()
        anims += self.compared.anim_set_color(RED)

        return anims

    def anim_reset_colors(self):
        anims = []
        if self.focused_index != None:
            anims += self.numbers[self.focused_index].anim_set_color(BLUE)
        if self.compared_index != None:
            anims += self.numbers[self.compared_index].anim_set_color(BLUE)
        return anims

    def anim_stage_compared(self):
        return self.compared.anim_move_to(COMPARED_NUMBER_STAGE_POSITION)

    def anim_replace_with_compared(self, index):
        return self.compared.anim_morph_to(self.numbers[index])

    def anim_replace_with_focused(self, index):
        return self.focused.anim_morph_to(self.numbers[index])

    def get_center(self):
        return sum([number.get_center() for number in self.numbers]) / len(
            self.numbers
        )


class InsertionSort(Scene):
    def construct(self):
        title = Text("Insertion Sort", font_size=50)

        array = Array(ARRAY)
        raw_array = ARRAY

        lequal = Tex("$\\boldsymbol{\\le}$", font_size=30)
        lequal.move_to(COMPARISON_SIGN_POSITION)
        lequal.set_color(YELLOW_A)

        greater = Tex("$\\boldsymbol{>}$", font_size=30)
        greater.move_to(COMPARISON_SIGN_POSITION)
        greater.set_color(YELLOW_A)

        self.play(Write(title))
        self.wait(1)
        self.play(FadeOut(title))
        self.wait(1)
        self.play(*array.anim_create())

        for i in range(1, len(raw_array)):
            value = raw_array[i]

            self.play(*array.anim_focus(i))
            self.play(*array.anim_stage_focused())

            prev = i - 1

            self.play(*array.anim_compare(prev))
            self.play(*array.anim_stage_compared())
            self.play(FadeIn(lequal if raw_array[prev] <= value else greater))

            while prev != -1 and value < raw_array[prev]:
                raw_array[prev + 1] = raw_array[prev]

                self.play(
                    *array.anim_replace_with_compared(prev + 1),
                    FadeOut(lequal if raw_array[prev] <= value else greater)
                )

                prev -= 1

                if prev != -1:
                    self.play(*array.anim_compare(prev))
                    self.play(*array.anim_stage_compared())
                    self.play(
                        FadeIn(lequal if raw_array[prev] <= value else greater)
                    )

            if prev != -1:
                self.play(
                    *array.anim_replace_with_compared(prev),
                    FadeOut(lequal if raw_array[prev] <= value else greater)
                )

            raw_array[prev + 1] = value

            self.play(*array.anim_replace_with_focused(prev + 1))

        self.play(*array.anim_reset_colors())
        self.wait(1)
