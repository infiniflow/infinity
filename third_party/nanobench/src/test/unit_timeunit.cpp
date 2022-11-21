#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <thread>

// Benchmarks insertion and removal in multiple different containers.
// This uses a very fast random generator.
TEST_CASE("unit_timeunits") {
    auto bench = ankerl::nanobench::Bench();

    bench.timeUnit(std::chrono::milliseconds(1), "ms");
    bench.run("sleep 2ms", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    });

    bench.run("sleep 1ms", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    });

    // changing the unit should create a new header

    bench.timeUnit(std::chrono::hours(24 * 14), "fortnight");
    bench.run("sleep 1ms", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    });

    bench.timeUnit(std::chrono::duration<int64_t, std::pico>(1), "ps");
    bench.run("sleep 1ms", [] {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    });
}
