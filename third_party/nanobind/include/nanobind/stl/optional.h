/*
    nanobind/stl/optional.h: type caster for std::optional<...>

    Copyright (c) 2022 Yoshiki Matsuda and Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <optional>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename T> struct remove_opt_mono<std::optional<T>>
    : remove_opt_mono<T> { };

template <typename T>
struct type_caster<std::optional<T>> {
    using Caster = make_caster<T>;

    NB_TYPE_CASTER(std::optional<T>, optional_name(Caster::Name))

    type_caster() : value(std::nullopt) { }

    bool from_python(handle src, uint8_t flags, cleanup_list* cleanup) noexcept {
        if (src.is_none()) {
            value = std::nullopt;
            return true;
        }

        Caster caster;
        if (!caster.from_python(src, flags_for_local_caster<T>(flags), cleanup) ||
            !caster.template can_cast<T>())
            return false;

        value.emplace(caster.operator cast_t<T>());

        return true;
    }

    template <typename T_>
    static handle from_cpp(T_ &&value, rv_policy policy, cleanup_list *cleanup) noexcept {
        if (!value)
            return none().release();

        return Caster::from_cpp(forward_like_<T_>(*value), policy, cleanup);
    }
};

template <> struct type_caster<std::nullopt_t> {
    bool from_python(handle src, uint8_t, cleanup_list *) noexcept {
        if (src.is_none())
            return true;
        return false;
    }

    static handle from_cpp(std::nullopt_t, rv_policy, cleanup_list *) noexcept {
        return none().release();
    }

    NB_TYPE_CASTER(std::nullopt_t, const_name("None"))
};

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
