#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>
#include <nanobind/trampoline.h>
#include <nanobind/intrusive/counter.h>
#include <nanobind/intrusive/ref.h>

namespace nb = nanobind;
using namespace nb::literals;

static int test_constructed = 0;
static int test_destructed = 0;

class Test : public nb::intrusive_base {
public:
    Test() { test_constructed++; }

    virtual ~Test() { test_destructed++; }

    virtual int value(int i) const { return 123 + i; }

    static Test *create_raw() { return new Test(); }
    static nb::ref<Test> create_ref() { return new Test(); }
};

class PyTest : Test {
    NB_TRAMPOLINE(Test, 1);
    virtual int value(int i) const {
        NB_OVERRIDE(value, i);
    }
};

NB_MODULE(test_intrusive_ext, m) {
    nb::intrusive_init(
        [](PyObject *o) noexcept {
            nb::gil_scoped_acquire guard;
            Py_INCREF(o);
        },
        [](PyObject *o) noexcept {
            nb::gil_scoped_acquire guard;
            Py_DECREF(o);
        });

    nb::class_<nb::intrusive_base>(
        m, "intrusive_base",
        nb::intrusive_ptr<nb::intrusive_base>(
            [](nb::intrusive_base *o, PyObject *po) noexcept { o->set_self_py(po); }));

    nb::class_<Test, nb::intrusive_base, PyTest>(m, "Test")
        .def(nb::init<>())
        .def("value", &Test::value)
        .def_static("create_raw", &Test::create_raw)
        .def_static("create_ref", &Test::create_ref);

    m.def("reset", [] {
        test_constructed = 0;
        test_destructed = 0;
    });

    m.def("stats", []() -> std::pair<int, int> {
        return { test_constructed, test_destructed };
    });

    m.def("get_value_1", [](Test *o) { nb::ref<Test> x(o); return x->value(1); });
    m.def("get_value_2", [](nb::ref<Test> x) { return x->value(2); });
    m.def("get_value_3", [](const nb::ref<Test> &x) { return x->value(3); });
}
