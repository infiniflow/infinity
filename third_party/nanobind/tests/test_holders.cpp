#if defined(__GNUC__)
// warning: '..' declared with greater visibility than the type of its field '..'
#  pragma GCC diagnostic ignored "-Wattributes"
#endif

#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/unique_ptr.h>
#include <nanobind/stl/pair.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

static int created = 0;
static int deleted = 0;

struct Example {
    int value;
    Example(int value) : value(value) { created++; }
    ~Example() { deleted++; }

    static Example *make(int value) { return new Example(value); }
    static std::shared_ptr<Example> make_shared(int value) {
        return std::make_shared<Example>(value);
    }
};

struct SharedWrapper { std::shared_ptr<Example> value; };
struct UniqueWrapper { std::unique_ptr<Example> value; };
struct UniqueWrapper2 { std::unique_ptr<Example, nb::deleter<Example>> value; };

struct ExampleST : std::enable_shared_from_this<ExampleST> {
    int value;
    ExampleST(int value) : value(value) { created++; }
    ~ExampleST() { deleted++; }

    static ExampleST *make(int value) { return new ExampleST(value); }
    static std::shared_ptr<ExampleST> make_shared(int value) {
        return std::make_shared<ExampleST>(value);
    }
};
struct SharedWrapperST {
    std::shared_ptr<ExampleST> value;
    ExampleST* get() const { return value.get(); }
};

static_assert(nb::detail::has_shared_from_this_v<ExampleST>);
static_assert(!nb::detail::has_shared_from_this_v<Example>);

enum class PetKind { Cat, Dog };
struct Pet { const PetKind kind; };
struct Dog : Pet { Dog() : Pet{PetKind::Dog} { } };
struct Cat : Pet { Cat() : Pet{PetKind::Cat} { } };

namespace nanobind::detail {
    template <> struct type_hook<Pet> {
        static const std::type_info *get(Pet *p) {
            if (p) {
                switch (p->kind) {
                    case PetKind::Dog: return &typeid(Dog);
                    case PetKind::Cat: return &typeid(Cat);
                }
            }
            return &typeid(Pet);
        }
    };
} // namespace nanobind::detail

NB_MODULE(test_holders_ext, m) {
    nb::class_<Example>(m, "Example")
        .def(nb::init<int>())
        .def_rw("value", &Example::value)
        .def_static("make", &Example::make)
        .def_static("make_shared", &Example::make_shared);

    // ------- shared_ptr -------

    nb::class_<SharedWrapper>(m, "SharedWrapper")
        .def(nb::init<std::shared_ptr<Example>>())
        .def_rw("ptr", &SharedWrapper::value)
        .def_prop_rw("value",
            [](SharedWrapper &t) { return t.value->value; },
            [](SharedWrapper &t, int value) { t.value->value = value; });

    m.def("query_shared_1", [](Example *shared) { return shared->value; });
    m.def("query_shared_2",
          [](std::shared_ptr<Example> &shared) { return shared->value; });
    m.def("query_shared_3",
          [](std::shared_ptr<const Example> &shared) { return shared->value; });
    m.def("passthrough",
          [](std::shared_ptr<Example> shared) { return shared; });
    m.def("passthrough_2",
          [](std::shared_ptr<const Example> shared) { return shared; });

    // ------- enable_shared_from_this -------

    nb::class_<ExampleST>(m, "ExampleST")
        .def(nb::init<int>())
        .def("has_shared_from_this", [](ExampleST& self) {
            return !self.weak_from_this().expired();
        })
        .def("shared_from_this", [](ExampleST& self) {
            return self.shared_from_this();
        })
        .def("use_count", [](ExampleST& self) {
            return self.weak_from_this().use_count();
        })
        .def_rw("value", &ExampleST::value)
        .def_static("make", &ExampleST::make)
        .def_static("make_shared", &ExampleST::make_shared);

    struct DerivedST : ExampleST {
        using ExampleST::ExampleST;
    };
    static_assert(nb::detail::has_shared_from_this_v<DerivedST>);
    nb::class_<DerivedST, ExampleST>(m, "DerivedST")
        .def(nb::init<int>())
        .def_static("make", [](int v) {
            return static_cast<DerivedST*>(ExampleST::make(v));
        })
        .def_static("make_shared", [](int v) {
            return std::static_pointer_cast<DerivedST>(ExampleST::make_shared(v));
        });

    nb::class_<SharedWrapperST>(m, "SharedWrapperST")
        .def(nb::init<std::shared_ptr<ExampleST>>())
        .def_static("from_existing", [](ExampleST *obj) {
            return SharedWrapperST{obj->shared_from_this()};
        })
        .def_static("from_wrapper", [](SharedWrapperST& w) {
            return SharedWrapperST{w.value};
        })
        .def("use_count", [](SharedWrapperST& self) {
            return self.value.use_count();
        })
        .def("same_owner", [](SharedWrapperST& self, ExampleST& other) {
            auto self_s = self.value;
            auto other_s = other.shared_from_this();
            return !self_s.owner_before(other_s) &&
                   !other_s.owner_before(self_s);
        })
        .def("get_own", &SharedWrapperST::get)
        .def("get_ref", &SharedWrapperST::get, nb::rv_policy::reference)
        .def_rw("ptr", &SharedWrapperST::value)
        .def_prop_rw("value",
            [](SharedWrapperST &t) { return t.value->value; },
            [](SharedWrapperST &t, int value) { t.value->value = value; });

    m.def("owns_cpp", [](nb::handle h) { return nb::inst_state(h).second; });
    m.def("same_owner", [](const SharedWrapperST& a,
                           const SharedWrapperST& b) {
        return !a.value.owner_before(b.value) && !b.value.owner_before(a.value);
    });

    // ------- unique_ptr -------

    m.def("unique_from_cpp",
          [](int val) { return std::make_unique<Example>(val); },
          nb::arg() = 1);
    m.def("unique_from_cpp_2", []() {
        return std::unique_ptr<Example, nb::deleter<Example>>(new Example(2));
    });

    nb::class_<UniqueWrapper>(m, "UniqueWrapper")
        .def(nb::init<std::unique_ptr<Example>>())
        .def("get", [](UniqueWrapper *uw) { return std::move(uw->value); });

    nb::class_<UniqueWrapper2>(m, "UniqueWrapper2")
        .def(nb::init<std::unique_ptr<Example, nb::deleter<Example>>>())
        .def("get", [](UniqueWrapper2 *uw) { return std::move(uw->value); });

    m.def("passthrough_unique",
          [](std::unique_ptr<Example> unique) { return unique; },
          nb::arg().none());
    m.def("passthrough_unique_2",
          [](std::unique_ptr<Example, nb::deleter<Example>> unique) { return unique; });

    m.def("passthrough_unique_pairs",
          [](std::vector<std::pair<std::unique_ptr<Example>,
                                   std::unique_ptr<Example>>> v,
             bool clear) {
              if (clear)
                  v.clear();
              return v;
          }, nb::arg("v"), nb::arg("clear") = false);

    m.def("stats", []{ return std::make_pair(created, deleted); });
    m.def("reset", []{ created = deleted = 0; });

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

    m.def("u_polymorphic_factory", []() { return std::unique_ptr<PolymorphicBase>(new PolymorphicSubclass()); });
    m.def("u_polymorphic_factory_2", []() { return std::unique_ptr<PolymorphicBase>(new AnotherPolymorphicSubclass()); });
    m.def("u_factory", []() { return std::unique_ptr<Base>(new Subclass()); });
    m.def("u_factory_2", []() { return std::unique_ptr<Base>(new AnotherSubclass()); });

    m.def("s_polymorphic_factory", []() { return std::shared_ptr<PolymorphicBase>(new PolymorphicSubclass()); });
    m.def("s_polymorphic_factory_2", []() { return std::shared_ptr<PolymorphicBase>(new AnotherPolymorphicSubclass()); });
    m.def("s_factory", []() { return std::shared_ptr<Base>(new Subclass()); });
    m.def("s_factory_2", []() { return std::shared_ptr<Base>(new AnotherSubclass()); });

    nb::class_<Pet>(m, "Pet");
    nb::class_<Dog>(m, "Dog");
    nb::class_<Cat>(m, "Cat");

    nb::enum_<PetKind>(m, "PetKind")
        .value("Cat", PetKind::Cat)
        .value("Dog", PetKind::Dog);

    m.def("make_pet", [](PetKind kind) -> Pet* {
        switch (kind) {
            case PetKind::Dog:
                return new Dog();
            case PetKind::Cat:
                return new Cat();
            default:
                throw std::runtime_error("Internal error");
        }
    });

    m.def("make_pet_u", [](PetKind kind) -> std::unique_ptr<Pet> {
        switch (kind) {
            case PetKind::Dog:
                return std::make_unique<Dog>();
            case PetKind::Cat:
                return std::make_unique<Cat>();
            default:
                throw std::runtime_error("Internal error");
        }
    });

    m.def("make_pet_s", [](PetKind kind) -> std::shared_ptr<Pet> {
        switch (kind) {
            case PetKind::Dog:
                return std::make_shared<Dog>();
            case PetKind::Cat:
                return std::make_shared<Cat>();
            default:
                throw std::runtime_error("Internal error");
        }
    });

    struct ExampleWrapper {
        Example value{5};
        std::shared_ptr<Example> value_nullable;
    };

    nb::class_<ExampleWrapper>(m, "ExampleWrapper")
        .def(nb::init<>())
        .def_rw("value", &ExampleWrapper::value)
        .def_rw("value_nullable", &ExampleWrapper::value_nullable, nb::arg().none());
}
