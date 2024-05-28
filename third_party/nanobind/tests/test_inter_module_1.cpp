#include <nanobind/nanobind.h>
#include "inter_module.h"

namespace nb = nanobind;

NB_MODULE(test_inter_module_1_ext, m) {
    m.def("create_shared", &create_shared);
}
