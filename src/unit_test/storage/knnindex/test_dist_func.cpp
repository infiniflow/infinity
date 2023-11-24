#include "unit_test/base_test.h"
#include <random>

import dist_func_l2_2;
import lvq_store;

using namespace infinity;

class DistFuncTest : public BaseTest {};

uint32_t U8IPTest(const uint8_t *v1, const uint8_t *v2, size_t dim) {
    uint32_t res = 0;
    for (size_t i = 0; i < dim; ++i) {
        res += uint16_t(v1[i]) * v2[i];
    }
    return res;
}

uint16_t U8L1Test(const uint8_t *v, size_t dim) {
    uint16_t res = 0;
    for (size_t i = 0; i < dim; ++i) {
        res += v[i];
    }
    return res;
}

float F32L2Test(const float *v1, const float *v2, size_t dim) {
    float res = 0;
    for (size_t i = 0; i < dim; ++i) {
        float t = v1[i] - v2[i];
        res += t * t;
    }
    return res;
}

TEST_F(DistFuncTest, test1) {
    size_t dim = 32;
    size_t vec_n = 10000;

    auto vecs1 = std::make_unique<uint8_t[]>(dim * vec_n);
    auto vecs2 = std::make_unique<uint8_t[]>(dim * vec_n);

    assert(dim % 32 == 0);

    // generate a random vector of uint8_t
    std::default_random_engine rng;
    std::uniform_int_distribution<uint8_t> dist(0, 255);
    for (size_t i = 0; i < vec_n; ++i) {
        for (size_t j = 0; j < dim; ++j) {
            vecs1[i * dim + j] = dist(rng);
            vecs2[i * dim + j] = dist(rng);
        }
    }

    for (size_t i = 0; i < vec_n; ++i) {
        auto v1 = vecs1.get() + i * dim;
        auto v2 = vecs2.get() + i * dim;
        uint32_t res = U8IPSIMD16ExtAVX(v1, v2, dim);
        uint32_t res2 = U8IPTest(v1, v2, dim);
        EXPECT_EQ(res, res2);
    }
}

TEST_F(DistFuncTest, test2) {
    using LVQ8Store = LVQStore<float, uint8_t>;
    using LVQ8 = LVQ8Store::RtnType;

    size_t dim = 128;
    size_t vec_n = 10000;

    auto vecs1 = std::make_unique<float[]>(dim * vec_n);
    auto vecs2 = std::make_unique<float[]>(dim * vec_n);

    // generate a random vector of float
    std::default_random_engine rng;
    std::uniform_int_distribution<int> max_dist(0, 10000);
    for (size_t j = 0; j < dim; ++j) {
        std::uniform_real_distribution<float> dist1(0, max_dist(rng));
        std::uniform_real_distribution<float> dist2(0, max_dist(rng));
        for (size_t i = 0; i < vec_n; ++i) {
            vecs1[i * dim + j] = dist1(rng);
            vecs2[i * dim + j] = dist2(rng);
        }
    }

    auto lvq_store = LVQ8Store::Make(vec_n, dim, {0, true});
    lvq_store.AddBatchVec(vecs1.get(), vec_n);

    auto space = std::make_unique<uint8_t[]>(dim);
    for (size_t i = 0; i < vec_n; ++i) {
        float dist, dist2, dist3;
        const float *v1 = vecs1.get() + i * dim;
        const float *v2 = vecs2.get() + i * dim;

        LVQ8 lvq1 = lvq_store.GetVec(i);
        LVQ8 lvq2 = lvq_store.Convert(v2);

        dist = L2Sqr3(lvq1, lvq2, dim);

        {
            float v1[dim];
            float v2[dim];
            lvq_store.DecompressForTest(lvq1, v1);
            lvq_store.DecompressForTest(lvq2, v2);

            dist2 = F32L2Test(v1, v2, dim);
        }

        dist3 = F32L2Test(v1, v2, dim);

        std::cout << dist << "\t" << dist2 << "\t" << dist3 << std::endl;
        EXPECT_EQ(dist, dist2);
    }
}
