from typing import Tuple
from manim import *
import manim

ARRAY = [0, 3, 2, 5, 1, 4, 1]
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

    def set_value_opacity(self, opacity):
        self.text.set_opacity(opacity)


class Array:
    def __init__(self, array=None, numbers=None, scene=None):
        if numbers == None:
            self.numbers = [Number(value=number) for number in array]

            for i, number in enumerate(self.numbers):
                number.shift(i * RIGHT * 1.5)

            offset = -self.get_center()

            for number in self.numbers:
                number.shift(offset)
        else:
            self.numbers = numbers

        self.scene = scene
        self.highlighters = []

    def hide_values(self):
        for number in self.numbers:
            number.set_value_opacity(0)

    def anim_create(self):
        return sum([number.anim_create() for number in self.numbers], [])

    def anim_fade_in(self):
        return sum([number.anim_fade_in() for number in self.numbers], [])

    def get_center(self):
        return sum([number.get_center() for number in self.numbers]) / len(
            self.numbers
        )

    def anim_swap(self, i, j):
        return self.numbers[i].anim_swap_value_with(self.numbers[j])

    def anim_highlight_max(self):
        max_number = max(self.numbers, key=lambda number: number.get_value())
        return max_number.anim_set_color(RED)

    def anim_init_table(self):
        max_value = max(
            self.numbers, key=lambda number: number.get_value()
        ).get_value()

        # Create table.
        self.table = manim.Table(
            [
                [str(i) for i in range(max_value + 1)],
                ["0"] * (max_value + 1),
            ],
            include_outer_lines=True,
        )
        self.table.scale(0.75)
        self.table.shift(DOWN * 2.5)

        anims = [Create(self.table)]

        # Shift camera.
        anims += [self.scene.camera.frame.animate.shift(SHIFT_DOWN * 1)]

        # Highlight the last column label.
        anims += self.anim_highlight_table_cells(
            (max_value, 0), (max_value, 0), RED
        )
        return anims

    def anim_highlight_table_cells(
        self,
        top_left: Tuple[int, int],
        bottom_right: Tuple[int, int],
        color: str,
        reset_others: bool = True,
    ):
        anims = []

        # Hide existing highlighter.
        if reset_others:
            if len(self.highlighters) != 0:
                anims += [
                    FadeOut(highlighter) for highlighter in self.highlighters
                ]
            self.highlighters = []

        column_width = self.table.width / len(self.table.get_rows()[0])
        row_height = self.table.height / len(self.table.get_rows())

        # Create new highlighter.
        self.highlighters.append(
            Rectangle(
                width=(bottom_right[0] - top_left[0] + 1) * column_width,
                height=(bottom_right[1] - top_left[1] + 1) * row_height,
                stroke_width=5,
                stroke_color=color,
                fill_opacity=0.1,
                fill_color=color,
            )
        )

        self.highlighters[-1].move_to(self.table.get_cell((1, 1)).get_center())
        self.highlighters[-1].shift(
            (top_left[0] + (bottom_right[0] - top_left[0]) / 2)
            * column_width
            * RIGHT
            + (top_left[1] + (bottom_right[1] - top_left[1]) / 2)
            * row_height
            * DOWN
        )

        anims += [Create(self.highlighters[-1])]

        return anims

    def anim_highlight_number(self, index: int, color: str):
        anims = []

        # Hide all existing number highlights.
        for i, number in enumerate(self.numbers):
            if i != index:
                anims += number.anim_set_color(BLUE)

        # Highlight number.
        anims += self.numbers[index].anim_set_color(color)

        return anims

    def anim_set_table_cell_value(self, cell: Tuple[int, int], value: Any):
        # Replace-transform the cell value in to a new one.
        cell = self.table.get_entries()[
            cell[0] + len(self.table.get_columns()) * cell[1]
        ]
        new_text = Text(str(value))
        new_text.move_to(cell.get_center())
        new_text.set_color(cell.get_color())
        new_text.scale(0.75)
        return [ReplacementTransform(cell, new_text)]

    def anim_clear_highlights(self):
        anims = []

        # Clear number highlights.
        for number in self.numbers:
            anims += number.anim_set_color(BLUE)

        # Clear table highlights.
        anims += [FadeOut(highlighter) for highlighter in self.highlighters]
        self.highlighters = []

        return anims

    def anim_morph_copy_of_table_cell_into_another(
        self, from_cell: Tuple[int, int], to_cell: Tuple[int, int], value: Any
    ):
        # Make a copy of from_cell and move it to to_cell simultaneously fading from_cell out.
        # Simultaneously replace-transform to-cell value to value.

        # Get the from_cell and to_cell.
        from_entry = self.table.get_entries()[
            from_cell[0] + len(self.table.get_columns()) * from_cell[1]
        ]
        to_entry = self.table.get_entries()[
            to_cell[0] + len(self.table.get_columns()) * to_cell[1]
        ]

        # # Create a copy of from_cell.
        from_entry_copy = from_entry.copy()

        # Animate move-fade from_cell_copy to to_cell.
        anims = [
            UpdateFromAlphaFunc(
                from_entry_copy,
                lambda m, a: m.move_to(
                    interpolate(
                        from_entry.get_center(), to_entry.get_center(), a
                    )
                )
                and m.set_opacity(1 - a),
            )
        ]

        # Animate replace-transform to_cell value to value.
        anims += self.anim_set_table_cell_value(to_cell, value)

        return anims

    def shift(self, *args, **kwargs):
        for number in self.numbers:
            number.shift(*args, **kwargs)

    def anim_set_opacity(self, opacity: float):
        return sum(
            [number.anim_set_opacity(opacity) for number in self.numbers], []
        )


class CountingSort(MovingCameraScene):
    def construct(self):
        title = Text("Counting Sort", font_size=50)

        self.array = Array(array=ARRAY, scene=self)
        self.raw_array = ARRAY

        self.play(Write(title))
        self.wait(1)
        self.play(FadeOut(title))
        self.wait(1)

        self.play(*self.array.anim_create())

        self.counting_sort()

        self.wait(1)

    def counting_sort(self):
        max_value = max(self.raw_array)
        self.play(*self.array.anim_highlight_max())

        counts = [0] * (max_value + 1)
        self.play(*self.array.anim_init_table(), run_time=2)
        self.wait(1)

        for i, number in enumerate(self.raw_array):
            counts[number] += 1
            self.play(
                *self.array.anim_highlight_number(i, RED),
                *self.array.anim_highlight_table_cells(
                    (number, 0), (number, 1), RED
                ),
                *self.array.anim_set_table_cell_value(
                    (number, 1), counts[number]
                ),
            )

        self.play(
            *self.array.anim_clear_highlights(),
        )

        for i in range(1, len(counts)):
            counts[i] += counts[i - 1]
            self.play(
                *self.array.anim_highlight_table_cells((i - 1, 1), (i, 1), RED),
                *self.array.anim_morph_copy_of_table_cell_into_another(
                    (i - 1, 1), (i, 1), counts[i]
                ),
            )

        output = list(range(len(self.raw_array)))
        self.output_array = Array(array=output, scene=self)
        self.output_array.hide_values()
        self.output_array.shift(5 * DOWN)

        indices = list(range(len(self.raw_array)))
        self.indices_array = Array(array=indices, scene=self)
        for number in self.indices_array.numbers:
            number.circle.set_opacity(0)
            number.text.scale(0.75)
            number.text.set_color(GRAY)
        self.indices_array.shift(5 * DOWN + 0.75 * UP)

        self.play(
            *self.array.anim_clear_highlights(),
            *self.output_array.anim_create(),
            *self.indices_array.anim_create(),
            self.camera.frame.animate.shift(1 * DOWN),
        )
        self.wait(1)

        for i in range(len(self.raw_array) - 1, -1, -1):
            value = self.raw_array[i]
            index = counts[value] - 1
            self.play(
                *self.array.anim_highlight_number(i, PINK),
                *self.array.anim_highlight_table_cells(
                    (value, 0), (value, 0), PINK
                ),
            )
            self.play(
                *self.array.anim_highlight_table_cells(
                    (value, 1), (value, 1), RED, reset_others=False
                ),
                *self.array.anim_set_table_cell_value((value, 1), index),
            )
            self.play(
                *self.output_array.anim_highlight_number(index, RED),
            )
            self.play(
                *self.output_array.numbers[index].anim_set_value(value),
                *self.output_array.numbers[index].anim_set_color(BLUE),
                *self.array.anim_clear_highlights(),
            )
            counts[value] -= 1

        self.wait(1)
        self.play(
            *self.array.anim_set_opacity(0),
            self.array.table.animate.set_opacity(0),
            *self.indices_array.anim_set_opacity(0),
            self.camera.frame.animate.shift(2.35 * DOWN),
        )
