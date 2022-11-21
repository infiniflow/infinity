#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <iostream>

TEST_CASE("preconfigured_name") {
    ankerl::nanobench::Bench bench;
    bench.name("start & stop 1").run([] {
        std::vector<int> v = {{11, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7}};
    });

    bench.name("run2");
    bench.run([] {
        std::vector<int> v = {{11, 5, 5, 5, 5, 5, 5, 5, 5, 6, 7}};
    });
}

TEST_CASE("keep_only_results") {

    std::vector<ankerl::nanobench::Result> results;
    ankerl::nanobench::Bench bench;
    ankerl::nanobench::Rng rng(123);

    bench.run("a", [&] {
        rng();
    });
    results.insert(results.end(), bench.results().begin(),
                   bench.results().end());
}
