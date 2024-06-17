/*
    nanobind/nb_attr.h: Annotations for function and class declarations

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

NAMESPACE_BEGIN(NB_NAMESPACE)

struct scope {
    PyObject *value;
    NB_INLINE scope(handle value) : value(value.ptr()) {}
};

struct name {
    const char *value;
    NB_INLINE name(const char *value) : value(value) {}
};

struct arg_v;
struct arg {
    NB_INLINE constexpr explicit arg(const char *name = nullptr) : name_(name), signature_(nullptr) { }
    template <typename T> NB_INLINE arg_v operator=(T &&value) const;
    NB_INLINE arg &noconvert(bool value = true) {
        convert_ = !value;
        return *this;
    }
    NB_INLINE arg &none(bool value = true) {
        none_ = value;
        return *this;
    }

    NB_INLINE arg &sig(const char *value) {
        signature_ = value;
        return *this;
    }

    const char *name_, *signature_;
    uint8_t convert_{ true };
    bool none_{ false };
};

struct arg_v : arg {
    object value;
    NB_INLINE arg_v(const arg &base, object &&value)
        : arg(base), value(std::move(value)) {}
};

template <typename... Ts> struct call_guard {
    using type = detail::tuple<Ts...>;
};

struct dynamic_attr {};
struct is_weak_referenceable {};
struct is_method {};
struct is_implicit {};
struct is_operator {};
struct is_arithmetic {};
struct is_final {};
struct is_generic {};
struct kw_only {};

template <size_t /* Nurse */, size_t /* Patient */> struct keep_alive {};
template <typename T> struct supplement {};
template <typename T> struct intrusive_ptr {
    intrusive_ptr(void (*set_self_py)(T *, PyObject *) noexcept)
        : set_self_py(set_self_py) { }
    void (*set_self_py)(T *, PyObject *) noexcept;
};

struct type_slots {
    type_slots (const PyType_Slot *value) : value(value) { }
    const PyType_Slot *value;
};

struct type_slots_callback {
    using cb_t = void (*)(const detail::type_init_data *t,
                          PyType_Slot *&slots, size_t max_slots) noexcept;
    type_slots_callback(cb_t callback) : callback(callback) { }
    cb_t callback;
};

struct sig {
    const char *value;
    sig(const char *value) : value(value) { }
};

struct is_getter { };

NAMESPACE_BEGIN(literals)
constexpr arg operator"" _a(const char *name, size_t) { return arg(name); }
NAMESPACE_END(literals)

NAMESPACE_BEGIN(detail)

enum class func_flags : uint32_t {
    /* Low 3 bits reserved for return value policy */

    /// Did the user specify a name for this function, or is it anonymous?
    has_name = (1 << 4),
    /// Did the user specify a scope in which this function should be installed?
    has_scope = (1 << 5),
    /// Did the user specify a docstring?
    has_doc = (1 << 6),
    /// Did the user specify nb::arg/arg_v annotations for all arguments?
    has_args = (1 << 7),
    /// Does the function signature contain an *args-style argument?
    has_var_args = (1 << 8),
    /// Does the function signature contain an *kwargs-style argument?
    has_var_kwargs = (1 << 9),
    /// Is this function a method of a class?
    is_method = (1 << 10),
    /// Is this function a method called __init__? (automatically generated)
    is_constructor = (1 << 11),
    /// Can this constructor be used to perform an implicit conversion?
    is_implicit = (1 << 12),
    /// Is this function an arithmetic operator?
    is_operator = (1 << 13),
    /// When the function is GCed, do we need to call func_data_prelim::free_capture?
    has_free = (1 << 14),
    /// Should the func_new() call return a new reference?
    return_ref = (1 << 15),
    /// Does this overload specify a custom function signature (for docstrings, typing)
    has_signature = (1 << 16),
    /// Does this function have one or more nb::keep_alive() annotations?
    has_keep_alive = (1 << 17)
};

struct arg_data {
    const char *name;
    const char *signature;
    PyObject *name_py;
    PyObject *value;
    bool convert;
    bool none;
};

template <size_t Size> struct func_data_prelim {
    // A small amount of space to capture data used by the function/closure
    void *capture[3];

    // Callback to clean up the 'capture' field
    void (*free_capture)(void *);

    /// Implementation of the function call
    PyObject *(*impl)(void *, PyObject **, uint8_t *, rv_policy,
                      cleanup_list *);

    /// Function signature description
    const char *descr;

    /// C++ types referenced by 'descr'
    const std::type_info **descr_types;

    /// Supplementary flags
    uint32_t flags;

    /// Total number of parameters accepted by the C++ function; nb::args
    /// and nb::kwargs parameters are counted as one each. If the
    /// 'has_args' flag is set, then there is one arg_data structure
    /// for each of these.
    uint16_t nargs;

    /// Number of paramters to the C++ function that may be filled from
    /// Python positional arguments without additional ceremony. nb::args and
    /// nb::kwargs parameters are not counted in this total, nor are any
    /// parameters after nb::args or after a nb::kw_only annotation.
    /// The parameters counted here may be either named (nb::arg("name"))
    /// or unnamed (nb::arg()). If unnamed, they are effectively positional-only.
    /// nargs_pos is always <= nargs.
    uint16_t nargs_pos;

    // ------- Extra fields -------

    const char *name;
    const char *doc;
    PyObject *scope;

    // *WARNING*: nanobind regularly receives requests from users who run it
    // through Clang-Tidy, or who compile with increased warnings levels, like
    //
    //   -Wpedantic, -Wcast-qual, -Wsign-conversion, etc.
    //
    // (i.e., beyond -Wall -Wextra and /W4 that are currently already used)
    //
    // Their next step is to open a big pull request needed to silence all of
    // the resulting messages. This comment is strategically placed here
    // because the zero-length array construction below will almost certainly
    // be flagged in this process.
    //
    // My policy on this is as follows: I am always happy to fix issues in the
    // codebase. However, many of the resulting change requests are in the
    // "ritual purification" category: things that cause churn, decrease
    // readability, and which don't fix actual problems. It's a never-ending
    // cycle because each new revision of such tooling adds further warnings
    // and purification rites.
    //
    // So just to be clear: I do not wish to pepper this codebase with
    // "const_cast" and #pragmas/comments to avoid warnings in external
    // tooling just so those users can have a "silent" build. I don't think it
    // is reasonable for them to impose their own style on this project.
    //
    // As a workaround it is likely possible to restrict the scope of style
    // checks to particular C++ namespaces or source code locations.
#if defined(_MSC_VER)
    // MSVC doesn't support zero-length arrays
    arg_data args[Size == 0 ? 1 : Size];
#else
    // GCC and Clang do.
    arg_data args[Size];
#endif
};

template <typename F>
NB_INLINE void func_extra_apply(F &f, const name &name, size_t &) {
    f.name = name.value;
    f.flags |= (uint32_t) func_flags::has_name;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, const scope &scope, size_t &) {
    f.scope = scope.value;
    f.flags |= (uint32_t) func_flags::has_scope;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, const sig &s, size_t &) {
    f.flags |= (uint32_t) func_flags::has_signature;
    f.name = s.value;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, const char *doc, size_t &) {
    f.doc = doc;
    f.flags |= (uint32_t) func_flags::has_doc;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, is_method, size_t &) {
    f.flags |= (uint32_t) func_flags::is_method;
}

template <typename F>
NB_INLINE void func_extra_apply(F &, is_getter, size_t &) { }

template <typename F>
NB_INLINE void func_extra_apply(F &f, is_implicit, size_t &) {
    f.flags |= (uint32_t) func_flags::is_implicit;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, is_operator, size_t &) {
    f.flags |= (uint32_t) func_flags::is_operator;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, rv_policy pol, size_t &) {
    f.flags = (f.flags & ~0b111) | (uint16_t) pol;
}

template <typename F>
NB_INLINE void func_extra_apply(F &, std::nullptr_t, size_t &) { }

template <typename F>
NB_INLINE void func_extra_apply(F &f, const arg &a, size_t &index) {
    arg_data &arg = f.args[index++];
    arg.name = a.name_;
    arg.signature = a.signature_;
    arg.value = nullptr;
    arg.convert = a.convert_;
    arg.none = a.none_;
}

template <typename F>
NB_INLINE void func_extra_apply(F &f, const arg_v &a, size_t &index) {
    arg_data &arg = f.args[index++];
    arg.name = a.name_;
    arg.signature = a.signature_;
    arg.value = a.value.ptr();
    arg.convert = a.convert_;
    arg.none = a.none_;
}

template <typename F>
NB_INLINE void func_extra_apply(F &, kw_only, size_t &) {}

template <typename F, typename... Ts>
NB_INLINE void func_extra_apply(F &, call_guard<Ts...>, size_t &) {}

template <typename F, size_t Nurse, size_t Patient>
NB_INLINE void func_extra_apply(F &f, nanobind::keep_alive<Nurse, Patient>, size_t &) {
    f.flags |= (uint32_t) func_flags::has_keep_alive;
}

template <typename... Ts> struct func_extra_info {
    using call_guard = void;
    static constexpr bool keep_alive = false;
};

template <typename T, typename... Ts> struct func_extra_info<T, Ts...>
    : func_extra_info<Ts...> { };

template <typename... Cs, typename... Ts>
struct func_extra_info<nanobind::call_guard<Cs...>, Ts...> : func_extra_info<Ts...> {
    static_assert(std::is_same_v<typename func_extra_info<Ts...>::call_guard, void>,
                  "call_guard<> can only be specified once!");
    using call_guard = nanobind::call_guard<Cs...>;
};

template <size_t Nurse, size_t Patient, typename... Ts>
struct func_extra_info<nanobind::keep_alive<Nurse, Patient>, Ts...> : func_extra_info<Ts...> {
    static constexpr bool keep_alive = true;
};

template <typename T>
NB_INLINE void process_keep_alive(PyObject **, PyObject *, T *) { }

template <size_t Nurse, size_t Patient>
NB_INLINE void
process_keep_alive(PyObject **args, PyObject *result,
                   nanobind::keep_alive<Nurse, Patient> *) {
    keep_alive(Nurse   == 0 ? result : args[Nurse - 1],
               Patient == 0 ? result : args[Patient - 1]);
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
