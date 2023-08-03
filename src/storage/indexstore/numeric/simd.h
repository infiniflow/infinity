#pragma once

#ifdef __SSE__

#include <x86intrin.h>
#define ctz(x) __builtin_ctz(x)

namespace infinity {

template<typename T>
int
LinearSearch(T *data, int start, int count, T key) {
    uint32_t c = start;
    uint32_t end = start + count;

#undef COMPARE
#define COMPARE(c)      if (key <= data[c]) {                           \
                            if (key < data[c])                          \
                                return -1;                              \
                            return c;                                   \
                        }

    while (c + 8 <= end) {
        COMPARE(c)
        COMPARE(c + 1)
        COMPARE(c + 2)
        COMPARE(c + 3)
        COMPARE(c + 4)
        COMPARE(c + 5)
        COMPARE(c + 6)
        COMPARE(c + 7)
        c += 8;
    }

    while (c < end) {
        COMPARE(c)
        c++;
    }

    /* the new key is > the last key in the page */
    return -1;
}

template<typename T>
inline int
LinearSearchSSE(T *data, int start, int count, T key) {
    return LinearSearch(data, start, count, key);
}

template<typename T>
inline int
SSEThreshold() {
    return 16;
}

template<>
inline int
SSEThreshold<uint32_t>() {
    return 16;
}

template<>
inline int
SSEThreshold<float>() {
    return 16;
}

template<>
inline int
SSEThreshold<uint64_t>() {
    return 4;
}

template<>
inline int
SSEThreshold<double>() {
    return 4;
}

template<typename T>
int
FindSimd(size_t node_count, T *data, const btree_key_t *hkey) {
    assert(hkey->size == sizeof(T));
    T key = *(T *)hkey->data_;

    // Run a binary search, but fall back to linear search as soon as
    // the remaining range is too small
    int threshold = SSEThreshold<T>();
    int i, l = 0, r = (int)node_count;
    int last = (int)node_count + 1;

    /* repeat till we found the key or the remaining range is so small that
     * we rather perform a linear search (which is faster for small ranges) */
    while (r - l > threshold) {
        /* get the median item; if it's identical with the "last" item,
         * we've found the slot */
        i = (l + r) / 2;

        if (i == last) {
            assert(i >= 0);
            assert(i < (int)node_count);
            return -1;
        }

        /* found it? */
        T d = data[i];
        /* if the key is < the current item: search "to the left" */
        if (key < d) {
            if (r == 0) {
                assert(i == 0);
                return -1;
            }
            r = i;
        }
        /* if the key is > the current item: search "to the right" */
        else if (key > d) {
            last = i;
            l = i;
        }
        /* otherwise we found the key */
        else
            return i;
    }

    // still here? then perform a linear search for the remaining range
    assert(r - l <= threshold);
    if (r + threshold < (int)node_count)
        return LinearSearchSSE(data, l, threshold, key);
    return LinearSearch(data, l, r - l, key);
}

template<>
inline int
LinearSearchSSE<uint16_t>(uint16_t *data, int start, int count, uint16_t key) {
    assert(count == 16);
    __m128i key8 = _mm_set1_epi16(key);

    __m128i v1 = _mm_loadu_si128((const __m128i *)&data[start + 0]);
    __m128i v2 = _mm_loadu_si128((const __m128i *)&data[start + 8]);

    __m128i cmp0 = _mm_cmpeq_epi16(key8, v1);
    __m128i cmp1 = _mm_cmpeq_epi16(key8, v2);

    __m128i pack01 = _mm_packs_epi16(cmp0, cmp1);

    int res = _mm_movemask_epi8(pack01);
    if (res > 0)
        return start + ctz(~res + 1);

    assert(16 == count);
    /* the new key is > the last key in the page */
    return -1;
}

template<>
inline int
LinearSearchSSE<uint32_t>(uint32_t *data, int start, int count, uint32_t key) {
    assert(count == 16);
    __m128i key4 = _mm_set1_epi32(key);

    __m128i v1 = _mm_loadu_si128((const __m128i *)&data[start + 0]);
    __m128i v2 = _mm_loadu_si128((const __m128i *)&data[start + 4]);
    __m128i v3 = _mm_loadu_si128((const __m128i *)&data[start + 8]);
    __m128i v4 = _mm_loadu_si128((const __m128i *)&data[start + 12]);

    __m128i cmp0 = _mm_cmpeq_epi32(key4, v1);
    __m128i cmp1 = _mm_cmpeq_epi32(key4, v2);
    __m128i cmp2 = _mm_cmpeq_epi32(key4, v3);
    __m128i cmp3 = _mm_cmpeq_epi32(key4, v4);

    __m128i pack01 = _mm_packs_epi32(cmp0, cmp1);
    __m128i pack23 = _mm_packs_epi32(cmp2, cmp3);
    __m128i pack0123 = _mm_packs_epi16(pack01, pack23);

    int res = _mm_movemask_epi8(pack0123);
    if (res > 0)
        return start + ctz(~res + 1);

    assert(16 == count);
    /* the new key is > the last key in the page */
    return -1;
}

template<>
inline int
LinearSearchSSE<float>(float *data, int start, int count, float key) {
    assert(count == 16);
    __m128 key4 = _mm_set1_ps(key);

    __m128 v1 = _mm_loadu_ps((const float *)&data[start + 0]);
    __m128 v2 = _mm_loadu_ps((const float *)&data[start + 4]);
    __m128 v3 = _mm_loadu_ps((const float *)&data[start + 8]);
    __m128 v4 = _mm_loadu_ps((const float *)&data[start + 12]);

    __m128 cmp0 = _mm_cmpeq_ps(key4, v1);
    __m128 cmp1 = _mm_cmpeq_ps(key4, v2);
    __m128 cmp2 = _mm_cmpeq_ps(key4, v3);
    __m128 cmp3 = _mm_cmpeq_ps(key4, v4);

    __m128i pack01 = _mm_packs_epi32(_mm_castps_si128(cmp0),
                                     _mm_castps_si128(cmp1));
    __m128i pack23 = _mm_packs_epi32(_mm_castps_si128(cmp2),
                                     _mm_castps_si128(cmp3));
    __m128i pack0123 = _mm_packs_epi16(pack01, pack23);

    int res = _mm_movemask_epi8(pack0123);
    if (res > 0)
        return start + ctz(~res + 1);

    assert(16 == count);
    /* the new key is > the last key in the page */
    return -1;
}

#ifdef __SSE4_1__
template<>
inline int
LinearSearchSSE<double>(double *data, int start, int count, double key) {
    assert(count == 4);
    __m128d key2 = _mm_set1_pd(key);

    __m128d v1 = _mm_loadu_pd(&data[start + 0]);
    __m128d v2 = _mm_loadu_pd(&data[start + 2]);

    __m128d cmp0 = _mm_cmpeq_pd(key2, v1);
    __m128d cmp1 = _mm_cmpeq_pd(key2, v2);

    __m128i low  = _mm_shuffle_epi32(_mm_castpd_si128(cmp0), 0xD8);
    __m128i high = _mm_shuffle_epi32(_mm_castpd_si128(cmp1), 0xD8);
    __m128i pack = _mm_unpacklo_epi64(low, high);

    __m128i pack01 = _mm_packs_epi32(pack, _mm_setzero_si128());
    __m128i pack0123 = _mm_packs_epi16(pack01, _mm_setzero_si128());

    int res = _mm_movemask_epi8(pack0123);
    if (res > 0)
        return start + ctz(~res + 1);

    assert(4 == count);
    /* the new key is > the last key in the page */
    return -1;
}

template<>
inline int
LinearSearchSSE<uint64_t>(uint64_t *data, int start, int count, uint64_t key) {
    assert(count == 4);
    __m128i key2 = _mm_set1_epi64x(key);

    __m128i v1 = _mm_loadu_si128((const __m128i *)&data[start + 0]);
    __m128i v2 = _mm_loadu_si128((const __m128i *)&data[start + 2]);

    __m128i cmp0 = _mm_cmpeq_epi64(key2, v1);
    __m128i cmp1 = _mm_cmpeq_epi64(key2, v2);

    __m128i low  = _mm_shuffle_epi32(cmp0, 0xD8);
    __m128i high = _mm_shuffle_epi32(cmp1, 0xD8);
    __m128i pack = _mm_unpacklo_epi64(low, high);

    __m128i pack01 = _mm_packs_epi32(pack, _mm_setzero_si128());
    __m128i pack0123 = _mm_packs_epi16(pack01, _mm_setzero_si128());

    int res = _mm_movemask_epi8(pack0123);
    if (res > 0)
        return start + ctz(~res + 1);

    assert(4 == count);
    /* the new key is > the last key in the page */
    return -1;
}
#endif

}

#endif // __SSE__
