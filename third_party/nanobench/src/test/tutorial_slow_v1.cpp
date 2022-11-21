#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <chrono>
#include <thread>

TEST_CASE("tutorial_slow_v1") {
    ankerl::nanobench::Bench().run("sleep 100ms, auto", [&] {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
}
