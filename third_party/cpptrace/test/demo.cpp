#include <cpptrace/cpptrace.hpp>
#include <cpptrace/version.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

void trace() {
    cpptrace::generate_trace().print();
    cpptrace::generate_trace().print_with_snippets();
    throw cpptrace::logic_error("foobar");
}

void foo(int n) {
    if(n == 0) {
        trace();
    } else {
        foo(n - 1);
    }
}

template<typename... Args>
void foo(int, Args... args) {
    foo(args...);
}

void function_two(int, float) {
    foo(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
}

void function_one(int) {
    function_two(0, 0);
}

int main() {
    cpptrace::absorb_trace_exceptions(false);
    cpptrace::register_terminate_handler();
    function_one(0);
}
