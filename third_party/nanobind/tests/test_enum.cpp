#include <nanobind/nanobind.h>

namespace nb = nanobind;

enum class Enum  : uint32_t { A, B, C = (uint32_t) -1 };
enum class SEnum : int32_t { A, B, C = (int32_t) -1 };
enum ClassicEnum { Item1, Item2 };

struct EnumProperty { Enum get_enum() { return Enum::A; } };

NB_MODULE(test_enum_ext, m) {
    nb::enum_<Enum>(m, "Enum", "enum-level docstring")
        .value("A", Enum::A, "Value A")
        .value("B", Enum::B, "Value B")
        .value("C", Enum::C, "Value C");

    nb::enum_<SEnum>(m, "SEnum", nb::is_arithmetic())
        .value("A", SEnum::A)
        .value("B", SEnum::B)
        .value("C", SEnum::C);

    auto ce = nb::enum_<ClassicEnum>(m, "ClassicEnum")
        .value("Item1", ClassicEnum::Item1)
        .value("Item2", ClassicEnum::Item2)
        .export_values();

    ce.def("get_value", [](ClassicEnum &x) { return (int) x; })
      .def_prop_ro("my_value", [](ClassicEnum &x) { return (int) x; })
      .def("foo", [](ClassicEnum x) { return x; })
      .def_static("bar", [](ClassicEnum x) { return x; });

    m.def("from_enum", [](Enum value) { return (uint32_t) value; }, nb::arg().noconvert());
    m.def("to_enum", [](uint32_t value) { return (Enum) value; });
    m.def("from_enum", [](SEnum value) { return (int32_t) value; }, nb::arg().noconvert());

    m.def("from_enum_implicit", [](Enum value) { return (uint32_t) value; });

    m.def("from_enum_default_0", [](Enum value) { return (uint32_t) value; }, nb::arg("value") = Enum::A);
    m.def("from_enum_default_1", [](SEnum value) { return (uint32_t) value; }, nb::arg("value") = SEnum::A);

    // test for issue #39
    nb::class_<EnumProperty>(m, "EnumProperty")
        .def(nb::init<>())
        .def_prop_ro("read_enum", &EnumProperty::get_enum);
}
