#include "nanobind/nanobind.h"
import infinity;

int add(int a, int b) {
    return a + b;
}

int mul(int a, int b) {
    return a * b;
}

NB_MODULE(embedded_infinity, m) {
    m.def("add", &add);
    m.def("mul", &mul);
    m.def("hello", &infinity::Infinity::Hello);
}
