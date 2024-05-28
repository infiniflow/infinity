#include <nanobind/make_iterator.h>
#include <nanobind/stl/unordered_map.h>
#include <nanobind/stl/string.h>

namespace nb = nanobind;

NB_MODULE(test_make_iterator_ext, m) {
    struct StringMap {
        std::unordered_map<std::string, std::string> map;
        decltype(map.cbegin()) begin() const { return map.cbegin(); }
        decltype(map.cend()) end() const { return map.cend(); }
    };

    nb::class_<StringMap>(m, "StringMap")
        .def(nb::init<>())
        .def(nb::init<std::unordered_map<std::string, std::string>>())
        .def("__iter__",
             [](const StringMap &map) {
                 return nb::make_key_iterator(nb::type<StringMap>(),
                                              "key_iterator",
                                              map.begin(),
                                              map.end());
             }, nb::keep_alive<0, 1>())
        .def("items",
             [](const StringMap &map) {
                 return nb::make_iterator(nb::type<StringMap>(),
                                          "item_iterator",
                                          map.begin(),
                                          map.end());
             }, nb::keep_alive<0, 1>())
        .def("values", [](const StringMap &map) {
            return nb::make_value_iterator(nb::type<StringMap>(),
                                           "value_iterator",
                                           map.begin(),
                                           map.end());
        }, nb::keep_alive<0, 1>());

    nb::handle mod = m;
    m.def("iterator_passthrough", [mod](nb::iterator s) -> nb::iterator {
        return nb::make_iterator(mod, "pt_iterator", std::begin(s), std::end(s));
    });

    // test of map where dereferencing the iterator returns a value,
    // not a reference
    struct IdentityMap {
        struct iterator {
            int val;
            std::pair<int, int> operator*() const { return {val, val}; }
            iterator& operator++() { ++val; return *this; }
            bool operator==(const iterator& other) const {
                return val == other.val;
            }
        };

        iterator begin() const { return iterator{0}; }
        iterator end() const { return iterator{10}; }
    };

    nb::class_<IdentityMap>(m, "IdentityMap")
        .def(nb::init<>())
        .def("__iter__",
             [](const IdentityMap &map) {
                 return nb::make_key_iterator(nb::type<IdentityMap>(),
                                              "key_iterator",
                                              map.begin(),
                                              map.end());
             }, nb::keep_alive<0, 1>())
        .def("items",
             [](const IdentityMap &map) {
                 return nb::make_iterator(nb::type<IdentityMap>(),
                                          "item_iterator",
                                          map.begin(),
                                          map.end());
             }, nb::keep_alive<0, 1>())
        .def("values", [](const IdentityMap &map) {
            return nb::make_value_iterator(nb::type<IdentityMap>(),
                                           "value_iterator",
                                           map.begin(),
                                           map.end());
        }, nb::keep_alive<0, 1>());

    nb::list all;
    all.append("iterator_passthrough");
    all.append("StringMap");
    all.append("IdentityMap");
    m.attr("__all__") = all;
}
