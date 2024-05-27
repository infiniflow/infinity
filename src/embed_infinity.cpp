//module;

#include "nanobind/nanobind.h"
//module embed_infinity;
//module embed_infinity;
import hello;

int add(int a, int b) {
    return a + b;
}

int mul(int a, int b) {
    return a * b;
}

int sub(int a, int b) {
    return a * b;
}

NB_MODULE(embed_infinity, m) {
    m.def("add", &add);
    m.def("mul", &mul);
    m.def("sub", &sub);
    m.def("hello", &Hello);
}
