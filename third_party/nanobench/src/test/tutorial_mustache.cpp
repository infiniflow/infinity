#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <fstream>
#include <random>

namespace {

void gen(std::string const& typeName, char const* mustacheTemplate,
         ankerl::nanobench::Bench const& bench) {

    std::ofstream templateOut("mustache.template." + typeName);
    templateOut << mustacheTemplate;

    std::ofstream renderOut("mustache.render." + typeName);
    ankerl::nanobench::render(mustacheTemplate, bench, renderOut);
}

} // namespace

TEST_CASE("tutorial_mustache") {
    ankerl::nanobench::Bench bench;
    bench.title("Benchmarking std::mt19937_64 and std::knuth_b");

    std::mt19937_64 rng1;
    bench.run("std::mt19937_64", [&] {
        ankerl::nanobench::doNotOptimizeAway(rng1());
    });

    std::knuth_b rng2;
    bench.run("std::knuth_b", [&] {
        ankerl::nanobench::doNotOptimizeAway(rng2());
    });

    gen("json", ankerl::nanobench::templates::json(), bench);
    gen("html", ankerl::nanobench::templates::htmlBoxplot(), bench);
    gen("csv", ankerl::nanobench::templates::csv(), bench);
}
