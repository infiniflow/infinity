//
// Created by yzq on 11/8/23.
//
module;

#include <immintrin.h>
import stl;
export module some_simd_functions;

namespace infinity {
export f32 L2Distance_simd(const f32 *vector1, const f32 *vector2, u32 dimension) {
    f32 distance_1{};
    f32 distance_2{};
    u32 i = 0;
    _mm_prefetch(vector1, _MM_HINT_NTA);
    _mm_prefetch(vector2, _MM_HINT_NTA);
    for (; i + 16 <= dimension; i += 16) {
        _mm_prefetch(vector1 + i + 16, _MM_HINT_NTA);
        _mm_prefetch(vector2 + i + 16, _MM_HINT_NTA);
        auto diff_1 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto diff_2 = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i + 8), _mm256_loadu_ps(vector2 + i + 8));
        auto mul_1 = _mm256_mul_ps(diff_1, diff_1);
        auto mul_2 = _mm256_mul_ps(diff_2, diff_2);
        // use hadd to add up mul
        mul_1 = _mm256_hadd_ps(mul_1, mul_1);
        mul_2 = _mm256_hadd_ps(mul_2, mul_2);
        mul_1 = _mm256_hadd_ps(mul_1, mul_1);
        mul_2 = _mm256_hadd_ps(mul_2, mul_2);
        distance_1 += mul_1[0] + mul_1[4];
        distance_2 += mul_2[0] + mul_2[4];
    }
    f32 distance = distance_1 + distance_2;
    if (i + 8 <= dimension) {
        auto diff = _mm256_sub_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto mul = _mm256_mul_ps(diff, diff);
        // use hadd to add up mul
        mul = _mm256_hadd_ps(mul, mul);
        mul = _mm256_hadd_ps(mul, mul);
        distance += mul[0] + mul[4];
        i += 8;
    }
    for (; i < dimension; ++i) {
        auto diff = vector1[i] - vector2[i];
        distance += diff * diff;
    }
    return distance;
}

export f32 IPDistance_simd(const f32 *vector1, const f32 *vector2, u32 dimension) {
    f32 distance_1{};
    f32 distance_2{};
    u32 i = 0;
    _mm_prefetch(vector1, _MM_HINT_NTA);
    _mm_prefetch(vector2, _MM_HINT_NTA);
    for (; i + 16 <= dimension; i += 16) {
        _mm_prefetch(vector1 + i + 16, _MM_HINT_NTA);
        _mm_prefetch(vector2 + i + 16, _MM_HINT_NTA);
        auto mul_1 = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        auto mul_2 = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i + 8), _mm256_loadu_ps(vector2 + i + 8));
        // use hadd to add up mul
        mul_1 = _mm256_hadd_ps(mul_1, mul_1);
        mul_2 = _mm256_hadd_ps(mul_2, mul_2);
        mul_1 = _mm256_hadd_ps(mul_1, mul_1);
        mul_2 = _mm256_hadd_ps(mul_2, mul_2);
        distance_1 += mul_1[0] + mul_1[4];
        distance_2 += mul_2[0] + mul_2[4];
    }
    f32 distance = distance_1 + distance_2;
    if (i + 8 <= dimension) {
        auto mul = _mm256_mul_ps(_mm256_loadu_ps(vector1 + i), _mm256_loadu_ps(vector2 + i));
        // use hadd to add up mul
        mul = _mm256_hadd_ps(mul, mul);
        mul = _mm256_hadd_ps(mul, mul);
        distance += mul[0] + mul[4];
        i += 8;
    }
    for (; i < dimension; ++i) {
        distance += vector1[i] * vector2[i];
    }
    return distance;
}

} // namespace infinity