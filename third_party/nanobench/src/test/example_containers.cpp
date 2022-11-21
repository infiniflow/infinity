#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <set>
#include <unordered_set>

// Benchmarks insertion and removal in multiple different containers.
// This uses a very fast random generator.
TEST_CASE("example_containers") {
    ankerl::nanobench::Bench bench;
    bench.title("random insert & erase in containers");

    // creates a random number generator to use in the benchmarks
    ankerl::nanobench::Rng rng;
    std::set<uint64_t> s;
    uint64_t const bitmask = UINT64_C(0xff);

    bench
        .run("std::set",
             [&] {
                 s.insert(rng() & bitmask);
                 s.erase(rng() & bitmask);
             })
        .doNotOptimizeAway(&s);

    std::unordered_set<uint64_t> us;
    bench
        .run("std::unordered_set",
             [&] {
                 us.insert(rng() & bitmask);
                 us.erase(rng() & bitmask);
             })
        .doNotOptimizeAway(&us);
}
