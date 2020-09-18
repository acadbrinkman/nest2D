#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <libnest2d/libnest2d.hpp>

#include "../tools/printer_parts.hpp"
#include "../tools/svgtools.hpp"

namespace py = pybind11;

using Point = libnest2d::Point;
using Box = libnest2d::Box;
using Item = libnest2d::Item;
using PackGroup = libnest2d::PackGroup;
using SVGWriter = libnest2d::svg::SVGWriter<libnest2d::PolygonImpl>;

using NfpPlacer = libnest2d::NfpPlacer;
using FirstFitSelection = libnest2d::FirstFitSelection;
using NestConfig = libnest2d::NestConfig<NfpPlacer, FirstFitSelection>;
using PlacerConfig = NfpPlacer::Config;
using Radians = libnest2d::Radians;
using Alignment = PlacerConfig::Alignment;

// Cannot use standard scale of 1000000, because Box only supports up to signed 32 bit integers
const static double SCALE = 1000.0;

PYBIND11_MODULE(nest2D, m)
{
    m.doc() = "2D irregular bin packaging and nesting for python";


    py::class_<Radians>(m, "Radians", "Radian type")
        .def(py::init([](double rads){
            return std::unique_ptr<Radians>(new Radians(rads));
        }))
        .def("val", [](const Radians &rad){ return double(rad); })
        .def("__repr__",
             [](const Radians &rad) {
                 return boost::lexical_cast<std::string>(double(rad));
        });


    py::class_<PlacerConfig> placer_config(m, "placer_config");

    placer_config.def(py::init<>())
        .def_readwrite("rotations", &PlacerConfig::rotations)
        .def_readwrite("alignment", &PlacerConfig::alignment)
        .def_readwrite("starting_point", &PlacerConfig::starting_point);
        

    py::enum_<PlacerConfig::Alignment>(placer_config, "Alignment")
        .value("CENTER", PlacerConfig::Alignment::CENTER)
        .value("BOTTOM_LEFT", PlacerConfig::Alignment::BOTTOM_LEFT)
        .value("BOTTOM_RIGHT", PlacerConfig::Alignment::BOTTOM_RIGHT)
        .value("TOP_LEFT", PlacerConfig::Alignment::TOP_LEFT)
        .value("TOP_RIGHT", PlacerConfig::Alignment::TOP_RIGHT)
        .value("DONT_ALIGN", PlacerConfig::Alignment::DONT_ALIGN)
        .export_values();



    py::class_<Point>(m, "Point", "2D Point")
        //.def(py::init<int, int>(), py::arg("x"), py::arg("y"))
        .def(py::init([](double x, double y){
            return std::unique_ptr<Point>(new Point(x * SCALE, y * SCALE));
        }))
        //.def_property_readonly("x", &Point::X)
        .def_property_readonly("x", [](const Point &p) { return p.X / SCALE; })
        .def_property_readonly("y", [](const Point &p) { return p.Y / SCALE; })
        .def("__repr__",
             [](const Point &p) {
                 std::string r("Point(");
                 r += boost::lexical_cast<std::string>(p.X / SCALE);
                 r += ", ";
                 r += boost::lexical_cast<std::string>(p.Y / SCALE);
                 r += ")";
                 return r;
             })
        .def("__eq__",
             [](const Point &p, const Point &q) {
                 return p == q;
             });

    // see lib/libnest2d/include/libnest2d/geometry_traits.hpp
    py::class_<Box>(m, "Box", "2D Box point pair")
        //.def(py::init<int, int>())
        // custom constructor to define box center
        .def(py::init([](double x, double y) {
            int xint, yint;
            xint = x * SCALE;
            yint = y * SCALE;
            return std::unique_ptr<Box>(new Box(xint, yint, {xint / 2, yint / 2}));
        }))
        .def("center", [](const Box &b){ return b.center(); })
        .def("height", [](const Box &b){ return b.height() / SCALE; })
        .def("width", [](const Box &b){ return b.width() / SCALE; })
        .def("area", [](const Box &b){ return (b.area() / SCALE) / SCALE; })
        .def("minCorner", [](const Box &b){ return b.minCorner(); })
        .def("maxCorner", [](const Box &b){ return b.maxCorner(); })
        .def("__repr__",
             [](const Box &b) {
                 std::string r("Box(area: ");
                 r += boost::lexical_cast<std::string>((b.area() / SCALE)/SCALE);
                 r += ", width: ";
                 r += boost::lexical_cast<std::string>(b.width() / SCALE);
                 r += ", height: ";
                 r += boost::lexical_cast<std::string>(b.height() / SCALE);
                 r += ")";
                 return r;
             });

    // Item is a shape defined by points
    // see lib/libnest2d/include/libnest2d/nester.hpp
    py::class_<Item>(m, "Item", "Item")
        .def(py::init<std::vector<Point>>())
        .def(py::init([](std::vector<Point> &contour, std::vector<std::vector<Point>> &holes) {
            return std::unique_ptr<Item>(new Item(contour, holes));
        }))
        .def("binId", [](const Item &i) { return i.binId(); })
        .def("area", [](const Item &i) { return (i.area() / SCALE) / SCALE; })
        .def("rotation", [](const Item &i) { return double(i.rotation()); })
        .def("translation", [](const Item &i) { return i.translation(); })
        .def("vertexCount", [](const Item &i) { return i.vertexCount(); })
        .def("toString", [](const Item &i) { return i.toString(); })
        .def("boundingBox", [](const Item &i) { return i.boundingBox(); })
        .def("__repr__",
             [](const Item &i) {
                 std::string r("Item(area: ");
                 r += boost::lexical_cast<std::string>((i.area() / SCALE)/ SCALE);
                 r += ", bin_id: ";
                 r += boost::lexical_cast<std::string>(i.binId());
                 r += ", vertices: ";
                 r += boost::lexical_cast<std::string>(i.vertexCount());
                 r += ")";
                 return r;
             });

    // mm conversion
    m.attr("MM_IN_COORDS") = SCALE;

    // The nest function takes two parameters input and box
    // see lib/libnest2d/include/libnest2d/libnest2d.hpp
    m.def(
        "nest", [](std::vector<Item> &input, const Box &box, const PlacerConfig &placer_config) {      

            size_t bins = libnest2d::nest(input, box, 0, NestConfig(placer_config),
                                  libnest2d::ProgressFunction{[](unsigned cnt) {
                std::cout << "parts left: " << cnt << std::endl;
            }});

            PackGroup pgrp(bins);

            for (Item &itm : input)
            {
                if (itm.binId() >= 0)
                    pgrp[size_t(itm.binId())].emplace_back(itm);
                //py::print("bin_id: ", itm.binId());
                //py::print("vertices: ", itm.vertexCount());
            }

            //return pgrp;
            // we need to convert c++ type to python using py::cast
            py::object obj = py::cast(pgrp);
            return obj;
        },
        py::arg("input"), py::arg("box"), py::arg("placer_config") = PlacerConfig(), "Nest and pack the input items into the box bin.");

    py::class_<SVGWriter>(m, "SVGWriter", "SVGWriter tools to write pack_group to SVG.")
        .def(py::init([]() {
            // custom constructor
            SVGWriter::Config conf;
            conf.mm_in_coord_units = SCALE;
            return std::unique_ptr<SVGWriter>(new SVGWriter(conf));
        }))
        .def("write_packgroup", [](SVGWriter &sw, const PackGroup &pgrp, const Box &box) {
            sw.setSize(box); // TODO make own call
            sw.writePackGroup(pgrp);
        })
        .def("save", [](SVGWriter &sw) {
            sw.save("out");
        })
        .def("__repr__", [](const SVGWriter &sw) {
            std::string r("SVGWriter(");
            r += ")";
            return r;
        });
}
