#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <fstream>
#include <limits>
#include <random>

TEST_CASE("example_random_uniform01") {
    ankerl::nanobench::Bench bench;
    bench.title("random double in [0, 1(").relative(true);

    std::random_device dev;
    std::default_random_engine defaultRng(dev());
    double d = 0;
    bench
        .run("std::default_random_engine & std::uniform_real_distribution",
             [&] {
                 d += std::uniform_real_distribution<>{}(defaultRng);
             })
        .doNotOptimizeAway(d);

    d = 0;
    bench
        .run("std::default_random_engine && std::generate_canonical",
             [&] {
                 d += std::generate_canonical<
                     double, std::numeric_limits<double>::digits>(defaultRng);
             })
        .doNotOptimizeAway(d);

    ankerl::nanobench::Rng nanobenchRng{123};
    d = 0;
    bench
        .run("ankerl::nanobench::Rng & std::uniform_real_distribution",
             [&] {
                 d += std::uniform_real_distribution<>{}(nanobenchRng);
             })
        .doNotOptimizeAway(d);

    bench
        .run("ankerl::nanobench::Rng & std::generate_canonical",
             [&] {
                 d += std::generate_canonical<
                     double, std::numeric_limits<double>::digits>(nanobenchRng);
             })
        .doNotOptimizeAway(d);

    d = 0;
    bench
        .run("ankerl::nanobench::Rng::uniform01()",
             [&] {
                 d += nanobenchRng.uniform01();
             })
        .doNotOptimizeAway(d);

    std::ofstream fout("example_random_uniform01.json");
    bench.render(ankerl::nanobench::templates::json(), fout);
}
