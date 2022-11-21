<a id="top"></a>
# ankerl::nanobench

![ankerl::nanobench logo](src/docs/nanobench-logo-small.svg)

[![Release](https://img.shields.io/github/release/martinus/nanobench.svg)](https://github.com/martinus/nanobench/releases)
[![GitHub license](https://img.shields.io/github/license/martinus/nanobench.svg)](https://raw.githubusercontent.com/martinus/nanobench/master/LICENSE)
[![Travis CI Build Status](https://travis-ci.com/martinus/nanobench.svg?branch=master)](https://travis-ci.com/martinus/nanobench)
[![Appveyor Build Status](https://ci.appveyor.com/api/projects/status/github/martinus/nanobench?branch=master&svg=true)](https://ci.appveyor.com/project/martinus/nanobench)
[![Join the chat at https://gitter.im/nanobench/community](https://badges.gitter.im/nanobench/community.svg)](https://gitter.im/nanobench/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

`ankerl::nanobench` is a platform independent microbenchmarking library for C++11/14/17/20.

```cpp
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
```

The whole executable runs for ~60ms and prints

```markdown
|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|                7.52 |      132,948,239.79 |    1.1% |            6.65 |           24.07 |  0.276 |           1.00 |    8.9% |      0.00 | `some double ops`
```

Which github renders as

|               ns/op |                op/s |    err% |          ins/op |          cyc/op |    IPC |         bra/op |   miss% |     total | benchmark
|--------------------:|--------------------:|--------:|----------------:|----------------:|-------:|---------------:|--------:|----------:|:----------
|                7.52 |      132,948,239.79 |    1.1% |            6.65 |           24.07 |  0.276 |           1.00 |    8.9% |      0.00 | `some double ops`

The benchmarked code takes **7.52** nanoseconds to run, so ~**133** million times per seconds. Measurements fluctuate by
**1.1%**. On average **6.65** instructions are executed in **24.07** CPU cycles, resulting in **0.276** instructions per
cycle. A **single** branch is in the code, which branch prediction missed in **8.9%** of the cases. Total runtime of
the benchmark with the name `some double ops` is **0.00**, so just a few milliseconds.

# Design Goals

* **Ease of use**: Simple & [powerful API](https://nanobench.ankerl.com/reference.html), fast compile times, [easy to integrate anywhere](https://nanobench.ankerl.com/tutorial.html#installation).
* **Fast**: Get accurate results as fast as possible. nanobench is [~80 times faster than google benchmark](https://nanobench.ankerl.com/comparison.html#runtime).
* **Accurate**: Get deterministic, repeatable, and accurate results that you can make sound decisions on.
* **Robust**: Be robust against outliers, warn if results are not reliable.

# Documentation

[Extensive documentation is available](https://nanobench.ankerl.com).

# More

* [Code of Conduct](src/docs/CODE_OF_CONDUCT.md) - Contributor Covenant Code of Conduct
* I need a better logo. Currently I use a small bench. Nanobench. Ha ha.

