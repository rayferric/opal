from manim import *

ARRAY = [28399, 31812, 10294, 71302, 18053, 10620]


class RadixSort(MovingCameraScene):
    def construct(self):
        title = Text("Radix Sort", font_size=50)

        self.play(Write(title))
        self.wait(1)
        self.play(FadeOut(title))
        self.wait(1)

        # Create number list.
        vgroup = VGroup()
        digits = []
        for i, n in enumerate(ARRAY):
            number_vgroup = VGroup()
            digits.append([])

            if i != 0:
                separator = Line(RIGHT * -0.25, RIGHT * 2.25)
                separator.shift(DOWN * i)
                separator.shift(UP * 0.5)
                vgroup.add(separator)

            for j in range(len(str(n))):
                digit = Text(str(n)[j])
                digit.shift(RIGHT * j * 0.5)
                number_vgroup.add(digit)
                digits[-1].append((int(str(n)[j]), digit))

            number_vgroup.shift(DOWN * i)

            vgroup.add(number_vgroup)
        vgroup.shift(-vgroup.get_center())

        vgroup_copy = vgroup.copy()

        self.play(Write(vgroup))

        # Morph number list to table.

        self.table = Table(
            [[c for c in str(n)] for n in ARRAY],
            include_outer_lines=True,
        )
        self.table.scale(0.75)

        for entry in self.table.get_entries():
            entry.set_opacity(0)

        anims = []

        for i, number_vgroup in enumerate(vgroup):
            if i % 2 == 0:
                table_row = self.table.get_rows()[i // 2]
                for digit, cell in zip(number_vgroup, table_row):
                    anims.append(digit.animate.move_to(cell))
            else:
                anims.append(FadeOut(number_vgroup))

        self.play(*anims, Create(self.table))

        # Sort table.
        for digit_index in range(len(digits[0]) - 1, -1, -1):
            self.play(*self.anim_highlight_column(digit_index))

            # Find permutation.
            permutation = list(range(len(digits)))
            # permutation.sort(key=lambda i: digits[i][digit_index][0])
            self.insertion_sort(
                permutation,
                lambda i, j: digits[i][digit_index][0]
                < digits[j][digit_index][0],
            )

            # Animate-permute table rows.
            anims = []

            new_digits = digits.copy()
            for i, j in enumerate(permutation):
                new_digits[i] = digits[j]

                for k, (n, digit) in enumerate(digits[j]):
                    anims.append(
                        digit.animate.move_to(
                            self.table.get_cell((i + 1, k + 1))
                        )
                    )

            digits = new_digits

            self.play(*anims)

        # Morph table to list.
        # digits contains the digits in the right table order.
        anims = []

        for i, number_vgroup in enumerate(vgroup_copy):
            if i % 2 == 0:
                table_row = digits[i // 2]
                for j in range(len(table_row)):
                    anims.append(
                        table_row[j][1].animate.move_to(number_vgroup[j])
                    )
            else:
                anims.append(FadeIn(number_vgroup))

        self.play(*self.hide_highlighter())
        self.play(*anims, FadeOut(self.table))

        self.wait(1)

    def hide_highlighter(self):
        anims = []

        if hasattr(self, "highlighter") and self.highlighter:
            anims += [FadeOut(self.highlighter)]
            self.highlighter = None

        return anims

    def anim_highlight_column(self, i):
        top_left = (i, 0)
        bottom_right = (i, len(self.table.get_rows()) - 1)

        anims = self.hide_highlighter()

        column_width = self.table.width / len(self.table.get_rows()[0])
        row_height = self.table.height / len(self.table.get_rows())

        # Create new highlighter.
        self.highlighter = Rectangle(
            width=(bottom_right[0] - top_left[0] + 1) * column_width,
            height=(bottom_right[1] - top_left[1] + 1) * row_height,
            stroke_width=5,
            stroke_color=RED,
            fill_opacity=0.1,
            fill_color=RED,
        )

        self.highlighter.move_to(self.table.get_cell((1, 1)).get_center())
        self.highlighter.shift(
            (top_left[0] + (bottom_right[0] - top_left[0]) / 2)
            * column_width
            * RIGHT
            + (top_left[1] + (bottom_right[1] - top_left[1]) / 2)
            * row_height
            * DOWN
        )

        return anims + [Create(self.highlighter)]

    def insertion_sort(
        self, array: list, comparator: callable = lambda a, b: a < b
    ):
        for i in range(1, len(array)):
            key = array[i]
            j = i - 1

            while j >= 0 and comparator(key, array[j]):
                array[j + 1] = array[j]
                j -= 1

            array[j + 1] = key
