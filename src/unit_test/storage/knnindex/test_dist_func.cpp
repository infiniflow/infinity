#include "unit_test/base_test.h"
#include <random>

import dist_func_l2_2;

class DistFuncTest : public BaseTest {};

uint16_t U8IPTest(const uint8_t *v1, const uint8_t *v2, size_t dim) {
    uint16_t res = 0;
    for (size_t i = 0; i < dim; ++i) {
        res += v1[i] * v2[i];
    }
    return res;
}

TEST_F(DistFuncTest, test1) {
    using namespace infinity;

    size_t dim = 128;
    size_t vec_n = 10000;

    auto vecs1 = std::make_unique<uint8_t[]>(dim * vec_n);
    auto vecs2 = std::make_unique<uint8_t[]>(dim * vec_n);

    // generate a random vector of uint8_t
    std::default_random_engine rng;
    std::uniform_real_distribution<float> distrib_real;

    for (size_t i = 0; i < vec_n; ++i) {
        auto v1 = vecs1.get() + i * dim;
        auto v2 = vecs2.get() + i * dim;
        auto res = U8IPSIMD16ExtAVX(v1, v2, dim);
        auto res2 = U8IPTest(v1, v2, dim);
        EXPECT_EQ(res, res2);
    }
}