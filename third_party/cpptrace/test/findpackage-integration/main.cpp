#include <cpptrace/cpptrace.hpp>

void trace() {
    cpptrace::generate_trace().print();
}

void foo(int) {
    trace();
}

int main() {
    foo(0);
}
