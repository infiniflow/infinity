#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

TEST_CASE("tutorial_fast_v2") {
    uint64_t x = 1;
    ankerl::nanobench::Bench().run("++x", [&]() {
        ankerl::nanobench::doNotOptimizeAway(x += 1);
    });
}
