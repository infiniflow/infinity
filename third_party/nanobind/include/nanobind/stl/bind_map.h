/*
    nanobind/stl/bind_map.h: Automatic creation of bindings for map-style containers

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE file.
*/

#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/make_iterator.h>
#include <nanobind/operators.h>
#include <nanobind/stl/detail/traits.h>

NAMESPACE_BEGIN(NB_NAMESPACE)
NAMESPACE_BEGIN(detail)

template <typename Map, typename Key, typename Value>
inline void map_set(Map &m, const Key &k, const Value &v) {
    if constexpr (detail::is_copy_assignable_v<Value>) {
        m[k] = v;
    } else {
        auto r = m.emplace(k, v);
        if (!r.second) {
            // Value is not copy-assignable. Erase and retry
            m.erase(r.first);
            m.emplace(k, v);
        }
    }
}

NAMESPACE_END(detail)

template <typename Map,
          rv_policy Policy = rv_policy::automatic_reference,
          typename... Args>
class_<Map> bind_map(handle scope, const char *name, Args &&...args) {
    using Key = typename Map::key_type;
    using Value = typename Map::mapped_type;

    using ValueRef = typename detail::iterator_value_access<typename Map::iterator>::result_type;

    static_assert(
        !detail::is_base_caster_v<detail::make_caster<Value>> ||
        detail::is_copy_constructible_v<Value> ||
        (Policy != rv_policy::automatic_reference &&
         Policy != rv_policy::copy),
        "bind_map(): the generated __getitem__ would copy elements, so the "
        "value type must be copy-constructible");

    handle cl_cur = type<Map>();
    if (cl_cur.is_valid()) {
        // Binding already exists, don't re-create
        return borrow<class_<Map>>(cl_cur);
    }

    auto cl = class_<Map>(scope, name, std::forward<Args>(args)...)
        .def(init<>(),
             "Default constructor")

        .def("__len__", [](const Map &m) { return m.size(); })

        .def("__bool__",
             [](const Map &m) { return !m.empty(); },
             "Check whether the map is nonempty")

        .def("__repr__",
             [](handle_t<Map> h) {
                return steal<str>(detail::repr_map(h.ptr()));
             })

        .def("__contains__",
             [](const Map &m, const Key &k) { return m.find(k) != m.end(); })

        .def("__contains__", // fallback for incompatible types
             [](const Map &, handle) { return false; })

        .def("__iter__",
             [](Map &m) {
                 return make_key_iterator<Policy>(type<Map>(), "KeyIterator",
                                                  m.begin(), m.end());
             },
             keep_alive<0, 1>())

        .def("__getitem__",
             [](Map &m, const Key &k) -> ValueRef {
                 auto it = m.find(k);
                 if (it == m.end())
                     throw key_error();
                 return (*it).second;
             }, Policy)

        .def("__delitem__",
            [](Map &m, const Key &k) {
                auto it = m.find(k);
                if (it == m.end())
                    throw key_error();
                m.erase(it);
            })

        .def("clear", [](Map &m) { m.clear(); },
             "Remove all items");


    if constexpr (detail::is_copy_constructible_v<Map>) {
        cl.def(init<const Map &>(), "Copy constructor");

        cl.def("__init__", [](Map *m, typed<dict, Key, Value> d) {
            new (m) Map();
            for (auto [k, v] : borrow<dict>(std::move(d)))
                m->emplace(cast<Key>(k), cast<Value>(v));
        }, "Construct from a dictionary");

        implicitly_convertible<dict, Map>();
    }

    // Assignment operator for copy-assignable/copy-constructible types
    if constexpr (detail::is_copy_assignable_v<Value> ||
                  detail::is_copy_constructible_v<Value>) {
        cl.def("__setitem__", [](Map &m, const Key &k, const Value &v) {
            detail::map_set<Map, Key, Value>(m, k, v);
        });

        cl.def("update", [](Map &m, const Map &m2) {
            for (auto &kv : m2)
                detail::map_set<Map, Key, Value>(m, kv.first, kv.second);
        },
        "Update the map with element from `arg`");
    }

    if constexpr (detail::is_equality_comparable_v<Map>) {
        cl.def(self == self)
          .def(self != self);
    }

    // Item, value, and key views
    struct KeyView   { Map &map; };
    struct ValueView { Map &map; };
    struct ItemView  { Map &map; };

    class_<ItemView>(cl, "ItemView")
        .def("__len__", [](ItemView &v) { return v.map.size(); })
        .def("__iter__",
             [](ItemView &v) {
                 return make_iterator<Policy>(type<Map>(), "ItemIterator",
                                              v.map.begin(), v.map.end());
             },
             keep_alive<0, 1>());

    class_<KeyView>(cl, "KeyView")
        .def("__contains__", [](KeyView &v, const Key &k) { return v.map.find(k) != v.map.end(); })
        .def("__contains__", [](KeyView &, handle) { return false; })
        .def("__len__", [](KeyView &v) { return v.map.size(); })
        .def("__iter__",
             [](KeyView &v) {
                 return make_key_iterator<Policy>(type<Map>(), "KeyIterator",
                                                  v.map.begin(), v.map.end());
             },
             keep_alive<0, 1>());

    class_<ValueView>(cl, "ValueView")
        .def("__len__", [](ValueView &v) { return v.map.size(); })
        .def("__iter__",
             [](ValueView &v) {
                 return make_value_iterator<Policy>(type<Map>(), "ValueIterator",
                                                    v.map.begin(), v.map.end());
             },
             keep_alive<0, 1>());

    cl.def("keys",   [](Map &m) { return new KeyView{m};   }, keep_alive<0, 1>(),
           "Returns an iterable view of the map's keys.");
    cl.def("values", [](Map &m) { return new ValueView{m}; }, keep_alive<0, 1>(),
           "Returns an iterable view of the map's values.");
    cl.def("items",  [](Map &m) { return new ItemView{m};  }, keep_alive<0, 1>(),
           "Returns an iterable view of the map's items.");

    return cl;
}

NAMESPACE_END(NB_NAMESPACE)
