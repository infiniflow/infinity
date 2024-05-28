#include <nanobind/nanobind.h>
#include <nanobind/eval.h>
#include <nanobind/stl/pair.h>

namespace nb = nanobind;

NB_MODULE(test_eval_ext, m) {
    auto global = nb::dict(nb::module_::import_("__main__").attr("__dict__"));

    m.def("test_eval_statements", [global]() {
        auto local = nb::dict();
        local["call_test"] = nb::cpp_function([&]() -> int { return 42; });

        // Regular string literal
        nb::exec("message = 'Hello World!'\n"
                 "x = call_test()",
                 global,
                 local);

        // Multi-line raw string literal
        nb::exec(R"(
            if x == 42:
                print(message)
            else:
                raise RuntimeError
            )",
                 global,
                 local);
        auto x = nb::cast<int>(local["x"]);
        return x == 42;
    });

    m.def("test_eval", [global]() {
        auto local = nb::dict();
        local["x"] = nb::int_(42);
        auto x = nb::eval("x", global, local);
        return nb::cast<int>(x) == 42;
    });

    m.def("test_eval_single_statement", []() {
        auto local = nb::dict();
        local["call_test"] = nb::cpp_function([&]() -> int { return 42; });

        auto result = nb::eval<nb::eval_single_statement>("x = call_test()", nb::dict(), local);
        auto x = nb::cast<int>(local["x"]);
        return result.is_none() && x == 42;
    });

    m.def("test_eval_failure", []() {
        try {
            nb::eval("nonsense code ...");
        } catch (nb::python_error &) {
            return true;
        }
        return false;
    });

    // test_eval_closure
    m.def("test_eval_closure", []() {
        nb::dict global;
        global["closure_value"] = 42;
        nb::dict local;
        local["closure_value"] = 0;
        nb::exec(R"(
            local_value = closure_value

            def func_global():
                return closure_value

            def func_local():
                return local_value
            )",
                 global,
                 local);
        return std::make_pair(global, local);
    });

    m.def("globals_contains_a", []() {
        return nb::globals().contains("a");
    });

    m.def("globals_add_b", []() {
        auto globals = nb::globals();
        globals["b"] = 123;
        return globals;
    });
}
