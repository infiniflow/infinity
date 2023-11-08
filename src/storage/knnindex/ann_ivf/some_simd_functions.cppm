//
// Created by yzq on 11/8/23.
//
module;

#include <immintrin.h>
import stl;
export module some_simd_functions;

namespace infinity {
export f32 L2Distance_simd(const f32 *vector1, const f32 *vector2, u32 dimension) {
    f32 distance{};
    u32 i = 0;
    for (; i + 8 <= dimension; i += 8) {
        auto diff = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto mul = _mm256_mul_ps(diff, diff);
        // use hadd to add up mul
        mul = _mm256_hadd_ps(mul, mul);
        mul = _mm256_hadd_ps(mul, mul);
        distance += mul[0] + mul[4];
    }
    for (; i < dimension; ++i) {
        auto diff = vector1[i] - vector2[i];
        distance += diff * diff;
    }
    return distance;
}

export f32 IPDistance_simd(const f32 *vector1, const f32 *vector2, u32 dimension) {
    f32 distance{};
    u32 i = 0;
    for (; i + 8 <= dimension; i += 8) {
        auto mul = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        // use hadd to add up mul
        mul = _mm256_hadd_ps(mul, mul);
        mul = _mm256_hadd_ps(mul, mul);
        distance += mul[0] + mul[4];
    }
    for (; i < dimension; ++i) {
        auto diff = vector1[i] - vector2[i];
        distance += diff * diff;
    }
    return distance;
}

} // namespace infinity