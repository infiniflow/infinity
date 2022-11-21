#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <thread>

// Benchmarks insertion and removal in multiple different containers.
// This uses a very fast random generator.
TEST_CASE("unit_cold") {
    int x = 0;
    ankerl::nanobench::Bench().epochs(1).epochIterations(1).run(
        "x and sleep", [&] {
            ++x;
            //std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });

    REQUIRE(x == 1);
}
