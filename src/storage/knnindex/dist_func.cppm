module;

#include <concepts>
#include <immintrin.h>

import stl;

export module dist_func;

namespace infinity {

#ifdef __AVX__
#define USE_AVX
#ifdef __AVX512F__
#define USE_AVX512
#endif
#endif

template<typename T>
static T L2Sqr(const T *v1, const T *v2, size_t dim) {
    T res = 0;
    for (size_t i = 0; i < dim; ++i) {
        T t = *v1 - *v2;
        ++v1;
        ++v2;
        res += t * t;
    }
    return res;
}

#if defined(USE_AVX)

// Favor using AVX if available.
static float FloatL2SqrSIMD16ExtAVX(const float *pVect1, const float *pVect2, size_t dim) {
    float  TmpRes[8];
    size_t qty16 = dim >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);

    __m256 diff, v1, v2;
    __m256 sum = _mm256_set1_ps(0);

    while (pVect1 < pEnd1) {
        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        diff = _mm256_sub_ps(v1, v2);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));

        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        diff = _mm256_sub_ps(v1, v2);
        sum = _mm256_add_ps(sum, _mm256_mul_ps(diff, diff));
    }

    _mm256_store_ps(TmpRes, sum);
    return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7];
}

#endif

export template <typename DataType>
using DistFunc = DataType(*)(const DataType *, const DataType *, size_t);

export template <typename DataType, typename SpaceType>
concept SpaceConcept = requires(SpaceType space) {
    { space.Dimension() } -> std::same_as<size_t>;
    { space.template DistFuncPtr<DataType>() } -> std::same_as<DistFunc<DataType>>;
};

export class DistFuncL2 {
    const size_t dim_;

public:
    DistFuncL2(size_t dim) : dim_(dim) {}

    size_t Dimension() const { return dim_; }

    template <typename DataType>
    DistFunc<DataType> DistFuncPtr() = delete;

    template <>
    inline DistFunc<float> DistFuncPtr<float>() {
        // return L2Sqr<float>;
        if (dim_ % 16 != 0) {
            return nullptr;
        }
        return FloatL2SqrSIMD16ExtAVX;
        // if (dim_ % 16 == 0) {
        // } else if (dim_ % 4 == 0) {
        // } else if (dim_ > 16) {
        // } else if (dim_ > 4) {
        // }
    }
};

} // namespace infinity
