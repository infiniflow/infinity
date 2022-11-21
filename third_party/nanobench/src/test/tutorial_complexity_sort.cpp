#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <iostream>

TEST_CASE("tutorial_complexity_sort") {
    ankerl::nanobench::Rng rng{123};
    ankerl::nanobench::Bench bench;

    std::vector<uint64_t> data;

    for (size_t n = 10; n < 100000; n *= 2) {
        // prepare a set with range number of elements
        while (data.size() < n) {
            data.push_back(rng());
        }

        // sort should be O(n log n), shuffle is O(n), so we expect O(n log n).
        bench.complexityN(n).run("std::sort", [&] {
            // we use our own shuffle method instead of std::shuffle, because it
            // is *much* faster. Performance is very important, because we
            // actually want to benchmark std::sort and not shuffling.
            rng.shuffle(data);

            // The actual code under benchmark
            std::sort(data.begin(), data.end());
        });
    }

    // calculates bigO of all preconfigured complexity functions
    std::cout << bench.complexityBigO() << std::endl;

    // calculates bigO for a custom function
    auto logLogN = bench.complexityBigO("O(log log n)", [](double n) {
        return std::log2(std::log2(n));
    });
    std::cout << logLogN << std::endl;
}
