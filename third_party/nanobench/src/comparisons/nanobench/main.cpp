// https://github.com/martinus/nanobench
// g++ -O2 -I../../include main.cpp -o m

#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

#include <chrono>
#include <random>
#include <thread>

int main(int, char**) {
    uint64_t x = 1;
    ankerl::nanobench::Bench().run("x += x", [&]() {
        ankerl::nanobench::doNotOptimizeAway(x += x);
    });

    ankerl::nanobench::Bench().run("sleep 10ms", [&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    });

    std::random_device dev;
    std::mt19937_64 rng(dev());
    ankerl::nanobench::Bench().run("random fluctuations", [&]() {
        // each run, perform a random number of rng calls
        auto iterations = rng() & UINT64_C(0xff);
        for (uint64_t i = 0; i < iterations; ++i) {
            (void)rng();
        }
    });
}
