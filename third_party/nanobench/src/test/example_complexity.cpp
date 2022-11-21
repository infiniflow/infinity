#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <set>

TEST_CASE("example_complexity_quadratic") {
    // create an ankerl::nanobench::Config object that is used in all the
    // benchmarks
    ankerl::nanobench::Bench bench;
    ankerl::nanobench::Rng rng{123};

    // run the same benchmark multiple times with different ranges
    for (size_t range = 10; range <= 1000; range *= 2) {
        // create vector with random data
        std::vector<double> vec(range, 0.0);
        for (auto& x : vec) {
            x = rng.uniform01();
        }

        // each run is configured with complexityN(range) to specify the run's
        // input N
        bench.complexityN(range).run(
            "minimum pair " + std::to_string(range), [&] {
                // Actual algorithm we want to evaluate
                double minVal = std::numeric_limits<double>::max();
                for (size_t i = 0; i < vec.size() - 1; ++i) {
                    for (size_t j = i + 1; j < vec.size(); ++j) {
                        auto diff = vec[i] - vec[j];
                        minVal = std::min(minVal, diff * diff);
                    }
                }
                ankerl::nanobench::doNotOptimizeAway(minVal);
            });
    }

    // after all the runs are done, calculate the BigO, and show the results
    std::cout << bench.complexityBigO() << std::endl;
}
