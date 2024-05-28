#include <nanobind/nanobind.h>
#include <nanobind/trampoline.h>
#include <nanobind/operators.h>
#include <nanobind/stl/optional.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/tuple.h>
#include <map>
#include <memory>
#include <cstring>
#include <vector>
#include <nanobind/stl/detail/traits.h>
#include "inter_module.h"

namespace nb = nanobind;
using namespace nb::literals;

static int default_constructed = 0, value_constructed = 0, copy_constructed = 0,
           move_constructed = 0, copy_assigned = 0, move_assigned = 0,
           destructed = 0, pickled = 0, unpickled = 0;

struct Struct;
std::unique_ptr<Struct> struct_tmp;
static std::vector<int> struct_destructed;

struct Struct {
    int i = 5;

    Struct() { default_constructed++; }
    Struct(int i) : i(i) { value_constructed++; }
    Struct(const Struct &s) : i(s.i) { copy_constructed++; }
    Struct(Struct &&s) noexcept : i(s.i) { s.i = 0; move_constructed++; }
    Struct &operator=(const Struct &s) { i = s.i; copy_assigned++; return *this; }
    Struct &operator=(Struct &&s) noexcept { i = s.i; s.i = 0; move_assigned++; return *this; }
    ~Struct() { destructed++; if (nb::is_alive()) struct_destructed.push_back(i); }

    int value() const { return i; }
    int getstate() const { ++pickled; return i; }
    void set_value(int value) { i = value; }
    void setstate(int value) { unpickled++; i = value; }

    static int static_test(int) { return 1; }
    static int static_test(float) { return 2; }

    static Struct* create_take() { return new Struct(10); }
    static Struct  create_move() { return Struct(11); }
    static Struct* create_copy() { return struct_tmp.get(); }
    static Struct* create_reference() { return struct_tmp.get(); }
    Struct &self() { return *this; }
};

struct PairStruct {
    Struct s1;
    Struct s2;
};

struct Big {
    char data[1024];
    Big() { memset(data, 0xff, 1024); }
};

struct alignas(128) BigAligned {
    char data[128];
    BigAligned() {
        if (((uintptr_t) data) % 128)
            throw std::runtime_error("data is not aligned!");
        memset(data, 0xff, 128);
    }
};

struct Animal {
    virtual ~Animal() = default;
    virtual std::string name() const { return "Animal"; }
    virtual std::string what() const = 0;
    virtual void void_ret() { }
};

struct StaticProperties {
    static int value;
};

struct StaticProperties2 : StaticProperties { };

int StaticProperties::value = 23;

static Py_ssize_t sq_length_dummy(PyObject *) {
    return 123;
}

struct Wrapper {
    std::shared_ptr<Wrapper> value;
};

struct StructWithWeakrefs : Struct { };

struct StructWithWeakrefsAndDynamicAttrs : Struct { };

struct UniqueInt {
    static std::map<int, std::weak_ptr<UniqueInt>> instances;

    static std::shared_ptr<UniqueInt> make(int val) {
        std::weak_ptr<UniqueInt>& entry = instances[val];
        std::shared_ptr<UniqueInt> ret = entry.lock();
        if (!ret) {
            entry = ret = std::shared_ptr<UniqueInt>(new UniqueInt(val));
        }
        ++ret->nlook;
        return ret;
    }
    int value() const { return val; }
    int lookups() const { return nlook; }

  private:
    UniqueInt(int v) : val(v) {}
    int val;
    int nlook = 0;
};
std::map<int, std::weak_ptr<UniqueInt>> UniqueInt::instances;

int wrapper_tp_traverse(PyObject *self, visitproc visit, void *arg) {
    Wrapper *w = nb::inst_ptr<Wrapper>(self);

    // If c->value corresponds to an associated CPython object, return it
    nb::object value = nb::find(w->value);

    // Inform the Python GC about it
    Py_VISIT(value.ptr());

    return 0;
}

int wrapper_tp_clear(PyObject *self) {
    Wrapper *w = nb::inst_ptr<Wrapper>(self);
    w->value.reset();
    return 0;
}

PyType_Slot wrapper_slots[] = {
    { Py_tp_traverse, (void *) wrapper_tp_traverse },
    { Py_tp_clear, (void *) wrapper_tp_clear },
    { 0, 0 }
};

NB_MODULE(test_classes_ext, m) {
    struct_tmp = std::make_unique<Struct>(12);

    auto cls = nb::class_<Struct>(m, "Struct", "Some documentation")
        .def(nb::init<>())
        .def(nb::init<int>())
        .def("value", &Struct::value)
        .def("set_value", &Struct::set_value, "value"_a)
        .def("self", &Struct::self, nb::rv_policy::none)
        .def("none", [](Struct &) -> const Struct * { return nullptr; })
        .def("__getstate__", &Struct::getstate)
        .def("__setstate__", &Struct::setstate)
        .def_static("static_test", nb::overload_cast<int>(&Struct::static_test))
        .def_static("static_test", nb::overload_cast<float>(&Struct::static_test))
        .def_static("create_move", &Struct::create_move)
        .def_static("create_reference", &Struct::create_reference,
                    nb::rv_policy::reference)
        .def_static("create_copy", &Struct::create_copy,
                    nb::rv_policy::copy)
        .def_static("create_take", &Struct::create_take);

    if (!nb::type<Struct>().is(cls))
        nb::detail::raise("type lookup failed!");

    nb::class_<PairStruct>(m, "PairStruct")
        .def(nb::init<>())
        .def_rw("s1", &PairStruct::s1, "A documented property")
        .def_rw("s2", &PairStruct::s2);

    m.def("stats", []{
        nb::dict d;
        d["default_constructed"] = default_constructed;
        d["value_constructed"] = value_constructed;
        d["copy_constructed"] = copy_constructed;
        d["move_constructed"] = move_constructed;
        d["copy_assigned"] = copy_assigned;
        d["move_assigned"] = move_assigned;
        d["destructed"] = destructed;
        d["pickled"] = pickled;
        d["unpickled"] = unpickled;
        return d;
    });

    m.def("reset", []() {
        default_constructed = 0;
        value_constructed = 0;
        copy_constructed = 0;
        move_constructed = 0;
        copy_assigned = 0;
        move_assigned = 0;
        destructed = 0;
        pickled = 0;
        unpickled = 0;
    });

    // test06_big

    nb::class_<Big>(m, "Big", "A class\nwith a multi-line\ndocstring..")
        .def(nb::init<>());

    nb::class_<BigAligned>(m, "BigAligned")
        .def(nb::init<>());

    // test09_trampoline
    // test10_trampoline_failures

    struct PyAnimal : Animal {
        NB_TRAMPOLINE(Animal, 3);

        PyAnimal() {
            default_constructed++;
        }

        ~PyAnimal() override {
            destructed++;
        }

        std::string name() const override {
            NB_OVERRIDE(name);
        }

        std::string what() const override {
            NB_OVERRIDE_PURE(what);
        }

        void void_ret() override {
            NB_OVERRIDE(void_ret);
        }
    };

    struct Dog : Animal {
        Dog(const std::string &s) : s(s) { }
        std::string name() const override { return "Dog"; }
        std::string what() const override { return s; }
        std::string s;
    };

    struct PyDog : Dog {
        NB_TRAMPOLINE(Dog, 2);

        PyDog(const std::string &s) : Dog(s) { }

        std::string name() const override {
            NB_OVERRIDE(name);
        }

        std::string what() const override {
            NB_OVERRIDE(what);
        }
    };

    struct Cat : Animal {
        Cat(const std::string &s) : s(s) { }
        std::string name() const override { return "Cat"; }
        std::string what() const override { return s; }
        std::string s;
    };

    struct Foo { };

    auto animal = nb::class_<Animal, PyAnimal>(m, "Animal")
        .def(nb::init<>(), "A constructor")
        .def("name", &Animal::name, "A method")
        .def("what", &Animal::what);

    nb::class_<Dog, Animal, PyDog>(m, "Dog")
        .def(nb::init<const std::string &>());

    nb::class_<Cat>(m, "Cat", animal)
        .def(nb::init<const std::string &>());

    m.def("go", [](Animal *a) {
        return a->name() + " says " + a->what();
    });

    m.def("animal_passthrough", [](Animal *a) { return a; }, nb::rv_policy::none);
    m.def("dog_passthrough", [](Dog *d) { return d; }, nb::rv_policy::none);

    m.def("void_ret", [](Animal *a) { a->void_ret(); });

    m.def("call_function", [](nb::handle h) {
        return h(1, 2, "hello", true, 4);
    });

    m.def("call_method", [](nb::handle h) {
        return h.attr("f")(1, 2, "hello", true, 4);
    });

    // test11_large_pointers
    nb::class_<Foo>(m, "Foo");
    m.def("i2p", [](uintptr_t x) { return (Foo *) x; }, nb::rv_policy::reference);
    m.def("p2i", [](Foo *x) { return (uintptr_t) x; }, "x"_a = nullptr);

    // test12_implicitly_convertible
    struct A { int a; };
    struct B { int b; };
    struct B2 : B { B2(int i) : B{i} { } };
    struct C { int c; };

    struct D {
        D(const A &a) : value(a.a + 10) { }
        D(const B *b) : value(b->b + 100) { }
        D(C c) : value(c.c + 1000) { }
        D(int d) : value(d + 10000) { }
        D(float) : value(0) { throw std::runtime_error("Fail!"); }
        D(std::nullptr_t) : value(0) {}
        // notice dangling access:
        ~D() { static_cast<volatile int&>(value) = -100; }
        int value;
    };

    nb::class_<A>(m, "A")
        .def(nb::init<int>());

    nb::class_<B>(m, "B")
        .def(nb::init<int>());

    nb::class_<B2, B>(m, "B2")
        .def(nb::init<int>());

    nb::class_<C>(m, "C")
        .def(nb::init<int>());

    nb::class_<D>(m, "D")
        .def(nb::init_implicit<const A &>())
        .def(nb::init_implicit<const B *>())
        .def(nb::init_implicit<int>())
        .def(nb::init_implicit<float>())
        .def_rw("value", &D::value);

    m.def("get_d", [](const D &d) { return d.value; });
    m.def("get_optional_d", [](std::optional<const D*> arg) {
        return arg ? arg.value()->value : -1;
    }, nb::arg().none());
    m.def("get_d_via_cast", [](nb::object obj) {
        int by_val = -1, by_ptr = -1, by_opt_val = -1, by_opt_ptr = -1;
        try {
            by_val = nb::cast<D>(obj).value;
        } catch (const nb::cast_error&) {}
        try {
            by_ptr = nb::cast<D*>(obj)->value;
        } catch (const nb::cast_error&) {}
        try {
            by_opt_val = nb::cast<std::optional<D>>(obj)->value;
        } catch (const nb::cast_error&) {}
        try {
            by_opt_ptr = nb::cast<std::optional<D*>>(obj).value()->value;
        } catch (const nb::cast_error&) {}
        return nb::make_tuple(by_val, by_ptr, by_opt_val, by_opt_ptr);
    });
    m.def("get_d_via_try_cast", [](nb::object obj) {
        int by_val = -1, by_ptr = -1, by_opt_val = -1, by_opt_ptr = -1;
        if (D val(nullptr); nb::try_cast(obj, val))
            by_val = val.value;
        if (D* ptr; nb::try_cast(obj, ptr))
            by_ptr = ptr->value;
        if (std::optional<D> opt_val; nb::try_cast(obj, opt_val))
            by_opt_val = opt_val->value;
        if (std::optional<D*> opt_ptr; nb::try_cast(obj, opt_ptr))
            by_opt_ptr = opt_ptr.value()->value;
        return nb::make_tuple(by_val, by_ptr, by_opt_val, by_opt_ptr);
    });

    struct Int {
        int i;
        Int operator+(Int o) const { return {i + o.i}; }
        Int operator-(float j) const { return {int(i - j)}; }
        bool operator==(Int o) const { return i == o.i; }
        Int &operator+=(Int o) {
            i += o.i;
            return *this;
        }
    };

    // test13_operators
    nb::class_<Int>(m, "Int")
        .def(nb::init<int>())
        .def(nb::self + nb::self)
        .def(nb::self += nb::self)
        .def(nb::self - float())
        .def("__repr__", [](const Int &i) { return std::to_string(i.i); });


    // test15: Test nb::keep_alive feature
    m.def(
        "keep_alive_arg", [](nb::handle, nb::handle ret) { return ret; },
        nb::keep_alive<0, 1>());

    m.def(
        "keep_alive_ret", [](nb::handle, nb::handle ret) { return ret; },
        nb::keep_alive<1, 0>());

    // test17_name_qualname_module()
    m.def("f", []{});
    struct MyClass { struct NestedClass { }; };
    nb::class_<MyClass> mcls(m, "MyClass");
    nb::class_<MyClass::NestedClass> ncls(mcls, "NestedClass");
    mcls.def(nb::init<>());
    mcls.def("f", [](MyClass&){});
    ncls.def("f", [](MyClass::NestedClass&){});

    // test18_static_properties
    nb::class_<StaticProperties>(m, "StaticProperties")
        .def_rw_static("value", &StaticProperties::value, "Static property docstring")
        .def_static("get", []{ return StaticProperties::value; } );

    nb::class_<StaticProperties2, StaticProperties>(m, "StaticProperties2");

    // test19_supplement
    struct ClassWithSupplement { };
    struct Supplement {
        uint8_t data[0xFF];
    };

    auto scls = nb::class_<ClassWithSupplement>(m, "ClassWithSupplement", nb::supplement<Supplement>())
        .def(nb::init<>());

    Supplement &supplement = nb::type_supplement<Supplement>(scls);
    for (uint8_t i = 0; i < 0xFF; ++i)
        supplement.data[i] = i;

    m.def("check_supplement", [](nb::handle h) {
        if (nb::isinstance<ClassWithSupplement>(h)) {
            Supplement &s2 = nb::type_supplement<Supplement>(h.type());
            for (uint16_t i = 0; i < 0xFF; ++i) {
                if (s2.data[i] != i)
                    return false;
            }
            return true;
        }
        return false;
    });

    // test20_type_callback
    PyType_Slot slots[] {
        { Py_sq_length, (void *) sq_length_dummy },
        { 0, nullptr }
    };

    struct ClassWithLen { };
    nb::class_<ClassWithLen>(m, "ClassWithLen", nb::type_slots(slots))
        .def(nb::init<>());

    // test21_low_level
    m.def("test_lowlevel", []() {
        nb::handle py_type = nb::type<Struct>();
        if (!(nb::type_check(py_type) &&
              nb::type_size(py_type) == sizeof(Struct) &&
              nb::type_align(py_type) == alignof(Struct) &&
              nb::type_info(py_type) == typeid(Struct)))
            throw std::runtime_error("Internal error!");

        nb::object py_inst = nb::inst_alloc(py_type);
        if (!(nb::inst_check(py_inst) && py_inst.type().is(py_type) &&
              !nb::inst_ready(py_inst)))
            throw std::runtime_error("Internal error! (2)");

        // Get a C++ pointer to the uninitialized instance data
        Struct *ptr = nb::inst_ptr<Struct>(py_inst);

        // Perform an in-place construction of the C++ object
        new (ptr) Struct(123);

        nb::inst_mark_ready(py_inst);
        if (!nb::inst_ready(py_inst))
            throw std::runtime_error("Internal error! (3)");

        nb::object py_inst_2 = nb::inst_alloc(py_type);
        if (nb::inst_ready(py_inst_2))
            throw std::runtime_error("Internal error! (4)");

        nb::inst_copy(py_inst_2, py_inst);
        if (!nb::inst_ready(py_inst_2))
            throw std::runtime_error("Internal error! (5)");

        nb::inst_destruct(py_inst);
        if (nb::inst_ready(py_inst))
            throw std::runtime_error("Internal error! (6)");

        nb::inst_move(py_inst, py_inst_2);
        if (!nb::inst_ready(py_inst))
            throw std::runtime_error("Internal error! (7)");

        nb::handle py_type_pair = nb::type<PairStruct>();
        PairStruct *ps = new PairStruct{Struct(123), Struct(456)};
        nb::object py_inst_3 = nb::inst_take_ownership(py_type_pair, ps);
        if (!(nb::inst_check(py_inst_3) && py_inst_3.type().is(py_type_pair) &&
              nb::inst_state(py_inst_3) == std::make_pair(true, true)))
            throw std::runtime_error("Internal error! (8)");

        nb::object py_inst_4 = nb::inst_reference(py_type, &ps->s1, py_inst_3);
        if (!(nb::inst_check(py_inst_4) && py_inst_4.type().is(py_type) &&
              nb::inst_state(py_inst_4) == std::make_pair(true, false)))
            throw std::runtime_error("Internal error! (9)");

        return nb::make_tuple(py_inst, py_inst_2, py_inst_3, py_inst_4);
    });

    // test22_handle_t
    m.def("test_handle_t", [](nb::handle_t<Struct> h) { return borrow(h); });

    // test23_type_object_t
    m.def("test_type_object_t", [](nb::type_object_t<Struct> h) -> nb::object { return std::move(h); });

    // test24_none_arg
    m.def("none_0", [](Struct *s) { return s == nullptr; });
    m.def("none_1", [](Struct *s) { return s == nullptr; }, nb::arg());
    m.def("none_2", [](Struct *s) { return s == nullptr; }, nb::arg("arg"));
    m.def("none_3", [](Struct *s) { return s == nullptr; }, nb::arg().none());
    m.def("none_4", [](Struct *s) { return s == nullptr; }, nb::arg("arg").none());

    // test25_is_final
    struct FinalType { };
    nb::class_<FinalType>(m, "FinalType", nb::is_final())
        .def(nb::init<>());

    // test26_dynamic_attr
    struct StructWithAttr : Struct { };
    nb::class_<StructWithAttr, Struct>(m, "StructWithAttr", nb::dynamic_attr())
        .def(nb::init<int>());

    nb::class_<Wrapper>(m, "Wrapper", nb::type_slots(wrapper_slots))
        .def(nb::init<>())
        .def_rw("value", &Wrapper::value);

    // The following isn't tested on the Python side, we just want to make sure it compiles
    struct NonCopyable {
        NonCopyable() = default;
        NonCopyable(const NonCopyable&) = delete;
    };

    using NonCopyableVec = std::vector<NonCopyable>;
    nb::class_<NonCopyableVec>(m, "NonCopyableVec");

    m.def("is_int_1", [](nb::handle h) { return nb::isinstance<int>(h); });
    m.def("is_int_2", [](nb::handle h) { return nb::isinstance<nb::int_>(h); });
    m.def("is_struct", [](nb::handle h) { return nb::isinstance<Struct>(h); });

    struct Base { ~Base() = default; };
    struct PolymorphicBase { virtual ~PolymorphicBase() = default; };
    struct Subclass : Base { };
    struct PolymorphicSubclass : PolymorphicBase { };
    struct AnotherSubclass : Base { };
    struct AnotherPolymorphicSubclass : PolymorphicBase { };

    nb::class_<Base> (m, "Base");
    nb::class_<Subclass> (m, "Subclass");
    nb::class_<PolymorphicBase> (m, "PolymorphicBase");
    nb::class_<PolymorphicSubclass> (m, "PolymorphicSubclass");

    m.def("polymorphic_factory", []() { return (PolymorphicBase *) new PolymorphicSubclass(); });
    m.def("polymorphic_factory_2", []() { return (PolymorphicBase *) new AnotherPolymorphicSubclass(); });
    m.def("factory", []() { return (Base *) new Subclass(); });
    m.def("factory_2", []() { return (Base *) new AnotherSubclass(); });

    m.def("check_shared", [](Shared *) { });

    m.def("try_cast_1", [](nb::handle h) {
        Struct s;
        bool rv = nb::try_cast<Struct>(h, s);
        return std::make_pair(rv, std::move(s));
    }, nb::arg().none());

    m.def("try_cast_2", [](nb::handle h) {
        Struct s;
        Struct &s2 = s;
        bool rv = nb::try_cast<Struct &>(h, s2);
        return std::make_pair(rv, std::move(s2));
    }, nb::arg().none());

    m.def("try_cast_3", [](nb::handle h) {
        Struct *sp = nullptr;
        bool rv = nb::try_cast<Struct *>(h, sp);
        return std::make_pair(rv, sp);
    }, nb::arg().none(), nb::rv_policy::none);

    m.def("try_cast_4", [](nb::handle h) {
        int i = 0;
        bool rv = nb::try_cast<int>(h, i);
        return std::make_pair(rv, i);
    });

#if !defined(Py_LIMITED_API)
    m.def("test_slots", []() {
        nb::object wrapper_tp = nb::module_::import_("test_classes_ext").attr("Wrapper");
        return nb::make_tuple(
            nb::type_get_slot(wrapper_tp, Py_tp_traverse) == (void *) wrapper_tp_traverse,
            nb::type_get_slot(&PyLong_Type, Py_tp_init) == (void *) PyLong_Type.tp_init,
            nb::type_get_slot(&PyLong_Type, Py_nb_add) == (void *) PyLong_Type.tp_as_number->nb_add
        );
    });
#endif

    // Used by test41_implicit_conversion_keep_alive
    struct IncrementingStruct : Struct {
        IncrementingStruct(const Struct &s) : Struct(s) {
            i++;
        }
    };

    nb::class_<IncrementingStruct, Struct>(m, "IncrementingStruct")
        .def(nb::init_implicit<const Struct &>());

    m.def("get_destructed", []() {
        nb::list out;
        for (int i : struct_destructed)
            out.append(i);
        struct_destructed.clear();
        return out;
    });

    m.def(
        "get_incrementing_struct_value",
        [](IncrementingStruct &s) { return new Struct(s.i + 100); },
        nb::keep_alive<0, 1>());

    nb::class_<StructWithWeakrefs, Struct>(m, "StructWithWeakrefs", nb::is_weak_referenceable())
        .def(nb::init<int>());

    nb::class_<StructWithWeakrefsAndDynamicAttrs, Struct>(m, "StructWithWeakrefsAndDynamicAttrs",
               nb::is_weak_referenceable(), nb::dynamic_attr())
        .def(nb::init<int>());

    union Union {
        int i;
        float f;
    };
    nb::class_<Union>(m, "Union")
        .def(nb::init<>())
        .def_rw("i", &Union::i)
        .def_rw("f", &Union::f);

    struct HiddenBase {
        int value = 10;
        int vget() const { return value; }
        void vset(int v) { value = v; }
        int get_answer() const { return value * 10; }
    };
    struct BoundDerived : HiddenBase {
        virtual int polymorphic() { return value; }
    };
    nb::class_<BoundDerived>(m, "BoundDerived")
        .def(nb::init<>())
        .def_rw("value", &BoundDerived::value)
        .def_prop_rw("prop", &BoundDerived::vget, &BoundDerived::vset)
        .def("get_answer", &BoundDerived::get_answer)
        .def("polymorphic", &BoundDerived::polymorphic);

    nb::class_<UniqueInt>(m, "UniqueInt")
        .def(nb::new_(&UniqueInt::make))
        .def(nb::new_([](std::string s) {
            return UniqueInt::make(std::atoi(s.c_str()));
        }))
        .def("value", &UniqueInt::value)
        .def("lookups", &UniqueInt::lookups);
}
