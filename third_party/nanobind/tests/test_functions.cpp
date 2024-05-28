#include <nanobind/nanobind.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/string.h>

namespace nb = nanobind;
using namespace nb::literals;

int call_guard_value = 0;

struct my_call_guard {
    my_call_guard() { call_guard_value = 1; }
    ~my_call_guard() { call_guard_value = 2; }
};

int test_31(int i) noexcept { return i; }

NB_MODULE(test_functions_ext, m) {
    m.doc() = "function testcase";

    // Function without inputs/outputs
    m.def("test_01", []() { });

    // Simple binary function (via function pointer)
    auto test_02 = [](int up, int down) -> int { return up - down; };
    m.def("test_02", (int (*)(int, int)) test_02, "up"_a = 8, "down"_a = 1);

    // Simple binary function with capture object
    int i = 42;
    m.def("test_03", [i](int j, int k) -> int { return i + j - k; });

    // Large capture object requiring separate storage
    uint64_t k = 10, l = 11, m_ = 12, n = 13, o = 14;
    m.def("test_04", [k, l, m_, n, o]() -> int { return (int) (k + l + m_ + n + o); });

    // Overload chain with two docstrings
    m.def("test_05", [](int) -> int { return 1; }, "doc_1");
    nb::object first_overload = m.attr("test_05");
    m.def("test_05", [](float) -> int { return 2; }, "doc_2");
#if !defined(PYPY_VERSION)
    // Make sure we don't leak the previous member of the overload chain
    // (pypy's refcounts are bogus and will not help us with this check)
    if (first_overload.ptr()->ob_refcnt != 1) {
        throw std::runtime_error("Overload was leaked!");
    }
#endif
    first_overload.reset();

    /// Function raising an exception
    m.def("test_06", []() { throw std::runtime_error("oops!"); });

    /// Function taking some positional/keyword args and nb::[kw]args
    m.def("test_07", [](int, int, nb::args args, nb::kwargs kwargs) {
        return std::make_pair(args.size(), kwargs.size());
    });

    /// As above, but with nb::arg annotations
    m.def("test_07", [](int, int, nb::args args, nb::kwargs kwargs) {
        return std::make_pair(args.size(), kwargs.size());
    }, "a"_a, "b"_a, "myargs"_a, "mykwargs"_a);

    /// Test successful/unsuccessful tuple conversion
    m.def("test_tuple", []() { return nb::make_tuple("Hello", 123); });
    m.def("test_bad_tuple", []() { struct Foo{}; return nb::make_tuple("Hello", Foo()); });

    /// Perform a Python function call from C++
    m.def("test_call_1", [](nb::object o) { return o(1); });
    m.def("test_call_2", [](nb::object o) { return o(1, 2); });

    /// Test expansion of args/kwargs-style arguments
    m.def("test_call_extra", [](nb::object o, nb::args args, nb::kwargs kwargs) {
        return o(1, 2, *args, **kwargs, "extra"_a = 5);
    });

    /// Test list manipulation
    m.def("test_list", [](nb::list l) {
        int result = 0;
        for (size_t i = 0; i < l.size(); ++i)
            result += nb::cast<int>(l[i]);
        l[2] = 123;
        l.append(result);
    });

    /// Test tuple manipulation
    m.def("test_tuple", [](nb::tuple l) {
        int result = 0;
        for (size_t i = 0; i < l.size(); ++i)
            result += nb::cast<int>(l[i]);
        return result;
    });

    /// Test call_guard feature
    m.def("test_call_guard_wrapper_rvalue_ref", [](int&& i) { return i; },
          nb::call_guard<my_call_guard>());

    m.def("test_call_guard", []() {
        return call_guard_value;
    }, nb::call_guard<my_call_guard>());

    m.def("call_guard_value", []() { return call_guard_value; });

    m.def("test_release_gil", []() -> bool {
#if defined(Py_LIMITED_API)
        return false;
#else
        return PyGILState_Check();
#endif
    }, nb::call_guard<nb::gil_scoped_release>());

    m.def("test_print", []{
        nb::print("Test 1");
        nb::print(nb::str("Test 2"));
    });

    m.def("test_iter", [](nb::object in) {
        nb::list l;
        for (nb::handle h : in)
            l.append(h);
        return l;
    });

    m.def("test_iter_tuple", [](nb::tuple in) {
        nb::list l;
        for (nb::handle h : in)
            l.append(h);
        return l;
    });

    m.def("test_iter_list", [](nb::list in) {
        nb::list l;
        for (nb::handle h : in)
            l.append(h);
        return l;
    });

    // Overload chain with a raw docstring that has precedence
    m.def("test_08", [](int) -> int { return 1; }, "first docstring");
    m.def("test_08", [](float) -> int { return 2; },
          nb::sig("def test_08(x: typing.Annotated[float, 'foo']) -> int"),
          "another docstring");

    // Manual type check
    m.def("test_09", [](nb::type_object t) -> bool { return t.is(&PyBool_Type); });

    // nb::dict iterator
    m.def("test_10", [](nb::dict d) {
        nb::dict result;
        for (auto [k, v] : d)
            result[k] = v;
        return result;
    });

    m.def("test_10_contains", [](nb::dict d) {
        return d.contains(nb::str("foo"));
    });

    // Test implicit conversion of various types
    m.def("test_11_sl",  [](signed long x)        { return x; });
    m.def("test_11_ul",  [](unsigned long x)      { return x; });
    m.def("test_11_sll", [](signed long long x)   { return x; });
    m.def("test_11_ull", [](unsigned long long x) { return x; });

    // Test string caster
    m.def("test_12", [](const char *c) { return nb::str(c); });
    m.def("test_13", []() -> const char * { return "test"; });
    m.def("test_14", [](nb::object o) -> const char * { return nb::cast<const char *>(o); });

    // Test bytes type
    m.def("test_15",   [](nb::bytes o) -> const char * { return o.c_str(); });
    m.def("test_15_d", [](nb::bytes o) { return nb::bytes(o.data(), o.size()); });
    m.def("test_16",   [](const char *c) { return nb::bytes(c); });
    m.def("test_17",   [](nb::bytes c) { return c.size(); });
    m.def("test_18",   [](const char *c, int size) { return nb::bytes(c, size); });

    // Test int type
    m.def("test_19", [](nb::int_ i) { return i + nb::int_(123); });
    m.def("test_20", [](nb::str s) { return nb::int_(s) + nb::int_(123); });
    m.def("test_21", [](nb::int_ i) { return (int) i; });

    // Test capsule wrapper
    m.def("test_22", []() -> void * { return (void*) 1; });
    m.def("test_23", []() -> void * { return nullptr; });
    m.def("test_24", [](void *p) { return (uintptr_t) p; }, "p"_a.none());

    // Test slice
    m.def("test_25", [](nb::slice s) { return s; });
    m.def("test_26", []() { return nb::slice(4); });
    m.def("test_27", []() { return nb::slice(1, 10); });
    m.def("test_28", []() { return nb::slice(5, -5, -2); });

    // Test ellipsis
    m.def("test_29", [](nb::ellipsis) { return nb::ellipsis(); });

    // Traceback test
    m.def("test_30", [](nb::callable f) -> std::string {
        nb::gil_scoped_release g;
        try {
            nb::gil_scoped_acquire g2;
            f();
        } catch (const nb::python_error &e) {
            return e.what();
        }
        return "Unknown";
    });

    m.def("test_31", &test_31);
    m.def("test_32", [](int i) noexcept { return i; });

    m.def("identity_i8", [](int8_t  i) { return i; });
    m.def("identity_u8", [](uint8_t i) { return i; });
    m.def("identity_i16", [](int16_t  i) { return i; });
    m.def("identity_u16", [](uint16_t i) { return i; });
    m.def("identity_i32", [](int32_t  i) { return i; });
    m.def("identity_u32", [](uint32_t i) { return i; });
    m.def("identity_i64", [](int64_t  i) { return i; });
    m.def("identity_u64", [](uint64_t i) { return i; });

    m.attr("test_33") = nb::cpp_function([](nb::object self, int y) {
        return nb::cast<int>(self.attr("x")) + y;
    }, nb::is_method());

    m.attr("test_34") = nb::cpp_function([](nb::object self, int y) {
        return nb::cast<int>(self.attr("x")) * y;
    }, nb::arg("y"), nb::is_method());

    m.def("test_35", []() {
        const char *name = "Foo";

        auto callback = [=]() {
            return nb::str("Test {}").format(name);
        };

        return nb::cpp_function(callback);
    });

    m.def("test_cast_char", [](nb::handle h) {
        return nb::cast<char>(h);
    });

    m.def("test_cast_str", [](nb::handle h) {
        return nb::cast<const char *>(h);
    });

    m.def("test_set", []() {
        nb::set s;
        s.add("123");
        s.add(123);
        return s;
    });

    m.def("test_set_contains", [](nb::set s, nb::handle h) { return s.contains(h); });

    m.def("test_del_list", [](nb::list l) { nb::del(l[2]); });
    m.def("test_del_dict", [](nb::dict l) { nb::del(l["a"]); });

    static int imut = 10;
    static const int iconst = 100;
    m.def("test_ptr_return", []() { return std::make_pair(&imut, &iconst); });

    // These are caught at compile time, uncomment and rebuild to verify:

    // No nb::arg annotations:
    //m.def("bad_args1", [](nb::args, int) {});

    // kw_only in wrong place (1):
    //m.def("bad_args2", [](nb::args, int) {}, nb::kw_only(), "args"_a, "i"_a);

    // kw_only in wrong place (2):
    //m.def("bad_args3", [](nb::args, int) {}, "args"_a, "i"_a, nb::kw_only());

    // kw_only in wrong place (3):
    //m.def("bad_args4", [](int, nb::kwargs) {}, "i"_a, "kwargs"_a, nb::kw_only());

    // kw_only specified twice:
    //m.def("bad_args5", [](int, int) {}, nb::kw_only(), "i"_a, nb::kw_only(), "j"_a);

    m.def("test_args_kwonly",
          [](int i, double j, nb::args args, int z) {
              return nb::make_tuple(i, j, args, z);
          }, "i"_a, "j"_a, "args"_a, "z"_a);
    m.def("test_args_kwonly_kwargs",
          [](int i, double j, nb::args args, int z, nb::kwargs kwargs) {
              return nb::make_tuple(i, j, args, z, kwargs);
          }, "i"_a, "j"_a, "args"_a, nb::kw_only(), "z"_a, "kwargs"_a);
    m.def("test_kwonly_kwargs",
          [](int i, double j, nb::kwargs kwargs) {
              return nb::make_tuple(i, j, kwargs);
          }, "i"_a, nb::kw_only(), "j"_a, "kwargs"_a);

    m.def("test_kw_only_all",
          [](int i, int j) { return nb::make_tuple(i, j); },
          nb::kw_only(), "i"_a, "j"_a);
    m.def("test_kw_only_some",
          [](int i, int j, int k) { return nb::make_tuple(i, j, k); },
          nb::arg(), nb::kw_only(), "j"_a, "k"_a);
    m.def("test_kw_only_with_defaults",
          [](int i, int j, int k, int z) { return nb::make_tuple(i, j, k, z); },
          nb::arg() = 3, "j"_a = 4, nb::kw_only(), "k"_a = 5, "z"_a);
    m.def("test_kw_only_mixed",
          [](int i, int j) { return nb::make_tuple(i, j); },
          "i"_a, nb::kw_only(), "j"_a);

    struct kw_only_methods {
        kw_only_methods(int _v) : v(_v) {}
        int v;
    };

    nb::class_<kw_only_methods>(m, "kw_only_methods")
        .def(nb::init<int>(), nb::kw_only(), "v"_a)
        .def_rw("v", &kw_only_methods::v)
        .def("method_2k",
             [](kw_only_methods&, int i, int j) { return nb::make_tuple(i, j); },
             nb::kw_only(), "i"_a = 1, "j"_a = 2)
        .def("method_1p1k",
             [](kw_only_methods&, int i, int j) { return nb::make_tuple(i, j); },
             "i"_a = 1, nb::kw_only(), "j"_a = 2);

    m.def("test_any", [](nb::any a) { return a; } );

    m.def("test_wrappers_list", []{
        nb::list l1, l2;
        l1.append(1);
        l2.append(2);
        l1.extend(l2);

        bool b = nb::len(l1) == 2 && nb::len(l2) == 1 &&
            l1[0].equal(nb::int_(1)) && l1[1].equal(nb::int_(2));

        l1.clear();
        return b && nb::len(l1) == 0;
    });

    m.def("test_wrappers_dict", []{
        nb::dict d1, d2;
        d1["a"] = 1;
        d2["b"] = 2;
        d1.update(d2);

        bool b = nb::len(d1) == 2 && nb::len(d2) == 1 &&
            d1["a"].equal(nb::int_(1)) &&
            d1["b"].equal(nb::int_(2));

        d1.clear();
        return b && nb::len(d1) == 0;
    });

    m.def("test_wrappers_set", []{
        nb::set s;
        s.add("a");
        s.add("b");

        bool b = nb::len(s) == 2 && s.contains("a") && s.contains("b");

        b &= s.discard("a");
        b &= !s.discard("q");

        b &= !s.contains("a") && s.contains("b");
        s.clear();
        b &= s.size() == 0;

        return b;
    });

    m.def("hash_it", [](nb::handle h) { return nb::hash(h); });
}
