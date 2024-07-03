/*
    nanobind/stl/complex.h: type caster for std::complex<...>

    Copyright (c) 2023 Degottex Gilles and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <complex>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename T> struct type_caster<std::complex<T>> {
    NB_TYPE_CASTER(std::complex<T>, const_name("complex"))

    template <bool Recursive = true>
    bool from_python(handle src, uint8_t flags,
                     cleanup_list *cleanup) noexcept {
        (void) flags;
        (void) cleanup;

        if (PyComplex_Check(src.ptr())) {
            value = std::complex<T>(
                (T) PyComplex_RealAsDouble(src.ptr()),
                (T) PyComplex_ImagAsDouble(src.ptr())
            );
            return true;
        }

        if constexpr (Recursive) {
            if (!PyFloat_CheckExact(src.ptr()) &&
                !PyLong_CheckExact(src.ptr()) &&
                PyObject_HasAttrString(src.ptr(), "imag")) {
                try {
                    object tmp = handle(&PyComplex_Type)(src);
                    return from_python<false>(tmp, flags, cleanup);
                } catch (...) {
                    return false;
                }
            }
        }

        make_caster<T> caster;
        if (caster.from_python(src, flags, cleanup)) {
            value = std::complex<T>(caster.operator cast_t<T>());
            return true;
        }

        return false;
    }

    template <typename T2>
    static handle from_cpp(T2 &&value, rv_policy policy,
                           cleanup_list *cleanup) noexcept {
        (void) policy;
        (void) cleanup;

        return PyComplex_FromDoubles((double) value.real(),
                                     (double) value.imag());
    }
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
