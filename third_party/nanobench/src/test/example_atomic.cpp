#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <iostream>

// Demonstrates a very simple benchmark that evalutes the performance of a CAS
// operation using std::atomic. On my system, this prints:
//
// | relative |               ns/op |                op/s |   MdAPE | benchmark
// |---------:|--------------------:|--------------------:|--------:|:----------------------------------------------
// |          |                5.63 |      177,553,749.61 |    0.0% |
// `compare_exchange_strong`
//
// example from https://github.com/cameron314/microbench
TEST_CASE("performance_counters") {
    ankerl::nanobench::Bench bench;

    bench.run("start & stop counting", [&] {
        std::vector<int> v = {{11, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7}};
    });
}
