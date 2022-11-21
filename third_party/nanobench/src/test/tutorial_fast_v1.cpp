#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

TEST_CASE("tutorial_fast_v1") {
    uint64_t x = 1;
    ankerl::nanobench::Bench().run("++x", [&]() {
        ++x;
    });
}
