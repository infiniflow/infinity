/*
    nanobind/nb_class.h: Functionality for binding C++ classes/structs

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

/// Flags about a type that persist throughout its lifetime
enum class type_flags : uint32_t {
    /// Does the type provide a C++ destructor?
    is_destructible          = (1 << 0),

    /// Does the type provide a C++ copy constructor?
    is_copy_constructible    = (1 << 1),

    /// Does the type provide a C++ move constructor?
    is_move_constructible    = (1 << 2),

    /// Is the 'destruct' field of the type_data structure set?
    has_destruct             = (1 << 4),

    /// Is the 'copy' field of the type_data structure set?
    has_copy                 = (1 << 5),

    /// Is the 'move' field of the type_data structure set?
    has_move                 = (1 << 6),

    /// Internal: does the type maintain a list of implicit conversions?
    has_implicit_conversions = (1 << 7),

    /// Is this a python type that extends a bound C++ type?
    is_python_type           = (1 << 8),

    /// This type does not permit subclassing from Python
    is_final                 = (1 << 9),

    /// Instances of this type support dynamic attribute assignment
    has_dynamic_attr         = (1 << 10),

    /// The class uses an intrusive reference counting approach
    intrusive_ptr            = (1 << 11),

    /// Is this a class that inherits from enable_shared_from_this?
    /// If so, type_data::keep_shared_from_this_alive is also set.
    has_shared_from_this     = (1 << 12),

    /// Instances of this type can be referenced by 'weakref'
    is_weak_referenceable    = (1 << 13),

    /// A custom signature override was specified
    has_signature            = (1 << 14),

    /// The class implements __class_getitem__ similar to typing.Generic
    is_generic               = (1 << 15),

    /// Is this an arithmetic enumeration?
    is_arithmetic            = (1 << 16),

    /// Is the number type underlying the enumeration signed?
    is_signed                = (1 << 17)

    // One more flag bits available (18) without needing
    // a larger reorganization
};

/// Flags about a type that are only relevant when it is being created.
/// These are currently stored in type_data::flags alongside the type_flags
/// for more efficient memory layout, but could move elsewhere if we run
/// out of flags.
enum class type_init_flags : uint32_t {
    /// Is the 'supplement' field of the type_init_data structure set?
    has_supplement           = (1 << 19),

    /// Is the 'doc' field of the type_init_data structure set?
    has_doc                  = (1 << 20),

    /// Is the 'base' field of the type_init_data structure set?
    has_base                 = (1 << 21),

    /// Is the 'base_py' field of the type_init_data structure set?
    has_base_py              = (1 << 22),

    /// This type provides extra PyType_Slot fields
    has_type_slots           = (1 << 23),

    all_init_flags           = (0x1f << 19)
};

// See internals.h
struct nb_alias_chain;

/// Information about a type that persists throughout its lifetime
struct type_data {
    uint32_t size;
    uint32_t align : 8;
    uint32_t flags : 24;
    const char *name;
    const std::type_info *type;
    PyTypeObject *type_py;
    nb_alias_chain *alias_chain;
    void (*destruct)(void *);
    void (*copy)(void *, const void *);
    void (*move)(void *, void *) noexcept;
    union {
        // Implicit conversions for C++ type bindings
        struct {
            const std::type_info **cpp;
            bool (**py)(PyTypeObject *, PyObject *, cleanup_list *) noexcept;
        } implicit;

        // Forward and reverse mappings for enumerations
        struct {
            void *fwd;
            void *rev;
        } enum_tbl;
    };
    void (*set_self_py)(void *, PyObject *) noexcept;
    bool (*keep_shared_from_this_alive)(PyObject *) noexcept;
#if defined(Py_LIMITED_API)
    size_t dictoffset;
    size_t weaklistoffset;
#endif
};

/// Information about a type that is only relevant when it is being created
struct type_init_data : type_data {
    PyObject *scope;
    const std::type_info *base;
    PyTypeObject *base_py;
    const char *doc;
    const PyType_Slot *type_slots;
    size_t supplement;
};

NB_INLINE void type_extra_apply(type_init_data &t, const handle &h) {
    t.flags |= (uint32_t) type_init_flags::has_base_py;
    t.base_py = (PyTypeObject *) h.ptr();
}

NB_INLINE void type_extra_apply(type_init_data &t, const char *doc) {
    t.flags |= (uint32_t) type_init_flags::has_doc;
    t.doc = doc;
}

NB_INLINE void type_extra_apply(type_init_data &t, type_slots c) {
    t.flags |= (uint32_t) type_init_flags::has_type_slots;
    t.type_slots = c.value;
}

template <typename T>
NB_INLINE void type_extra_apply(type_init_data &t, intrusive_ptr<T> ip) {
    t.flags |= (uint32_t) type_flags::intrusive_ptr;
    t.set_self_py = (void (*)(void *, PyObject *) noexcept) ip.set_self_py;
}

NB_INLINE void type_extra_apply(type_init_data &t, is_final) {
    t.flags |= (uint32_t) type_flags::is_final;
}

NB_INLINE void type_extra_apply(type_init_data &t, dynamic_attr) {
    t.flags |= (uint32_t) type_flags::has_dynamic_attr;
}

NB_INLINE void type_extra_apply(type_init_data & t, is_weak_referenceable) {
    t.flags |= (uint32_t) type_flags::is_weak_referenceable;
}

NB_INLINE void type_extra_apply(type_init_data & t, is_generic) {
    t.flags |= (uint32_t) type_flags::is_generic;
}

NB_INLINE void type_extra_apply(type_init_data & t, const sig &s) {
    t.flags |= (uint32_t) type_flags::has_signature;
    t.name = s.value;
}

template <typename T>
NB_INLINE void type_extra_apply(type_init_data &t, supplement<T>) {
    static_assert(std::is_trivially_default_constructible_v<T>,
                  "The supplement must be a POD (plain old data) type");
    static_assert(alignof(T) <= alignof(void *),
                  "The alignment requirement of the supplement is too high.");
    t.flags |= (uint32_t) type_init_flags::has_supplement | (uint32_t) type_flags::is_final;
    t.supplement = sizeof(T);
}

struct enum_init_data {
    const std::type_info *type;
    PyObject *scope;
    const char *name;
    const char *docstr;
    uint32_t flags;
};

NB_INLINE void enum_extra_apply(enum_init_data &e, is_arithmetic) {
    e.flags |= (uint32_t) type_flags::is_arithmetic;
}

NB_INLINE void enum_extra_apply(enum_init_data &e, const char *doc) {
    e.docstr = doc;
}

template <typename T>
NB_INLINE void enum_extra_apply(enum_init_data &, T) {
    static_assert(
        std::is_void_v<T>,
        "Invalid enum binding annotation. The implementation of "
        "enums changed nanobind 2.0.0: only nb::is_arithmetic and "
        "docstrings can be passed since this change.");
}

template <typename T> void wrap_copy(void *dst, const void *src) {
    new ((T *) dst) T(*(const T *) src);
}

template <typename T> void wrap_move(void *dst, void *src) noexcept {
    new ((T *) dst) T(std::move(*(T *) src));
}

template <typename T> void wrap_destruct(void *value) noexcept {
    ((T *) value)->~T();
}

template <typename, template <typename, typename> typename, typename...>
struct extract;

template <typename T, template <typename, typename> typename Pred>
struct extract<T, Pred> {
    using type = T;
};

template <typename T, template <typename, typename> typename Pred,
          typename Tv, typename... Ts>
struct extract<T, Pred, Tv, Ts...> {
    using type = std::conditional_t<
        Pred<T, Tv>::value,
        Tv,
        typename extract<T, Pred, Ts...>::type
    >;
};

template <typename T, typename Arg> using is_alias = std::is_base_of<T, Arg>;
template <typename T, typename Arg> using is_base = std::is_base_of<Arg, T>;

enum op_id : int;
enum op_type : int;
struct undefined_t;
template <op_id id, op_type ot, typename L = undefined_t, typename R = undefined_t> struct op_;

// The header file include/nanobind/stl/detail/traits.h extends this type trait
template <typename T, typename SFINAE = int>
struct is_copy_constructible : std::is_copy_constructible<T> { };

template <typename T>
constexpr bool is_copy_constructible_v = is_copy_constructible<T>::value;

NAMESPACE_END(detail)

// Low level access to nanobind type objects
inline bool type_check(handle h) { return detail::nb_type_check(h.ptr()); }
inline size_t type_size(handle h) { return detail::nb_type_size(h.ptr()); }
inline size_t type_align(handle h) { return detail::nb_type_align(h.ptr()); }
inline const std::type_info& type_info(handle h) { return *detail::nb_type_info(h.ptr()); }
template <typename T>
inline T &type_supplement(handle h) { return *(T *) detail::nb_type_supplement(h.ptr()); }
inline str type_name(handle h) { return steal<str>(detail::nb_type_name(h.ptr())); };

// Low level access to nanobind instance objects
inline bool inst_check(handle h) { return type_check(h.type()); }
inline str inst_name(handle h) {
    return steal<str>(detail::nb_inst_name(h.ptr()));
};
inline object inst_alloc(handle h) {
    return steal(detail::nb_inst_alloc((PyTypeObject *) h.ptr()));
}
inline object inst_alloc_zero(handle h) {
    return steal(detail::nb_inst_alloc_zero((PyTypeObject *) h.ptr()));
}
inline object inst_take_ownership(handle h, void *p) {
    return steal(detail::nb_inst_take_ownership((PyTypeObject *) h.ptr(), p));
}
inline object inst_reference(handle h, void *p, handle parent = handle()) {
    return steal(detail::nb_inst_reference((PyTypeObject *) h.ptr(), p, parent.ptr()));
}
inline void inst_zero(handle h) { detail::nb_inst_zero(h.ptr()); }
inline void inst_set_state(handle h, bool ready, bool destruct) {
    detail::nb_inst_set_state(h.ptr(), ready, destruct);
}
inline std::pair<bool, bool> inst_state(handle h) {
    return detail::nb_inst_state(h.ptr());
}
inline void inst_mark_ready(handle h) { inst_set_state(h, true, true); }
inline bool inst_ready(handle h) { return inst_state(h).first; }
inline void inst_destruct(handle h) { detail::nb_inst_destruct(h.ptr()); }
inline void inst_copy(handle dst, handle src) { detail::nb_inst_copy(dst.ptr(), src.ptr()); }
inline void inst_move(handle dst, handle src) { detail::nb_inst_move(dst.ptr(), src.ptr()); }
inline void inst_replace_copy(handle dst, handle src) { detail::nb_inst_replace_copy(dst.ptr(), src.ptr()); }
inline void inst_replace_move(handle dst, handle src) { detail::nb_inst_replace_move(dst.ptr(), src.ptr()); }
template <typename T> T *inst_ptr(handle h) { return (T *) detail::nb_inst_ptr(h.ptr()); }
inline void *type_get_slot(handle h, int slot_id) {
#if NB_TYPE_GET_SLOT_IMPL
    return detail::type_get_slot((PyTypeObject *) h.ptr(), slot_id);
#else
    return PyType_GetSlot((PyTypeObject *) h.ptr(), slot_id);
#endif
}


template <typename... Args> struct init {
    template <typename T, typename... Ts> friend class class_;
    NB_INLINE init() {}

private:
    template <typename Class, typename... Extra>
    NB_INLINE static void execute(Class &cl, const Extra&... extra) {
        using Type = typename Class::Type;
        using Alias = typename Class::Alias;
        cl.def(
            "__init__",
            [](pointer_and_handle<Type> v, Args... args) {
                if constexpr (!std::is_same_v<Type, Alias> &&
                              std::is_constructible_v<Type, Args...>) {
                    if (!detail::nb_inst_python_derived(v.h.ptr())) {
                        new (v.p) Type{ (detail::forward_t<Args>) args... };
                        return;
                    }
                }
                new ((void *) v.p) Alias{ (detail::forward_t<Args>) args... };
            },
            extra...);
    }
};

template <typename Arg> struct init_implicit {
    template <typename T, typename... Ts> friend class class_;
    NB_INLINE init_implicit() { }

private:
    template <typename Class, typename... Extra>
    NB_INLINE static void execute(Class &cl, const Extra&... extra) {
        using Type = typename Class::Type;
        using Alias = typename Class::Alias;

        cl.def(
            "__init__",
            [](pointer_and_handle<Type> v, Arg arg) {
                if constexpr (!std::is_same_v<Type, Alias> &&
                              std::is_constructible_v<Type, Arg>) {
                    if (!detail::nb_inst_python_derived(v.h.ptr())) {
                        new ((Type *) v.p) Type{ (detail::forward_t<Arg>) arg };
                        return;
                    }
                }
                new ((Alias *) v.p) Alias{ (detail::forward_t<Arg>) arg };
            }, is_implicit(), extra...);

        using Caster = detail::make_caster<Arg>;

        if constexpr (!detail::is_class_caster_v<Caster>) {
            detail::implicitly_convertible(
                [](PyTypeObject *, PyObject *src,
                   detail::cleanup_list *cleanup) noexcept -> bool {
                    return Caster().from_python(
                        src, detail::cast_flags::convert, cleanup);
                },
                &typeid(Type));
        }
    }
};

namespace detail {
    // This is 'inline' so we can define it in a header and not pay
    // for it if unused, and also 'noinline' so we don't generate
    // multiple copies and produce code bloat.
    NB_NOINLINE inline void wrap_base_new(handle cls, bool do_wrap) {
        if (PyCFunction_Check(cls.attr("__new__").ptr())) {
            if (do_wrap) {
                cpp_function_def(
                    [](handle type) {
                        if (!type_check(type))
                            detail::raise_cast_error();
                        return inst_alloc(type);
                    },
                    scope(cls), name("__new__"));
            }
        } else {
            if (!do_wrap) {
                // We already defined the wrapper, so this zero-arg overload
                // would be unreachable. Raise an error rather than hiding it.
                raise("nanobind: %s must define its zero-argument __new__ "
                      "before any other overloads", type_name(cls).c_str());
            }
        }
    }
}

template <typename Func, typename Sig = detail::function_signature_t<Func>>
struct new_;

template <typename Func, typename Return, typename... Args>
struct new_<Func, Return(Args...)> {
    std::remove_reference_t<Func> func;

    new_(Func &&f) : func((detail::forward_t<Func>) f) {}

    template <typename Class, typename... Extra>
    NB_INLINE void execute(Class &cl, const Extra&... extra) {
        // If this is the first __new__ overload we're defining, then wrap
        // nanobind's built-in __new__ so we overload with it instead of
        // replacing it; this is important for pickle support.
        // We can't do this if the user-provided __new__ takes no
        // arguments, because it would make an ambiguous overload set.
        detail::wrap_base_new(cl, sizeof...(Args) != 0);
        cl.def_static(
            "__new__",
            [func = (detail::forward_t<Func>) func](handle, Args... args) {
                return func((detail::forward_t<Args>) args...);
            },
            extra...);
        cl.def("__init__", [](handle, Args...) {}, extra...);
    }
};
template <typename Func> new_(Func&& f) -> new_<Func>;

template <typename T> struct for_setter {
    T value;
    for_setter(const T &value) : value(value) { }
};

template <typename T> struct for_getter {
    T value;
    for_getter(const T &value) : value(value) { }
};

template <typename T> for_getter(T) -> for_getter<std::decay_t<T>>;
template <typename T> for_setter(T) -> for_setter<std::decay_t<T>>;

namespace detail {
    template <typename T> auto filter_getter(const T &v) { return v; }
    template <typename T> auto filter_getter(const for_getter<T> &v) { return v.value; }
    template <typename T> std::nullptr_t filter_getter(const for_setter<T> &) { return nullptr; }

    template <typename T> auto filter_setter(const T &v) { return v; }
    template <typename T> auto filter_setter(const for_setter<T> &v) { return v.value; }
    template <typename T> std::nullptr_t filter_setter(const for_getter<T> &) { return nullptr; }
}

template <typename T, typename... Ts>
class class_ : public object {
public:
    NB_OBJECT_DEFAULT(class_, object, "type", PyType_Check);
    using Type = T;
    using Base  = typename detail::extract<T, detail::is_base,  Ts...>::type;
    using Alias = typename detail::extract<T, detail::is_alias, Ts...>::type;

    static_assert(sizeof(Alias) < (1 << 24), "Instance size is too big!");
    static_assert(alignof(Alias) < (1 << 8), "Instance alignment is too big!");
    static_assert(
        sizeof...(Ts) == !std::is_same_v<Base, T> + !std::is_same_v<Alias, T>,
        "nanobind::class_<> was invoked with extra arguments that could not be handled");

    static_assert(
        detail::is_base_caster_v<detail::make_caster<Type>>,
        "You attempted to bind a type that is already intercepted by a type "
        "caster. Having both at the same time is not allowed. Are you perhaps "
        "binding an STL type, while at the same time including a matching "
        "type caster from <nanobind/stl/*>? Or did you perhaps forget to "
        "declare NB_MAKE_OPAQUE(..) to specifically disable the type caster "
        "catch-all for a specific type? Please review the documentation "
        "to learn about the difference between bindings and type casters.");

    template <typename... Extra>
    NB_INLINE class_(handle scope, const char *name, const Extra &... extra) {
        detail::type_init_data d;

        d.flags = 0;
        d.align = (uint8_t) alignof(Alias);
        d.size = (uint32_t) sizeof(Alias);
        d.name = name;
        d.scope = scope.ptr();
        d.type = &typeid(T);

        if constexpr (!std::is_same_v<Base, T>) {
            d.base = &typeid(Base);
            d.flags |= (uint32_t) detail::type_init_flags::has_base;
        }

        if constexpr (detail::is_copy_constructible_v<T>) {
            d.flags |= (uint32_t) detail::type_flags::is_copy_constructible;

            if constexpr (!std::is_trivially_copy_constructible_v<T>) {
                d.flags |= (uint32_t) detail::type_flags::has_copy;
                d.copy = detail::wrap_copy<T>;
            }
        }

        if constexpr (std::is_move_constructible<T>::value) {
            d.flags |= (uint32_t) detail::type_flags::is_move_constructible;

            if constexpr (!std::is_trivially_move_constructible_v<T>) {
                d.flags |= (uint32_t) detail::type_flags::has_move;
                d.move = detail::wrap_move<T>;
            }
        }

        if constexpr (std::is_destructible_v<T>) {
            d.flags |= (uint32_t) detail::type_flags::is_destructible;

            if constexpr (!std::is_trivially_destructible_v<T>) {
                d.flags |= (uint32_t) detail::type_flags::has_destruct;
                d.destruct = detail::wrap_destruct<T>;
            }
        }

        if constexpr (detail::has_shared_from_this_v<T>) {
            d.flags |= (uint32_t) detail::type_flags::has_shared_from_this;
            d.keep_shared_from_this_alive = [](PyObject *self) noexcept {
                // weak_from_this().lock() is equivalent to shared_from_this(),
                // except that it returns an empty shared_ptr instead of
                // throwing an exception if there is no active shared_ptr
                // for this object. (Added in C++17.)
                if (auto sp = inst_ptr<T>(self)->weak_from_this().lock()) {
                    detail::keep_alive(self, new auto(std::move(sp)),
                                       [](void *p) noexcept {
                                           delete (decltype(sp) *) p;
                                       });
                    return true;
                }
                return false;
            };
        }

        (detail::type_extra_apply(d, extra), ...);

        m_ptr = detail::nb_type_new(&d);
    }

    template <typename Func, typename... Extra>
    NB_INLINE class_ &def(const char *name_, Func &&f, const Extra &... extra) {
        cpp_function_def<T>((detail::forward_t<Func>) f, scope(*this),
                            name(name_), is_method(), extra...);
        return *this;
    }

    template <typename... Args, typename... Extra>
    NB_INLINE class_ &def(init<Args...> &&arg, const Extra &... extra) {
        arg.execute(*this, extra...);
        return *this;
    }

    template <typename Arg, typename... Extra>
    NB_INLINE class_ &def(init_implicit<Arg> &&arg, const Extra &... extra) {
        arg.execute(*this, extra...);
        return *this;
    }

    template <typename Func, typename... Extra>
    NB_INLINE class_ &def(new_<Func> &&arg, const Extra &... extra) {
        arg.execute(*this, extra...);
        return *this;
    }

    template <typename Func, typename... Extra>
    NB_INLINE class_ &def_static(const char *name_, Func &&f,
                                 const Extra &... extra) {
        static_assert(
            !std::is_member_function_pointer_v<Func>,
            "def_static(...) called with a non-static member function pointer");
        cpp_function_def((detail::forward_t<Func>) f, scope(*this), name(name_),
                         extra...);
        return *this;
    }

    template <typename Getter, typename Setter, typename... Extra>
    NB_INLINE class_ &def_prop_rw(const char *name_, Getter &&getter,
                                  Setter &&setter, const Extra &...extra) {
        object get_p, set_p;

        if constexpr (!std::is_same_v<Getter, std::nullptr_t>)
            get_p = cpp_function<T>((detail::forward_t<Getter>) getter,
                                    is_method(), is_getter(),
                                    rv_policy::reference_internal,
                                    detail::filter_getter(extra)...);

        if constexpr (!std::is_same_v<Setter, std::nullptr_t>)
            set_p = cpp_function<T>((detail::forward_t<Setter>) setter,
                                    is_method(), detail::filter_setter(extra)...);

        detail::property_install(m_ptr, name_, get_p.ptr(), set_p.ptr());
        return *this;
    }

    template <typename Getter, typename Setter, typename... Extra>
    NB_INLINE class_ &def_prop_rw_static(const char *name_, Getter &&getter,
                                         Setter &&setter,
                                         const Extra &...extra) {
        object get_p, set_p;

        if constexpr (!std::is_same_v<Getter, std::nullptr_t>)
            get_p = cpp_function((detail::forward_t<Getter>) getter, is_getter(),
                                 rv_policy::reference,
                                 detail::filter_getter(extra)...);

        if constexpr (!std::is_same_v<Setter, std::nullptr_t>)
            set_p = cpp_function((detail::forward_t<Setter>) setter,
                                 detail::filter_setter(extra)...);

        detail::property_install_static(m_ptr, name_, get_p.ptr(), set_p.ptr());
        return *this;
    }

    template <typename Getter, typename... Extra>
    NB_INLINE class_ &def_prop_ro(const char *name_, Getter &&getter,
                                  const Extra &...extra) {
        return def_prop_rw(name_, getter, nullptr, extra...);
    }

    template <typename Getter, typename... Extra>
    NB_INLINE class_ &def_prop_ro_static(const char *name_,
                                         Getter &&getter,
                                         const Extra &...extra) {
        return def_prop_rw_static(name_, getter, nullptr, extra...);
    }

    template <typename C, typename D, typename... Extra>
    NB_INLINE class_ &def_rw(const char *name, D C::*p,
                             const Extra &...extra) {
        // Unions never satisfy is_base_of, thus the is_same alternative
        static_assert(std::is_base_of_v<C, T> || std::is_same_v<C, T>,
                      "def_rw() requires a (base) class member!");

        using Q =
            std::conditional_t<detail::is_base_caster_v<detail::make_caster<D>>,
                               const D &, D &&>;

        def_prop_rw(name,
            [p](const T &c) -> const D & { return c.*p; },
            [p](T &c, Q value) { c.*p = (Q) value; },
            extra...);

        return *this;
    }

    template <typename D, typename... Extra>
    NB_INLINE class_ &def_rw_static(const char *name, D *p,
                                    const Extra &...extra) {
        using Q =
            std::conditional_t<detail::is_base_caster_v<detail::make_caster<D>>,
                               const D &, D &&>;

        def_prop_rw_static(name,
            [p](handle) -> const D & { return *p; },
            [p](handle, Q value) { *p = (Q) value; }, extra...);

        return *this;
    }

    template <typename C, typename D, typename... Extra>
    NB_INLINE class_ &def_ro(const char *name, D C::*p,
                             const Extra &...extra) {
        // Unions never satisfy is_base_of, thus the is_same alternative
        static_assert(std::is_base_of_v<C, T> || std::is_same_v<C, T>,
                      "def_ro() requires a (base) class member!");

        def_prop_ro(name,
            [p](const T &c) -> const D & { return c.*p; }, extra...);

        return *this;
    }

    template <typename D, typename... Extra>
    NB_INLINE class_ &def_ro_static(const char *name, D *p,
                                    const Extra &...extra) {
        def_prop_ro_static(name,
            [p](handle) -> const D & { return *p; }, extra...);

        return *this;
    }

    template <detail::op_id id, detail::op_type ot, typename L, typename R, typename... Extra>
    class_ &def(const detail::op_<id, ot, L, R> &op, const Extra&... extra) {
        op.execute(*this, extra...);
        return *this;
    }

    template <detail::op_id id, detail::op_type ot, typename L, typename R, typename... Extra>
    class_ & def_cast(const detail::op_<id, ot, L, R> &op, const Extra&... extra) {
        op.execute_cast(*this, extra...);
        return *this;
    }
};

template <typename T> class enum_ : public object {
public:
    static_assert(std::is_enum_v<T>, "nanobind::enum_<> requires an enumeration type!");

    using Base = class_<T>;
    using Underlying = std::underlying_type_t<T>;

    template <typename... Extra>
    NB_INLINE enum_(handle scope, const char *name, const Extra &... extra) {
        detail::enum_init_data ed { };
        ed.type = &typeid(T);
        ed.scope = scope.ptr();
        ed.name = name;
        ed.flags = std::is_signed_v<Underlying>
                       ? (uint32_t) detail::type_flags::is_signed
                       : 0;
        (detail::enum_extra_apply(ed, extra), ...);
        m_ptr = detail::enum_create(&ed);
    }

    NB_INLINE enum_ &value(const char *name, T value, const char *doc = nullptr) {
        detail::enum_append(m_ptr, name, (int64_t) value, doc);
        return *this;
    }

    NB_INLINE enum_ &export_values() { detail::enum_export(m_ptr); return *this; }


    template <typename Func, typename... Extra>
    NB_INLINE enum_ &def(const char *name_, Func &&f, const Extra &... extra) {
        cpp_function_def<T>((detail::forward_t<Func>) f, scope(*this),
                            name(name_), is_method(), extra...);
        return *this;
    }

    template <typename Func, typename... Extra>
    NB_INLINE enum_ &def_static(const char *name_, Func &&f,
                                 const Extra &... extra) {
        static_assert(
            !std::is_member_function_pointer_v<Func>,
            "def_static(...) called with a non-static member function pointer");
        cpp_function_def((detail::forward_t<Func>) f, scope(*this), name(name_),
                         extra...);
        return *this;
    }

    template <typename Getter, typename Setter, typename... Extra>
    NB_INLINE enum_ &def_prop_rw(const char *name_, Getter &&getter,
                                 Setter &&setter, const Extra &...extra) {
        object get_p, set_p;

        if constexpr (!std::is_same_v<Getter, std::nullptr_t>)
            get_p = cpp_function<T>((detail::forward_t<Getter>) getter,
                                    is_method(), is_getter(),
                                    rv_policy::reference_internal,
                                    detail::filter_getter(extra)...);

        if constexpr (!std::is_same_v<Setter, std::nullptr_t>)
            set_p = cpp_function<T>((detail::forward_t<Setter>) setter,
                                    is_method(), detail::filter_setter(extra)...);

        detail::property_install(m_ptr, name_, get_p.ptr(), set_p.ptr());
        return *this;
    }


    template <typename Getter, typename... Extra>
    NB_INLINE enum_ &def_prop_ro(const char *name_, Getter &&getter,
                                 const Extra &...extra) {
        return def_prop_rw(name_, getter, nullptr, extra...);
    }
};

template <typename Source, typename Target> void implicitly_convertible() {
    using Caster = detail::make_caster<Source>;

    if constexpr (detail::is_base_caster_v<Caster>) {
        detail::implicitly_convertible(&typeid(Source), &typeid(Target));
    } else {
        detail::implicitly_convertible(
            [](PyTypeObject *, PyObject *src,
               detail::cleanup_list *cleanup) noexcept -> bool {
                return Caster().from_python(src, detail::cast_flags::convert,
                                            cleanup);
            },
            &typeid(Target));
    }
}

NAMESPACE_END(NB_NAMESPACE)
