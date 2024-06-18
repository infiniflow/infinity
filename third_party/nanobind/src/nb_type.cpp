/*
    src/nb_type.cpp: libnanobind functionality for binding classes

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#include "nb_internals.h"

#if defined(_MSC_VER)
#  pragma warning(disable: 4706) // assignment within conditional expression
#endif

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

static PyObject **nb_dict_ptr(PyObject *self) {
    PyTypeObject *tp = Py_TYPE(self);
#if defined(Py_LIMITED_API)
    Py_ssize_t dictoffset = nb_type_data(tp)->dictoffset;
#else
    Py_ssize_t dictoffset = tp->tp_dictoffset;
#endif
    return dictoffset ? (PyObject **) ((uint8_t *) self + dictoffset) : nullptr;
}

static PyObject **nb_weaklist_ptr(PyObject *self) {
    PyTypeObject *tp = Py_TYPE(self);
#if defined(Py_LIMITED_API)
    Py_ssize_t weaklistoffset = nb_type_data(tp)->weaklistoffset;
#else
    Py_ssize_t weaklistoffset = tp->tp_weaklistoffset;
#endif
    return weaklistoffset ? (PyObject **) ((uint8_t *) self + weaklistoffset) : nullptr;
}

static PyGetSetDef inst_getset[] = {
    { "__dict__", PyObject_GenericGetDict, PyObject_GenericSetDict, nullptr, nullptr },
    { nullptr, nullptr, nullptr, nullptr, nullptr }
};

static int inst_clear(PyObject *self) {
    PyObject **dict = nb_dict_ptr(self);
    if (dict)
        Py_CLEAR(*dict);
    return 0;
}

static int inst_traverse(PyObject *self, visitproc visit, void *arg) {
    PyObject **dict = nb_dict_ptr(self);
    if (dict)
        Py_VISIT(*dict);
#if PY_VERSION_HEX >= 0x03090000
    Py_VISIT(Py_TYPE(self));
#endif
    return 0;
}

static int inst_init(PyObject *self, PyObject *, PyObject *) {
    const type_data *t = nb_type_data(Py_TYPE(self));
    PyErr_Format(PyExc_TypeError, "%s: no constructor defined!", t->name);
    return -1;
}

/// Allocate memory for a nb_type instance with internal storage
PyObject *inst_new_int(PyTypeObject *tp) {
    bool gc = PyType_HasFeature(tp, Py_TPFLAGS_HAVE_GC);

    nb_inst *self;
    if (NB_LIKELY(!gc))
        self = PyObject_New(nb_inst, tp);
    else
        self = (nb_inst *) PyType_GenericAlloc(tp, 0);

    if (NB_LIKELY(self)) {
        const type_data *t = nb_type_data(tp);
        uint32_t align = (uint32_t) t->align;
        bool intrusive = t->flags & (uint32_t) type_flags::intrusive_ptr;

        uintptr_t payload = (uintptr_t) (self + 1);

        if (NB_UNLIKELY(align > sizeof(void *)))
            payload = (payload + align - 1) / align * align;

        self->offset = (int32_t) ((intptr_t) payload - (intptr_t) self);
        self->direct = 1;
        self->internal = 1;
        self->state = nb_inst::state_uninitialized;
        self->destruct = 0;
        self->cpp_delete = 0;
        self->clear_keep_alive = 0;
        self->intrusive = intrusive;
        self->unused = 0;

        // Update hash table that maps from C++ to Python instance
        auto [it, success] = internals->inst_c2p.try_emplace((void *) payload, self);
        check(success, "nanobind::detail::inst_new_int(): unexpected collision!");
    }

    return (PyObject *) self;

}

/// Allocate memory for a nb_type instance with external storage
PyObject *inst_new_ext(PyTypeObject *tp, void *value) {
    bool gc = PyType_HasFeature(tp, Py_TPFLAGS_HAVE_GC);

    nb_inst *self;
    if (NB_LIKELY(!gc)) {
        self = (nb_inst *) PyObject_Malloc(sizeof(nb_inst));
        if (!self)
            return PyErr_NoMemory();
        PyObject_Init((PyObject *) self, tp);
    } else {
        self = (nb_inst *) PyType_GenericAlloc(tp, 0);
        if (!self)
            return nullptr;
    }

    // Compute offset to instance value
    int32_t offset = (int32_t) ((intptr_t) value - (intptr_t) self);

    bool direct = (intptr_t) self + offset == (intptr_t) value;
    if (NB_UNLIKELY(!direct)) {
        // Location is not representable as signed 32 bit offset
        if (!gc) {
            /// Allocate memory for an extra pointer
            nb_inst *self_2 =
                (nb_inst *) PyObject_Realloc(self, sizeof(nb_inst) + sizeof(void *));

            if (NB_UNLIKELY(!self_2)) {
                PyObject_Free(self);
                return PyErr_NoMemory();
            }

            self = self_2;
        }

        *(void **) (self + 1) = value;
        offset = (int32_t) sizeof(nb_inst);
    }

    const type_data *t = nb_type_data(tp);
    bool intrusive = t->flags & (uint32_t) type_flags::intrusive_ptr;

    self->offset = offset;
    self->direct = direct;
    self->internal = 0;
    self->state = nb_inst::state_uninitialized;
    self->destruct = 0;
    self->cpp_delete = 0;
    self->clear_keep_alive = 0;
    self->intrusive = intrusive;
    self->unused = 0;

    // Update hash table that maps from C++ to Python instance
    auto [it, success] = internals->inst_c2p.try_emplace(value, self);

    if (NB_UNLIKELY(!success)) {
        void *entry = it->second;

        // Potentially convert the map value into linked list format
        if (!nb_is_seq(entry)) {
            nb_inst_seq *first = (nb_inst_seq *) PyMem_Malloc(sizeof(nb_inst_seq));
            check(first, "nanobind::detail::inst_new_ext(): list element "
                         "allocation failed!");
            first->inst = (PyObject *) entry;
            first->next = nullptr;
            entry = it.value() = nb_mark_seq(first);
        }

        nb_inst_seq *seq = nb_get_seq(entry);
        while (true) {
            check((nb_inst *) seq->inst != self,
                  "nanobind::detail::inst_new_ext(): duplicate instance!");
            if (!seq->next)
                break;
            seq = seq->next;
        }

        nb_inst_seq *next = (nb_inst_seq *) PyMem_Malloc(sizeof(nb_inst_seq));
        check(next,
              "nanobind::detail::inst_new_ext(): list element allocation failed!");

        next->inst = (PyObject *) self;
        next->next = nullptr;
        seq->next = next;
    }

    return (PyObject *) self;
}

static void inst_dealloc(PyObject *self) {
    PyTypeObject *tp = Py_TYPE(self);
    const type_data *t = nb_type_data(tp);

    bool gc = PyType_HasFeature(tp, Py_TPFLAGS_HAVE_GC);
    if (NB_UNLIKELY(gc)) {
        PyObject_GC_UnTrack(self);

        if (t->flags & (uint32_t) type_flags::has_dynamic_attr) {
            PyObject **dict = nb_dict_ptr(self);
            if (dict)
                Py_CLEAR(*dict);
        }
    }

    if (t->flags & (uint32_t) type_flags::is_weak_referenceable &&
        nb_weaklist_ptr(self) != nullptr) {
#if defined(PYPY_VERSION)
        PyObject **weaklist = nb_weaklist_ptr(self);
        if (weaklist)
            Py_CLEAR(*weaklist);
#else
        PyObject_ClearWeakRefs(self);
#endif
    }

    nb_inst *inst = (nb_inst *) self;
    void *p = inst_ptr(inst);

    if (inst->destruct) {
        check(t->flags & (uint32_t) type_flags::is_destructible,
              "nanobind::detail::inst_dealloc(\"%s\"): attempted to call "
              "the destructor of a non-destructible type!", t->name);
        if (t->flags & (uint32_t) type_flags::has_destruct)
            t->destruct(p);
    }

    if (inst->cpp_delete) {
        if (NB_LIKELY(t->align <= (uint32_t) __STDCPP_DEFAULT_NEW_ALIGNMENT__))
            operator delete(p);
        else
            operator delete(p, std::align_val_t(t->align));
    }

    if (NB_UNLIKELY(inst->clear_keep_alive)) {
        size_t self_hash = ptr_hash()(self);
        nb_ptr_map &keep_alive = internals->keep_alive;
        nb_ptr_map::iterator it = keep_alive.find(self, self_hash);
        check(it != keep_alive.end(),
              "nanobind::detail::inst_dealloc(\"%s\"): inconsistent "
              "keep_alive information", t->name);

        nb_weakref_seq *s = (nb_weakref_seq *) it->second;
        keep_alive.erase_fast(it);

        do {
            nb_weakref_seq *c = s;
            s = c->next;

            if (c->callback)
                c->callback(c->payload);
            else
                Py_DECREF((PyObject *) c->payload);

            PyObject_Free(c);
        } while (s);
    }

    size_t p_hash = ptr_hash()(p);

    // Update hash table that maps from C++ to Python instance
    nb_ptr_map &inst_c2p = internals->inst_c2p;
    nb_ptr_map::iterator it = inst_c2p.find(p, p_hash);
    bool found = false;

    if (NB_LIKELY(it != inst_c2p.end())) {
        void *entry = it->second;
        if (NB_LIKELY(entry == inst)) {
            found = true;
            inst_c2p.erase_fast(it);
        } else if (nb_is_seq(entry)) {
            // Multiple objects are associated with this address. Find the right one!
            nb_inst_seq *seq = nb_get_seq(entry),
                        *pred = nullptr;

            do {
                if ((nb_inst *) seq->inst == inst) {
                    found = true;

                    if (pred) {
                        pred->next = seq->next;
                    } else {
                        if (seq->next)
                            it.value() = nb_mark_seq(seq->next);
                        else
                            inst_c2p.erase_fast(it);
                    }

                    PyMem_Free(seq);
                    break;
                }

                pred = seq;
                seq = seq->next;
            } while (seq);
        }
    }

    check(found,
          "nanobind::detail::inst_dealloc(\"%s\"): attempted to delete an "
          "unknown instance (%p)!", t->name, p);

    if (NB_UNLIKELY(gc))
        PyObject_GC_Del(self);
    else
        PyObject_Free(self);

    Py_DECREF(tp);
}

type_data *nb_type_c2p(nb_internals *internals_,
                       const std::type_info *type) {
    nb_type_map_fast &type_c2p_fast = internals_->type_c2p_fast;
    nb_type_map_slow &type_c2p_slow = internals_->type_c2p_slow;

    nb_type_map_fast::iterator it_fast = type_c2p_fast.find(type);
    if (it_fast != type_c2p_fast.end())
        return it_fast->second;

    nb_type_map_slow::iterator it_slow = type_c2p_slow.find(type);
    if (it_slow != type_c2p_slow.end()) {
        type_data *d = it_slow->second;

        nb_alias_chain *chain = (nb_alias_chain *) PyMem_Malloc(sizeof(nb_alias_chain));
        check(chain, "Could not allocate nb_alias_chain entry!");
        chain->next = d->alias_chain;
        chain->value = type;
        d->alias_chain = chain;
        type_c2p_fast[type] = d;
        return d;
    }

    return nullptr;
}

void nb_type_unregister(type_data *t) noexcept {
    nb_type_map_slow &type_c2p_slow = internals->type_c2p_slow;
    nb_type_map_fast &type_c2p_fast = internals->type_c2p_fast;

    size_t n_del_slow = type_c2p_slow.erase(t->type),
           n_del_fast = type_c2p_fast.erase(t->type);

    bool fail = n_del_fast != 1 || n_del_slow != 1;
    if (!fail) {
        nb_alias_chain *cur = t->alias_chain;
        while (cur) {
            nb_alias_chain *next = cur->next;
            n_del_fast = type_c2p_fast.erase(cur->value);
            if (n_del_fast != 1) {
                fail = true;
                break;
            }
            PyMem_Free(cur);
            cur = next;
        }
    }

    check(!fail,
          "nanobind::detail::nb_type_unregister(\"%s\"): could not "
          "find type!", t->name);
}

static void nb_type_dealloc(PyObject *o) {
    type_data *t = nb_type_data((PyTypeObject *) o);

    if (t->type && (t->flags & (uint32_t) type_flags::is_python_type) == 0)
        nb_type_unregister(t);

    if (t->flags & (uint32_t) type_flags::has_implicit_conversions) {
        free(t->implicit.cpp);
        free(t->implicit.py);
    }

    free((char *) t->name);
    NB_SLOT(PyType_Type, tp_dealloc)(o);
}

/// Called when a C++ type is extended from within Python
static int nb_type_init(PyObject *self, PyObject *args, PyObject *kwds) {
    if (NB_TUPLE_GET_SIZE(args) != 3) {
        PyErr_SetString(PyExc_RuntimeError,
                        "nb_type_init(): invalid number of arguments!");
        return -1;
    }

    PyObject *bases = NB_TUPLE_GET_ITEM(args, 1);
    if (!PyTuple_CheckExact(bases) || NB_TUPLE_GET_SIZE(bases) != 1) {
        PyErr_SetString(PyExc_RuntimeError,
                        "nb_type_init(): invalid number of bases!");
        return -1;
    }

    PyObject *base = NB_TUPLE_GET_ITEM(bases, 0);
    if (!PyType_Check(base)) {
        PyErr_SetString(PyExc_RuntimeError, "nb_type_init(): expected a base type object!");
        return -1;
    }

    type_data *t_b = nb_type_data((PyTypeObject *) base);
    if (t_b->flags & (uint32_t) type_flags::is_final) {
        PyErr_Format(PyExc_TypeError, "The type '%s' prohibits subclassing!",
                     t_b->name);
        return -1;
    }

    int rv = NB_SLOT(PyType_Type, tp_init)(self, args, kwds);
    if (rv)
        return rv;

    type_data *t = nb_type_data((PyTypeObject *) self);

    *t = *t_b;
    t->flags |=  (uint32_t) type_flags::is_python_type;
    t->flags &= ~((uint32_t) type_flags::has_implicit_conversions);
    PyObject *name = nb_type_name(self);
    t->name = strdup_check(PyUnicode_AsUTF8AndSize(name, nullptr));
    Py_DECREF(name);
    t->type_py = (PyTypeObject *) self;
    t->implicit.cpp = nullptr;
    t->implicit.py = nullptr;
    t->alias_chain = nullptr;

    return 0;
}

/// Special case to handle 'Class.property = value' assignments
static int nb_type_setattro(PyObject* obj, PyObject* name, PyObject* value) {
    nb_internals *int_p = internals;
    int_p->nb_static_property_enabled = false;
    PyObject *cur = PyObject_GetAttr(obj, name);
    int_p->nb_static_property_enabled = true;

    if (cur) {
        PyTypeObject *tp = int_p->nb_static_property;
        if (Py_TYPE(cur) == tp) {
            int rv = int_p->nb_static_property_descr_set(cur, obj, value);
            Py_DECREF(cur);
            return rv;
        }
        Py_DECREF(cur);

        const char *cname = PyUnicode_AsUTF8AndSize(name, nullptr);
        if (!cname) {
            PyErr_Clear(); // probably a non-string attribute name
        } else if (cname[0] == '@') {
            /* Prevent type attributes starting with an `@` sign from being
               rebound or deleted. This is useful to safely stash owning
               references. The ``nb::enum_<>`` class, e.g., uses this to ensure
               indirect ownership of a borrowed reference in the supplemental
               type data. */
            PyErr_Format(PyExc_AttributeError,
                         "internal nanobind attribute '%s' cannot be "
                         "reassigned or deleted.", cname);
            return -1;
        }
    } else {
        PyErr_Clear();
    }

    return NB_SLOT(PyType_Type, tp_setattro)(obj, name, value);
}

#if NB_TYPE_FROM_METACLASS_IMPL || NB_TYPE_GET_SLOT_IMPL

struct nb_slot {
#if NB_TYPE_GET_SLOT_IMPL
    uint8_t indirect_1;
    uint8_t indirect_2;
#endif
    uint8_t direct;
};

template <size_t I1, size_t I2, size_t Offset1, size_t Offset2> nb_slot constexpr Ei() {
    // Compile-time check to ensure that indices and alignment match our expectation
    static_assert(I1 == I2 && (Offset1 % sizeof(void *)) == 0 && (Offset2 % sizeof(void *)) == 0,
                  "nb_slot construction: internal error");

#if NB_TYPE_GET_SLOT_IMPL
    size_t o = 0;
    switch (Offset1) {
        case offsetof(PyHeapTypeObject, as_async):    o = offsetof(PyTypeObject, tp_as_async); break;
        case offsetof(PyHeapTypeObject, as_number):   o = offsetof(PyTypeObject, tp_as_number); break;
        case offsetof(PyHeapTypeObject, as_mapping):  o = offsetof(PyTypeObject, tp_as_mapping); break;
        case offsetof(PyHeapTypeObject, as_sequence): o = offsetof(PyTypeObject, tp_as_sequence); break;
        case offsetof(PyHeapTypeObject, as_buffer):   o = offsetof(PyTypeObject, tp_as_buffer); break;
        default: break;
    }

    return {
        (uint8_t) (o / sizeof(void *)),
        (uint8_t) ((Offset2 - Offset1) / sizeof(void *)),
        (uint8_t) (Offset2 / sizeof(void *)),
    };
#else
    return { (uint8_t) (Offset2 / sizeof(void *)) };
#endif
}

// Precomputed mapping from type slot ID to an entry in the data structure
#define E(i1, p1, p2, name)                            \
    Ei<i1, Py_##p2##_##name,                           \
       offsetof(PyHeapTypeObject, p1),                 \
       offsetof(PyHeapTypeObject, p1.p2##_##name)>()

#if PY_VERSION_HEX < 0x03090000
#  define Py_bf_getbuffer 1
#  define Py_bf_releasebuffer 2
#endif

static constexpr nb_slot type_slots[] {
    E(1,  as_buffer, bf, getbuffer),
    E(2,  as_buffer, bf, releasebuffer),
    E(3,  as_mapping, mp, ass_subscript),
    E(4,  as_mapping, mp, length),
    E(5,  as_mapping, mp, subscript),
    E(6,  as_number, nb, absolute),
    E(7,  as_number, nb, add),
    E(8,  as_number, nb, and),
    E(9,  as_number, nb, bool),
    E(10, as_number, nb, divmod),
    E(11, as_number, nb, float),
    E(12, as_number, nb, floor_divide),
    E(13, as_number, nb, index),
    E(14, as_number, nb, inplace_add),
    E(15, as_number, nb, inplace_and),
    E(16, as_number, nb, inplace_floor_divide),
    E(17, as_number, nb, inplace_lshift),
    E(18, as_number, nb, inplace_multiply),
    E(19, as_number, nb, inplace_or),
    E(20, as_number, nb, inplace_power),
    E(21, as_number, nb, inplace_remainder),
    E(22, as_number, nb, inplace_rshift),
    E(23, as_number, nb, inplace_subtract),
    E(24, as_number, nb, inplace_true_divide),
    E(25, as_number, nb, inplace_xor),
    E(26, as_number, nb, int),
    E(27, as_number, nb, invert),
    E(28, as_number, nb, lshift),
    E(29, as_number, nb, multiply),
    E(30, as_number, nb, negative),
    E(31, as_number, nb, or),
    E(32, as_number, nb, positive),
    E(33, as_number, nb, power),
    E(34, as_number, nb, remainder),
    E(35, as_number, nb, rshift),
    E(36, as_number, nb, subtract),
    E(37, as_number, nb, true_divide),
    E(38, as_number, nb, xor),
    E(39, as_sequence, sq, ass_item),
    E(40, as_sequence, sq, concat),
    E(41, as_sequence, sq, contains),
    E(42, as_sequence, sq, inplace_concat),
    E(43, as_sequence, sq, inplace_repeat),
    E(44, as_sequence, sq, item),
    E(45, as_sequence, sq, length),
    E(46, as_sequence, sq, repeat),
    E(47, ht_type, tp, alloc),
    E(48, ht_type, tp, base),
    E(49, ht_type, tp, bases),
    E(50, ht_type, tp, call),
    E(51, ht_type, tp, clear),
    E(52, ht_type, tp, dealloc),
    E(53, ht_type, tp, del),
    E(54, ht_type, tp, descr_get),
    E(55, ht_type, tp, descr_set),
    E(56, ht_type, tp, doc),
    E(57, ht_type, tp, getattr),
    E(58, ht_type, tp, getattro),
    E(59, ht_type, tp, hash),
    E(60, ht_type, tp, init),
    E(61, ht_type, tp, is_gc),
    E(62, ht_type, tp, iter),
    E(63, ht_type, tp, iternext),
    E(64, ht_type, tp, methods),
    E(65, ht_type, tp, new),
    E(66, ht_type, tp, repr),
    E(67, ht_type, tp, richcompare),
    E(68, ht_type, tp, setattr),
    E(69, ht_type, tp, setattro),
    E(70, ht_type, tp, str),
    E(71, ht_type, tp, traverse),
    E(72, ht_type, tp, members),
    E(73, ht_type, tp, getset),
    E(74, ht_type, tp, free),
    E(75, as_number, nb, matrix_multiply),
    E(76, as_number, nb, inplace_matrix_multiply),
    E(77, as_async, am, await),
    E(78, as_async, am, aiter),
    E(79, as_async, am, anext),
    E(80, ht_type, tp, finalize),
#if PY_VERSION_HEX >= 0x030A0000 && !defined(PYPY_VERSION)
    E(81, as_async, am, send),
#endif
};

#if NB_TYPE_GET_SLOT_IMPL
void *type_get_slot(PyTypeObject *t, int slot_id) {
    nb_slot slot = type_slots[slot_id - 1];

    if (PyType_HasFeature(t, Py_TPFLAGS_HEAPTYPE)) {
        return ((void **) t)[slot.direct];
    } else {
        if (slot.indirect_1)
            return ((void ***) t)[slot.indirect_1][slot.indirect_2];
        else
            return ((void **) t)[slot.indirect_2];
    }
}
#endif

#endif

static PyObject *nb_type_from_metaclass(PyTypeObject *meta, PyObject *mod,
                                        PyType_Spec *spec) {
#if NB_TYPE_FROM_METACLASS_IMPL == 0
    // Life is good, PyType_FromMetaclass() is available
    return PyType_FromMetaclass(meta, mod, spec, nullptr);
#else
    /* The fallback code below emulates PyType_FromMetaclass() on Python prior
       to version 3.12. It requires access to CPython-internal structures, which
       is why nanobind can only target the stable ABI on version 3.12+. */

    const char *name = strrchr(spec->name, '.');
    if (name)
        name++;
    else
        name = spec->name;

    PyObject *name_o = PyUnicode_InternFromString(name);
    if (!name_o)
        return nullptr;

    const char *name_cstr = PyUnicode_AsUTF8AndSize(name_o, nullptr);
    if (!name_cstr) {
        Py_DECREF(name_o);
        return nullptr;
    }

    PyHeapTypeObject *ht = (PyHeapTypeObject *) PyType_GenericAlloc(meta, 0);
    if (!ht) {
        Py_DECREF(name_o);
        return nullptr;
    }

    ht->ht_name = name_o;
    ht->ht_qualname = name_o;
    Py_INCREF(name_o);

#if PY_VERSION_HEX >= 0x03090000
    if (mod) {
        Py_INCREF(mod);
        ht->ht_module = mod;
    }
#else
    (void) mod;
#endif

    PyTypeObject *tp = &ht->ht_type;
    tp->tp_name = name_cstr;
    tp->tp_basicsize = spec->basicsize;
    tp->tp_itemsize = spec->itemsize;
    tp->tp_flags = spec->flags | Py_TPFLAGS_HEAPTYPE;
    tp->tp_as_async = &ht->as_async;
    tp->tp_as_number = &ht->as_number;
    tp->tp_as_sequence = &ht->as_sequence;
    tp->tp_as_mapping = &ht->as_mapping;
    tp->tp_as_buffer = &ht->as_buffer;

    PyType_Slot *ts = spec->slots;
    bool fail = false;
    while (true) {
        int slot = ts->slot;

        if (slot == 0) {
            break;
        } else if (slot * sizeof(nb_slot) < (int) sizeof(type_slots)) {
            *(((void **) ht) + type_slots[slot - 1].direct) = ts->pfunc;
        } else {
            PyErr_Format(PyExc_RuntimeError,
                         "nb_type_from_metaclass(): unhandled slot %i", slot);
            fail = true;
            break;
        }
        ts++;
    }

    // Bring type object into a safe state (before error handling)
    const PyMemberDef *members = tp->tp_members;
    const char *doc = tp->tp_doc;
    tp->tp_members = nullptr;
    tp->tp_doc = nullptr;
    Py_XINCREF(tp->tp_base);

    if (doc && !fail) {
        size_t size = strlen(doc) + 1;
        char *target = (char *) PyObject_Malloc(size);
        if (!target) {
            PyErr_NoMemory();
            fail = true;
        } else {
            memcpy(target, doc, size);
            tp->tp_doc = target;
        }
    }

    if (members && !fail) {
        while (members->name) {
            if (members->type == T_PYSSIZET && members->flags == READONLY) {
                if (strcmp(members->name, "__dictoffset__") == 0)
                    tp->tp_dictoffset = members->offset;
                else if (strcmp(members->name, "__weaklistoffset__") == 0)
                    tp->tp_weaklistoffset = members->offset;
                else if (strcmp(members->name, "__vectorcalloffset__") == 0)
                    tp->tp_vectorcall_offset = members->offset;
                else
                    fail = true;
            } else {
                fail = true;
            }

            if (fail) {
                PyErr_Format(
                    PyExc_RuntimeError,
                    "nb_type_from_metaclass(): unhandled tp_members entry!");
                break;
            }

            members++;
        }
    }

    if (fail || PyType_Ready(tp) != 0) {
        Py_DECREF(tp);
        return nullptr;
    }

    return (PyObject *) tp;
#endif
}

static PyTypeObject *nb_type_tp(size_t supplement) noexcept {
    object key = steal(PyLong_FromSize_t(supplement));

    PyTypeObject *tp =
        (PyTypeObject *) PyDict_GetItem(internals->nb_type_dict, key.ptr());

    if (NB_UNLIKELY(!tp)) {
        PyType_Slot slots[] = {
            { Py_tp_base, &PyType_Type },
            { Py_tp_dealloc, (void *) nb_type_dealloc },
            { Py_tp_setattro, (void *) nb_type_setattro },
            { Py_tp_init, (void *) nb_type_init },
            { 0, nullptr }
        };

#if PY_VERSION_HEX >= 0x030C0000
        int basicsize = -(int) (sizeof(type_data) + supplement),
            itemsize = 0;
#else
        int basicsize = (int) (PyType_Type.tp_basicsize + (sizeof(type_data) + supplement)),
            itemsize = (int) PyType_Type.tp_itemsize;
#endif

        char name[17 + 20 + 1];
        snprintf(name, sizeof(name), "nanobind.nb_type_%zu", supplement);

        PyType_Spec spec = {
            /* .name = */ name,
            /* .basicsize = */ basicsize,
            /* .itemsize = */ itemsize,
            /* .flags = */ Py_TPFLAGS_DEFAULT,
            /* .slots = */ slots
        };

        tp = (PyTypeObject *) nb_type_from_metaclass(
            internals->nb_meta, internals->nb_module, &spec);

        handle(tp).attr("__module__") = "nanobind";

        int rv = 1;
        if (tp)
            rv = PyDict_SetItem(internals->nb_type_dict, key.ptr(), (PyObject *) tp);
        check(rv == 0, "nb_type type creation failed!");

        Py_DECREF(tp);
    }

    return tp;
}

// This helper function extracts the function/class name from a custom signature attribute
NB_NOINLINE char *extract_name(const char *cmd, const char *prefix, const char *s) {
    (void) cmd;

    // Move to the last line
    const char *p = strrchr(s, '\n');
    p = p ? (p + 1) : s;

    // Check that the last line starts with the right prefix
    size_t prefix_len = strlen(prefix);
    check(strncmp(p, prefix, prefix_len) == 0,
          "%s(): last line of custom signature \"%s\" must start with \"%s\"!",
          cmd, s, prefix);
    p += prefix_len;

    // Find the opening parenthesis
    const char *p2 = strchr(p, '(');
    check(p2 != nullptr,
          "%s(): last line of custom signature \"%s\" must contain an opening "
          "parenthesis (\"(\")!", cmd, s);

    // A few sanity checks
    size_t len = strlen(p);
    char last = p[len ? (len - 1) : 0];

    check(last != ':' && last != ' ',
          "%s(): custom signature \"%s\" should not end with \":\" or \" \"!", cmd, s);
    check((p2 == p || (p[0] != ' ' && p2[-1] != ' ')),
          "%s(): custom signature \"%s\" contains leading/trailing space around name!", cmd, s);

    size_t size = p2 - p;
    char *result = (char *) malloc_check(size + 1);
    memcpy(result, p, size);
    result[size] = '\0';

    return result;
}

#if PY_VERSION_HEX >= 0x03090000
static PyMethodDef class_getitem_method[] = {
    { "__class_getitem__", Py_GenericAlias, METH_O | METH_CLASS, nullptr },
    { nullptr }
};
#endif

/// Called when a C++ type is bound via nb::class_<>
PyObject *nb_type_new(const type_init_data *t) noexcept {
    bool has_doc               = t->flags & (uint32_t) type_init_flags::has_doc,
         has_base              = t->flags & (uint32_t) type_init_flags::has_base,
         has_base_py           = t->flags & (uint32_t) type_init_flags::has_base_py,
         has_type_slots        = t->flags & (uint32_t) type_init_flags::has_type_slots,
         has_supplement        = t->flags & (uint32_t) type_init_flags::has_supplement,
         has_dynamic_attr      = t->flags & (uint32_t) type_flags::has_dynamic_attr,
         is_weak_referenceable = t->flags & (uint32_t) type_flags::is_weak_referenceable,
         is_generic            = t->flags & (uint32_t) type_flags::is_generic,
         intrusive_ptr         = t->flags & (uint32_t) type_flags::intrusive_ptr,
         has_shared_from_this  = t->flags & (uint32_t) type_flags::has_shared_from_this,
         has_signature         = t->flags & (uint32_t) type_flags::has_signature;

    const char *t_name = t->name;
    if (has_signature)
        t_name =
            extract_name("nanobind::detail::nb_type_new", "class ", t->name);

    str name = steal<str>(PyUnicode_InternFromString(t_name)),
        qualname = name;
    object modname;
    PyObject *mod = nullptr;

    // Update hash table that maps from std::type_info to Python type
    auto [it, success] = internals->type_c2p_slow.try_emplace(t->type, nullptr);
    if (!success) {
        PyErr_WarnFormat(PyExc_RuntimeWarning, 1, "nanobind: type '%s' was already registered!\n", t_name);
        PyObject *tp = (PyObject *) it->second->type_py;
        Py_INCREF(tp);
        if (has_signature)
            free((char *) t_name);
        return tp;
    }

    if (t->scope != nullptr) {
        if (PyModule_Check(t->scope)) {
            mod = t->scope;
            modname = getattr(t->scope, "__name__", handle());
        } else {
            modname = getattr(t->scope, "__module__", handle());

            object scope_qualname = getattr(t->scope, "__qualname__", handle());
            if (scope_qualname.is_valid())
                qualname = steal<str>(
                    PyUnicode_FromFormat("%U.%U", scope_qualname.ptr(), name.ptr()));
        }
    }

    if (modname.is_valid())
        name = steal<str>(
            PyUnicode_FromFormat("%U.%U", modname.ptr(), name.ptr()));

    constexpr size_t ptr_size = sizeof(void *);
    size_t basicsize = sizeof(nb_inst) + t->size;
    if (t->align > ptr_size)
        basicsize += t->align - ptr_size;

    PyObject *base = nullptr;

#if PY_VERSION_HEX >= 0x03090000
    bool generic_base = false;
#endif

    if (has_base_py) {
        check(!has_base,
              "nanobind::detail::nb_type_new(\"%s\"): multiple base types "
              "specified!", t_name);
        base = (PyObject *) t->base_py;

        #if PY_VERSION_HEX >= 0x03090000 && !defined(PYPY_VERSION) // see https://github.com/pypy/pypy/issues/4914
        if (Py_TYPE(base) == &Py_GenericAliasType) {
            base = PyObject_GetAttrString(base, "__origin__");
            check(base != nullptr,
                  "nanobind::detail::nb_type_new(\"%s\"): could not access base of type alias!", t_name);
            Py_DECREF(base);
            generic_base = true;
        }
        #endif

        check(nb_type_check(base),
              "nanobind::detail::nb_type_new(\"%s\"): base type is not a "
              "nanobind type!", t_name);
    } else if (has_base) {
        nb_type_map_slow::iterator it2 = internals->type_c2p_slow.find(t->base);
        check(it2 != internals->type_c2p_slow.end(),
                  "nanobind::detail::nb_type_new(\"%s\"): base type \"%s\" not "
                  "known to nanobind!", t_name, type_name(t->base));
        base = (PyObject *) it2->second->type_py;
    }

    type_data *tb = nullptr;
    if (base) {
        // Check if the base type already has dynamic attributes
        tb = nb_type_data((PyTypeObject *) base);
        if (tb->flags & (uint32_t) type_flags::has_dynamic_attr)
            has_dynamic_attr = true;

        if (tb->flags & (uint32_t) type_flags::is_weak_referenceable)
            is_weak_referenceable = true;

        /* Handle a corner case (base class larger than derived class)
           which can arise when extending trampoline base classes */
        size_t base_basicsize = sizeof(nb_inst) + tb->size;
        if (tb->align > ptr_size)
            base_basicsize += tb->align - ptr_size;
        if (base_basicsize > basicsize)
            basicsize = base_basicsize;
    }

    bool base_intrusive_ptr =
        tb && (tb->flags & (uint32_t) type_flags::intrusive_ptr);

    char *name_copy = strdup_check(name.c_str());

    constexpr size_t nb_type_max_slots = 10,
                     nb_extra_slots = 80,
                     nb_total_slots = nb_type_max_slots +
                                      nb_extra_slots + 1;

    PyMemberDef members[3] { };
    PyType_Slot slots[nb_total_slots], *s = slots;
    PyType_Spec spec = {
        /* .name = */ name_copy,
        /* .basicsize = */ (int) basicsize,
        /* .itemsize = */ 0,
        /* .flags = */ Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        /* .slots = */ slots
    };

    if (base)
        *s++ = { Py_tp_base, (void *) base };

    *s++ = { Py_tp_init, (void *) inst_init };
    *s++ = { Py_tp_new, (void *) inst_new_int };
    *s++ = { Py_tp_dealloc, (void *) inst_dealloc };

    if (has_doc)
        *s++ = { Py_tp_doc, (void *) t->doc };

    if (has_type_slots) {
        size_t num_avail = nb_extra_slots;
        size_t i = 0;
        while (t->type_slots[i].slot) {
            check(i != num_avail,
                  "nanobind::detail::nb_type_new(\"%s\"): ran out of "
                  "type slots!", t_name);
            *s++ = t->type_slots[i++];
        }
    }

    bool has_traverse = false;
    bool has_getset = false;
    for (PyType_Slot *ts = slots; ts != s; ++ts) {
        has_traverse |= ts->slot == Py_tp_traverse;
        has_getset |= ts->slot == Py_tp_getset;
    }

    Py_ssize_t dictoffset = 0, weaklistoffset = 0;
    int num_members = 0;

    // realign to sizeof(void*) if needed
    if (has_dynamic_attr || is_weak_referenceable)
        basicsize = (basicsize + ptr_size - 1) / ptr_size * ptr_size;

    if (has_dynamic_attr) {
        dictoffset = (Py_ssize_t) basicsize;
        basicsize += ptr_size;

        members[num_members] = PyMemberDef{ "__dictoffset__", T_PYSSIZET,
                                            dictoffset, READONLY, nullptr };
        ++num_members;

        // Install GC traverse and clear routines if not inherited/overridden
        if (!has_traverse) {
            *s++ = { Py_tp_traverse, (void *) inst_traverse };
            *s++ = { Py_tp_clear, (void *) inst_clear };
            has_traverse = true;
        }
        spec.basicsize = (int) basicsize;

        if (!has_getset) {
            *s++ = { Py_tp_getset, (void *) inst_getset };
        }
    }

    if (is_weak_referenceable) {
        weaklistoffset = (Py_ssize_t) basicsize;
        basicsize += ptr_size;

        members[num_members] = PyMemberDef{ "__weaklistoffset__", T_PYSSIZET,
                                            weaklistoffset, READONLY, nullptr };
        ++num_members;

        // Install GC traverse and clear routines if not inherited/overridden
        if (!has_traverse) {
            *s++ = { Py_tp_traverse, (void *) inst_traverse };
            *s++ = { Py_tp_clear, (void *) inst_clear };
            has_traverse = true;
        }
        spec.basicsize = (int) basicsize;
    }

    if (num_members > 0)
        *s++ = { Py_tp_members, (void*) members };

#if PY_VERSION_HEX < 0x03090000
    (void) is_generic; // unsupported on Python 3.8
#else
    if (is_generic)
        *s++ = { Py_tp_methods, (void*) class_getitem_method };
#endif

    if (has_traverse)
        spec.flags |= Py_TPFLAGS_HAVE_GC;

    *s++ = { 0, nullptr };

    PyTypeObject *metaclass = nb_type_tp(has_supplement ? t->supplement : 0);

    PyObject *result = nb_type_from_metaclass(metaclass, mod, &spec);
    if (!result) {
        python_error err;
        check(false,
              "nanobind::detail::nb_type_new(\"%s\"): type construction "
              "failed: %s!", t_name, err.what());
    }

    type_data *to = nb_type_data((PyTypeObject *) result);
    *to = *t; // note: slices off _init parts
    to->flags &= ~(uint32_t) type_init_flags::all_init_flags;

    if (!intrusive_ptr && base_intrusive_ptr) {
        to->flags |= (uint32_t) type_flags::intrusive_ptr;
        to->set_self_py = tb->set_self_py;
    }

    if (!has_shared_from_this && tb &&
        (tb->flags & (uint32_t) type_flags::has_shared_from_this)) {
        to->flags |= (uint32_t) type_flags::has_shared_from_this;
        to->keep_shared_from_this_alive = tb->keep_shared_from_this_alive;
    }

    to->name = name_copy;
    to->type_py = (PyTypeObject *) result;
    to->alias_chain = nullptr;

    if (has_dynamic_attr) {
        to->flags |= (uint32_t) type_flags::has_dynamic_attr;
        #if defined(Py_LIMITED_API)
            to->dictoffset = dictoffset;
        #endif
    }

    if (is_weak_referenceable) {
        to->flags |= (uint32_t) type_flags::is_weak_referenceable;
        #if defined(Py_LIMITED_API)
            to->weaklistoffset = weaklistoffset;
        #endif
    }

    if (t->scope != nullptr)
        setattr(t->scope, t_name, result);

    setattr(result, "__qualname__", qualname.ptr());

    if (modname.is_valid())
        setattr(result, "__module__", modname.ptr());

    internals->type_c2p_fast[t->type] = to;
    internals->type_c2p_slow[t->type] = to;

    if (has_signature) {
        setattr(result, "__nb_signature__", str(t->name));
        free((char *) t_name);
    }

#if PY_VERSION_HEX >= 0x03090000
    if (generic_base)
        setattr(result, "__orig_bases__", make_tuple(handle(t->base_py)));
#endif

    return result;
}


PyObject *call_one_arg(PyObject *fn, PyObject *arg) noexcept {
    PyObject *result;
#if PY_VERSION_HEX < 0x03090000
    PyObject *args = PyTuple_New(1);
    if (!args)
        return nullptr;
    Py_INCREF(arg);
    NB_TUPLE_SET_ITEM(args, 0, arg);
    result = PyObject_CallObject(fn, args);
    Py_DECREF(args);
#else
    PyObject *args[2] = { nullptr, arg };
    result = PyObject_Vectorcall(fn, args + 1, NB_VECTORCALL_ARGUMENTS_OFFSET + 1, nullptr);
#endif
    return result;
}

/// Encapsulates the implicit conversion part of nb_type_get()
static NB_NOINLINE bool nb_type_get_implicit(PyObject *src,
                                             const std::type_info *cpp_type_src,
                                             const type_data *dst_type,
                                             nb_internals *internals_,
                                             cleanup_list *cleanup, void **out) noexcept {
    if (dst_type->implicit.cpp && cpp_type_src) {
        const std::type_info **it = dst_type->implicit.cpp;
        const std::type_info *v;

        while ((v = *it++)) {
            if (v == cpp_type_src || *v == *cpp_type_src)
                goto found;
        }

        it = dst_type->implicit.cpp;
        while ((v = *it++)) {
            const type_data *d = nb_type_c2p(internals_, v);
            if (d && PyType_IsSubtype(Py_TYPE(src), d->type_py))
                goto found;
        }
    }

    if (dst_type->implicit.py) {
        bool (**it)(PyTypeObject *, PyObject *, cleanup_list *) noexcept =
            dst_type->implicit.py;
        bool (*v2)(PyTypeObject *, PyObject *, cleanup_list *) noexcept;

        while ((v2 = *it++)) {
            if (v2(dst_type->type_py, src, cleanup))
                goto found;
        }
    }

    return false;

found:
    PyObject *result = call_one_arg((PyObject *) dst_type->type_py, src);

    if (result) {
        cleanup->append(result);
        *out = inst_ptr((nb_inst *) result);
        return true;
    } else {
        PyErr_Clear();

        if (internals->print_implicit_cast_warnings) {
#if !defined(Py_LIMITED_API)
            const char *name = Py_TYPE(src)->tp_name;
#else
            PyObject *name_py = nb_inst_name(src);
            const char *name = PyUnicode_AsUTF8AndSize(name_py, nullptr);
#endif
            // Can't use PyErr_Warn*() if conversion failed due to a stack overflow
            fprintf(stderr,
                    "nanobind: implicit conversion from type '%s' to type '%s' "
                    "failed!\n", name, dst_type->name);

#if defined(Py_LIMITED_API)
            Py_DECREF(name_py);
#endif
        }

        return false;
    }
}

// Attempt to retrieve a pointer to a C++ instance
bool nb_type_get(const std::type_info *cpp_type, PyObject *src, uint8_t flags,
                 cleanup_list *cleanup, void **out) noexcept {
    // Convert None -> nullptr
    if (src == Py_None) {
        *out = nullptr;
        return true;
    }

    PyTypeObject *src_type = Py_TYPE(src);
    const std::type_info *cpp_type_src = nullptr;
    const bool src_is_nb_type = nb_type_check((PyObject *) src_type);

    type_data *dst_type = nullptr;
    nb_internals *internals_ = internals;

    // If 'src' is a nanobind-bound type
    if (NB_LIKELY(src_is_nb_type)) {
        type_data *t = nb_type_data(src_type);
        cpp_type_src = t->type;

        // Check if the source / destination typeid are an exact match
        bool valid = cpp_type == cpp_type_src || *cpp_type == *cpp_type_src;

        // If not, look up the Python type and check the inheritance chain
        if (NB_UNLIKELY(!valid)) {
            dst_type = nb_type_c2p(internals_, cpp_type);
            if (dst_type)
                valid = PyType_IsSubtype(src_type, dst_type->type_py);
        }

        // Success, return the pointer if the instance is correctly initialized
        if (NB_LIKELY(valid)) {
            nb_inst *inst = (nb_inst *) src;

            static_assert(cast_flags::construct == nb_inst::state_ready,
                          "this function is optimized assuming that "
                          "cast_flags::construct == nb_inst::state_ready");

            // (flags & construct)       state  xor-result  should accept?
            //          [normal] 0  [uninit] 0  0           no
            //          [normal] 0  [relinq] 1  1           no
            //          [normal] 0   [ready] 2  2           yes
            //       [construct] 2  [uninit] 0  2           yes
            //       [construct] 2  [relinq] 1  3           no
            //       [construct] 2   [ready] 2  0           no

            if (NB_UNLIKELY(((flags & (uint8_t) cast_flags::construct) ^ inst->state) != nb_inst::state_ready)) {
                constexpr const char* errors[4] = {
                    /* 0=uninit */ "attempted to access an uninitialized instance",
                    /* 1=relinq */ "attempted to access a relinquished instance",
                    /* 2=ready */ "attempted to initialize an already-initialized instance",
                    /* 3=invalid */ "instance state has become corrupted",
                };
                PyErr_WarnFormat(
                    PyExc_RuntimeWarning, 1, "nanobind: %s of type '%s'!\n",
                    errors[inst->state], t->name);
                return false;
            }

            *out = inst_ptr(inst);

            return true;
        }
    }

    // Try an implicit conversion as last resort (if possible & requested)
    if ((flags & (uint16_t) cast_flags::convert) && cleanup) {
        if (!src_is_nb_type)
            dst_type = nb_type_c2p(internals_, cpp_type);

        if (dst_type &&
            (dst_type->flags & (uint32_t) type_flags::has_implicit_conversions))
            return nb_type_get_implicit(src, cpp_type_src, dst_type, internals_,
                                        cleanup, out);
    }

    return false;
}

static PyObject *keep_alive_callback(PyObject *self, PyObject *const *args,
                                     Py_ssize_t nargs) {
    check(nargs == 1 && PyWeakref_CheckRefExact(args[0]),
          "nanobind::detail::keep_alive_callback(): invalid input!");
    Py_DECREF(args[0]); // self
    Py_DECREF(self); // patient
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef keep_alive_callback_def = {
    "keep_alive_callback", (PyCFunction) (void *) keep_alive_callback,
    METH_FASTCALL, nullptr
};

void keep_alive(PyObject *nurse, PyObject *patient) {
    if (!patient || !nurse || nurse == Py_None || patient == Py_None)
        return;

    if (nb_type_check((PyObject *) Py_TYPE(nurse))) {
        nb_weakref_seq **pp =
            (nb_weakref_seq **) &internals->keep_alive[nurse];

        do {
            nb_weakref_seq *p = *pp;
            if (!p)
                break;
            else if (p->payload == patient && !p->callback)
                return;
            pp = &p->next;
        } while (true);

        nb_weakref_seq *s =
            (nb_weakref_seq *) PyObject_Malloc(sizeof(nb_weakref_seq));
        check(s, "nanobind::detail::keep_alive(): out of memory!");

        s->payload = patient;
        s->callback = nullptr;
        s->next = nullptr;
        *pp = s;

        Py_INCREF(patient);
        ((nb_inst *) nurse)->clear_keep_alive = true;
    } else {
        PyObject *callback =
            PyCFunction_New(&keep_alive_callback_def, patient);

        PyObject *weakref = PyWeakref_NewRef(nurse, callback);
        if (!weakref) {
            Py_DECREF(callback);
            PyErr_Clear();
            raise("nanobind::detail::keep_alive(): could not create a weak "
                  "reference! Likely, the 'nurse' argument you specified is not "
                  "a weak-referenceable type!");
        }
        check(callback,
              "nanobind::detail::keep_alive(): callback creation failed!");

        // Increase patient reference count, leak weak reference
        Py_INCREF(patient);
        Py_DECREF(callback);
    }
}

void keep_alive(PyObject *nurse, void *payload,
                void (*callback)(void *) noexcept) noexcept {
    check(nurse, "nanobind::detail::keep_alive(): 'nurse' is undefined!");

    if (nb_type_check((PyObject *) Py_TYPE(nurse))) {
        nb_weakref_seq
            **pp = (nb_weakref_seq **) &internals->keep_alive[nurse],
            *s   = (nb_weakref_seq *) PyObject_Malloc(sizeof(nb_weakref_seq));
        check(s, "nanobind::detail::keep_alive(): out of memory!");

        s->payload = payload;
        s->callback = callback;
        s->next = *pp;
        *pp = s;

        ((nb_inst *) nurse)->clear_keep_alive = true;
    } else {
        PyObject *patient = capsule_new(payload, nullptr, callback);
        keep_alive(nurse, patient);
        Py_DECREF(patient);
    }
}

static PyObject *nb_type_put_common(void *value, type_data *t, rv_policy rvp,
                                    cleanup_list *cleanup,
                                    bool *is_new) noexcept {
    // The reference_internals RVP needs a self pointer, give up if unavailable
    if (rvp == rv_policy::reference_internal && (!cleanup || !cleanup->self()))
        return nullptr;

    const bool intrusive = t->flags & (uint32_t) type_flags::intrusive_ptr;
    if (intrusive)
        rvp = rv_policy::take_ownership;

    const bool create_new = rvp == rv_policy::copy || rvp == rv_policy::move;

    nb_inst *inst;
    if (create_new)
        inst = (nb_inst *) inst_new_int(t->type_py);
    else
        inst = (nb_inst *) inst_new_ext(t->type_py, value);

    if (!inst)
        return nullptr;

    void *new_value = inst_ptr(inst);
    if (rvp == rv_policy::move) {
        if (t->flags & (uint32_t) type_flags::is_move_constructible) {
            if (t->flags & (uint32_t) type_flags::has_move) {
                try {
                    t->move(new_value, value);
                } catch (...) {
                    Py_DECREF(inst);
                    return nullptr;
                }
            } else {
                memcpy(new_value, value, t->size);
                memset(value, 0, t->size);
            }
        } else {
            check(t->flags & (uint32_t) type_flags::is_copy_constructible,
                  "nanobind::detail::nb_type_put(\"%s\"): attempted to move "
                  "an instance that is neither copy- nor move-constructible!",
                  t->name);

            rvp = rv_policy::copy;
        }
    }

    if (rvp == rv_policy::copy) {
        check(t->flags & (uint32_t) type_flags::is_copy_constructible,
              "nanobind::detail::nb_type_put(\"%s\"): attempted to copy "
              "an instance that is not copy-constructible!", t->name);

        if (t->flags & (uint32_t) type_flags::has_copy) {
            try {
                t->copy(new_value, value);
            } catch (...) {
                Py_DECREF(inst);
                return nullptr;
            }
        } else {
            memcpy(new_value, value, t->size);
        }
    }

    // If we can find an existing C++ shared_ptr for this object, and
    // the instance we're creating just holds a pointer, then take out
    // another C++ shared_ptr that shares ownership with the existing
    // one, and tie its lifetime to the Python object. This is the
    // same thing done by the <nanobind/stl/shared_ptr.h> caster when
    // returning shared_ptr<T> to Python explicitly.
    if ((t->flags & (uint32_t) type_flags::has_shared_from_this) &&
        !create_new && t->keep_shared_from_this_alive((PyObject *) inst))
        rvp = rv_policy::reference;
    else if (is_new)
        *is_new = true;

    inst->destruct = rvp != rv_policy::reference && rvp != rv_policy::reference_internal;
    inst->cpp_delete = rvp == rv_policy::take_ownership;
    inst->state = nb_inst::state_ready;

    if (rvp == rv_policy::reference_internal)
        keep_alive((PyObject *) inst, cleanup->self());

    if (intrusive)
        t->set_self_py(new_value, (PyObject *) inst);

    return (PyObject *) inst;
}

PyObject *nb_type_put(const std::type_info *cpp_type,
                      void *value, rv_policy rvp,
                      cleanup_list *cleanup,
                      bool *is_new) noexcept {
    // Convert nullptr -> None
    if (!value) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    nb_internals *internals_ = internals;
    nb_ptr_map &inst_c2p = internals_->inst_c2p;
    type_data *td = nullptr;

    auto lookup_type = [cpp_type, internals_, &td]() -> bool {
        if (!td) {
            type_data *d = nb_type_c2p(internals_, cpp_type);
            if (!d)
                return false;
            td = d;
        }
        return true;
    };

    if (rvp != rv_policy::copy) {
        // Check if the instance is already registered with nanobind
        nb_ptr_map::iterator it = inst_c2p.find(value);

        if (it != inst_c2p.end()) {
            void *entry = it->second;
            nb_inst_seq seq;

            if (NB_UNLIKELY(nb_is_seq(entry))) {
                seq = *nb_get_seq(entry);
            } else {
                seq.inst = (PyObject *) entry;
                seq.next = nullptr;
            }

            while (true) {
                PyTypeObject *tp = Py_TYPE(seq.inst);

                if (nb_type_data(tp)->type == cpp_type) {
                    Py_INCREF(seq.inst);
                    return seq.inst;
                }

                if (!lookup_type())
                    return nullptr;

                if (PyType_IsSubtype(tp, td->type_py)) {
                    Py_INCREF(seq.inst);
                    return seq.inst;
                }

                if (seq.next == nullptr)
                    break;

                seq = *seq.next;
            }
        } else if (rvp == rv_policy::none) {
            return nullptr;
        }
    }

    // Look up the corresponding Python type if not already done
    if (!lookup_type())
        return nullptr;

    return nb_type_put_common(value, td, rvp, cleanup, is_new);
}

PyObject *nb_type_put_p(const std::type_info *cpp_type,
                        const std::type_info *cpp_type_p,
                        void *value, rv_policy rvp,
                        cleanup_list *cleanup,
                        bool *is_new) noexcept {
    // Convert nullptr -> None
    if (!value) {
        Py_INCREF(Py_None);
        return Py_None;
    }

    // Check if the instance is already registered with nanobind
    nb_internals *internals_ = internals;
    nb_ptr_map &inst_c2p = internals_->inst_c2p;

    // Look up the corresponding Python type
    type_data *td = nullptr,
              *td_p = nullptr;

    auto lookup_type = [cpp_type, cpp_type_p, internals_, &td, &td_p]() -> bool {
        if (!td) {
            type_data *d = nb_type_c2p(internals_, cpp_type);
            if (!d)
                return false;
            td = d;

            if (cpp_type_p && cpp_type_p != cpp_type)
                td_p = nb_type_c2p(internals_, cpp_type_p);
        }

        return true;
    };

    if (rvp != rv_policy::copy) {
        // Check if the instance is already registered with nanobind
        nb_ptr_map::iterator it = inst_c2p.find(value);

        if (it != inst_c2p.end()) {
            void *entry = it->second;
            nb_inst_seq seq;

            if (NB_UNLIKELY(nb_is_seq(entry))) {
                seq = *nb_get_seq(entry);
            } else {
                seq.inst = (PyObject *) entry;
                seq.next = nullptr;
            }

            while (true) {
                PyTypeObject *tp = Py_TYPE(seq.inst);

                const std::type_info *p = nb_type_data(tp)->type;

                if (p == cpp_type || p == cpp_type_p) {
                    Py_INCREF(seq.inst);
                    return seq.inst;
                }

                if (!lookup_type())
                    return nullptr;

                if (PyType_IsSubtype(tp, td->type_py) ||
                    (td_p && PyType_IsSubtype(tp, td_p->type_py))) {
                    Py_INCREF(seq.inst);
                    return seq.inst;
                }

                if (seq.next == nullptr)
                    break;

                seq = *seq.next;
            }
        } else if (rvp == rv_policy::none) {
            return nullptr;
        }
    }

    // Look up the corresponding Python type if not already done
    if (!lookup_type())
        return nullptr;

    return nb_type_put_common(value, td_p ? td_p : td, rvp, cleanup, is_new);
}

static void nb_type_put_unique_finalize(PyObject *o,
                                        const std::type_info *cpp_type,
                                        bool cpp_delete, bool is_new) {
    (void) cpp_type;
    check(cpp_delete || !is_new,
          "nanobind::detail::nb_type_put_unique(type='%s', cpp_delete=%i): "
          "ownership status has become corrupted.",
          type_name(cpp_type), cpp_delete);

    nb_inst *inst = (nb_inst *) o;

    if (cpp_delete) {
        check(inst->state == (is_new ? nb_inst::state_ready
                                     : nb_inst::state_relinquished) &&
                  (bool) inst->destruct == is_new &&
                  (bool) inst->cpp_delete == is_new,
              "nanobind::detail::nb_type_put_unique(type='%s', cpp_delete=%i): "
              "unexpected status flags! (state=%i, destruct=%i, cpp_delete=%i)",
              type_name(cpp_type), cpp_delete, inst->state, inst->destruct,
              inst->cpp_delete);

        inst->state = nb_inst::state_ready;
        inst->destruct = inst->cpp_delete = true;
    } else {
        check(inst->state == nb_inst::state_relinquished,
                  "nanobind::detail::nb_type_put_unique('%s'): ownership "
                  "status has become corrupted.", type_name(cpp_type));

        inst->state = nb_inst::state_ready;
    }
}

PyObject *nb_type_put_unique(const std::type_info *cpp_type,
                             void *value,
                             cleanup_list *cleanup, bool cpp_delete) noexcept {
    rv_policy policy = cpp_delete ? rv_policy::take_ownership : rv_policy::none;

    bool is_new = false;
    PyObject *o = nb_type_put(cpp_type, value, policy, cleanup, &is_new);

    if (o)
        nb_type_put_unique_finalize(o, cpp_type, cpp_delete, is_new);

    return o;
}

PyObject *nb_type_put_unique_p(const std::type_info *cpp_type,
                               const std::type_info *cpp_type_p,
                               void *value,
                               cleanup_list *cleanup, bool cpp_delete) noexcept {
    rv_policy policy = cpp_delete ? rv_policy::take_ownership : rv_policy::none;

    bool is_new = false;
    PyObject *o =
        nb_type_put_p(cpp_type, cpp_type_p, value, policy, cleanup, &is_new);

    if (o)
        nb_type_put_unique_finalize(o, cpp_type, cpp_delete, is_new);

    return o;
}

static void warn_relinquish_failed(const char *why, PyObject *o) noexcept {
    PyObject *name = nb_inst_name(o);
    int rc = PyErr_WarnFormat(
        PyExc_RuntimeWarning, 1,
        "nanobind::detail::nb_relinquish_ownership(): could not "
        "transfer ownership of a Python instance of type '%U' to C++. %s",
        name, why);
    if (rc != 0) // user has configured warnings-as-errors
        PyErr_WriteUnraisable(o);
    Py_DECREF(name);
}

bool nb_type_relinquish_ownership(PyObject *o, bool cpp_delete) noexcept {
    nb_inst *inst = (nb_inst *) o;

    /* This function is called after nb_type_get() succeeds, so the instance
       should be ready; but the !ready case is possible if an attempt is made to
       transfer ownership of the same object to C++ multiple times as part of
       the same data structure. For example, converting Python (foo, foo) to C++
       std::pair<std::unique_ptr<T>, std::unique_ptr<T>>. */

    if (inst->state != nb_inst::state_ready) {
        warn_relinquish_failed(
            "The resulting data structure would have multiple "
            "std::unique_ptrs, each thinking that they own the same instance, "
            "which is not allowed.", o);
        return false;
    }

    if (cpp_delete) {
        if (!inst->cpp_delete || !inst->destruct || inst->internal) {
            warn_relinquish_failed(
                "This is only possible when the instance was previously "
                "constructed on the C++ side and is now owned by Python, which "
                "was not the case here. You could change the unique pointer "
                "signature to std::unique_ptr<T, nb::deleter<T>> to work "
                "around this issue.", o);
            return false;
        }

        inst->cpp_delete = false;
        inst->destruct = false;
    }

    inst->state = nb_inst::state_relinquished;
    return true;
}

void nb_type_restore_ownership(PyObject *o, bool cpp_delete) noexcept {
    nb_inst *inst = (nb_inst *) o;

    check(inst->state == nb_inst::state_relinquished,
          "nanobind::detail::nb_type_restore_ownership('%s'): ownership "
          "status has become corrupted.",
          PyUnicode_AsUTF8AndSize(nb_inst_name(o), nullptr));

    inst->state = nb_inst::state_ready;
    if (cpp_delete) {
        inst->cpp_delete = true;
        inst->destruct = true;
    }
}

bool nb_type_isinstance(PyObject *o, const std::type_info *t) noexcept {
    type_data *d = nb_type_c2p(internals, t);
    if (d)
        return PyType_IsSubtype(Py_TYPE(o), d->type_py);
    else
        return false;
}

PyObject *nb_type_lookup(const std::type_info *t) noexcept {
    type_data *d = nb_type_c2p(internals, t);
    if (d)
        return (PyObject *) d->type_py;
    else
        return nullptr;
}

bool nb_type_check(PyObject *t) noexcept {
    PyTypeObject *meta  = Py_TYPE(t),
                 *meta2 = Py_TYPE((PyObject *) meta);

    return meta2 == nb_meta_cache;
}

size_t nb_type_size(PyObject *t) noexcept {
    return nb_type_data((PyTypeObject *) t)->size;
}

size_t nb_type_align(PyObject *t) noexcept {
    return nb_type_data((PyTypeObject *) t)->align;
}

const std::type_info *nb_type_info(PyObject *t) noexcept {
    return nb_type_data((PyTypeObject *) t)->type;
}

void *nb_type_supplement(PyObject *t) noexcept {
    return nb_type_data((PyTypeObject *) t) + 1;
}

PyObject *nb_inst_alloc(PyTypeObject *t) {
    PyObject *result = inst_new_int(t);
    if (!result)
        raise_python_error();
    return result;
}

PyObject *nb_inst_reference(PyTypeObject *t, void *ptr, PyObject *parent) {
    PyObject *result = inst_new_ext(t, ptr);
    if (!result)
        raise_python_error();
    nb_inst *nbi = (nb_inst *) result;
    nbi->destruct = nbi->cpp_delete = false;
    nbi->state = nb_inst::state_ready;
    if (parent)
        keep_alive(result, parent);
    return result;
}

PyObject *nb_inst_take_ownership(PyTypeObject *t, void *ptr) {
    PyObject *result = inst_new_ext(t, ptr);
    if (!result)
        raise_python_error();
    nb_inst *nbi = (nb_inst *) result;
    nbi->destruct = nbi->cpp_delete = true;
    nbi->state = nb_inst::state_ready;
    return result;
}

void *nb_inst_ptr(PyObject *o) noexcept {
    return inst_ptr((nb_inst *) o);
}

void nb_inst_zero(PyObject *o) noexcept {
    nb_inst *nbi = (nb_inst *) o;
    type_data *td = nb_type_data(Py_TYPE(o));
    memset(inst_ptr(nbi), 0, td->size);
    nbi->state = nb_inst::state_ready;
    nbi->destruct = true;
}

PyObject *nb_inst_alloc_zero(PyTypeObject *t) {
    PyObject *result = inst_new_int(t);
    if (!result)
        raise_python_error();
    nb_inst *nbi = (nb_inst *) result;
    type_data *td = nb_type_data(t);
    memset(inst_ptr(nbi), 0, td->size);
    nbi->state = nb_inst::state_ready;
    nbi->destruct = true;
    return result;
}

void nb_inst_set_state(PyObject *o, bool ready, bool destruct) noexcept {
    nb_inst *nbi = (nb_inst *) o;
    nbi->state = ready ? nb_inst::state_ready : nb_inst::state_uninitialized;
    nbi->destruct = destruct;
    nbi->cpp_delete = destruct && !nbi->internal;
}

std::pair<bool, bool> nb_inst_state(PyObject *o) noexcept {
    nb_inst *nbi = (nb_inst *) o;
    return { nbi->state == nb_inst::state_ready, (bool) nbi->destruct };
}

void nb_inst_destruct(PyObject *o) noexcept {
    nb_inst *nbi = (nb_inst *) o;
    type_data *t = nb_type_data(Py_TYPE(o));

    check(nbi->state != nb_inst::state_relinquished,
          "nanobind::detail::nb_inst_destruct(\"%s\"): attempted to destroy "
          "an object whose ownership had been transferred away!",
          t->name);

    if (nbi->destruct) {
        check(t->flags & (uint32_t) type_flags::is_destructible,
              "nanobind::detail::nb_inst_destruct(\"%s\"): attempted to call "
              "the destructor of a non-destructible type!",
              t->name);
        if (t->flags & (uint32_t) type_flags::has_destruct)
            t->destruct(inst_ptr(nbi));
        nbi->destruct = false;
    }

    nbi->state = nb_inst::state_uninitialized;
}

void nb_inst_copy(PyObject *dst, const PyObject *src) noexcept {
    if (src == dst)
        return;

    PyTypeObject *tp = Py_TYPE((PyObject *) src);
    type_data *t = nb_type_data(tp);

    check(tp == Py_TYPE(dst) &&
              (t->flags & (uint32_t) type_flags::is_copy_constructible),
          "nanobind::detail::nb_inst_copy(): invalid arguments!");

    nb_inst *nbi = (nb_inst *) dst;
    const void *src_data = inst_ptr((nb_inst *) src);
    void *dst_data = inst_ptr(nbi);

    if (t->flags & (uint32_t) type_flags::has_copy)
        t->copy(dst_data, src_data);
    else
        memcpy(dst_data, src_data, t->size);

    nbi->state = nb_inst::state_ready;
    nbi->destruct = true;
}

void nb_inst_move(PyObject *dst, const PyObject *src) noexcept {
    if (src == dst)
        return;

    PyTypeObject *tp = Py_TYPE((PyObject *) src);
    type_data *t = nb_type_data(tp);

    check(tp == Py_TYPE(dst) &&
              (t->flags & (uint32_t) type_flags::is_move_constructible),
          "nanobind::detail::nb_inst_move(): invalid arguments!");

    nb_inst *nbi = (nb_inst *) dst;
    void *src_data = inst_ptr((nb_inst *) src);
    void *dst_data = inst_ptr(nbi);

    if (t->flags & (uint32_t) type_flags::has_move) {
        t->move(dst_data, src_data);
    } else {
        memcpy(dst_data, src_data, t->size);
        memset(src_data, 0, t->size);
    }

    nbi->state = nb_inst::state_ready;
    nbi->destruct = true;
}

void nb_inst_replace_move(PyObject *dst, const PyObject *src) noexcept {
    if (src == dst)
        return;
    nb_inst *nbi = (nb_inst *) dst;
    bool destruct = nbi->destruct;
    nbi->destruct = true;
    nb_inst_destruct(dst);
    nb_inst_move(dst, src);
    nbi->destruct = destruct;
}

void nb_inst_replace_copy(PyObject *dst, const PyObject *src) noexcept {
    if (src == dst)
        return;
    nb_inst *nbi = (nb_inst *) dst;
    bool destruct = nbi->destruct;
    nbi->destruct = true;
    nb_inst_destruct(dst);
    nb_inst_copy(dst, src);
    nbi->destruct = destruct;
}

#if defined(Py_LIMITED_API)
type_data *nb_type_data_static(PyTypeObject *o) noexcept {
    return (type_data *) PyObject_GetTypeData((PyObject *) o, Py_TYPE((PyObject *) o));
}
#endif

PyObject *nb_type_name(PyObject *t) noexcept {
    error_scope s;

#if PY_VERSION_HEX >= 0x030B0000
    PyObject *result = PyType_GetName((PyTypeObject *) t);
#else
    PyObject *result = PyObject_GetAttrString(t, "__name__");
#endif

    if (PyType_HasFeature((PyTypeObject *) t, Py_TPFLAGS_HEAPTYPE)) {
        PyObject *mod = PyObject_GetAttrString(t, "__module__");
        PyObject *combined = PyUnicode_FromFormat("%U.%U", mod, result);
        Py_DECREF(mod);
        Py_DECREF(result);
        result = combined;
    }

    return result;
}

PyObject *nb_inst_name(PyObject *o) noexcept {
        return nb_type_name((PyObject *) Py_TYPE(o));
}

bool nb_inst_python_derived(PyObject *o) noexcept {
    return nb_type_data(Py_TYPE(o))->flags &
           (uint32_t) type_flags::is_python_type;
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
