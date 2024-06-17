/*
    src/trampoline.cpp: support for overriding virtual functions in Python

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include <nanobind/trampoline.h>
#include "nb_internals.h"

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

void trampoline_new(void **data, size_t size, void *ptr) noexcept {
    // GIL is held when the trampoline constructor runs
    nb_ptr_map &inst_c2p = internals->inst_c2p;
    nb_ptr_map::iterator it = inst_c2p.find(ptr);
    check(it != inst_c2p.end() && (((uintptr_t) it->second) & 1) == 0,
          "nanobind::detail::trampoline_new(): unique instance not found!");

    data[0] = it->second;
    memset(data + 1, 0, sizeof(void *) * 2 * size);
}

void trampoline_release(void **data, size_t size) noexcept {
    // GIL is held when the trampoline destructor runs
    for (size_t i = 0; i < size; ++i)
        Py_XDECREF((PyObject *) data[i*2 + 2]);
}

static void trampoline_enter_internal(void **data, size_t size,
                                      const char *name, bool pure, ticket *t) {
    const PyObject *None = Py_None;
    PyGILState_STATE state{ };
    const char *error = nullptr;
    PyObject *key = nullptr, *value = nullptr;
    PyTypeObject *value_tp = nullptr;
    size_t offset = 0;

    // First, perform a quick sweep without lock
    for (size_t i = 0; i < size; i++) {
        void *d_name  = data[2*i + 1],
             *d_value = data[2*i + 2];
        if (name == d_name && d_value) {
            if (d_value != None) {
                t->state = PyGILState_Ensure();
                t->key = (PyObject *) d_value;
                return;
            } else {
                if (pure) {
                    error = "tried to call a pure virtual function";
                    state = PyGILState_Ensure();
                    goto fail;
                } else {
                    return;
                }
            }
        }
    }

    // Nothing found -- retry, now with lock held
    state = PyGILState_Ensure();
    for (size_t i = 0; i < size; i++) {
        void *d_name  = data[2*i + 1],
             *d_value = data[2*i + 2];
        if (name == d_name && d_value) {
            if (d_value != None) {
                t->state = state;
                t->key = (PyObject *) d_value;
                return;
            } else {
                if (pure) {
                    error = "tried to call a pure virtual function";
                    goto fail;
                } else {
                    PyGILState_Release(state);
                    return;
                }
            }
        }
    }

    // Sill no luck -- perform a lookup and populate the trampoline
    for (; offset < size; offset++) {
        if (data[2 * offset + 1] == nullptr &&
            data[2 * offset + 2] == nullptr)
            break;
    }

    if (offset == size) {
        error = "the trampoline ran out of slots (you will need to increase "
                "the value provided to the NB_TRAMPOLINE() macro)";
        goto fail;
    }

    key = PyUnicode_InternFromString(name);
    if (!key) {
        error = "could not intern string";
        goto fail;
    }

    value = PyObject_GetAttr((PyObject *) data[0], key);
    if (!value) {
        error = "lookup failed";
        goto fail;
    }

    value_tp = Py_TYPE(value);
    Py_CLEAR(value);

    if (value_tp == internals->nb_func || value_tp == internals->nb_method ||
        value_tp == internals->nb_bound_method) {
        Py_DECREF(key);

        if (pure) {
            error = "tried to call a pure virtual function";
            goto fail;
        }

        Py_INCREF(Py_None);
        key = Py_None;
    }

    data[2 * offset + 1] = (void *) name;
    data[2 * offset + 2] = key;

    if (key != None) {
        t->state = state;
        t->key = key;
        return;
    } else {
        PyGILState_Release(state);
        return;
    }

fail:
    type_data *td = nb_type_data(Py_TYPE((PyObject *) data[0]));
    PyGILState_Release(state);

    raise("nanobind::detail::get_trampoline('%s::%s()'): %s!",
          td->name, name, error);
}

NB_THREAD_LOCAL ticket *current_ticket = nullptr;

void trampoline_enter(void **data, size_t size, const char *name, bool pure, ticket *t) {
    trampoline_enter_internal(data, size, name, pure, t);

    if (t->key) {
        t->self = (PyObject *) data[0];
        t->prev = current_ticket;

        if (t->prev && t->prev->self.is(t->self) && t->prev->key.is(t->key)) {
            t->self = handle();
            t->key = handle();
            t->prev = nullptr;
            PyGILState_Release(t->state);
            if (pure)
                raise("nanobind::detail::get_trampoline('%s()'): tried to call "
                      "a pure virtual function!", name);
            return;
        }

        current_ticket = t;
    }
}

void trampoline_leave(ticket *t) noexcept {
    if (!t->key)
        return;
    current_ticket = t->prev;
    PyGILState_Release(t->state);
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
