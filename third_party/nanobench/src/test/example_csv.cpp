#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <deque>
#include <iostream>
#include <list>
#include <vector>

namespace {

template <typename ContainerT>
static void testBenchSet(char const* label, int n,
                         ankerl::nanobench::Bench& bench) {
    bench.run(label, [&] {
        ContainerT items;
        for (int i = 0; i < n; ++i) {
            items.push_back(i);
        }
        ankerl::nanobench::doNotOptimizeAway(&items);
    });
}

void exampleCsv(bool useCsv) {
    ankerl::nanobench::Bench bench;
    if (useCsv) {
        bench.output(nullptr);
    }

    for (int n = 100; n <= 10000; n *= 10) {
        bench.title("Size " + std::to_string(n));
        testBenchSet<std::vector<int>>("std::vector<int>", n, bench);
        testBenchSet<std::deque<int>>("std::deque<int>", n, bench);
        testBenchSet<std::list<int>>("std::list<int>", n, bench);
        if (useCsv) {
            // could also use ankerl::nanobench::templates::csv() which contains
            // a header
            bench.render(ankerl::nanobench::templates::csv(), std::cout);
        }
    }
}

} // namespace

TEST_CASE("example_csv_csv") {
    exampleCsv(true);
}

TEST_CASE("example_csv_md") {
    exampleCsv(false);
}
