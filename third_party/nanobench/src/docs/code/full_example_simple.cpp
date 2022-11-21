#define ANKERL_NANOBENCH_IMPLEMENT
#include <nanobench.h>

int main() {
    double d = 1.0;
    ankerl::nanobench::Bench().run("some double ops", [&] {
        d += 1.0 / d;
        if (d > 5.0) {
            d -= 5.0;
        }
        ankerl::nanobench::doNotOptimizeAway(d);
    });
}