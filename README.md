# Introduction

This is a fork of the original [Nest2D](https://github.com/markfink/nest2D) repository. This fork was made to improve the usability of the nest2D library within python by adding method calls to various properties of the defined structures from C++. Pull requests are welcome and will be merged when accepted.  

Nest2D is a 2D bin packaging tool for python.

Nest2D works with the C++ libnest2d backend for speed. For python C++ interoperability we use pybind11.
 
The library is written in a way that it should be usable out of the box with a very simple interface. The backend is reasonably fast and robust, being built on top of boost geometry and the [polyclipping](http://www.angusj.com/delphi/clipper.php) library.


# Installing

Due to git submodules existing within this project, the '--recursive' option must be used when cloning this repository. Furthermore, it requires cmake, boost, and polyclpping to be installed. A dockerfile and docker-compose.yml file are provided for development purposes on machine where it is not possible to install those libraries due to lack of permissions, a windows machine, etc.. 

```bash
$ sudo apt-get update 
$ sudo apt-get install -y cmake libboost-all-dev libpolyclipping-dev
$ git clone https://github.com/acadbrinkman/nest2D --recursive
$ cd nest2D
$ pip install .
```

# Example

A simple example may be the best way to demonstrate the usage of the library.

``` python
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

    sw = SVGWriter()
    sw.write_packgroup(pgrp, box)
    sw.save()
```

It is worth to note that the type of the polygon carried by the Item objects is
the type defined as a polygon by the geometry backend. In the example we use the
clipper backend and clipper works with integer coordinates.


## Example call

``` bash
$ pip install nest2D
$ python examples/simple_sample.py
```

## Example output

![Alt text](https://raw.githubusercontent.com/markfink/nest2d/master/docs/media/example_output.svg?sanitize=true)


## License

Unfortunately libnest2d is provided with a contractible GPL type license so we can not release this with better license terms. Details can be found in the LICENSE file. By using, distributing, or contributing to this project, you agree to the
terms and conditions of this license.


# References

- [pybind11](https://github.com/pybind/pybind11)
- [libnest2d](https://github.com/tamasmeszaros/libnest2d)
- [SVGNest](https://github.com/Jack000/SVGnest)
- [An effective heuristic for the two-dimensional irregular
bin packing problem](http://www.cs.stir.ac.uk/~goc/papers/EffectiveHueristic2DAOR2013.pdf)
- [Complete and robust no-fit polygon generation for the irregular stock cutting problem](https://www.sciencedirect.com/science/article/abs/pii/S0377221706001639)
