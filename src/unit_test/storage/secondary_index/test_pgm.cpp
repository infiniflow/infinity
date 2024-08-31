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

#include "gtest/gtest.h"
import base_test;
import stl;
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
