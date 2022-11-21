#include <sltbench/Bench.h> // https://github.com/ivafanas/sltbench

#include <chrono>
#include <random>
#include <thread>

// cmake build as online instructions describes
//
// g++ -O3 -I/home/martinus/git/sltbench/install/include -c main.cpp
// g++ -o m -L/home/martinus/git/sltbench/install/lib main.o -lsltbench

uint64_t x = 1;
void ComparisonFast() {
    sltbench::DoNotOptimize(x += x);
}

SLTBENCH_FUNCTION(ComparisonFast);

void ComparisonSlow() {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}
SLTBENCH_FUNCTION(ComparisonSlow);

std::random_device dev;
std::mt19937_64 rng(dev());

void ComparisonFluctuating() {
    // each run, perform a random number of rng calls
    auto iterations = rng() & UINT64_C(0xff);
    for (uint64_t i = 0; i < iterations; ++i) {
        (void)rng();
    }
}
SLTBENCH_FUNCTION(ComparisonFluctuating);

SLTBENCH_MAIN();
