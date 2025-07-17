#include <cpptrace/cpptrace.hpp>

#include <benchmark/benchmark.h>

#include <iostream>

struct unwind_benchmark_info {
    benchmark::State& state;
    size_t& stack_depth;
};

void unwind_loop(unwind_benchmark_info info) {
    auto& [state, depth] = info;
    depth = cpptrace::generate_raw_trace().frames.size();
    for(auto _ : state) {
        benchmark::DoNotOptimize(cpptrace::generate_raw_trace());
    }
}

void foo(unwind_benchmark_info info, int n) {
    if(n == 0) {
        unwind_loop(info);
    } else {
        foo(info, n - 1);
    }
}

template<typename... Args>
void foo(unwind_benchmark_info info, int, Args... args) {
    foo(info, args...);
}

void function_two(unwind_benchmark_info info, int, float) {
    foo(info, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
}

void function_one(unwind_benchmark_info info, int) {
    function_two(info, 0, 0);
}

static void unwinding(benchmark::State& state) {
    size_t stack_depth = 0;
    function_one({state, stack_depth}, 0);
    static bool did_print = false;
    if(!did_print) {
        did_print = true;
        std::cerr<<"[info] Unwinding benchmark stack depth: "<<stack_depth<<std::endl;
    }
}

// Register the function as a benchmark
BENCHMARK(unwinding);

// Run the benchmark
BENCHMARK_MAIN();
