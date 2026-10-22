// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

#include "unit_test/gtest_expand.h"

module infinity_core:ut.test_pgm;

import :ut.base_test;

import third_party;

using namespace infinity;

constexpr uint32_t TestNum = 200;

template <typename T>
struct PGMT;

template <typename T>
    requires std::integral<T> || std::same_as<T, float>
struct PGMT<T> {
    using type = PGMIndex<T, 64, 4, float>;
};

template <typename T>
    requires std::same_as<T, double>
struct PGMT<T> {
    using type = PGMIndex<T, 64, 4, double>;
};

template <typename T>
class TestPGM : public BaseTest {
public:
    using PGM = PGMT<T>::type;
    std::array<T, TestNum> data;
    uint32_t cnt = TestNum;
    std::unique_ptr<PGM> pgm;

    void SetUp() override {
        std::random_device rd;
        std::mt19937 gen(rd());
        if constexpr (std::integral<T>) {
            std::uniform_int_distribution<T> distrib(std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max() - 1);
            std::generate(data.begin(), data.end(), [&] { return distrib(gen); });
        } else if constexpr (std::floating_point<T>) {
            std::uniform_real_distribution<T> distrib(std::numeric_limits<T>::lowest() / 2, std::numeric_limits<T>::max() / 2);
            std::generate(data.begin(), data.end(), [&] { return distrib(gen); });
        } else {
            static_assert(false, "Unsupported type");
        }
        std::sort(data.begin(), data.end());
        cnt = std::unique(data.begin(), data.end()) - data.begin();
        pgm = std::make_unique<PGM>(data.begin(), data.begin() + cnt);
    }
};

using TestPGMTypes = ::testing::Types<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t, float, double>;

TYPED_TEST_SUITE(TestPGM, TestPGMTypes);

TYPED_TEST(TestPGM, TestPGMTypeSupport) {
    for (uint32_t i = 0; i < this->cnt; ++i) {
        auto [pos, lo, hi] = this->pgm->search(this->data[i]);
        EXPECT_LE(lo, i);
        EXPECT_GE(hi, i);
    }
}

namespace {

// The index is persisted as the packed `Segment` array, so the model itself - not
// only the search range - has to be reproducible: the C++ reference and the Rust
// rewrite write the same index file. `Segment` and `segments` are protected in
// `pgm::PGMIndex`, hence the probe subclass.
template <typename T>
struct PGMBytesProbe : PGMT<T>::type {
    using Base = typename PGMT<T>::type;
    using Base::Base;
    using Segment = typename Base::Segment;

    size_t segment_count() const { return this->segments.size(); }
    const Segment &segment(size_t i) const { return this->segments[i]; }

    std::vector<unsigned char> bytes() const {
        std::vector<unsigned char> result(this->segments.size() * sizeof(Segment));
        if (!result.empty()) {
            std::memcpy(result.data(), this->segments.data(), result.size());
        }
        return result;
    }
};

std::vector<unsigned char> Bytes(std::initializer_list<unsigned> values) {
    std::vector<unsigned char> result;
    result.reserve(values.size());
    for (unsigned value : values) {
        result.push_back(static_cast<unsigned char>(value));
    }
    return result;
}

// 0, 10, 20, ..., 990: a single straight line, so the fitted model is one segment.
template <typename T>
std::vector<T> LinearKeys() {
    std::vector<T> keys(100);
    for (size_t i = 0; i < keys.size(); ++i) {
        keys[i] = static_cast<T>(i) * static_cast<T>(10);
    }
    return keys;
}

// Keys stay distinct, but `last + 1` rounds back to `last` at this magnitude.
template <typename T>
std::vector<T> HugeKeys() {
    const T base = std::is_same_v<T, float> ? static_cast<T>(1e30) : static_cast<T>(1e300);
    const T step = std::is_same_v<T, float> ? static_cast<T>(1e24) : static_cast<T>(1e285);
    std::vector<T> keys(100);
    for (size_t i = 0; i < keys.size(); ++i) {
        keys[i] = base + static_cast<T>(i) * step;
    }
    return keys;
}

} // namespace

// The guard segments of a floating-point index are appended by `build_level`. It
// probes the next representable key: `sentinel - 1` is +inf for floating-point
// keys and `size_t(+inf)` is undefined, which would make the segment count depend
// on the compiler (g++ adds an extra segment, clang++ does not).
TEST(TestPGMModel, LinearFloatKeys) {
    const auto keys = LinearKeys<float>();
    PGMBytesProbe<float> index(keys.begin(), keys.end());

    // one fitted segment + one guard segment for keys after the last + the +inf sentinel
    ASSERT_EQ(index.segment_count(), 3u);
    // The fit is not the ideal line: interpolation lands on intercept 31, and the
    // guard key is the next representable float after 990.
    EXPECT_EQ(index.segment(0).key, 0.0f);
    EXPECT_EQ(static_cast<double>(index.segment(0).slope), 0.068181820213794708);
    EXPECT_EQ(index.segment(0).intercept, 31u);
    EXPECT_EQ(index.segment(1).key, std::nextafter(990.0f, std::numeric_limits<float>::infinity()));
    EXPECT_EQ(index.segment(1).slope, 0.0f);
    EXPECT_EQ(index.segment(1).intercept, 100u);
    EXPECT_TRUE(std::isinf(index.segment(2).key));

    // Segment = {float key, float slope, uint32_t intercept} = 12 bytes.
    // clang-format off
    EXPECT_EQ(index.bytes(),
              Bytes({0x00, 0x00, 0x00, 0x00, 0xe9, 0xa2, 0x8b, 0x3d, 0x1f, 0x00, 0x00, 0x00, //
                     0x01, 0x80, 0x77, 0x44, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, //
                     0x00, 0x00, 0x80, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00}));
    // clang-format on
}

TEST(TestPGMModel, LinearDoubleKeys) {
    const auto keys = LinearKeys<double>();
    PGMBytesProbe<double> index(keys.begin(), keys.end());

    ASSERT_EQ(index.segment_count(), 3u);
    EXPECT_EQ(index.segment(0).key, 0.0);
    EXPECT_EQ(index.segment(0).slope, 0.068181818181818191);
    EXPECT_EQ(index.segment(0).intercept, 32u);
    EXPECT_EQ(index.segment(1).key, std::nextafter(990.0, std::numeric_limits<double>::infinity()));
    EXPECT_EQ(index.segment(1).slope, 0.0);
    EXPECT_EQ(index.segment(1).intercept, 100u);
    EXPECT_TRUE(std::isinf(index.segment(2).key));

    // Segment = {double key, double slope, uint32_t intercept} = 20 bytes.
    // clang-format off
    EXPECT_EQ(index.bytes(),
              Bytes({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0xd1, 0x45, 0x17, 0x5d, 0x74, 0xb1, 0x3f, 0x20, 0x00, 0x00, 0x00, //
                     0x01, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x8e, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, //
                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00}));
    // clang-format on
}

// Integral keys keep probing with `sentinel - 1` (a real key), so their models must
// stay byte-identical: the floating-point fix must not change them.
TEST(TestPGMModel, LinearUInt64Keys) {
    std::vector<uint64_t> keys(100);
    for (size_t i = 0; i < keys.size(); ++i) {
        keys[i] = i * 10;
    }
    PGMBytesProbe<uint64_t> index(keys.begin(), keys.end());

    ASSERT_EQ(index.segment_count(), 2u);
    EXPECT_EQ(index.segment(0).key, 0u);
    EXPECT_EQ(static_cast<float>(index.segment(0).slope), 0.1646464616060257f);
    EXPECT_EQ(index.segment(0).intercept, 0u);
    EXPECT_EQ(index.segment(1).key, std::numeric_limits<uint64_t>::max());

    // Segment = {uint64_t key, float slope, uint32_t intercept} = 16 bytes.
    // clang-format off
    EXPECT_EQ(index.bytes(),
              Bytes({0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x15, 0x99, 0x28, 0x3e, 0x00, 0x00, 0x00, 0x00, //
                     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00}));
    // clang-format on
}

// `last + 1` rounds back to `last` for large keys, so the integer convention would
// give the guard segment a key equal to the last data key. It has to be strictly
// greater, otherwise the segment is unreachable and the index carries a dead entry.
TEST(TestPGMModel, GuardKeyFollowsLastKey) {
    {
        const auto keys = HugeKeys<float>();
        PGMBytesProbe<float> index(keys.begin(), keys.end());
        ASSERT_GE(index.segment_count(), 3u);
        for (size_t i = 0; i + 1 < index.segment_count(); ++i) {
            EXPECT_LT(index.segment(i).key, index.segment(i + 1).key) << "segment " << i;
        }
        EXPECT_EQ(index.segment(index.segment_count() - 2).key, std::nextafter(keys.back(), std::numeric_limits<float>::infinity()));
    }
    {
        const auto keys = HugeKeys<double>();
        PGMBytesProbe<double> index(keys.begin(), keys.end());
        ASSERT_GE(index.segment_count(), 3u);
        for (size_t i = 0; i + 1 < index.segment_count(); ++i) {
            EXPECT_LT(index.segment(i).key, index.segment(i + 1).key) << "segment " << i;
        }
        EXPECT_EQ(index.segment(index.segment_count() - 2).key, std::nextafter(keys.back(), std::numeric_limits<double>::infinity()));
    }
}

// For repeated keys the search range must cover the first occurrence.
TYPED_TEST(TestPGM, TestPGMRepeatedKeys) {
    using T = TypeParam;
    constexpr size_t group_size = 7;
    std::vector<T> keys;
    for (size_t i = 0; i < 100; ++i) {
        T key;
        if constexpr (std::integral<T>) {
            key = static_cast<T>(i);
        } else {
            key = static_cast<T>(i) * static_cast<T>(0.5);
        }
        keys.insert(keys.end(), group_size, key);
    }
    typename PGMT<T>::type index(keys.begin(), keys.end());

    for (size_t first = 0; first < keys.size(); first += group_size) {
        auto [pos, lo, hi] = index.search(keys[first]);
        EXPECT_LE(lo, first) << "first occurrence " << first;
        EXPECT_GT(hi, first) << "first occurrence " << first;
        EXPECT_LE(hi, keys.size()) << "first occurrence " << first;
    }
}

// Keys outside [first, last] - including the infinities and the type limits - must
// still produce a usable range instead of going through `size_t(+inf)` / `size_t(NaN)`.
TYPED_TEST(TestPGM, TestPGMOutOfRangeKeys) {
    using T = TypeParam;
    std::vector<T> keys(100);
    for (size_t i = 0; i < keys.size(); ++i) {
        if constexpr (std::integral<T>) {
            keys[i] = static_cast<T>(i + 1);
        } else {
            keys[i] = static_cast<T>(i + 1) * static_cast<T>(10);
        }
    }
    typename PGMT<T>::type index(keys.begin(), keys.end());
    const size_t n = keys.size();

    std::vector<T> probes{std::numeric_limits<T>::lowest(), std::numeric_limits<T>::max(), static_cast<T>(keys.back() + 1)};
    if constexpr (std::numeric_limits<T>::has_infinity) {
        probes.push_back(std::numeric_limits<T>::infinity());
        probes.push_back(-std::numeric_limits<T>::infinity());
    }
    if constexpr (std::is_signed_v<T>) {
        probes.push_back(static_cast<T>(keys.front() - 1));
    }

    for (T probe : probes) {
        auto [pos, lo, hi] = index.search(probe);
        EXPECT_LE(lo, hi) << "probe " << +probe;
        EXPECT_LE(hi, n) << "probe " << +probe;
        EXPECT_LE(pos, n) << "probe " << +probe;
    }

    // Below the first key the range starts at the beginning, above the last key it
    // ends at the end.
    EXPECT_EQ(index.search(std::numeric_limits<T>::lowest()).lo, 0u);
    EXPECT_EQ(index.search(static_cast<T>(keys.back() + 1)).hi, n);

    if constexpr (std::numeric_limits<T>::has_quiet_NaN) {
        auto [pos, lo, hi] = index.search(std::numeric_limits<T>::quiet_NaN());
        EXPECT_LE(lo, hi);
        EXPECT_LE(hi, n);
    }
}
