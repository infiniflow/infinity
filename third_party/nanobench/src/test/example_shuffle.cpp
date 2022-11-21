#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <algorithm>
#include <random>

TEST_CASE("shuffle") {
    // input data for shuffling
    std::vector<uint64_t> data(10000, 0);

    ankerl::nanobench::Bench bench;
    bench.relative(true).batch(data.size()).unit("elem");

    std::default_random_engine defaultRng(123);
    bench.run("std::shuffle with std::default_random_engine", [&]() {
        std::shuffle(data.begin(), data.end(), defaultRng);
    });

    ankerl::nanobench::Rng rng(123);
    bench.run("std::shuffle with ankerl::nanobench::Rng", [&]() {
        std::shuffle(data.begin(), data.end(), rng);
    });

    bench.run("ankerl::nanobench::Rng::shuffle", [&]() {
        rng.shuffle(data);
    });
}
