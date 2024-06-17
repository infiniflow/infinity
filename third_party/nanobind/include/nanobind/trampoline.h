/*
    nanobind/trampoline.h: functionality for overriding C++ virtual
    functions from within Python

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

struct ticket;

NB_CORE void trampoline_new(void **data, size_t size, void *ptr) noexcept;
NB_CORE void trampoline_release(void **data, size_t size) noexcept;
NB_CORE void trampoline_enter(void **data, size_t size, const char *name,
                              bool pure, ticket *ticket);
NB_CORE void trampoline_leave(ticket *ticket) noexcept;

template <size_t Size> struct trampoline {
    mutable void *data[2 * Size + 1];

    NB_INLINE trampoline(void *ptr) { trampoline_new(data, Size, ptr); }
    NB_INLINE ~trampoline() { trampoline_release(data, Size); }

    NB_INLINE handle base() const { return (PyObject *) data[0]; }
};

struct ticket {
    handle self;
    handle key;
    ticket *prev{};
    PyGILState_STATE state{};

    template <size_t Size>
    NB_INLINE ticket(const trampoline<Size> &t, const char *name, bool pure) {
        trampoline_enter(t.data, Size, name, pure, this);
    }

    NB_INLINE ~ticket() noexcept { trampoline_leave(this); }
};

#define NB_TRAMPOLINE(base, size)                                              \
    using NBBase = base;                                                       \
    using NBBase::NBBase;                                                      \
    nanobind::detail::trampoline<size> nb_trampoline{ this }

#define NB_OVERRIDE_NAME(name, func, ...)                                      \
    using nb_ret_type = decltype(NBBase::func(__VA_ARGS__));                   \
    nanobind::detail::ticket nb_ticket(nb_trampoline, name, false);            \
    if (nb_ticket.key.is_valid()) {                                                       \
        return nanobind::cast<nb_ret_type>(                                    \
            nb_trampoline.base().attr(nb_ticket.key)(__VA_ARGS__));            \
    } else                                                                     \
        return NBBase::func(__VA_ARGS__)

#define NB_OVERRIDE_PURE_NAME(name, func, ...)                                 \
    using nb_ret_type = decltype(NBBase::func(__VA_ARGS__));                   \
    nanobind::detail::ticket nb_ticket(nb_trampoline, name, true);             \
    return nanobind::cast<nb_ret_type>(                                        \
        nb_trampoline.base().attr(nb_ticket.key)(__VA_ARGS__))

#define NB_OVERRIDE(func, ...)                                                 \
    NB_OVERRIDE_NAME(#func, func, __VA_ARGS__)

#define NB_OVERRIDE_PURE(func, ...)                                            \
    NB_OVERRIDE_PURE_NAME(#func, func, __VA_ARGS__)

NAMESPACE_END(detail)
NAMESPACE_END(NB_NAMESPACE)
