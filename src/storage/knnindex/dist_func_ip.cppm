module;

#include "header.h"
#include <concepts>

import stl;
import dist_func;

export module dist_func_ip;

namespace infinity {

template <typename T>
concept IPDataType = requires(T data) {
    { data *data } -> std::same_as<T>;
    { data + data } -> std::same_as<T>;
};

template <typename T>
    requires IPDataType<T>
static T IPSqr(const T *v1, const T *v2, size_t dim) {
    T res = 0;
    for (size_t i = 0; i < dim; ++i) {
        T t = *v1 - *v2;
        ++v1;
        ++v2;
        res += t * t;
    }
    return res;
}

#if defined(USE_AVX512)
// Favor using AVX512 if available.
static float FloatIPSqrSIMD16ExtAVX512(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN64 TmpRes[16];
    size_t qty16 = qty >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);

    __m512 v1, v2;
    __m512 sum = _mm512_set1_ps(0);

    while (pVect1 < pEnd1) {
        v1 = _mm512_loadu_ps(pVect1);
        pVect1 += 16;
        v2 = _mm512_loadu_ps(pVect2);
        pVect2 += 16;
        sum = _mm512_add_ps(sum, _mm512_mul_ps(v2, v1));
    }

    _mm512_store_ps(TmpRes, sum);
    float res = -(TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7] + TmpRes[8] + TmpRes[9] + TmpRes[10] +
                  TmpRes[11] + TmpRes[12] + TmpRes[13] + TmpRes[14] + TmpRes[15]);

    return (res);
}
#endif

#if defined(USE_AVX)

// Favor using AVX if available.
static float FloatIPSqrSIMD16ExtAVX(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];
    size_t qty16 = qty >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);

    __m256 v1, v2;
    __m256 sum = _mm256_set1_ps(0);

    while (pVect1 < pEnd1) {
        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));

        v1 = _mm256_loadu_ps(pVect1);
        pVect1 += 8;
        v2 = _mm256_loadu_ps(pVect2);
        pVect2 += 8;
        sum = _mm256_add_ps(sum, _mm256_mul_ps(v1, v2));
    }

    _mm256_store_ps(TmpRes, sum);
    return -(TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3] + TmpRes[4] + TmpRes[5] + TmpRes[6] + TmpRes[7]);
}

#endif

#if defined(USE_SSE)

static float FloatIPSqrSIMD16ExtSSE(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];
    size_t qty16 = qty >> 4;

    const float *pEnd1 = pVect1 + (qty16 << 4);

    __m128 v1, v2;
    __m128 sum = _mm_set1_ps(0);

    while (pVect1 < pEnd1) {
        //_mm_prefetch((char*)(pVect2 + 16), _MM_HINT_T0);
        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));

        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));
    }

    _mm_store_ps(TmpRes, sum);
    return -(TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3]);
}
#endif

#if defined(USE_SSE) || defined(USE_AVX) || defined(USE_AVX512)
static DistFunc<float> FloatIPSqrSIMD16Ext = FloatIPSqrSIMD16ExtSSE;

static float FloatIPSqrSIMD16ExtResiduals(const float *pVect1v, const float *pVect2v, size_t qty) {
    size_t qty16 = qty >> 4 << 4;
    float res = FloatIPSqrSIMD16Ext(pVect1v, pVect2v, qty16);
    float *pVect1 = (float *)pVect1v + qty16;
    float *pVect2 = (float *)pVect2v + qty16;

    size_t qty_left = qty - qty16;
    float res_tail = IPSqr<float>(pVect1, pVect2, qty_left);
    return (res + res_tail);
}
#endif

#if defined(USE_SSE)
static float FloatIPSqrSIMD4Ext(const float *pVect1, const float *pVect2, size_t qty) {
    float PORTABLE_ALIGN32 TmpRes[8];

    size_t qty4 = qty >> 2;

    const float *pEnd1 = pVect1 + (qty4 << 2);

    __m128 v1, v2;
    __m128 sum = _mm_set1_ps(0);

    while (pVect1 < pEnd1) {
        v1 = _mm_loadu_ps(pVect1);
        pVect1 += 4;
        v2 = _mm_loadu_ps(pVect2);
        pVect2 += 4;
        sum = _mm_add_ps(sum, _mm_mul_ps(v1, v2));
    }
    _mm_store_ps(TmpRes, sum);
    return TmpRes[0] + TmpRes[1] + TmpRes[2] + TmpRes[3];
}

static float FloatIPSqrSIMD4ExtResiduals(const float *pVect1v, const float *pVect2v, size_t qty) {
    size_t qty4 = qty >> 2 << 2;

    float res = FloatIPSqrSIMD4Ext(pVect1v, pVect2v, qty4);
    size_t qty_left = qty - qty4;

    float *pVect1 = (float *)pVect1v + qty4;
    float *pVect2 = (float *)pVect2v + qty4;
    float res_tail = IPSqr<float>(pVect1, pVect2, qty_left);

    return (res + res_tail);
}
#endif

export template <typename T>
    requires IPDataType<T>
class DistFuncIP : public SpaceBase<T> {
    DistFunc<T> fstdistfunc_;

public:
    DistFuncIP(size_t dim) {
        fstdistfunc_ = IPSqr<T>;
#if defined(USE_SSE) || defined(USE_AVX) || defined(USE_AVX512)
#if defined(USE_AVX512)
        if (AVX512Capable())
            FloatIPSqrSIMD16Ext = FloatIPSqrSIMD16ExtAVX512;
        else if (AVXCapable())
            FloatIPSqrSIMD16Ext = FloatIPSqrSIMD16ExtAVX;
#elif defined(USE_AVX)
        if (AVXCapable())
            FloatIPSqrSIMD16Ext = FloatIPSqrSIMD16ExtAVX;
#endif

        if (dim % 16 == 0)
            fstdistfunc_ = FloatIPSqrSIMD16Ext;
        else if (dim % 4 == 0)
            fstdistfunc_ = FloatIPSqrSIMD4Ext;
        else if (dim > 16)
            fstdistfunc_ = FloatIPSqrSIMD16ExtResiduals;
        else if (dim > 4)
            fstdistfunc_ = FloatIPSqrSIMD4ExtResiduals;
#endif
    }

    virtual DistFunc<T> DistFuncPtr() const override { return fstdistfunc_; }
};

} // namespace infinity