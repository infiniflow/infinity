//
// Created by JinHai on 2022/11/21.
//
#define ANKERL_NANOBENCH_IMPLEMENT

#include "nanobench.h"

auto main () -> int {
    uint64_t x = 1;

    ankerl::nanobench::Bench().run("add", [&]() {
        ankerl::nanobench::doNotOptimizeAway(x += x);
    });

    return 0;
}
