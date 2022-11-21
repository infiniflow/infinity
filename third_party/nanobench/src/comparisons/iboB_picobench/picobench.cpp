#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

#include <chrono>
#include <initializer_list>
#include <random>
#include <thread>


// https://github.com/iboB/picobench
// g++ -O2 picobench.cpp -o pb

PICOBENCH_SUITE("ComparisonFast");
static void ComparisonFast(picobench::state& state) {
    uint64_t x = 1;
    for (auto _ : state) {
        x += x;
    }
    state.set_result(x);
}
PICOBENCH(ComparisonFast);

PICOBENCH_SUITE("ComparisonSlow");
void ComparisonSlow(picobench::state& state) {
    for (auto _ : state) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}
PICOBENCH(ComparisonSlow).iterations({1, 2, 5, 10});

PICOBENCH_SUITE("fluctuating");
void ComparisonFluctuating(picobench::state& state) {
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
PICOBENCH(ComparisonFluctuating);
