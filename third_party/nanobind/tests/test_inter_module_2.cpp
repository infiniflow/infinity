#include <nanobind/nanobind.h>
#include "inter_module.h"

namespace nb = nanobind;

NB_MODULE(test_inter_module_2_ext, m) {
    nb::class_<Shared>(m, "Shared");
    m.def("check_shared", &check_shared);
}
