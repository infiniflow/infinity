/*
    src/common.cpp: miscellaneous libnanobind functionality

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <nanobind/nanobind.h>
#include "nb_internals.h"

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

NB_NOINLINE static builtin_exception
create_exception(exception_type type, const char *fmt, va_list args_) {
    char buf[512];
    va_list args;

    va_copy(args, args_);
    int size = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (size < (int) sizeof(buf)) {
        return builtin_exception(type, buf);
    } else {
        scoped_pymalloc<char> temp(size + 1);

        va_copy(args, args_);
        vsnprintf(temp.get(), size + 1, fmt, args);
        va_end(args);

        return builtin_exception(type, temp.get());
    }
}

#if defined(__GNUC__)
    __attribute__((noreturn, __format__ (__printf__, 1, 2)))
#else
    [[noreturn]]
#endif
void raise(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    builtin_exception err =
        create_exception(exception_type::runtime_error, fmt, args);
    va_end(args);
    throw err;
}

#if defined(__GNUC__)
    __attribute__((noreturn, __format__ (__printf__, 1, 2)))
#else
    [[noreturn]]
#endif
void raise_type_error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    builtin_exception err =
        create_exception(exception_type::type_error, fmt, args);
    va_end(args);
    throw err;
}

/// Abort the process with a fatal error
#if defined(__GNUC__)
    __attribute__((noreturn, __format__ (__printf__, 1, 2)))
#else
    [[noreturn]]
#endif
void fail(const char *fmt, ...) noexcept {
    va_list args;
    fprintf(stderr, "Critical nanobind error: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    abort();
}

PyObject *capsule_new(const void *ptr, const char *name,
        void (*cleanup)(void *) noexcept) noexcept {
    auto capsule_cleanup = [](PyObject *o) {
        auto cleanup_2 = (void (*)(void *))(PyCapsule_GetContext(o));
        if (cleanup_2)
            cleanup_2(PyCapsule_GetPointer(o, PyCapsule_GetName(o)));
    };

    PyObject *c = PyCapsule_New((void *) ptr, name, capsule_cleanup);

    check(c, "nanobind::detail::capsule_new(): allocation failed!");

    int rv = PyCapsule_SetContext(c, (void *) cleanup);
    check(rv == 0, "nanobind::detail::capsule_new(): could not set context!");

    return c;
}

void raise_python_error() {
    check(PyErr_Occurred(),
          "nanobind::detail::raise_python_error() called without "
          "an error condition!");
    throw python_error();
}

void raise_next_overload_if_null(void *p) {
    if (NB_UNLIKELY(!p))
        throw next_overload();
}

void raise_cast_error() {
    throw cast_error();
}

// ========================================================================

void cleanup_list::release() noexcept {
    /* Don't decrease the reference count of the first
       element, it stores the 'self' element. */
    for (size_t i = 1; i < m_size; ++i)
        Py_DECREF(m_data[i]);
    if (m_capacity != Small)
        free(m_data);
    m_data = nullptr;
}

void cleanup_list::expand() noexcept {
    uint32_t new_capacity = m_capacity * 2;
    PyObject **new_data = (PyObject **) malloc(new_capacity * sizeof(PyObject *));
    check(new_data, "nanobind::detail::cleanup_list::expand(): out of memory!");
    memcpy(new_data, m_data, m_size * sizeof(PyObject *));
    if (m_capacity != Small)
        free(m_data);
    m_data = new_data;
    m_capacity = new_capacity;
}

// ========================================================================

PyObject *module_new(const char *name, PyModuleDef *def) noexcept {
    memset(def, 0, sizeof(PyModuleDef));
    def->m_name = name;
    def->m_size = -1;
    PyObject *m = PyModule_Create(def);
    check(m, "nanobind::detail::module_new(): allocation failed!");
    return m;
}

PyObject *module_import(const char *name) {
    PyObject *res = PyImport_ImportModule(name);
    if (!res)
        throw python_error();
    return res;
}

PyObject *module_import(PyObject *o) {
    PyObject *res = PyImport_Import(o);
    if (!res)
        throw python_error();
    return res;
}

PyObject *module_new_submodule(PyObject *base, const char *name,
                               const char *doc) noexcept {
    PyObject *name_py, *res;

#if !defined(PYPY_VERSION)
    PyObject *base_name = PyModule_GetNameObject(base);
    if (!base_name)
        goto fail;

    name_py = PyUnicode_FromFormat("%U.%s", base_name, name);
    Py_DECREF(base_name);
#else
    const char *base_name = PyModule_GetName(base);
    if (!base_name)
        goto fail;

    name_py = PyUnicode_FromFormat("%s.%s", base_name, name);
#endif
    if (!name_py)
        goto fail;

#if !defined(PYPY_VERSION)
    res = PyImport_AddModuleObject(name_py);
#else
    res = PyImport_AddModule(PyUnicode_AsUTF8(name_py));
#endif
    Py_DECREF(name_py);
    if (!res)
        goto fail;

    if (doc) {
        PyObject *doc_py = PyUnicode_FromString(doc);
        if (!doc_py)
            goto fail;
        int rv = PyObject_SetAttrString(res, "__doc__", doc_py);
        Py_DECREF(doc_py);
        if (rv)
            goto fail;
    }

    Py_INCREF(res); // extra reference for PyModule_AddObject

    if (PyModule_AddObject(base, name, res)) { // steals on success
        Py_DECREF(res);
        goto fail;
    }

    Py_INCREF(res); // turned borrowed into new reference
    return res;

fail:
    raise_python_error();
}

// ========================================================================

size_t obj_len(PyObject *o) {
    Py_ssize_t res = PyObject_Size(o);
    if (res < 0)
        raise_python_error();
    return (size_t) res;
}

size_t obj_len_hint(PyObject *o) noexcept {
#if !defined(Py_LIMITED_API)
    Py_ssize_t res = PyObject_LengthHint(o, 0);
    if (res < 0) {
        PyErr_Clear();
        res = 0;
    }
    return (size_t) res;
#else
    PyTypeObject *tp = Py_TYPE(o);
    lenfunc l = (lenfunc) type_get_slot(tp, Py_sq_length);
    if (!l)
        l = (lenfunc) type_get_slot(tp, Py_mp_length);

    if (l) {
        Py_ssize_t res = l(o);
        if (res < 0) {
            PyErr_Clear();
            res = 0;
        }
        return (size_t) res;
    }

    try {
        return cast<size_t>(handle(o).attr("__length_hint__")());
    } catch (...) {
        return 0;
    }
#endif
}

PyObject *obj_repr(PyObject *o) {
    PyObject *res = PyObject_Repr(o);
    if (!res)
        raise_python_error();
    return res;
}

bool obj_comp(PyObject *a, PyObject *b, int value) {
    int rv = PyObject_RichCompareBool(a, b, value);
    if (rv == -1)
        raise_python_error();
    return rv == 1;
}

PyObject *obj_op_1(PyObject *a, PyObject* (*op)(PyObject*)) {
    PyObject *res = op(a);
    if (!res)
        raise_python_error();
    return res;
}

PyObject *obj_op_2(PyObject *a, PyObject *b,
                   PyObject *(*op)(PyObject *, PyObject *) ) {
    PyObject *res = op(a, b);
    if (!res)
        raise_python_error();

    return res;
}

PyObject *obj_vectorcall(PyObject *base, PyObject *const *args, size_t nargsf,
                         PyObject *kwnames, bool method_call) {
    PyObject *res = nullptr;
    bool gil_error = false, cast_error = false;

    size_t nargs_total = (size_t) (NB_VECTORCALL_NARGS(nargsf) +
                         (kwnames ? NB_TUPLE_GET_SIZE(kwnames) : 0));

#if !defined(Py_LIMITED_API)
    if (!PyGILState_Check()) {
        gil_error = true;
        goto end;
    }
#endif

    for (size_t i = 0; i < nargs_total; ++i) {
        if (!args[i]) {
            cast_error = true;
            goto end;
        }
    }

#if PY_VERSION_HEX < 0x03090000
    if (method_call) {
        PyObject *self = PyObject_GetAttr(args[0], /* name = */ base);
        if (self) {
            res = _PyObject_Vectorcall(self, (PyObject **) args + 1, nargsf - 1, kwnames);
            Py_DECREF(self);
        }
    } else {
        res = _PyObject_Vectorcall(base, (PyObject **) args, nargsf, kwnames);
    }
#else
    res = (method_call ? PyObject_VectorcallMethod
                       : PyObject_Vectorcall)(base, args, nargsf, kwnames);
#endif

end:
    for (size_t i = 0; i < nargs_total; ++i)
        Py_XDECREF(args[i]);
    Py_XDECREF(kwnames);
    Py_DECREF(base);

    if (!res) {
        if (cast_error)
            raise_cast_error();
        else if (gil_error)
            raise("nanobind::detail::obj_vectorcall(): PyGILState_Check() failure.");
        else
            raise_python_error();
    }

    return res;
}


PyObject *obj_iter(PyObject *o) {
    PyObject *result = PyObject_GetIter(o);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *obj_iter_next(PyObject *o) {
    PyObject *result = PyIter_Next(o);
    if (!result && PyErr_Occurred())
        raise_python_error();
    return result;
}


// ========================================================================

PyObject *getattr(PyObject *obj, const char *key) {
    PyObject *res = PyObject_GetAttrString(obj, key);
    if (!res)
        raise_python_error();
    return res;
}

PyObject *getattr(PyObject *obj, PyObject *key) {
    PyObject *res = PyObject_GetAttr(obj, key);
    if (!res)
        raise_python_error();
    return res;
}

PyObject *getattr(PyObject *obj, const char *key, PyObject *def) noexcept {
    PyObject *res = PyObject_GetAttrString(obj, key);
    if (res)
        return res;
    PyErr_Clear();
    Py_XINCREF(def);
    return def;
}

PyObject *getattr(PyObject *obj, PyObject *key, PyObject *def) noexcept {
    PyObject *res = PyObject_GetAttr(obj, key);
    if (res)
        return res;
    PyErr_Clear();
    Py_XINCREF(def);
    return def;
}

void getattr_or_raise(PyObject *obj, const char *key, PyObject **out) {
    if (*out)
        return;

    PyObject *res = PyObject_GetAttrString(obj, key);
    if (!res)
        raise_python_error();

    *out = res;
}

void getattr_or_raise(PyObject *obj, PyObject *key, PyObject **out) {
    if (*out)
        return;

    PyObject *res = PyObject_GetAttr(obj, key);
    if (!res)
        raise_python_error();

    *out = res;
}

void setattr(PyObject *obj, const char *key, PyObject *value) {
    int rv = PyObject_SetAttrString(obj, key, value);
    if (rv)
        raise_python_error();
}

void setattr(PyObject *obj, PyObject *key, PyObject *value) {
    int rv = PyObject_SetAttr(obj, key, value);
    if (rv)
        raise_python_error();
}

void delattr(PyObject *obj, const char *key) {
    int rv = PyObject_DelAttrString(obj, key);
    if (rv)
        raise_python_error();
}

void delattr(PyObject *obj, PyObject *key) {
    int rv = PyObject_DelAttr(obj, key);
    if (rv)
        raise_python_error();
}

// ========================================================================

void getitem_or_raise(PyObject *obj, Py_ssize_t key, PyObject **out) {
    if (*out)
        return;

    PyObject *res = PySequence_GetItem(obj, key);
    if (!res)
        raise_python_error();

    *out = res;
}

void getitem_or_raise(PyObject *obj, const char *key_, PyObject **out) {
    if (*out)
        return;

    PyObject *key, *res;

    key = PyUnicode_FromString(key_);
    if (!key)
        raise_python_error();

    res = PyObject_GetItem(obj, key);
    Py_DECREF(key);

    if (!res)
        raise_python_error();

    *out = res;
}

void getitem_or_raise(PyObject *obj, PyObject *key, PyObject **out) {
    if (*out)
        return;

    PyObject *res = PyObject_GetItem(obj, key);
    if (!res)
        raise_python_error();

    *out = res;
}

void setitem(PyObject *obj, Py_ssize_t key, PyObject *value) {
    int rv = PySequence_SetItem(obj, key, value);
    if (rv)
        raise_python_error();
}

void setitem(PyObject *obj, const char *key_, PyObject *value) {
    PyObject *key = PyUnicode_FromString(key_);
    if (!key)
        raise_python_error();

    int rv = PyObject_SetItem(obj, key, value);
    Py_DECREF(key);

    if (rv)
        raise_python_error();
}

void setitem(PyObject *obj, PyObject *key, PyObject *value) {
    int rv = PyObject_SetItem(obj, key, value);
    if (rv)
        raise_python_error();
}

void delitem(PyObject *obj, Py_ssize_t key_) {
    PyObject *key = PyLong_FromSsize_t(key_);
    if (!key)
        raise_python_error();

    int rv = PyObject_DelItem(obj, key);
    Py_DECREF(key);

    if (rv)
        raise_python_error();
}

void delitem(PyObject *obj, const char *key_) {
    PyObject *key = PyUnicode_FromString(key_);
    if (!key)
        raise_python_error();

    int rv = PyObject_DelItem(obj, key);
    Py_DECREF(key);

    if (rv)
        raise_python_error();
}

void delitem(PyObject *obj, PyObject *key) {
    int rv = PyObject_DelItem(obj, key);
    if (rv)
        raise_python_error();
}

// ========================================================================

PyObject *str_from_obj(PyObject *o) {
    PyObject *result = PyObject_Str(o);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *str_from_cstr(const char *str) {
    PyObject *result = PyUnicode_FromString(str);
    if (!result)
        raise("nanobind::detail::str_from_cstr(): conversion error!");
    return result;
}

PyObject *str_from_cstr_and_size(const char *str, size_t size) {
    PyObject *result = PyUnicode_FromStringAndSize(str, (Py_ssize_t) size);
    if (!result)
        raise("nanobind::detail::str_from_cstr_and_size(): conversion error!");
    return result;
}

// ========================================================================

PyObject *bytes_from_obj(PyObject *o) {
    PyObject *result = PyBytes_FromObject(o);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *bytes_from_cstr(const char *str) {
    PyObject *result = PyBytes_FromString(str);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *bytes_from_cstr_and_size(const void *str, size_t size) {
    PyObject *result = PyBytes_FromStringAndSize((const char *) str, (Py_ssize_t) size);
    if (!result)
        raise_python_error();
    return result;
}

// ========================================================================

PyObject *bool_from_obj(PyObject *o) {
    int rv = PyObject_IsTrue(o);
    if (rv == -1)
        raise_python_error();
    return rv == 1 ? Py_True : Py_False;
}

PyObject *int_from_obj(PyObject *o) {
    PyObject *result = PyNumber_Long(o);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *float_from_obj(PyObject *o) {
    PyObject *result = PyNumber_Float(o);
    if (!result)
        raise_python_error();
    return result;
}

// ========================================================================

PyObject *tuple_from_obj(PyObject *o) {
    PyObject *result = PySequence_Tuple(o);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *list_from_obj(PyObject *o) {
    PyObject *result = PySequence_List(o);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *set_from_obj(PyObject *o) {
    PyObject *result = PySet_New(o);
    if (!result)
        raise_python_error();
    return result;
}

// ========================================================================

PyObject **seq_get(PyObject *seq, size_t *size_out, PyObject **temp_out) noexcept {
    PyObject *temp = nullptr;
    size_t size = 0;
    PyObject **result = nullptr;

    /* This function is used during overload resolution; if anything
       goes wrong, it fails gracefully without reporting errors. Other
       overloads will then be tried. */

    if (PyUnicode_CheckExact(seq) || PyBytes_CheckExact(seq)) {
        *size_out = 0;
        *temp_out = nullptr;
        return nullptr;
    }

#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION)
    if (PyTuple_CheckExact(seq)) {
        size = (size_t) PyTuple_GET_SIZE(seq);
        result = ((PyTupleObject *) seq)->ob_item;
        /* Special case for zero-sized lists/tuples. CPython
           sets ob_item to NULL, which this function incidentally uses to
           signal an error. Return a nonzero pointer that will, however,
           still trigger a segfault if dereferenced. */
        if (size == 0)
            result = (PyObject **) 1;
    } else if (PyList_CheckExact(seq)) {
        size = (size_t) PyList_GET_SIZE(seq);
        result = ((PyListObject *) seq)->ob_item;
        if (size == 0) // ditto
            result = (PyObject **) 1;
    } else if (PySequence_Check(seq)) {
        temp = PySequence_Fast(seq, "");

        if (temp)
            result = seq_get(temp, &size, temp_out);
        else
            PyErr_Clear();
    }
#else
    /* There isn't a nice way to get a PyObject** in Py_LIMITED_API. This
       is going to be slow, but hopefully also very future-proof.. */
    if (PySequence_Check(seq)) {
        Py_ssize_t size_seq = PySequence_Length(seq);

        if (size_seq >= 0) {
            result = (PyObject **) PyObject_Malloc(sizeof(PyObject *) *
                                                   (size_seq + 1));

            if (result) {
                result[size_seq] = nullptr;

                for (Py_ssize_t i = 0; i < size_seq; ++i) {
                    PyObject *o = PySequence_GetItem(seq, i);

                    if (o) {
                        result[i] = o;
                    } else {
                        for (Py_ssize_t j = 0; j < i; ++j)
                            Py_DECREF(result[j]);

                        PyObject_Free(result);
                        result = nullptr;
                        break;
                    }
                }
            }

            if (result) {
                temp = PyCapsule_New(result, nullptr, [](PyObject *o) {
                    PyObject **ptr = (PyObject **) PyCapsule_GetPointer(o, nullptr);
                    for (size_t i = 0; ptr[i] != nullptr; ++i)
                        Py_DECREF(ptr[i]);
                    PyObject_Free(ptr);
                });

                if (temp) {
                    size = (size_t) size_seq;
                } else if (!temp) {
                    PyErr_Clear();
                    for (Py_ssize_t i = 0; i < size_seq; ++i)
                        Py_DECREF(result[i]);

                    PyObject_Free(result);
                    result = nullptr;
                }
            }
        } else if (size_seq < 0) {
            PyErr_Clear();
        }
    }
#endif

    *temp_out = temp;
    *size_out = size;
    return result;
}


PyObject **seq_get_with_size(PyObject *seq, size_t size,
                             PyObject **temp_out) noexcept {

    /* This function is used during overload resolution; if anything
       goes wrong, it fails gracefully without reporting errors. Other
       overloads will then be tried. */

    PyObject *temp = nullptr,
             **result = nullptr;

#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION)
    if (PyTuple_CheckExact(seq)) {
        if (size == (size_t) PyTuple_GET_SIZE(seq)) {
            result = ((PyTupleObject *) seq)->ob_item;
            /* Special case for zero-sized lists/tuples. CPython
               sets ob_item to NULL, which this function incidentally uses to
               signal an error. Return a nonzero pointer that will, however,
               still trigger a segfault if dereferenced. */
            if (size == 0)
                result = (PyObject **) 1;
        }
    } else if (PyList_CheckExact(seq)) {
        if (size == (size_t) PyList_GET_SIZE(seq)) {
            result = ((PyListObject *) seq)->ob_item;
            if (size == 0) // ditto
                result = (PyObject **) 1;
        }
    } else if (PySequence_Check(seq)) {
        temp = PySequence_Fast(seq, "");

        if (temp)
            result = seq_get_with_size(temp, size, temp_out);
        else
            PyErr_Clear();
    }
#else
    /* There isn't a nice way to get a PyObject** in Py_LIMITED_API. This
       is going to be slow, but hopefully also very future-proof.. */
    if (PySequence_Check(seq)) {
        Py_ssize_t size_seq = PySequence_Length(seq);

        if (size == (size_t) size_seq) {
            result =
                (PyObject **) PyObject_Malloc(sizeof(PyObject *) * (size + 1));

            if (result) {
                result[size] = nullptr;

                for (Py_ssize_t i = 0; i < size_seq; ++i) {
                    PyObject *o = PySequence_GetItem(seq, i);

                    if (o) {
                        result[i] = o;
                    } else {
                        for (Py_ssize_t j = 0; j < i; ++j)
                            Py_DECREF(result[j]);

                        PyObject_Free(result);
                        result = nullptr;
                        break;
                    }
                }
            }

            if (result) {
                temp = PyCapsule_New(result, nullptr, [](PyObject *o) {
                    PyObject **ptr = (PyObject **) PyCapsule_GetPointer(o, nullptr);
                    for (size_t i = 0; ptr[i] != nullptr; ++i)
                        Py_DECREF(ptr[i]);
                    PyObject_Free(ptr);
                });

                if (!temp) {
                    PyErr_Clear();
                    for (Py_ssize_t i = 0; i < size_seq; ++i)
                        Py_DECREF(result[i]);

                    PyObject_Free(result);
                    result = nullptr;
                }
            }
        } else if (size_seq < 0) {
            PyErr_Clear();
        }
    }
#endif

    *temp_out = temp;
    return result;
}

// ========================================================================

static void property_install_impl(PyTypeObject *tp, PyObject *scope,
                                  const char *name, PyObject *getter,
                                  PyObject *setter) {
    PyObject *m = getter ? getter : setter;
    object doc = none();

    if (m && (Py_TYPE(m) == internals->nb_func ||
              Py_TYPE(m) == internals->nb_method)) {
        func_data *f = nb_func_data(m);
        if (f->flags & (uint32_t) func_flags::has_doc)
            doc = str(f->doc);
    }

    handle(scope).attr(name) = handle(tp)(
        getter ? handle(getter) : handle(Py_None),
        setter ? handle(setter) : handle(Py_None),
        handle(Py_None), // deleter
        doc
    );
}

void property_install(PyObject *scope, const char *name, PyObject *getter,
                      PyObject *setter) noexcept {
    property_install_impl(&PyProperty_Type, scope, name, getter, setter);
}

void property_install_static(PyObject *scope, const char *name,
                             PyObject *getter, PyObject *setter) noexcept {
    property_install_impl(nb_static_property_tp(), scope, name, getter,
                          setter);
}

// ========================================================================

void tuple_check(PyObject *tuple, size_t nargs) {
    for (size_t i = 0; i < nargs; ++i) {
        if (!NB_TUPLE_GET_ITEM(tuple, i))
            raise_cast_error();
    }
}

// ========================================================================

void print(PyObject *value, PyObject *end, PyObject *file) {
    if (!file)
        file = PySys_GetObject("stdout");

    int rv = PyFile_WriteObject(value, file, Py_PRINT_RAW);
    if (rv)
        raise_python_error();

    if (end)
        rv = PyFile_WriteObject(end, file, Py_PRINT_RAW);
    else
        rv = PyFile_WriteString("\n", file);

    if (rv)
        raise_python_error();
}

// ========================================================================

bool load_f64(PyObject *o, uint8_t flags, double *out) noexcept {
    bool is_float = PyFloat_CheckExact(o);

#if !defined(Py_LIMITED_API)
    if (NB_LIKELY(is_float)) {
        *out = (double) PyFloat_AS_DOUBLE(o);
        return true;
    }

    is_float = false;
#endif

    if (is_float || (flags & (uint8_t) cast_flags::convert)) {
        double result = PyFloat_AsDouble(o);

        if (result != -1.0 || !PyErr_Occurred()) {
            *out = (double) result;
            return true;
        } else {
            PyErr_Clear();
        }
    }

    return false;
}

bool load_f32(PyObject *o, uint8_t flags, float *out) noexcept {
    bool is_float = PyFloat_CheckExact(o);

#if !defined(Py_LIMITED_API)
    if (NB_LIKELY(is_float)) {
        *out = (float) PyFloat_AS_DOUBLE(o);
        return true;
    }

    is_float = false;
#endif

    if (is_float || (flags & (uint8_t) cast_flags::convert)) {
        double result = PyFloat_AsDouble(o);

        if (result != -1.0 || !PyErr_Occurred()) {
            *out = (float) result;
            return true;
        } else {
            PyErr_Clear();
        }
    }

    return false;
}

#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION) && PY_VERSION_HEX < 0x030c0000
// Direct access for compact integers. These functions are
// available as part of Python starting with version 3.12b1+

NB_INLINE bool PyUnstable_Long_IsCompact(const PyLongObject *o) {
    return abs(Py_SIZE(o)) <= 1;
}

NB_INLINE Py_ssize_t PyUnstable_Long_CompactValue(const PyLongObject *o) {
    return Py_SIZE(o) * (Py_ssize_t) o->ob_digit[0];
}
#endif

template <typename T, bool Recurse = true>
NB_INLINE bool load_int(PyObject *o, uint32_t flags, T *out) noexcept {
    if (NB_LIKELY(PyLong_CheckExact(o))) {
#if !defined(Py_LIMITED_API) && !defined(PYPY_VERSION)
        PyLongObject *l = (PyLongObject *) o;

        // Fast path for compact integers
        if (NB_LIKELY(PyUnstable_Long_IsCompact(l))) {
            Py_ssize_t value = PyUnstable_Long_CompactValue(l);
            T value_t = (T) value;

            if (NB_UNLIKELY((std::is_unsigned_v<T> && value < 0) ||
                            (sizeof(T) != sizeof(Py_ssize_t) &&
                             value != (Py_ssize_t) value_t)))
                return false;

            *out = value_t;
            return true;
        }
#endif

        // Slow path
        using T0 = std::conditional_t<sizeof(T) <= sizeof(long), long, long long>;
        using Tp = std::conditional_t<std::is_signed_v<T>, T0, std::make_unsigned_t<T0>>;

        Tp value_p;
        if constexpr (std::is_unsigned_v<Tp>)
            value_p = sizeof(T) <= sizeof(long) ? (Tp) PyLong_AsUnsignedLong(o)
                                                : (Tp) PyLong_AsUnsignedLongLong(o);
        else
            value_p = sizeof(T) <= sizeof(long) ? (Tp) PyLong_AsLong(o)
                                                : (Tp) PyLong_AsLongLong(o);

        if (value_p == Tp(-1) && PyErr_Occurred()) {
            PyErr_Clear();
            return false;
        }

        T value = (T) value_p;
        if constexpr (sizeof(Tp) != sizeof(T)) {
            if (value_p != (Tp) value)
                return false;
        }

        *out = value;
        return true;
    }

    if constexpr (Recurse) {
        if ((flags & (uint8_t) cast_flags::convert) && !PyFloat_Check(o)) {
            PyObject* temp = PyNumber_Long(o);
            if (temp) {
                bool result = load_int<T, false>(temp, 0, out);
                Py_DECREF(temp);
                return result;
            } else {
                PyErr_Clear();
            }
        }
    }

    return false;
}

bool load_u8(PyObject *o, uint8_t flags, uint8_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_i8(PyObject *o, uint8_t flags, int8_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_u16(PyObject *o, uint8_t flags, uint16_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_i16(PyObject *o, uint8_t flags, int16_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_u32(PyObject *o, uint8_t flags, uint32_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_i32(PyObject *o, uint8_t flags, int32_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_u64(PyObject *o, uint8_t flags, uint64_t *out) noexcept {
    return load_int(o, flags, out);
}

bool load_i64(PyObject *o, uint8_t flags, int64_t *out) noexcept {
    return load_int(o, flags, out);
}

// ========================================================================

void incref_checked(PyObject *o) noexcept {
    if (!o)
        return;
#if !defined(Py_LIMITED_API)
    if (!PyGILState_Check())
        fail("nanobind::detail::incref_check(): attempted to change the "
             "reference count of a Python object while the GIL was not held.");
#endif
    Py_INCREF(o);
}

void decref_checked(PyObject *o) noexcept {
    if (!o)
        return;
#if !defined(Py_LIMITED_API)
    if (!PyGILState_Check())
        fail("nanobind::detail::decref_check(): attempted to change the "
             "reference count of a Python object while the GIL was not held.");
#endif
    Py_DECREF(o);
}

// ========================================================================

void set_leak_warnings(bool value) noexcept {
    internals->print_leak_warnings = value;
}

void set_implicit_cast_warnings(bool value) noexcept {
    internals->print_implicit_cast_warnings = value;
}

// ========================================================================

void slice_compute(PyObject *slice, Py_ssize_t size, Py_ssize_t &start,
                   Py_ssize_t &stop, Py_ssize_t &step,
                   size_t &slice_length) {
    if (PySlice_Unpack(slice, &start, &stop, &step) < 0)
        detail::raise_python_error();
    Py_ssize_t slice_length_ =
        PySlice_AdjustIndices((Py_ssize_t) size, &start, &stop, step);
    slice_length = (size_t) slice_length_;
}

bool iterable_check(PyObject *o) noexcept {
#if !defined(Py_LIMITED_API)
    return Py_TYPE(o)->tp_iter != nullptr || PySequence_Check(o);
#else
    PyObject *it = PyObject_GetIter(o);
    if (it) {
        Py_DECREF(it);
        return true;
    } else {
        PyErr_Clear();
        return false;
    }
#endif
}

// ========================================================================

NB_CORE PyObject *repr_list(PyObject *o) {
    object s = steal(nb_inst_name(o));
    s += str("([");
    size_t len = obj_len(o);
    for (size_t i = 0; i < len; ++i) {
        s += repr(handle(o)[i]);
        if (i + 1 < len)
            s += str(", ");
    }
    s += str("])");
    return s.release().ptr();
}

NB_CORE PyObject *repr_map(PyObject *o) {
    object s = steal(nb_inst_name(o));
    s += str("({");
    bool first = true;
    for (handle kv : handle(o).attr("items")()) {
        if (!first)
            s += str(", ");
        s += repr(kv[0]) + str(": ") + repr(kv[1]);
        first = false;
    }
    s += str("})");
    return s.release().ptr();
}

// ========================================================================

bool issubclass(PyObject *a, PyObject *b) {
    int rv = PyObject_IsSubclass(a, b);
    if (rv == -1)
        raise_python_error();
    return bool(rv);
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
