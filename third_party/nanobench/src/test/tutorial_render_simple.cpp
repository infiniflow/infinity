#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <atomic>
#include <iostream>

TEST_CASE("tutorial_render_simple") {
    std::atomic<int> x(0);

    ankerl::nanobench::Bench()
        .output(nullptr)
        .run("std::vector",
             [&] {
                 ++x;
             })
        .render(ankerl::nanobench::templates::csv(), std::cout);
}
