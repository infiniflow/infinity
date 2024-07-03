#pragma once

#include <nanobind/nanobind.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename Array, typename Entry, size_t Size> struct array_caster {
    NB_TYPE_CASTER(Array, io_name(NB_TYPING_SEQUENCE, NB_TYPING_LIST) +
                              const_name("[") + make_caster<Entry>::Name +
                              const_name("]"))

    using Caster = make_caster<Entry>;

    bool from_python(handle src, uint8_t flags, cleanup_list *cleanup) noexcept {
        PyObject *temp;

        /* Will initialize 'temp' (NULL in the case of a failure.) */
        PyObject **o = seq_get_with_size(src.ptr(), Size, &temp);

        Caster caster;
        bool success = o != nullptr;

        flags = flags_for_local_caster<Entry>(flags);

        if (success) {
            for (size_t i = 0; i < Size; ++i) {
                if (!caster.from_python(o[i], flags, cleanup) ||
                    !caster.template can_cast<Entry>()) {
                    success = false;
                    break;
                }

                value[i] = caster.operator cast_t<Entry>();
            }

            Py_XDECREF(temp);
        }

        return success;
    }

    template <typename T>
    static handle from_cpp(T &&src, rv_policy policy, cleanup_list *cleanup) {
        object ret = steal(PyList_New(Size));

        if (ret.is_valid()) {
            Py_ssize_t index = 0;

            for (auto &value : src) {
                handle h = Caster::from_cpp(forward_like_<T>(value), policy, cleanup);

                if (!h.is_valid()) {
                    ret.reset();
                    break;
                }

                NB_LIST_SET_ITEM(ret.ptr(), index++, h.ptr());
            }
        }

        return ret.release();
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
