#include "benchmark.h"

#include <chrono>
#include <random>
#include <thread>

// Build instructions: https://github.com/google/benchmark#installation
// curl --output benchmark.h
// https://raw.githubusercontent.com/google/benchmark/master/include/benchmark/benchmark.h
// g++ -O2 main.cpp -Lgit/benchmark/build/src -lbenchmark -lpthread -o m
void ComparisonFast(benchmark::State& state) {
    uint64_t x = 1;
    for (auto _ : state) {
        x += x;
    }
    benchmark::DoNotOptimize(x);
}
BENCHMARK(ComparisonFast);

void ComparisonSlow(benchmark::State& state) {
    for (auto _ : state) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
BENCHMARK(ComparisonSlow);

void ComparisonFluctuating(benchmark::State& state) {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    for (auto _ : state) {
        // each run, perform a random number of rng calls
        auto iterations = rng() & UINT64_C(0xff);
        for (uint64_t i = 0; i < iterations; ++i) {
            (void)rng();
        }
    }
}
BENCHMARK(ComparisonFluctuating);

BENCHMARK_MAIN();