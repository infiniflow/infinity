#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <cmath>

TEST_CASE("string") {
    std::string shortString = "hello";
    ankerl::nanobench::Bench bench;
    bench.run("short string", [&] {
        (void)std::string(shortString);
    });

    std::string longString = "0123456789abcdefghijklmnopqrstuvwxyz";
    bench.run("long string", [&] {
        (void)std::string(longString);
    });
}

TEST_CASE("incorrect1") {
    // compiler optimizes sin() away, because it is unused
    ankerl::nanobench::Bench bench;
    bench.run("std::sin(2.32)", [&] {
        (void)std::sin(2.32);
    });
}
TEST_CASE("incorrect2") {
    // compiler can still calculate sin(2.32) at compile time and replace it
    // with the number. So we get a result, but it's still not what we want
    ankerl::nanobench::Bench bench;
    bench.run("std::sin(2.32)", [&] {
        ankerl::nanobench::doNotOptimizeAway(std::sin(2.32));
    });
}

TEST_CASE("incorrect3") {
    // we produce a side effect by always modifying x, but the result is never
    // used so the compiler might still optimize it away
    double x = 123.4;
    ankerl::nanobench::Bench bench;
    bench.run("x = std::sin(x)", [&] {
        x = std::sin(x);
    });
}

TEST_CASE("simplest_api") {
    // correct: std::sin() produces a side effect, and after benchmark the
    // result is checked.
    double x = 123.4;
    ankerl::nanobench::Bench bench;
    bench
        .run("x = std::sin(x) noop afterwards",
             [&] {
                 x = std::sin(x);
             })
        .doNotOptimizeAway(x);
    bench.run("x = std::sin(x) always noop", [&] {
        ankerl::nanobench::doNotOptimizeAway(x = std::sin(x));
    });
}

TEST_CASE("comparison") {
    double x = 1.321;

    auto bench =
        ankerl::nanobench::Bench().title("relative comparisons").relative(true);
    bench
        .run("x += x double",
             [&] {
                 x += x;
             })
        .doNotOptimizeAway(x);

    uint64_t n = 1;
    bench
        .run("n *= 0x42ad44f557ff4d43",
             [&]() ANKERL_NANOBENCH_NO_SANITIZE("integer") {
                 n *= UINT64_C(0x42ad44f557ff4d43);
             })
        .doNotOptimizeAway(n);

    x = 1.123;
    bench
        .run("std::sin(x)",
             [&] {
                 x += std::sin(x);
             })
        .doNotOptimizeAway(x);

    // The compiler might be smart enough to optimize this away, since log(1)
    // = 1.
    x = 1.123;
    bench
        .run("std::log(x)",
             [&] {
                 x += std::log(x);
             })
        .doNotOptimizeAway(x);

    x = 1.123;
    bench
        .run("1/x",
             [&] {
                 x += 1 / x;
             })
        .doNotOptimizeAway(x);

    bench.run("noop", [&] {});

    x = 1.123;
    bench
        .run("std::sqrt(x)",
             [&] {
                 x += std::sqrt(x);
             })
        .doNotOptimizeAway(x);
}

TEST_CASE("unit_api") {
    std::string str(200000, 'x');

    size_t h = 0;
    ankerl::nanobench::Bench bench;
    bench.batch(str.size())
        .unit("B")
        .run("std::hash",
             [&]() ANKERL_NANOBENCH_NO_SANITIZE("integer") {
                 h += std::hash<std::string>{}(str);
                 ++str[11];
             })
        .doNotOptimizeAway(h);
}
