// g++ -O2 asymptotes-google.cpp -isystem ~/git/benchmark/include/ -L/home/martinus/git/benchmark/build/src -lbenchmark -lpthread -o
// asymptotes
#include <benchmark/benchmark.h>

#include <set>

static void BM_StringCreation(benchmark::State& state) {
    for (auto _ : state) {
        std::string empty_string;
    }
}
// Register the function as a benchmark
BENCHMARK(BM_StringCreation);

// Define another benchmark
static void BM_StringCopy(benchmark::State& state) {
    std::string x = "hello";
    for (auto _ : state) {
        std::string copy(x);
    }
}
BENCHMARK(BM_StringCopy);

static void BM_set(benchmark::State& state) {
    std::set<size_t> s;
    for (size_t i = 0; i < state.range(0); ++i) {
        s.insert(i);
    }
    size_t i = 0;
    for (auto _ : state) {
        benchmark::DoNotOptimize(s.find(i));
        if (i++ == state.range(0)) {
            i = 0;
        }
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_set)->RangeMultiplier(2)->Range(1 << 10, 1 << 18)->Complexity();

static void BM_StringCompare(benchmark::State& state) {
    std::string s1(state.range(0), '-');
    std::string s2(state.range(0), '-');
    for (auto _ : state) {
        benchmark::DoNotOptimize(s1.compare(s2));
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_StringCompare)->RangeMultiplier(2)->Range(1 << 5, 1 << 22)->Complexity();

BENCHMARK_MAIN();
