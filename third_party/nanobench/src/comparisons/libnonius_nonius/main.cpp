#define NONIUS_RUNNER
#include <nonius/nonius_single.h++>

// g++ -O2 main.cpp -pthread -I. -o m

#include <chrono>
#include <random>
#include <thread>

NONIUS_PARAM(X, UINT64_C(1))

template <typename Fn>
struct volatilize_fn {
    Fn fn;
    auto operator()() const -> decltype(fn()) {
        volatile auto x = fn();
        return x;
    }
};

template <typename Fn>
auto volatilize(Fn&& fn) -> volatilize_fn<typename std::decay<Fn>::type> {
    return {std::forward<Fn>(fn)};
}

NONIUS_BENCHMARK("x += x", [](nonius::chronometer meter) {
    auto x = meter.param<X>();
    meter.measure(volatilize([&]() {
        return x += x;
    }));
})

NONIUS_BENCHMARK("sleep 10ms", [] {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
})

NONIUS_BENCHMARK("random fluctuations", [](nonius::chronometer meter) {
    std::random_device dev;
    std::mt19937_64 rng(dev());
    meter.measure([&] {
        // each run, perform a random number of rng calls
        auto iterations = rng() & UINT64_C(0xff);
        for (uint64_t i = 0; i < iterations; ++i) {
            (void)rng();
        }
    });
})
