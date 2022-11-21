#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <iostream>
#include <random>

// Source: http://quick-bench.com/2dBt6SOQTSlztlqmlo0w7pv6iNM
// https://www.reddit.com/r/prng/comments/fchmfd/romu_fast_nonlinear_pseudorandom_number_generators/fl6lfw9/

namespace {

#define ROTL(d, lrot) ((d << (lrot)) | (d >> (8 * sizeof(d) - (lrot))))

static uint64_t xState = 1u, yState = 1u, zState = 1u;

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t romuTrio_random() {
    uint64_t xp = xState, yp = yState, zp = zState;
    xState = 15241094284759029579u * zp;
    yState = yp - xp;
    yState = ROTL(yState, 12);
    zState = zp - yp;
    zState = ROTL(zState, 44);
    return xp;
}

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t romuDuo_random() {
    uint64_t xp = xState;
    xState = 15241094284759029579u * yState;
    yState = ROTL(yState, 36) + ROTL(yState, 15) - xp;
    return xp;
}

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t romuDuoJr_random() {
    uint64_t xp = xState;
    xState = 15241094284759029579u * yState;
    yState = yState - xp;
    yState = ROTL(yState, 27);
    return xp;
}

static uint64_t stateA = 1u, stateB = 1u;

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t tangle() {
    uint64_t s = (stateA += 0xC6BC279692B5C323u);
    uint64_t t = (stateB += 0x9E3779B97F4A7C16u);
    uint64_t z = (s ^ s >> 31) * t;
    return z ^ z >> 26;
}

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t orbit() {
    uint64_t s = (stateA += 0xC6BC279692B5C323u);
    uint64_t t = ((s == 0u) ? stateB : (stateB += 0x9E3779B97F4A7C15u));
    uint64_t z = (s ^ s >> 31) * ((t ^ t >> 22) | 1u);
    return z ^ z >> 26;
}

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t splitmix64() {
    uint64_t z = (stateA += 0x9E3779B97F4A7C15u);
    z = (z ^ z >> 30) * 0xbf58476d1ce4e5b9U;
    z = (z ^ z >> 27) * 0x94d049bb133111ebU;
    return z ^ z >> 31;
}

static uint64_t s[4];

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t xoshiroStarStar() {
    const uint64_t result = ROTL(s[1] * 5, 7) * 9;

    const uint64_t t = s[1] << 17;

    s[2] ^= s[0];
    s[3] ^= s[1];
    s[1] ^= s[2];
    s[0] ^= s[3];

    s[2] ^= t;

    s[3] = ROTL(s[3], 45);

    return result;
}

static uint64_t sr[4];

ANKERL_NANOBENCH_NO_SANITIZE("integer") uint64_t xoroshiroPlus() {
    const uint64_t s0 = sr[0];
    uint64_t s1 = sr[1];
    const uint64_t result = s0 + s1;

    s1 ^= s0;
    sr[0] = ROTL(s0, 24) ^ s1 ^ (s1 << 16); // a, b
    sr[1] = ROTL(s1, 37);                   // c

    return result;
}

} // namespace

TEST_CASE("example_random2") {
    auto bench = ankerl::nanobench::Bench().relative(true);

    std::mt19937_64 mt{};
    bench.run("std::mt19937_64", [&] {
        ankerl::nanobench::doNotOptimizeAway(mt());
    });

    bench.run("RomuTrio", [] {
        ankerl::nanobench::doNotOptimizeAway(romuTrio_random());
    });

    bench.run("RomuDuo", [] {
        ankerl::nanobench::doNotOptimizeAway(romuDuo_random());
    });

    bench.run("RomuDuoJr", [] {
        ankerl::nanobench::doNotOptimizeAway(romuDuoJr_random());
    });

    bench.run("Tangle", [] {
        ankerl::nanobench::doNotOptimizeAway(tangle());
    });

    bench.run("Orbit", [] {
        ankerl::nanobench::doNotOptimizeAway(orbit());
    });

    bench.run("SplitMix", [] {
        ankerl::nanobench::doNotOptimizeAway(splitmix64());
    });

    bench.run("XoshiroStarStar", [] {
        ankerl::nanobench::doNotOptimizeAway(xoshiroStarStar());
    });

    bench.run("XoroshiroPlus", [] {
        ankerl::nanobench::doNotOptimizeAway(xoroshiroPlus());
    });
}

class RomuMono32 {
public:
    RomuMono32(uint32_t seed)
        : mState{(seed & UINT32_C(0x1fffffff)) + UINT32_C(1156979152)} {}

    uint16_t operator()() noexcept {
        uint16_t result = static_cast<uint16_t>(mState >> 16U);
        mState *= UINT32_C(3611795771);
        mState = ROTL(mState, 12);
        return result;
    }

    uint32_t state() const noexcept {
        return mState;
    }

private:
    uint32_t mState;
};

TEST_CASE("romumono32_all_states"  * doctest::skip()) {

    uint32_t n = 0;
    RomuMono32 rm(123);
    auto initialState = rm.state();
    do {
        rm();
        ++n;
    } while (rm.state() != initialState);

    std::cout << n << std::endl;
}
