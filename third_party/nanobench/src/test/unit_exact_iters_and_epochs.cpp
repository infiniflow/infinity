#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

// Benchmarks insertion and removal in multiple different containers.
// This uses a very fast random generator.
TEST_CASE("test_exact_iters_and_epochs") {

    ankerl::nanobench::Bench bench;
    size_t x = 0;
    bench.epochs(7).epochIterations(123).run("++x", [&] {
        ++x;
    });
    REQUIRE(x == bench.epochs() * bench.epochIterations());

    int y = 0;
    bench.epochs(1).epochIterations(77).run("++y", [&] {
        ++y;
    });
    REQUIRE(y == bench.epochs() * bench.epochIterations());

    // one benchmark
    REQUIRE(bench.results().size() == 2);

    {
        // check first result
        auto const& r = bench.results()[0];
        REQUIRE(r.size() == 7U); // each epoch has a result
        for (size_t i = 0; i < r.size(); ++i) {
            REQUIRE(
                static_cast<int>(
                    0.5 +
                    r.get(i, ankerl::nanobench::Result::Measure::iterations)) ==
                123);
        }
        REQUIRE(
            static_cast<int>(
                0.5 + r.sum(ankerl::nanobench::Result::Measure::iterations)) ==
            7 * 123);
    }

    {
        // check second result
        auto const& r = bench.results()[1];
        REQUIRE(r.size() == 1U); // each epoch has a result
        for (size_t i = 0; i < r.size(); ++i) {
            REQUIRE(
                static_cast<int>(
                    0.5 +
                    r.get(i, ankerl::nanobench::Result::Measure::iterations)) ==
                77);
        }
        REQUIRE(
            static_cast<int>(
                0.5 + r.sum(ankerl::nanobench::Result::Measure::iterations)) ==
            1 * 77);
    }
}
