// https://github.com/catchorg/Catch2
// g++ -O2 catch.cpp -o c

#define CATCH_CONFIG_ENABLE_BENCHMARKING
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <chrono>
#include <random>
#include <thread>

TEST_CASE("comparison_fast") {
    uint64_t x = 1;
    BENCHMARK("x += x") {
        return x += x;
    };
}

TEST_CASE("comparison_slow") {
    BENCHMARK("sleep 10ms") {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    };
}

TEST_CASE("comparison_fluctuating_v2") {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    BENCHMARK("random fluctuations") {
        // each run, perform a random number of rng calls
        auto iterations = rng() & UINT64_C(0xff);
        for (uint64_t i = 0; i < iterations; ++i) {
            (void)rng();
        }
    };
}
