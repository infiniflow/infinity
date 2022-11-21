#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <algorithm>
#include <fstream>
#include <random>

TEST_CASE("shuffle_pyperf") {
    std::vector<uint64_t> data(500, 0); // input data for shuffling

    std::default_random_engine defaultRng(123);
    std::ofstream fout1("pyperf_shuffle_std.json");
    ankerl::nanobench::Bench()
        .epochs(100)
        .run("std::shuffle with std::default_random_engine",
             [&]() {
                 std::shuffle(data.begin(), data.end(), defaultRng);
             })
        .render(ankerl::nanobench::templates::pyperf(), fout1);

    std::ofstream fout2("pyperf_shuffle_nanobench.json");
    ankerl::nanobench::Rng rng(123);
    ankerl::nanobench::Bench()
        .epochs(100)
        .run("ankerl::nanobench::Rng::shuffle",
             [&]() {
                 rng.shuffle(data);
             })
        .render(ankerl::nanobench::templates::pyperf(), fout2);
}
