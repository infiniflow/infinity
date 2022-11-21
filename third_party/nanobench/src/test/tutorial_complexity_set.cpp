#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <iostream>
#include <set>

TEST_CASE("tutorial_complexity_set_find") {
    // Create a single benchmark instance that is used in multiple benchmark
    // runs, with different settings for complexityN.
    ankerl::nanobench::Bench bench;

    // a RNG to generate input data
    ankerl::nanobench::Rng rng;

    std::set<uint64_t> set;

    // Running the benchmark multiple times, with different number of elements
    for (auto setSize :
         {10U, 20U, 50U, 100U, 200U, 500U, 1000U, 2000U, 5000U, 10000U}) {

        // fill up the set with random data
        while (set.size() < setSize) {
            set.insert(rng());
        }

        // Run the benchmark, provide setSize as the scaling variable.
        bench.complexityN(set.size()).run("std::set find", [&] {
            ankerl::nanobench::doNotOptimizeAway(set.find(rng()));
        });
    }

    // calculate BigO complexy best fit and print the results
    std::cout << bench.complexityBigO() << std::endl;
}
