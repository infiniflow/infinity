#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <fstream>

TEST_CASE("always_the_same") {
    ankerl::nanobench::Bench bench;

    std::string shortString = "hello World!";

    ankerl::nanobench::Rng rng;
    for (int i = 0; i < 40; ++i) {
        bench
            .run("rng() " + std::to_string(i),
                 [&] {
                     rng();
                 })
            .doNotOptimizeAway(rng());
    }

    std::ofstream html("always_the_same.html");
    bench.render(ankerl::nanobench::templates::htmlBoxplot(), html);

    std::ofstream json("always_the_same.json");
    bench.render(ankerl::nanobench::templates::json(), json);
}
