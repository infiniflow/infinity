/*
    nanobind/stl/tuple.h: type caster for std::tuple<...>

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <tuple>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename... Ts> struct type_caster<std::tuple<Ts...>> {
    static constexpr size_t N  = sizeof...(Ts),
                            N1 = N > 0 ? N : 1;

    using Value = std::tuple<Ts...>;
    using Indices = std::make_index_sequence<N>;

    static constexpr auto Name =
        const_name(NB_TYPING_TUPLE "[") +
        const_name<N == 0>(const_name("()"), concat(make_caster<Ts>::Name...)) +
        const_name("]");

    /// This caster constructs instances on the fly (otherwise it would not be
    /// able to handle tuples containing references_). Because of this, only the
    /// `operator Value()` cast operator is implemented below, and the type
    /// alias below informs users of this class of this fact.
    template <typename T> using Cast = Value;

    bool from_python(handle src, uint8_t flags,
                     cleanup_list *cleanup) noexcept {
        return from_python_impl(src, flags, cleanup, Indices{});
    }

    template <size_t... Is>
    bool from_python_impl(handle src, uint8_t flags, cleanup_list *cleanup,
                          std::index_sequence<Is...>) noexcept {
        (void) src; (void) flags; (void) cleanup;

        PyObject *temp; // always initialized by the following line
        PyObject **o = seq_get_with_size(src.ptr(), N, &temp);

        bool success =
            (o && ... &&
             std::get<Is>(casters).from_python(o[Is], flags, cleanup));

        Py_XDECREF(temp);

        return success;
    }

    template <typename T>
    static handle from_cpp(T&& value, rv_policy policy,
                           cleanup_list *cleanup) noexcept {
        return from_cpp_impl((forward_t<T>) value, policy, cleanup, Indices{});
    }

    template <typename T>
    static handle from_cpp(T *value, rv_policy policy, cleanup_list *cleanup) {
        if (!value)
            return none().release();
        return from_cpp_impl(*value, policy, cleanup, Indices{});
    }

    template <typename T, size_t... Is>
    static handle from_cpp_impl(T &&value, rv_policy policy,
                                cleanup_list *cleanup,
                                std::index_sequence<Is...>) noexcept {
        (void) value; (void) policy; (void) cleanup;
        object o[N1];

        bool success =
            (... &&
             ((o[Is] = steal(make_caster<Ts>::from_cpp(
                   forward_like_<T>(std::get<Is>(value)), policy, cleanup))),
              o[Is].is_valid()));

        if (!success)
            return handle();

        PyObject *r = PyTuple_New(N);
        (NB_TUPLE_SET_ITEM(r, Is, o[Is].release().ptr()), ...);
        return r;
    }

    template <typename T>
    bool can_cast() const noexcept { return can_cast_impl(Indices{}); }

    explicit operator Value() { return cast_impl(Indices{}); }

    template <size_t... Is>
    bool can_cast_impl(std::index_sequence<Is...>) const noexcept {
        return (std::get<Is>(casters).template can_cast<Ts>() && ...);
    }
    template <size_t... Is> Value cast_impl(std::index_sequence<Is...>) {
        return Value(std::get<Is>(casters).operator cast_t<Ts>()...);
    }

    std::tuple<make_caster<Ts>...> casters;
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
