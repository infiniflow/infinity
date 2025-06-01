#include <ctrace/ctrace.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>

void trace(void) {
    ctrace_raw_trace raw_trace = ctrace_generate_raw_trace(1, INT_MAX);
    ctrace_object_trace obj_trace = ctrace_resolve_raw_trace_to_object_trace(&raw_trace);
    ctrace_stacktrace trace = ctrace_resolve_object_trace(&obj_trace);
    ctrace_print_stacktrace(&trace, stdout, 1);
    ctrace_free_stacktrace(&trace);
    ctrace_free_object_trace(&obj_trace);
    ctrace_free_raw_trace(&raw_trace);
    assert(raw_trace.frames == NULL && obj_trace.count == 0);
}

void bar(int n) {
    if(n == 0) {
        trace();
    } else {
        bar(n - 1);
    }
}

void foo(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j) {
    (void)a, (void)b, (void)c, (void)d, (void)e, (void)f, (void)g, (void)h, (void)i, (void)j;
    bar(1);
}

void function_two(int a, float b) {
    (void)a, (void)b;
    foo(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
}

void function_one(int a) {
    (void)a;
    function_two(0, 0);
}

int main(void) {
    function_one(0);
}
