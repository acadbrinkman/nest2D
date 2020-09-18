from nest2D import Point, Box, Item, nest, SVGWriter
from math import degrees

def add_shape1(n, items):
    for i in range(n):
        item = Item([
            Point(-5.0, 8.95405),
            Point(5.0, 8.954050),
            Point(5.000000, -0.045949),
            Point(4.972609, -0.568550),
            Point(3.500000, -8.954050),
            Point(-3.500000, -8.954050),
            Point(-4.972609, -0.568550),
            Point(-5.000000, -0.045949),
            Point(-5.000000, 8.954050)
        ])
        items.append(item)


def add_shape2(n, items):
    for i in range(n):
        item = Item([
            Point(-11.750000, 13.057900),
            Point(-9.807860, 15.000000),
            Point(4.392139, 24.000000),
            Point(11.750000, 24.000000),
            Point(11.750000, -24.000000),
            Point(4.392139, -24.000000),
            Point(-9.807860, -15.000000),
            Point(-11.750000, -13.057900),
            Point(-11.750000, 13.057900)
        ])
        items.append(item)


def main():
    box = Box(150, 150)
    input = []
    add_shape1(23, input)
    add_shape2(15, input)

    pgrp = nest(input, box)
    print(pgrp)

    sw = SVGWriter()
    sw.write_packgroup(pgrp, box)
    sw.save()


if __name__ == '__main__':
    main()
