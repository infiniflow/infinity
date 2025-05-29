#include <cpptrace/cpptrace.hpp>

#include <algorithm>
#include <array>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>

std::string normalize_filename(std::string name) {
    if(name.find('/') == 0 || (name.find(':') == 1 && std::isupper(name[0]))) {
        // build/integration if the file is really an object name resolved by libdl
        auto p = std::min({name.rfind("test/"), name.rfind("test\\"), name.rfind("build/integration")});
        return p == std::string::npos ? name : name.substr(p);
    } else {
        return name;
    }
}

void custom_print(const cpptrace::stacktrace&);

void trace() {
    auto trace = cpptrace::generate_trace();
    if(trace.empty()) {
        std::cerr << "<empty trace>" << std::endl;
    }
    custom_print(trace);
}

// padding to avoid upsetting existing trace expected files

void www(std::string&&, const std::string&, std::vector<std::string*>&&) {
    trace();
}

void jjj(void(*const[5])(float)) {
    www(std::string{}, "", {});
}

namespace Foo {
    struct Bar {};
}

void iii(Foo::Bar) {
    jjj(nullptr);
}

struct S {
    int foo(float) const volatile && {
        return 1;
    }
};

void hhh(int(*(*)[10])[20]) {
    iii(Foo::Bar{});
}

void ggg(const int * const *) {
    hhh(nullptr);
}

void fff(int(S::*)(float) const volatile &&) {
    ggg(nullptr);
}

//void eee(int(*(*(*)[10])())(float)) {
void eee(int(*(* const * volatile(*)[10])())(float)) {
    fff(&S::foo);
}

void ddd(int(*(*)[10])()) {
    eee(nullptr);
}

void ccc(int(*)[5][6][7][8]) {
    ddd(nullptr);
}

void bbb(int(*const (&)[5])(float, const int&)) {
    ccc(nullptr);
}

void aaa(int(&)[5]) {
    int(*const (arr)[5])(float, const int&) = {};
    bbb(arr);
}

int x;

void foo(int n) {
    if(n == 0) {
        x = 0;
        int arr[5];
        aaa(arr);
        x = 0;
    } else {
        x = 0;
        foo(n - 1);
        x = 0;
    }
}

template<typename... Args>
void foo(int, Args... args) {
    x = 0;
    foo(args...);
    x = 0;
}

void function_two(int, float) {
    x = 0;
    foo(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    x = 0;
}

void function_one(int) {
    x = 0;
    function_two(0, 0);
    x = 0;
}

int main() {
    x = 0;
    cpptrace::absorb_trace_exceptions(false);
    function_one(0);
    x = 0;
}

void custom_print(const cpptrace::stacktrace& trace) {
    for(const auto& frame : trace) {
        std::cout
            << normalize_filename(frame.filename)
            << "||"
            << frame.line.value()
            << "||"
            << frame.symbol
            << std::endl;
    }
}
