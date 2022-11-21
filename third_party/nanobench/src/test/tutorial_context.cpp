#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <iostream>
#include <cmath>

namespace {

template<typename T>
void fma() {
    T x(1), y(2), z(3);
    z = std::fma(x, y, z);
    ankerl::nanobench::doNotOptimizeAway(z);
}

template<typename T>
void plus_eq() {
    T x(1), y(2), z(3);
    z += x*y;
    ankerl::nanobench::doNotOptimizeAway(z);
}

char const* csv() {
    return R"DELIM("title";"name";"scalar";"foo";"elapsed";"total"
{{#result}}"{{title}}";"{{name}}";"{{context(scalar)}}";"{{context(foo)}}";{{median(elapsed)}};{{sumProduct(iterations, elapsed)}}
{{/result}})DELIM";
}

} // namespace

TEST_CASE("tutorial_context") {
    ankerl::nanobench::Bench bench;
    bench.title("Addition").output(nullptr);
    bench
        .context("scalar", "f32")
        .context("foo", "bar")
        .run("+=", plus_eq<float>)
        .run("fma", fma<float>);
    bench
        .context("scalar", "f64")
        .context("foo", "baz")
        .run("+=", plus_eq<double>)
        .run("fma", fma<double>);
    bench.render(csv(), std::cout);
    // Changing the title resets the results, but not the context:
    bench.title("New Title");
    bench.run("+=", plus_eq<float>);
    bench.render(csv(), std::cout);
    CHECK_EQ(bench.results().front().context("foo"), "baz"); // != bar
    // The context has to be reset manually, which causes render to fail:
    bench.title("Yet Another Title").clearContext();
    bench.run("+=", plus_eq<float>);
    CHECK_THROWS(bench.render(csv(), std::cout));
}
