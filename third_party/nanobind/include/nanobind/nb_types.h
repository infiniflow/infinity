/*
    nanobind/nb_types.h: nb::dict/str/list/..: C++ wrappers for Python types

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

NAMESPACE_BEGIN(NB_NAMESPACE)

/// Macro defining functions/constructors for nanobind::handle subclasses
#define NB_OBJECT(Type, Parent, Str, Check)                                    \
public:                                                                        \
    static constexpr auto Name = ::nanobind::detail::const_name(Str);          \
    NB_INLINE Type(handle h, ::nanobind::detail::borrow_t)                     \
        : Parent(h, ::nanobind::detail::borrow_t{}) { }                        \
    NB_INLINE Type(handle h, ::nanobind::detail::steal_t)                      \
        : Parent(h, ::nanobind::detail::steal_t{}) { }                         \
    NB_INLINE static bool check_(handle h) {                                   \
        return Check(h.ptr());                                                 \
    }

/// Like NB_OBJECT but allow null-initialization
#define NB_OBJECT_DEFAULT(Type, Parent, Str, Check)                            \
    NB_OBJECT(Type, Parent, Str, Check)                                        \
    NB_INLINE Type() : Parent() {}

/// Helper macro to create detail::api comparison functions
#define NB_DECL_COMP(name)                                                     \
    template <typename T2> NB_INLINE bool name(const api<T2> &o) const;

#define NB_IMPL_COMP(name, op)                                                 \
    template <typename T1> template <typename T2>                              \
    NB_INLINE bool api<T1>::name(const api<T2> &o) const {                     \
        return detail::obj_comp(derived().ptr(), o.derived().ptr(), op);       \
    }

/// Helper macros to create detail::api unary operators
#define NB_DECL_OP_1(name)                                                     \
    NB_INLINE object name() const;

#define NB_IMPL_OP_1(name, op)                                                 \
    template <typename T> NB_INLINE object api<T>::name() const {              \
        return steal(detail::obj_op_1(derived().ptr(), op));                   \
    }

/// Helper macros to create detail::api binary operators
#define NB_DECL_OP_2(name)                                                     \
    template <typename T2> NB_INLINE object name(const api<T2> &o) const;

#define NB_IMPL_OP_2(name, op)                                                 \
    template <typename T1> template <typename T2>                              \
    NB_INLINE object api<T1>::name(const api<T2> &o) const {                   \
        return steal(                                                          \
            detail::obj_op_2(derived().ptr(), o.derived().ptr(), op));         \
    }

#define NB_DECL_OP_2_I(name)                                                   \
    template <typename T2> NB_INLINE object name(const api<T2> &o);

#define NB_IMPL_OP_2_I(name, op)                                               \
    template <typename T1> template <typename T2>                              \
    NB_INLINE object api<T1>::name(const api<T2> &o) {                         \
        return steal(                                                          \
            detail::obj_op_2(derived().ptr(), o.derived().ptr(), op));         \
    }

#define NB_IMPL_OP_2_IO(name)                                                  \
    template <typename T> NB_INLINE decltype(auto) name(const api<T> &o) {     \
        return operator=(handle::name(o));                                     \
    }

// A few forward declarations
class object;
class handle;
class iterator;

template <typename T = object> NB_INLINE T borrow(handle h);
template <typename T = object> NB_INLINE T steal(handle h);

NAMESPACE_BEGIN(detail)

template <typename T, typename SFINAE = int> struct type_caster;
template <typename T> using make_caster = type_caster<intrinsic_t<T>>;

template <typename Impl> class accessor;
struct str_attr; struct obj_attr;
struct str_item; struct obj_item; struct num_item;
struct num_item_list; struct num_item_tuple;
class args_proxy; class kwargs_proxy;
struct borrow_t { };
struct steal_t { };
struct api_tag {
    constexpr static bool nb_typed = false;
};
class dict_iterator;
struct fast_iterator;

// Standard operations provided by every nanobind object
template <typename Derived> class api : public api_tag {
public:
    Derived &derived() { return static_cast<Derived &>(*this); }
    const Derived &derived() const { return static_cast<const Derived &>(*this); }

    NB_INLINE bool is(handle value) const;
    NB_INLINE bool is_none() const { return derived().ptr() == Py_None; }
    NB_INLINE bool is_type() const { return PyType_Check(derived().ptr()); }
    NB_INLINE bool is_valid() const { return derived().ptr() != nullptr; }
    NB_INLINE handle inc_ref() const &;
    NB_INLINE handle dec_ref() const &;
    iterator begin() const;
    iterator end() const;

    NB_INLINE handle type() const;
    NB_INLINE operator handle() const;

    accessor<obj_attr> attr(handle key) const;
    accessor<str_attr> attr(const char *key) const;
    accessor<str_attr> doc() const;

    accessor<obj_item> operator[](handle key) const;
    accessor<str_item> operator[](const char *key) const;
    template <typename T, enable_if_t<std::is_arithmetic_v<T>> = 1>
    accessor<num_item> operator[](T key) const;
    args_proxy operator*() const;

    template <rv_policy policy = rv_policy::automatic_reference,
              typename... Args>
    object operator()(Args &&...args) const;

    NB_DECL_COMP(equal)
    NB_DECL_COMP(not_equal)
    NB_DECL_COMP(operator<)
    NB_DECL_COMP(operator<=)
    NB_DECL_COMP(operator>)
    NB_DECL_COMP(operator>=)
    NB_DECL_OP_1(operator-)
    NB_DECL_OP_1(operator~)
    NB_DECL_OP_2(operator+)
    NB_DECL_OP_2(operator-)
    NB_DECL_OP_2(operator*)
    NB_DECL_OP_2(operator/)
    NB_DECL_OP_2(operator%)
    NB_DECL_OP_2(operator|)
    NB_DECL_OP_2(operator&)
    NB_DECL_OP_2(operator^)
    NB_DECL_OP_2(operator<<)
    NB_DECL_OP_2(operator>>)
    NB_DECL_OP_2(floor_div)
    NB_DECL_OP_2_I(operator+=)
    NB_DECL_OP_2_I(operator-=)
    NB_DECL_OP_2_I(operator*=)
    NB_DECL_OP_2_I(operator/=)
    NB_DECL_OP_2_I(operator%=)
    NB_DECL_OP_2_I(operator|=)
    NB_DECL_OP_2_I(operator&=)
    NB_DECL_OP_2_I(operator^=)
    NB_DECL_OP_2_I(operator<<=)
    NB_DECL_OP_2_I(operator>>=)
};

NAMESPACE_END(detail)

class handle : public detail::api<handle> {
    friend class python_error;
    friend struct detail::str_attr;
    friend struct detail::obj_attr;
    friend struct detail::str_item;
    friend struct detail::obj_item;
    friend struct detail::num_item;
public:
    static constexpr auto Name = detail::const_name("object");

    handle() = default;
    handle(const handle &) = default;
    handle(handle &&) noexcept = default;
    handle &operator=(const handle &) = default;
    handle &operator=(handle &&) noexcept = default;
    NB_INLINE handle(std::nullptr_t, detail::steal_t) : m_ptr(nullptr) { }
    NB_INLINE handle(std::nullptr_t) : m_ptr(nullptr) { }
    NB_INLINE handle(const PyObject *ptr) : m_ptr((PyObject *) ptr) { }
    NB_INLINE handle(const PyTypeObject *ptr) : m_ptr((PyObject *) ptr) { }

    const handle& inc_ref() const & noexcept {
#if defined(NDEBUG) && !defined(Py_LIMITED_API)
        Py_XINCREF(m_ptr);
#else
        detail::incref_checked(m_ptr);
#endif
        return *this;
    }

    const handle& dec_ref() const & noexcept {
#if defined(NDEBUG) && !defined(Py_LIMITED_API)
        Py_XDECREF(m_ptr);
#else
        detail::decref_checked(m_ptr);
#endif
        return *this;
    }

    NB_INLINE explicit operator bool() const { return m_ptr != nullptr; }
    NB_INLINE PyObject *ptr() const { return m_ptr; }
    NB_INLINE static bool check_(handle) { return true; }

protected:
    PyObject *m_ptr = nullptr;
};

class object : public handle {
public:
    static constexpr auto Name = detail::const_name("object");

    object() = default;
    object(const object &o) : handle(o) { inc_ref(); }
    object(object &&o) noexcept : handle(o) { o.m_ptr = nullptr; }
    ~object() { dec_ref(); }
    object(handle h, detail::borrow_t) : handle(h) { inc_ref(); }
    object(handle h, detail::steal_t) : handle(h) { }

    handle release() {
      handle temp(m_ptr);
      m_ptr = nullptr;
      return temp;
    }

    void reset() {
        dec_ref();
        m_ptr = nullptr;
    }

    object& operator=(const object &o) {
        handle temp(m_ptr);
        o.inc_ref();
        m_ptr = o.m_ptr;
        temp.dec_ref();
        return *this;
    }

    object& operator=(object &&o) noexcept {
        handle temp(m_ptr);
        m_ptr = o.m_ptr;
        o.m_ptr = nullptr;
        temp.dec_ref();
        return *this;
    }

    NB_IMPL_OP_2_IO(operator+=)
    NB_IMPL_OP_2_IO(operator%=)
    NB_IMPL_OP_2_IO(operator-=)
    NB_IMPL_OP_2_IO(operator*=)
    NB_IMPL_OP_2_IO(operator/=)
    NB_IMPL_OP_2_IO(operator|=)
    NB_IMPL_OP_2_IO(operator&=)
    NB_IMPL_OP_2_IO(operator^=)
    NB_IMPL_OP_2_IO(operator<<=)
    NB_IMPL_OP_2_IO(operator>>=)
};

template <typename T> NB_INLINE T borrow(handle h) {
    return { h, detail::borrow_t() };
}

template <typename T = object, typename T2,
          std::enable_if_t<std::is_base_of_v<object, T2> && !std::is_lvalue_reference_v<T2>, int> = 0>
NB_INLINE T borrow(T2 &&o) {
    return { o.release(), detail::steal_t() };
}

template <typename T> NB_INLINE T steal(handle h) {
    return { h, detail::steal_t() };
}

inline bool hasattr(handle h, const char *key) noexcept {
    return PyObject_HasAttrString(h.ptr(), key);
}

inline bool hasattr(handle h, handle key) noexcept {
    return PyObject_HasAttr(h.ptr(), key.ptr());
}

inline object getattr(handle h, const char *key) {
    return steal(detail::getattr(h.ptr(), key));
}

inline object getattr(handle h, handle key) {
    return steal(detail::getattr(h.ptr(), key.ptr()));
}

inline object getattr(handle h, const char *key, handle def) noexcept {
    return steal(detail::getattr(h.ptr(), key, def.ptr()));
}

inline object getattr(handle h, handle key, handle value) noexcept {
    return steal(detail::getattr(h.ptr(), key.ptr(), value.ptr()));
}

inline void setattr(handle h, const char *key, handle value) {
    detail::setattr(h.ptr(), key, value.ptr());
}

inline void setattr(handle h, handle key, handle value) {
    detail::setattr(h.ptr(), key.ptr(), value.ptr());
}

inline void delattr(handle h, const char *key) {
    detail::delattr(h.ptr(), key);
}

inline void delattr(handle h, handle key) {
    detail::delattr(h.ptr(), key.ptr());
}

class module_ : public object {
public:
    NB_OBJECT(module_, object, "types.ModuleType", PyModule_CheckExact);

    template <typename Func, typename... Extra>
    module_ &def(const char *name_, Func &&f, const Extra &...extra);

    static NB_INLINE module_ import_(const char *name) {
        return steal<module_>(detail::module_import(name));
    }

    static NB_INLINE module_ import_(handle name) {
        return steal<module_>(detail::module_import(name.ptr()));
    }

    NB_INLINE module_ def_submodule(const char *name,
                                    const char *doc = nullptr) {
        return steal<module_>(detail::module_new_submodule(m_ptr, name, doc));
    }
};

class capsule : public object {
    NB_OBJECT_DEFAULT(capsule, object, "types.CapsuleType", PyCapsule_CheckExact)

    capsule(const void *ptr, void (*cleanup)(void *) noexcept = nullptr) {
        m_ptr = detail::capsule_new(ptr, nullptr, cleanup);
    }

    capsule(const void *ptr, const char *name,
            void (*cleanup)(void *) noexcept = nullptr) {
        m_ptr = detail::capsule_new(ptr, name, cleanup);
    }

    const char *name() const { return PyCapsule_GetName(m_ptr); }

    void *data() const { return PyCapsule_GetPointer(m_ptr, name()); }
};

class bool_ : public object {
    NB_OBJECT_DEFAULT(bool_, object, "bool", PyBool_Check)

    explicit bool_(handle h)
        : object(detail::bool_from_obj(h.ptr()), detail::borrow_t{}) { }

    explicit bool_(bool value)
        : object(value ? Py_True : Py_False, detail::borrow_t{}) { }

    explicit operator bool() const {
        return m_ptr == Py_True;
    }
};

class int_ : public object {
    NB_OBJECT_DEFAULT(int_, object, "int", PyLong_Check)

    explicit int_(handle h)
        : object(detail::int_from_obj(h.ptr()), detail::steal_t{}) { }

    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 0>
    explicit int_(T value)
        : object(
              detail::type_caster<T>::from_cpp(value, rv_policy::copy, nullptr),
              detail::steal_t{}) {
        if (!m_ptr)
            raise_python_error();
    }

    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 0>
    explicit operator T() const {
        detail::type_caster<T> tc;
        if (!tc.from_python(m_ptr, 0, nullptr))
            throw std::out_of_range("Conversion of nanobind::int_ failed");
        return tc.value;
    }
};

class float_ : public object {
    NB_OBJECT_DEFAULT(float_, object, "float", PyFloat_Check)

    explicit float_(handle h)
        : object(detail::float_from_obj(h.ptr()), detail::steal_t{}) { }

    explicit float_(double value)
        : object(PyFloat_FromDouble(value), detail::steal_t{}) {
        if (!m_ptr)
            raise_python_error();
    }

#if !defined(Py_LIMITED_API)
    explicit operator double() const { return PyFloat_AS_DOUBLE(m_ptr); }
#else
    explicit operator double() const { return PyFloat_AsDouble(m_ptr); }
#endif
};

class str : public object {
    NB_OBJECT_DEFAULT(str, object, "str", PyUnicode_Check)

    explicit str(handle h)
        : object(detail::str_from_obj(h.ptr()), detail::steal_t{}) { }

    explicit str(const char *s)
        : object(detail::str_from_cstr(s), detail::steal_t{}) { }

    explicit str(const char *s, size_t n)
        : object(detail::str_from_cstr_and_size(s, n), detail::steal_t{}) { }

    template <typename... Args> str format(Args&&... args);

    const char *c_str() const { return PyUnicode_AsUTF8AndSize(m_ptr, nullptr); }
};

class bytes : public object {
    NB_OBJECT_DEFAULT(bytes, object, "bytes", PyBytes_Check)

    explicit bytes(handle h)
        : object(detail::bytes_from_obj(h.ptr()), detail::steal_t{}) { }

    explicit bytes(const char *s)
        : object(detail::bytes_from_cstr(s), detail::steal_t{}) { }

    explicit bytes(const void *s, size_t n)
        : object(detail::bytes_from_cstr_and_size(s, n), detail::steal_t{}) { }

    const char *c_str() const { return PyBytes_AsString(m_ptr); }

    const void *data() const { return (const void *) PyBytes_AsString(m_ptr); }

    size_t size() const { return (size_t) PyBytes_Size(m_ptr); }
};

class tuple : public object {
    NB_OBJECT(tuple, object, "tuple", PyTuple_Check)
    tuple() : object(PyTuple_New(0), detail::steal_t()) { }
    explicit tuple(handle h)
        : object(detail::tuple_from_obj(h.ptr()), detail::steal_t{}) { }
    size_t size() const { return (size_t) NB_TUPLE_GET_SIZE(m_ptr); }
    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 1>
    detail::accessor<detail::num_item_tuple> operator[](T key) const;

#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION)
    detail::fast_iterator begin() const;
    detail::fast_iterator end() const;
#endif
};

class type_object : public object {
    NB_OBJECT_DEFAULT(type_object, object, "type", PyType_Check)
};

class list : public object {
    NB_OBJECT(list, object, "list", PyList_Check)
    list() : object(PyList_New(0), detail::steal_t()) { }
    explicit list(handle h)
        : object(detail::list_from_obj(h.ptr()), detail::steal_t{}) { }
    size_t size() const { return (size_t) NB_LIST_GET_SIZE(m_ptr); }

    template <typename T> void append(T &&value);
    template <typename T> void insert(Py_ssize_t index, T &&value);

    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 1>
    detail::accessor<detail::num_item_list> operator[](T key) const;

    void clear() {
        if (PyList_SetSlice(m_ptr, 0, PY_SSIZE_T_MAX, nullptr))
            raise_python_error();
    }

    void extend(handle h) {
        if (PyList_SetSlice(m_ptr, PY_SSIZE_T_MAX, PY_SSIZE_T_MAX, h.ptr()))
            raise_python_error();
    }

    void sort() {
        if (PyList_Sort(m_ptr))
            raise_python_error();
    }

    void reverse() {
        if (PyList_Reverse(m_ptr))
            raise_python_error();
    }

#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION)
    detail::fast_iterator begin() const;
    detail::fast_iterator end() const;
#endif
};

class dict : public object {
    NB_OBJECT(dict, object, "dict", PyDict_Check)
    dict() : object(PyDict_New(), detail::steal_t()) { }
    size_t size() const { return (size_t) NB_DICT_GET_SIZE(m_ptr); }
    detail::dict_iterator begin() const;
    detail::dict_iterator end() const;
    list keys() const { return steal<list>(detail::obj_op_1(m_ptr, PyDict_Keys)); }
    list values() const { return steal<list>(detail::obj_op_1(m_ptr, PyDict_Values)); }
    list items() const { return steal<list>(detail::obj_op_1(m_ptr, PyDict_Items)); }
    template <typename T> bool contains(T&& key) const;
    void clear() { PyDict_Clear(m_ptr); }
    void update(handle h) {
        if (PyDict_Update(m_ptr, h.ptr()))
            raise_python_error();
    }
};

class set : public object {
    NB_OBJECT(set, object, "set", PySet_Check)
    set() : object(PySet_New(nullptr), detail::steal_t()) { }
    explicit set(handle h)
        : object(detail::set_from_obj(h.ptr()), detail::steal_t{}) { }
    size_t size() const { return (size_t) NB_SET_GET_SIZE(m_ptr); }
    template <typename T> bool contains(T&& key) const;
    template <typename T> void add(T &&value);
    void clear() {
        if (PySet_Clear(m_ptr))
            raise_python_error();
    }
    template <typename T> bool discard(T &&value);
};

class sequence : public object {
    NB_OBJECT_DEFAULT(sequence, object, NB_TYPING_SEQUENCE, PySequence_Check)
};

class mapping : public object {
    NB_OBJECT_DEFAULT(mapping, object, NB_TYPING_MAPPING, PyMapping_Check)
    list keys() const { return steal<list>(detail::obj_op_1(m_ptr, PyMapping_Keys)); }
    list values() const { return steal<list>(detail::obj_op_1(m_ptr, PyMapping_Values)); }
    list items() const { return steal<list>(detail::obj_op_1(m_ptr, PyMapping_Items)); }
    template <typename T> bool contains(T&& key) const;
};

class args : public tuple {
    NB_OBJECT_DEFAULT(args, tuple, "tuple", PyTuple_Check)
};

class kwargs : public dict {
    NB_OBJECT_DEFAULT(kwargs, dict, "dict", PyDict_Check)
};

class iterator : public object {
public:
    using difference_type = Py_ssize_t;
    using value_type = handle;
    using reference = const handle;
    using pointer = const handle *;

    NB_OBJECT_DEFAULT(iterator, object, NB_TYPING_ITERATOR, PyIter_Check)

    iterator& operator++() {
        m_value = steal(detail::obj_iter_next(m_ptr));
        return *this;
    }

    iterator operator++(int) {
        iterator rv = *this;
        m_value = steal(detail::obj_iter_next(m_ptr));
        return rv;
    }

    handle operator*() const {
        if (is_valid() && !m_value.is_valid())
            m_value = steal(detail::obj_iter_next(m_ptr));
        return m_value;
    }

    pointer operator->() const { operator*(); return &m_value; }

    static iterator sentinel() { return {}; }

    friend bool operator==(const iterator &a, const iterator &b) { return a->ptr() == b->ptr(); }
    friend bool operator!=(const iterator &a, const iterator &b) { return a->ptr() != b->ptr(); }

private:
    mutable object m_value;
};

class iterable : public object {
public:
    NB_OBJECT_DEFAULT(iterable, object, NB_TYPING_ITERABLE, detail::iterable_check)
};

/// Retrieve the Python type object associated with a C++ class
template <typename T> handle type() noexcept {
    return detail::nb_type_lookup(&typeid(detail::intrinsic_t<T>));
}

template <typename T>
NB_INLINE bool isinstance(handle h) noexcept {
    if constexpr (std::is_base_of_v<handle, T>)
        return T::check_(h);
    else if constexpr (detail::is_base_caster_v<detail::make_caster<T>>)
        return detail::nb_type_isinstance(h.ptr(), &typeid(detail::intrinsic_t<T>));
    else
        return detail::make_caster<T>().from_python(h, 0, nullptr);
}

NB_INLINE bool issubclass(handle h1, handle h2) {
    return detail::issubclass(h1.ptr(), h2.ptr());
}

NB_INLINE str repr(handle h) { return steal<str>(detail::obj_repr(h.ptr())); }
NB_INLINE size_t len(handle h) { return detail::obj_len(h.ptr()); }
NB_INLINE size_t len_hint(handle h) { return detail::obj_len_hint(h.ptr()); }
NB_INLINE size_t len(const tuple &t) { return (size_t) NB_TUPLE_GET_SIZE(t.ptr()); }
NB_INLINE size_t len(const list &l) { return (size_t) NB_LIST_GET_SIZE(l.ptr()); }
NB_INLINE size_t len(const dict &d) { return (size_t) NB_DICT_GET_SIZE(d.ptr()); }
NB_INLINE size_t len(const set &d) { return (size_t) NB_SET_GET_SIZE(d.ptr()); }

inline void print(handle value, handle end = handle(), handle file = handle()) {
    detail::print(value.ptr(), end.ptr(), file.ptr());
}

inline void print(const char *str, handle end = handle(), handle file = handle()) {
    print(nanobind::str(str), end, file);
}

inline object none() { return borrow(Py_None); }
inline dict builtins() { return borrow<dict>(PyEval_GetBuiltins()); }

inline iterator iter(handle h) {
    return steal<iterator>(detail::obj_iter(h.ptr()));
}

class slice : public object {
public:
    NB_OBJECT_DEFAULT(slice, object, "slice", PySlice_Check)
    slice(handle start, handle stop, handle step) {
        m_ptr = PySlice_New(start.ptr(), stop.ptr(), step.ptr());
        if (!m_ptr)
            raise_python_error();
    }

    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 0>
    explicit slice(T stop) : slice(Py_None, int_(stop), Py_None) {}
    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 0>
    slice(T start, T stop) : slice(int_(start), int_(stop), Py_None) {}
    template <typename T, detail::enable_if_t<std::is_arithmetic_v<T>> = 0>
    slice(T start, T stop, T step) : slice(int_(start), int_(stop), int_(step)) {}

    detail::tuple<Py_ssize_t, Py_ssize_t, Py_ssize_t, size_t> compute(size_t size) const {
        Py_ssize_t start, stop, step;
        size_t slice_length;
        detail::slice_compute(m_ptr, (Py_ssize_t) size, start, stop, step, slice_length);
        return detail::tuple(start, stop, step, slice_length);
    }
};

class ellipsis : public object {
    static bool is_ellipsis(PyObject *obj) { return obj == Py_Ellipsis; }

public:
    NB_OBJECT(ellipsis, object, "types.EllipsisType", is_ellipsis)
    ellipsis() : object(Py_Ellipsis, detail::borrow_t()) {}
};

class not_implemented : public object {
    static bool is_not_implemented(PyObject *obj) { return obj == Py_NotImplemented; }

public:
    NB_OBJECT(not_implemented, object, "types.NotImplementedType", is_not_implemented)
    not_implemented() : object(Py_NotImplemented, detail::borrow_t()) {}
};

class callable : public object {
public:
    NB_OBJECT(callable, object, NB_TYPING_CALLABLE, PyCallable_Check)
    using object::object;
};

class weakref : public object {
public:
    NB_OBJECT(weakref, object, "weakref.ReferenceType", PyWeakref_Check)

    explicit weakref(handle obj, handle callback = {})
        : object(PyWeakref_NewRef(obj.ptr(), callback.ptr()), detail::steal_t{}) {
        if (!m_ptr)
            raise_python_error();
    }
};

class any : public object {
public:
    using object::object;
    using object::operator=;
    static constexpr auto Name = detail::const_name("typing.Any");
};

template <typename T> class handle_t : public handle {
public:
    static constexpr auto Name = detail::make_caster<T>::Name;

    using handle::handle;
    using handle::operator=;
    handle_t(const handle &h) : handle(h) { }

    static bool check_(handle h) { return isinstance<T>(h); }
};

template <typename T> class type_object_t : public type_object {
public:
    static constexpr auto Name = detail::const_name(NB_TYPING_TYPE "[") +
                                 detail::make_caster<T>::Name +
                                 detail::const_name("]");

    using type_object::type_object;
    using type_object::operator=;

    static bool check_(handle h) {
        return PyType_Check(h.ptr()) &&
               PyType_IsSubtype((PyTypeObject *) h.ptr(),
                                (PyTypeObject *) nanobind::type<T>().ptr());
    }
};

template <typename T, typename...> class typed : public T {
public:
    constexpr static bool nb_typed = true;
    using T::T;
    using T::operator=;
};

template <typename T> struct pointer_and_handle {
    T *p;
    handle h;
};

NAMESPACE_BEGIN(detail)
template <typename Derived> NB_INLINE api<Derived>::operator handle() const {
    return derived().ptr();
}

template <typename Derived> NB_INLINE handle api<Derived>::type() const {
    return (PyObject *) Py_TYPE(derived().ptr());
}

template <typename Derived> NB_INLINE handle api<Derived>::inc_ref() const & {
    return operator handle().inc_ref();
}

template <typename Derived> NB_INLINE handle api<Derived>::dec_ref() const & {
    return operator handle().dec_ref();
}

template <typename Derived>
NB_INLINE bool api<Derived>::is(handle value) const {
    return derived().ptr() == value.ptr();
}

template <typename Derived> iterator api<Derived>::begin() const {
    return iter(*this);
}

template <typename Derived> iterator api<Derived>::end() const {
    return iterator::sentinel();
}

struct fast_iterator {
    using value_type = handle;
    using reference = const value_type;
    using difference_type = std::ptrdiff_t;

    fast_iterator() = default;
    fast_iterator(PyObject **value) : value(value) { }

    fast_iterator& operator++() { value++; return *this; }
    fast_iterator operator++(int) { fast_iterator rv = *this; value++; return rv; }
    friend bool operator==(const fast_iterator &a, const fast_iterator &b) { return a.value == b.value; }
    friend bool operator!=(const fast_iterator &a, const fast_iterator &b) { return a.value != b.value; }

    handle operator*() const { return *value; }

    PyObject **value;
};

class dict_iterator {
public:
    using value_type = std::pair<handle, handle>;
    using reference = const value_type;

    dict_iterator() : h(), pos(-1) { }

    dict_iterator(handle h) : h(h), pos(0) {
        increment();
    }

    dict_iterator& operator++() {
        increment();
        return *this;
    }

    dict_iterator operator++(int) {
        dict_iterator rv = *this;
        increment();
        return rv;
    }

    void increment() {
        if (PyDict_Next(h.ptr(), &pos, &key, &value) == 0)
            pos = -1;
    }

    value_type operator*() const { return { key, value }; }

    friend bool operator==(const dict_iterator &a, const dict_iterator &b) { return a.pos == b.pos; }
    friend bool operator!=(const dict_iterator &a, const dict_iterator &b) { return a.pos != b.pos; }

private:
    handle h;
    Py_ssize_t pos;
    PyObject *key = nullptr, *value = nullptr;
};

NB_IMPL_COMP(equal,      Py_EQ)
NB_IMPL_COMP(not_equal,  Py_NE)
NB_IMPL_COMP(operator<,  Py_LT)
NB_IMPL_COMP(operator<=, Py_LE)
NB_IMPL_COMP(operator>,  Py_GT)
NB_IMPL_COMP(operator>=, Py_GE)
NB_IMPL_OP_1(operator-,  PyNumber_Negative)
NB_IMPL_OP_1(operator~,  PyNumber_Invert)
NB_IMPL_OP_2(operator+,  PyNumber_Add)
NB_IMPL_OP_2(operator-,  PyNumber_Subtract)
NB_IMPL_OP_2(operator*,  PyNumber_Multiply)
NB_IMPL_OP_2(operator/,  PyNumber_TrueDivide)
NB_IMPL_OP_2(operator%,  PyNumber_Remainder)
NB_IMPL_OP_2(operator|,  PyNumber_Or)
NB_IMPL_OP_2(operator&,  PyNumber_And)
NB_IMPL_OP_2(operator^,  PyNumber_Xor)
NB_IMPL_OP_2(operator<<, PyNumber_Lshift)
NB_IMPL_OP_2(operator>>, PyNumber_Rshift)
NB_IMPL_OP_2(floor_div,  PyNumber_FloorDivide)
NB_IMPL_OP_2_I(operator+=, PyNumber_InPlaceAdd)
NB_IMPL_OP_2_I(operator%=, PyNumber_InPlaceRemainder)
NB_IMPL_OP_2_I(operator-=, PyNumber_InPlaceSubtract)
NB_IMPL_OP_2_I(operator*=, PyNumber_InPlaceMultiply)
NB_IMPL_OP_2_I(operator/=, PyNumber_InPlaceTrueDivide)
NB_IMPL_OP_2_I(operator|=, PyNumber_InPlaceOr)
NB_IMPL_OP_2_I(operator&=, PyNumber_InPlaceAnd)
NB_IMPL_OP_2_I(operator^=, PyNumber_InPlaceXor)
NB_IMPL_OP_2_I(operator<<=,PyNumber_InPlaceLshift)
NB_IMPL_OP_2_I(operator>>=,PyNumber_InPlaceRshift)

#undef NB_DECL_COMP
#undef NB_IMPL_COMP
#undef NB_DECL_OP_1
#undef NB_IMPL_OP_1
#undef NB_DECL_OP_2
#undef NB_IMPL_OP_2
#undef NB_DECL_OP_2_I
#undef NB_IMPL_OP_2_I
#undef NB_IMPL_OP_2_IO

NAMESPACE_END(detail)

inline detail::dict_iterator dict::begin() const { return { *this }; }
inline detail::dict_iterator dict::end() const { return { }; }

#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION)
inline detail::fast_iterator tuple::begin() const {
    return ((PyTupleObject *) m_ptr)->ob_item;
}
inline detail::fast_iterator tuple::end() const {
    PyTupleObject *v = (PyTupleObject *) m_ptr;
    return v->ob_item + v->ob_base.ob_size;
}
inline detail::fast_iterator list::begin() const {
    return ((PyListObject *) m_ptr)->ob_item;
}
inline detail::fast_iterator list::end() const {
    PyListObject *v = (PyListObject *) m_ptr;
    return v->ob_item + v->ob_base.ob_size;
}
#endif

template <typename T> void del(detail::accessor<T> &a) { a.del(); }
template <typename T> void del(detail::accessor<T> &&a) { a.del(); }

NAMESPACE_END(NB_NAMESPACE)
