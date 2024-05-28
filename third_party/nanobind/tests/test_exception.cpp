#include <nanobind/nanobind.h>

namespace nb = nanobind;

class MyError1 : public std::exception {
public:
    virtual const char *what() const noexcept { return "MyError1"; }
};

class MyError2 : public std::exception {
public:
    virtual const char *what() const noexcept { return "MyError2"; }
};

class MyError3 : public std::exception {
public:
    virtual const char *what() const noexcept { return "MyError3"; }
};

NB_MODULE(test_exception_ext, m) {
    m.def("raise_generic", [] { throw std::exception(); });
    m.def("raise_bad_alloc", [] { throw std::bad_alloc(); });
    m.def("raise_runtime_error", [] { throw std::runtime_error("a runtime error"); });
    m.def("raise_domain_error", [] { throw std::domain_error("a domain error"); });
    m.def("raise_invalid_argument", [] { throw std::invalid_argument("an invalid argument error"); });
    m.def("raise_length_error", [] { throw std::length_error("a length error"); });
    m.def("raise_out_of_range", [] { throw std::out_of_range("an out of range error"); });
    m.def("raise_range_error", [] { throw std::range_error("a range error"); });
    m.def("raise_overflow_error", [] { throw std::overflow_error("an overflow error"); });
    m.def("raise_index_error", [] { throw nb::index_error("an index error"); });
    m.def("raise_key_error", [] { throw nb::key_error("a key error"); });
    m.def("raise_value_error", [] { throw nb::value_error("a value error"); });
    m.def("raise_type_error", [] { throw nb::type_error("a type error"); });
    m.def("raise_import_error", [] { throw nb::import_error("an import error"); });
    m.def("raise_attribute_error", [] { throw nb::attribute_error("an attribute error"); });
    m.def("raise_stop_iteration", [] { throw nb::stop_iteration("a stop iteration error"); });

    m.def("raise_my_error_1", [] { throw MyError1(); });

    nb::register_exception_translator(
        [](const std::exception_ptr &p, void * /* unused */) {
            try {
                std::rethrow_exception(p);
            } catch (const MyError2 &e) {
                PyErr_SetString(PyExc_IndexError, e.what());
            }
        });
    m.def("raise_my_error_2", [] { throw MyError2(); });

    nb::exception<MyError3>(m, "MyError3");
    m.def("raise_my_error_3", [] { throw MyError3(); });

    m.def("raise_nested", [](nb::callable c) {
            int arg = 123;
            try {
                c(arg);
            } catch (nb::python_error &e) {
                nb::raise_from(e, PyExc_RuntimeError, "Call with value %i failed", arg);
            }
        }
    );
}
