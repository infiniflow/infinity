#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <random>

TEST_CASE("tutorial_fluctuating_v1") {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    ankerl::nanobench::Bench().run("random fluctuations", [&] {
        // each run, perform a random number of rng calls
        auto iterations = rng() & UINT64_C(0xff);
        for (uint64_t i = 0; i < iterations; ++i) {
            ankerl::nanobench::doNotOptimizeAway(rng());
        }
    });
}
