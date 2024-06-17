/*
    nanobind/stl/bind_vector.h: Automatic creation of bindings for vector-style containers

    Copyright (c) 2022 Wenzel Jakob

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/make_iterator.h>
#include <nanobind/stl/detail/traits.h>
#include <vector>
#include <algorithm>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

inline size_t wrap(Py_ssize_t i, size_t n) {
    if (i < 0)
        i += (Py_ssize_t) n;

    if (i < 0 || (size_t) i >= n)
        throw index_error();

    return (size_t) i;
}

template <> struct iterator_access<typename std::vector<bool>::iterator> {
    using result_type = bool;
    result_type operator()(typename std::vector<bool>::iterator &it) const { return *it; }
};

NAMESPACE_END(detail)


template <typename Vector,
          rv_policy Policy = rv_policy::automatic_reference,
          typename... Args>
class_<Vector> bind_vector(handle scope, const char *name, Args &&...args) {
    using ValueRef = typename detail::iterator_access<typename Vector::iterator>::result_type;
    using Value = std::decay_t<ValueRef>;

    static_assert(
        !detail::is_base_caster_v<detail::make_caster<Value>> ||
        detail::is_copy_constructible_v<Value> ||
        (Policy != rv_policy::automatic_reference &&
         Policy != rv_policy::copy),
        "bind_vector(): the generated __getitem__ would copy elements, so the "
        "element type must be copy-constructible");

    handle cl_cur = type<Vector>();
    if (cl_cur.is_valid()) {
        // Binding already exists, don't re-create
        return borrow<class_<Vector>>(cl_cur);
    }

    auto cl = class_<Vector>(scope, name, std::forward<Args>(args)...)
        .def(init<>(), "Default constructor")

        .def("__len__", [](const Vector &v) { return v.size(); })

        .def("__bool__",
             [](const Vector &v) { return !v.empty(); },
             "Check whether the vector is nonempty")

        .def("__repr__",
             [](handle_t<Vector> h) {
                return steal<str>(detail::repr_list(h.ptr()));
             })

        .def("__iter__",
             [](Vector &v) {
                 return make_iterator<Policy>(type<Vector>(), "Iterator",
                                              v.begin(), v.end());
             }, keep_alive<0, 1>())

        .def("__getitem__",
             [](Vector &v, Py_ssize_t i) -> ValueRef {
                 return v[detail::wrap(i, v.size())];
             }, Policy)

        .def("clear", [](Vector &v) { v.clear(); },
             "Remove all items from list.");

    if constexpr (detail::is_copy_constructible_v<Value>) {
        cl.def(init<const Vector &>(),
               "Copy constructor");

        cl.def("__init__", [](Vector *v, typed<iterable, Value> seq) {
            new (v) Vector();
            v->reserve(len_hint(seq));
            for (handle h : seq)
                v->push_back(cast<Value>(h));
        }, "Construct from an iterable object");

        implicitly_convertible<iterable, Vector>();

        cl.def("append",
               [](Vector &v, const Value &value) { v.push_back(value); },
               "Append `arg` to the end of the list.")

          .def("insert",
               [](Vector &v, Py_ssize_t i, const Value &x) {
                   if (i < 0)
                       i += (Py_ssize_t) v.size();
                   if (i < 0 || (size_t) i > v.size())
                       throw index_error();
                   v.insert(v.begin() + i, x);
               },
               "Insert object `arg1` before index `arg0`.")

           .def("pop",
                [](Vector &v, Py_ssize_t i) {
                    size_t index = detail::wrap(i, v.size());
                    Value result = std::move(v[index]);
                    v.erase(v.begin() + index);
                    return result;
                },
                arg("index") = -1,
                "Remove and return item at `index` (default last).")

          .def("extend",
               [](Vector &v, const Vector &src) {
                   v.insert(v.end(), src.begin(), src.end());
               },
               "Extend `self` by appending elements from `arg`.")

          .def("__setitem__",
               [](Vector &v, Py_ssize_t i, const Value &value) {
                   v[detail::wrap(i, v.size())] = value;
               })

          .def("__delitem__",
               [](Vector &v, Py_ssize_t i) {
                   v.erase(v.begin() + detail::wrap(i, v.size()));
               })

          .def("__getitem__",
               [](const Vector &v, const slice &slice) -> Vector * {
                   auto [start, stop, step, length] = slice.compute(v.size());
                   auto *seq = new Vector();
                   seq->reserve(length);

                   for (size_t i = 0; i < length; ++i) {
                       seq->push_back(v[start]);
                       start += step;
                   }

                   return seq;
               })

          .def("__setitem__",
               [](Vector &v, const slice &slice, const Vector &value) {
                   auto [start, stop, step, length] = slice.compute(v.size());

                   if (length != value.size())
                       throw index_error(
                           "The left and right hand side of the slice "
                           "assignment have mismatched sizes!");

                   for (size_t i = 0; i < length; ++i) {
                       v[start] = value[i];
                       start += step;
                    }
               })

          .def("__delitem__",
               [](Vector &v, const slice &slice) {
                   auto [start, stop, step, length] = slice.compute(v.size());
                   if (length == 0)
                       return;

                   stop = start + (length - 1) * step;
                   if (start > stop) {
                       std::swap(start, stop);
                       step = -step;
                   }

                   if (step == 1) {
                       v.erase(v.begin() + start, v.begin() + stop + 1);
                   } else {
                       for (size_t i = 0; i < length; ++i) {
                           v.erase(v.begin() + stop);
                           stop -= step;
                       }
                   }
               });
    }

    if constexpr (detail::is_equality_comparable_v<Value>) {
        cl.def(self == self)
          .def(self != self)

          .def("__contains__",
               [](const Vector &v, const Value &x) {
                   return std::find(v.begin(), v.end(), x) != v.end();
               })

          .def("__contains__", // fallback for incompatible types
               [](const Vector &, handle) { return false; })

          .def("count",
               [](const Vector &v, const Value &x) {
                   return std::count(v.begin(), v.end(), x);
               }, "Return number of occurrences of `arg`.")

          .def("remove",
               [](Vector &v, const Value &x) {
                   auto p = std::find(v.begin(), v.end(), x);
                   if (p != v.end())
                       v.erase(p);
                   else
                       throw value_error();
               },
               "Remove first occurrence of `arg`.");
    }

    return cl;
}

NAMESPACE_END(NB_NAMESPACE)
