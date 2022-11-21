#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

namespace {

ANKERL_NANOBENCH_NO_SANITIZE("integer", "undefined")
constexpr uint64_t rotl(uint64_t x, unsigned k) noexcept {
    return (x << k) | (x >> (64U - k));
}

// Romu generators, by Mark Overton, 2020-2-7.
//
// This code is not copyrighted and comes with no warranty of any kind, so it is
// as-is. You are free to modify and/or distribute it as you wish. You are only
// required to give credit where credit is due by: (1) not renaming a generator
// having an unmodified algorithm and constants; (2) prefixing the name of a
// generator having a modified algorithm or constants with "Romu"; (3)
// attributing the original invention to Mark Overton.

// Copy and paste the generator you want from those below.
// To compile, you will need to #include <stdint.h>
// Website: romu-random.org

//===== RomuTrio
//==================================================================================
//
// Great for general purpose work, including huge jobs.
// Est. capacity = 2^75 bytes. Register pressure = 6. State size = 192 bits.

uint64_t xState, yState, zState; // set to nonzero seed

ANKERL_NANOBENCH_NO_SANITIZE("integer")
uint64_t romuTrio_random() {
    uint64_t xp = xState, yp = yState, zp = zState;
    xState = 15241094284759029579u * zp;
    yState = yp - xp;
    yState = rotl(yState, 12);
    zState = zp - yp;
    zState = rotl(zState, 44);
    return xp;
}

void romuTrio_seed(uint64_t seed) {
    xState = seed;
    yState = UINT64_C(0xcc2d6b0743b14800);
    zState = UINT64_C(0xd932cff2dd2324a7);

    for (int i = 0; i < 10; ++i) {
        romuTrio_random();
    }
}

} // namespace

#include <iomanip>
#include <iostream>
#include <sstream>

TEST_CASE("unit_romutrio_seed") {
    std::ostringstream out;
    for (int skip = 0; skip < 10; ++skip) {
        for (uint64_t i = 0; i < 10; ++i) {
            romuTrio_seed(i);
            for (auto s = 0; s < skip; ++s) {
                romuTrio_random();
            }
            out << std::setw(16) << std::setfill('0') << std::hex
                << romuTrio_random() << " ";
        }
        out << std::endl;
    }
    std::cout << out.str() << std::endl;
}

TEST_CASE("unit_rng_state") {
    ankerl::nanobench::Rng rng{};
    for (size_t i = 0; i < 10; ++i) {
        rng();
    }
    auto state = rng.state();
    ankerl::nanobench::Rng rngLoaded(state);

    for (size_t i = 0; i < 100; ++i) {
        REQUIRE(rng() == rngLoaded());
    }
}
