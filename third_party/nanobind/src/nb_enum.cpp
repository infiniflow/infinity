#include "nb_internals.h"

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

struct int64_hash {
    size_t operator()(const int64_t value) const {
        return (size_t) fmix64((uint64_t) value);
    }
};

// This data structure is used to map Python instances to integers as well as
// the inverse. We're reusing the type to avoid generating essentially the same
// code for two template instantiations. The key/value types are big enough to
// hold both.
using enum_map = tsl::robin_map<int64_t, int64_t, int64_hash>;

PyObject *enum_create(enum_init_data *ed) noexcept {
    // Update hash table that maps from std::type_info to Python type
    auto [it, success] = internals->type_c2p_slow.try_emplace(ed->type, nullptr);
    if (!success) {
        PyErr_WarnFormat(PyExc_RuntimeWarning, 1, "nanobind: type '%s' was already registered!\n", ed->name);
        PyObject *tp = (PyObject *) it->second->type_py;
        Py_INCREF(tp);
        return tp;
    }

    handle scope(ed->scope);

    bool is_arithmetic = ed->flags & (uint32_t) type_flags::is_arithmetic;

    str name(ed->name), qualname = name;
    object modname;

    if (PyModule_Check(ed->scope)) {
        modname = getattr(scope, "__name__", handle());
    } else {
        modname = getattr(scope, "__module__", handle());

        object scope_qualname = getattr(scope, "__qualname__", handle());
        if (scope_qualname.is_valid())
            qualname = steal<str>(
                PyUnicode_FromFormat("%U.%U", scope_qualname.ptr(), name.ptr()));
    }

    const char *factory_name = is_arithmetic ? "IntEnum" : "Enum";

    object enum_mod = module_::import_("enum"),
           factory = enum_mod.attr(factory_name),
           result = factory(name, nanobind::tuple(),
                            arg("module") = modname,
                            arg("qualname") = qualname);

    scope.attr(name) = result;
    result.attr("__doc__") = ed->docstr ? str(ed->docstr) : none();

    if (is_arithmetic)
        result.attr("__str__") = enum_mod.attr("Enum").attr("__str__");

    result.attr("__repr__") = result.attr("__str__");

    type_init_data *t = new type_init_data();
    memset(t, 0, sizeof(type_data));
    t->name = strdup_check(ed->name);
    t->type = ed->type;
    t->type_py = (PyTypeObject *) result.ptr();
    t->flags = ed->flags;
    t->enum_tbl.fwd = new enum_map();
    t->enum_tbl.rev = new enum_map();
    t->scope = ed->scope;

    it.value() = t;

    internals->type_c2p_fast[ed->type] = t;
    internals->type_c2p_slow[ed->type] = t;

    result.attr("__nb_enum__") = capsule(t, [](void *p) noexcept {
        type_init_data *t = (type_init_data *) p;
        delete (enum_map *) t->enum_tbl.fwd;
        delete (enum_map *) t->enum_tbl.rev;
        nb_type_unregister(t);
        free((char*)t->name);
        delete t;
    });

    return result.release().ptr();
}

static type_init_data *enum_get_type_data(handle tp) {
    return (type_init_data *) (borrow<capsule>(handle(tp).attr("__nb_enum__"))).data();
}

void enum_append(PyObject *tp_, const char *name_, int64_t value_,
                 const char *doc) noexcept {
    handle tp(tp_),
           val_tp(&PyLong_Type),
           obj_tp((PyObject *) &PyBaseObject_Type);

    type_data *t = enum_get_type_data(tp);

    object val;
    if (t->flags & (uint32_t) type_flags::is_signed)
        val = steal(PyLong_FromLongLong((long long) value_));
    else
        val = steal(PyLong_FromUnsignedLongLong((unsigned long long) value_));

    dict value_map = tp.attr("_value2member_map_"),
         member_map = tp.attr("_member_map_");
    list member_names = tp.attr("_member_names_");
    str name(name_);

    if (member_map.contains(name))
        fail("refusing to add duplicate key \"%s\" to enumeration \"%s\"!",
             name_, type_name(tp).c_str());

    object el;
    if (issubclass(tp, val_tp))
        el = val_tp.attr("__new__")(tp, val);
    else
        el = obj_tp.attr("__new__")(tp);

    el.attr("_name_") = name;
    el.attr("__objclass__") = tp;
    el.attr("__init__")(val);
    el.attr("_sort_order_") = len(member_names);
    el.attr("_value_") = val;
    el.attr("__doc__") = doc ? str(doc) : none();

    // Compatibility with nanobind 1.x
    el.attr("__name__") = name;

    setattr(tp, name, el);

    if (!value_map.contains(val)) {
        member_names.append(name);
        value_map[val] = el;
    }

    member_map[name] = el;

    enum_map *fwd = (enum_map *) t->enum_tbl.fwd;
    fwd->emplace(value_, (int64_t) (uintptr_t) el.ptr());

    enum_map *rev = (enum_map *) t->enum_tbl.rev;
    rev->emplace((int64_t) (uintptr_t) el.ptr(), value_);
}

bool enum_from_python(const std::type_info *tp, PyObject *o, int64_t *out, uint8_t flags) noexcept {
    type_data *t = nb_type_c2p(internals, tp);
    if (!t)
        return false;

    enum_map *rev = (enum_map *) t->enum_tbl.rev;
    enum_map::iterator it = rev->find((int64_t) (uintptr_t) o);

    if (it != rev->end()) {
        *out = it->second;
        return true;
    }

    if (flags & (uint8_t) cast_flags::convert) {
        enum_map *fwd = (enum_map *) t->enum_tbl.fwd;

        if (t->flags & (uint32_t) type_flags::is_signed) {
            long long value = PyLong_AsLongLong(o);
            if (value == -1 && PyErr_Occurred()) {
                PyErr_Clear();
                return false;
            }
            enum_map::iterator it2 = fwd->find((int64_t) value);
            if (it2 != fwd->end()) {
                *out = (int64_t) value;
                return true;
            }
        } else {
            unsigned long long value = PyLong_AsUnsignedLongLong(o);
            if (value == (unsigned long long) -1 && PyErr_Occurred()) {
                PyErr_Clear();
                return false;
            }
            enum_map::iterator it2 = fwd->find((int64_t) value);
            if (it2 != fwd->end()) {
                *out = (int64_t) value;
                return true;
            }
        }

    }

    return false;
}

PyObject *enum_from_cpp(const std::type_info *tp, int64_t key) noexcept {
    type_data *t = nb_type_c2p(internals, tp);
    if (!t)
        return nullptr;

    enum_map *fwd = (enum_map *) t->enum_tbl.fwd;

    enum_map::iterator it = fwd->find(key);
    if (it != fwd->end()) {
        PyObject *value = (PyObject *) it->second;
        Py_INCREF(value);
        return value;
    }

    if (t->flags & (uint32_t) type_flags::is_signed)
        PyErr_Format(PyExc_ValueError, "%lli is not a valid %s.",
                     (long long) key, t->name);
    else
        PyErr_Format(PyExc_ValueError, "%llu is not a valid %s.",
                     (unsigned long long) key, t->name);
    return nullptr;
}

void enum_export(PyObject *tp) {
    type_init_data *t = enum_get_type_data(tp);

    handle scope = t->scope;
    for (handle item: handle(tp))
        scope.attr(item.attr("name")) = item;
}

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
