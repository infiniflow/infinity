#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <nanobind/stl/bind_map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

namespace nb = nanobind;

// testing for insertion of non-copyable class
class E_nc {
public:
    explicit E_nc(int i) : value{i} {}
    E_nc(const E_nc &) = delete;
    E_nc &operator=(const E_nc &) = delete;
    E_nc(E_nc &&) = default;
    E_nc &operator=(E_nc &&) = default;

    int value;
};

template <class Map>
Map *times_ten(int n) {
    auto *m = new Map();
    for (int i = 1; i <= n; i++) {
        m->emplace(int(i), E_nc(10 * i));
    }
    return m;
}

template <class NestMap>
NestMap *times_hundred(int n) {
    auto *m = new NestMap();
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= n; j++) {
            (*m)[i].emplace(int(j * 10), E_nc(100 * j));
        }
    }
    return m;
}

NB_MODULE(test_bind_map_ext, m) {
    // test_map_string_double
    nb::bind_map<std::map<std::string, double>>(m, "MapStringDouble");
    nb::bind_map<std::unordered_map<std::string, double>>(m, "UnorderedMapStringDouble");
    // test_map_string_double_const
    nb::bind_map<std::map<std::string, double const>>(m, "MapStringDoubleConst");
    nb::bind_map<std::unordered_map<std::string, double const>>(m,
                                                                "UnorderedMapStringDoubleConst");

    nb::class_<E_nc>(m, "ENC").def(nb::init<int>()).def_rw("value", &E_nc::value);

    // By default, the bindings produce a __getitem__ that makes a copy, which
    // won't take this non-copyable type: (uncomment to verify build error)
    //nb::bind_map<std::map<int, E_nc>>(m, "MapENC");
    //nb::bind_map<std::unordered_map<int, E_nc>>(m, "UmapENC");

    // But we can request reference semantics instead (some care required, read
    // the documentation):
    nb::bind_map<std::map<int, E_nc>,
                 nb::rv_policy::reference_internal>(m, "MapENC");
    nb::bind_map<std::unordered_map<int, E_nc>,
                 nb::rv_policy::reference_internal>(m, "UmapENC");

    m.def("get_mnc", &times_ten<std::map<int, E_nc>>);
    m.def("get_umnc", &times_ten<std::unordered_map<int, E_nc>>);

    // pybind11 issue #1885: binding nested std::map<X, Container<E>>
    // with E non-copyable
    nb::bind_map<std::map<int, std::vector<E_nc>>,
                 nb::rv_policy::reference_internal>(m, "MapVecENC");
    m.def("get_nvnc", [](int n) {
        auto *m = new std::map<int, std::vector<E_nc>>();
        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= n; j++) {
                (*m)[i].emplace_back(j);
            }
        }
        return m;
    });

    nb::bind_map<std::map<int, std::map<int, E_nc>>,
                 nb::rv_policy::reference_internal>(m, "MapMapENC");
    m.def("get_nmnc", &times_hundred<std::map<int, std::map<int, E_nc>>>);
    nb::bind_map<std::unordered_map<int, std::unordered_map<int, E_nc>>,
                 nb::rv_policy::reference_internal>(m, "UmapUmapENC");
    m.def("get_numnc", &times_hundred<std::unordered_map<int, std::unordered_map<int, E_nc>>>);
}
