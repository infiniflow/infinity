#pragma once

#include "builtin.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>

namespace infinity {

bool RadixPrepare(size_t n, size_t last[257], size_t ptr[256], size_t cnt[256]);

template <typename T>
void RadixSortCore(const size_t *last, T *a, size_t n, uint32_t *radix_scratch, unsigned int shift_width) __attribute__((noinline));

template <typename T>
void RadixSortCore(const size_t *last, T *a, size_t n, uint32_t *radix_scratch, unsigned int shift_width) {
    T temp, swap;
    // Go through all permutation cycles until all
    // elements are moved or found to be already in place
    size_t ptr[256];
    size_t i, j, k;
    memcpy(ptr, last, sizeof(ptr));
    i = 0;
    size_t remain = n;

    while (remain > 0) {
        // Find first uncompleted class
        while (ptr[i] == last[i + 1]) {
            i++;
        }

        // Grab first element to move
        j = ptr[i];
        uint32_t swapK = radix_scratch[j];
        k = (swapK >> shift_width) & 0xFF;

        // Swap into correct class until cycle completed
        if (i != k) {
            swap = a[j];
            do {
                size_t t(ptr[k]);
                temp = a[t];
                uint32_t temp_k(radix_scratch[t]);
                radix_scratch[t] = swapK;
                a[t] = swap;
                ptr[k]++;
                swapK = temp_k;
                swap = temp;
                k = (temp_k >> shift_width) & 0xFF;
                remain--;
            } while (i != k);
            // Place last element in cycle
            a[j] = swap;
            radix_scratch[j] = swapK;
        }
        ptr[k]++;
        remain--;
    }
}

template <typename T, typename GR>
unsigned int RadixFetch(T *a, size_t n, uint32_t *radix_scratch, GR R) __attribute__((noinline));

template <typename T, typename GR>
unsigned int RadixFetch(T *a, size_t n, uint32_t *radix_scratch, GR R) {
    size_t i = 0;
    uint32_t used_bits = 0;
    if (n > 3) {
        for (; i < n - 3; i += 4) {
            radix_scratch[i + 0] = R(a[i + 0]);
            radix_scratch[i + 1] = R(a[i + 1]);
            radix_scratch[i + 2] = R(a[i + 2]);
            radix_scratch[i + 3] = R(a[i + 3]);
            used_bits |= radix_scratch[i + 0];
            used_bits |= radix_scratch[i + 1];
            used_bits |= radix_scratch[i + 2];
            used_bits |= radix_scratch[i + 3];
        }
    }
    for (; i < n; i++) {
        radix_scratch[i] = R(a[i]);
        used_bits |= radix_scratch[i];
    }
    if (used_bits != 0) {
        int msb = Builtin::msb(used_bits);
        return (msb + 8) & ~0x7;
    }
    return 0;
}

/**
 * radix sort implementation.
 *
 * @param stack_depth recursion level reached; since RadixSort uses
 *                   lots of stack we try another algorithm if this
 *                   becomes too high.
 * @param a Pointer to the start of the array to sort
 * @param n number of data elements to sort
 * @param radix_scratch scratch area for upto 32bits of sorting data
 * @param radix_bits how many bits of sorting data radix_scratch contains
 * @param insert_sort_level when to fall back to simple insertion sort
 **/
template <typename T, typename GR, typename GE, typename GRE>
void RadixSort(GR R,
               GE E,
               GRE EE,
               int stack_depth,
               T *a,
               size_t n,
               uint32_t *radix_scratch,
               int radix_bits,
               unsigned insert_sort_level = 10,
               size_t topn = std::numeric_limits<size_t>::max()) {
    if (((stack_depth > 20) && (radix_bits == 0)) || (n < insert_sort_level)) {
        // switch to simpler sort if few elements
        if (n > 1) {
            std::sort(a, a + n, E);
        }
        return;
    }

    size_t last[257];
    size_t cnt[256];
    int shift_width = radix_bits - 8;
    for (bool all_in_one_bucket(true); all_in_one_bucket;) {
        while (radix_bits == 0) {
            // no data left in scratch buffer; fill up with upto 32 new bits
            radix_bits = RadixFetch(a, n, radix_scratch, R);
            if (radix_bits == 0) {
                if (EE.alwaysEofOnCheck() || radix_eof(a, n, EE)) {
                    // everything has reached end-of-string terminating zero,
                    // so we are done sorting.
                    return;
                }
            }
        }

        shift_width = radix_bits - 8;
        memset(cnt, 0, sizeof(cnt));
        size_t i = 0;
        if (n > 3) {
            for (; i < n - 3; i += 4) {
                cnt[(radix_scratch[i + 0] >> shift_width) & 0xFF]++;
                cnt[(radix_scratch[i + 1] >> shift_width) & 0xFF]++;
                cnt[(radix_scratch[i + 2] >> shift_width) & 0xFF]++;
                cnt[(radix_scratch[i + 3] >> shift_width) & 0xFF]++;
            }
        }
        for (; i < n; i++) {
            cnt[(radix_scratch[i] >> shift_width) & 0xFF]++;
        }

        // Accumulate cnt positions
        all_in_one_bucket = false;
        last[0] = 0;
        for (i = 1; (i < 257) && !all_in_one_bucket; i++) {
            last[i] = last[i - 1] + cnt[i - 1];
            all_in_one_bucket = (cnt[i - 1] == n);
        }

        radix_bits -= 8;
    }

    RadixSortCore(last, a, n, radix_scratch, shift_width);

    // Sort on next 8 bits of key
    for (size_t i(0), sum(0); (i < 256) && (sum < topn); i++) {
        const size_t l(last[i]);
        const size_t c(cnt[i]);
        if (c) {
            if (c > insert_sort_level) {
                RadixSort(R, E, EE, stack_depth + 1, &a[l], c, &radix_scratch[l], radix_bits, insert_sort_level, topn - sum);
            } else {
                std::sort(&a[l], &a[l] + c, E);
            }
            sum += c;
        }
    }
}

template <typename GR, typename T, int SHIFT>
class ShiftBasedRadixSorterBase {
protected:
    static void RadixFetch(GR R, size_t cnt[256], const T *a, size_t n) __attribute__((noinline));
    static void RadixSortCore(GR R, size_t ptr[256], size_t last[257], T *a, size_t n) __attribute__((noinline));
};

template <typename GR, typename T, int SHIFT>
void ShiftBasedRadixSorterBase<GR, T, SHIFT>::RadixFetch(GR R, size_t cnt[256], const T *a, size_t n) {
    memset(cnt, 0, 256 * sizeof(cnt[0]));
    size_t p(0);
    if (n > 3) {
        for (; p < n - 3; p += 4) {
            cnt[(R(a[p]) >> SHIFT) & 0xFF]++;
            cnt[(R(a[p + 1]) >> SHIFT) & 0xFF]++;
            cnt[(R(a[p + 2]) >> SHIFT) & 0xFF]++;
            cnt[(R(a[p + 3]) >> SHIFT) & 0xFF]++;
        }
    }
    for (; p < n; p++) {
        cnt[(R(a[p]) >> SHIFT) & 0xFF]++;
    }
}

template <typename GR, typename T, int SHIFT>
void ShiftBasedRadixSorterBase<GR, T, SHIFT>::RadixSortCore(GR R, size_t ptr[256], size_t last[257], T *a, size_t n) {
    // Go through all permutation cycles until all
    // elements are moved or found to be already in place
    size_t i(0), remain(n);
    size_t j, k;
    T temp, swap;

    while (remain > 0) {
        // Find first uncompleted class
        while (ptr[i] == last[i + 1]) {
            i++;
        }

        // Grab first element to move
        j = ptr[i];
        k = (R(a[j]) >> SHIFT) & 0xFF;

        // Swap into correct class until cycle completed
        if (i != k) {
            swap = a[j];
            do {
                temp = a[ptr[k]];
                a[ptr[k]++] = swap;
                k = (R(swap = temp) >> SHIFT) & 0xFF;
                remain--;
            } while (i != k);
            // Place last element in cycle
            a[j] = swap;
        }
        ptr[k]++;
        remain--;
    }
}

/**
 * @param T the type of the object being sorted
 * @param GR the functor used to fetch the number used for radix sorting. It must enure same sorting as GE.
 * @param GE the functor used for testing if one object is orderers ahead of another.
 * @param SHIFT is the number of significant bits in the radix - 8. Must a multiple of 8.
 * @param continue_after_radix_ends indicates if the radix only represents a prefix of the objects. If it is true we
 *        will continue using std::sort to order objects that have equal radix representation.
 */
template <typename T, typename GR, typename GE, int SHIFT, bool continue_after_radix_ends = false>
class ShiftBasedRadixSorter : private ShiftBasedRadixSorterBase<GR, T, SHIFT> {
public:
    static size_t RadixSort(GR R, GE E, T *a, size_t n, unsigned int insert_sort_level = 10, size_t topn = std::numeric_limits<size_t>::max());
    static size_t RadixSortInternal(GR R, GE E, T *a, size_t n, unsigned int insert_sort_level, size_t topn);

private:
    using Base = ShiftBasedRadixSorterBase<GR, T, SHIFT>;
};

template <typename T, typename GR, typename GE, int SHIFT, bool continue_after_radix_ends>
size_t ShiftBasedRadixSorter<T, GR, GE, SHIFT, continue_after_radix_ends>::RadixSortInternal(GR R,
                                                                                             GE E,
                                                                                             T *a,
                                                                                             size_t n,
                                                                                             unsigned int insert_sort_level,
                                                                                             size_t topn) {
    size_t last[257], ptr[256], cnt[256];
    size_t sum(n);

    Base::RadixFetch(R, cnt, a, n);

    bool sorted = RadixPrepare(n, last, ptr, cnt);

    if (!sorted) {
        Base::RadixSortCore(R, ptr, last, a, n);
    } else {
        return ShiftBasedRadixSorter<T, GR, GE, SHIFT - 8, continue_after_radix_ends>::RadixSortInternal(R, E, a, n, insert_sort_level, topn);
    }

    if (SHIFT > 0 || continue_after_radix_ends) {
        // Sort on next key
        sum = 0;
        for (unsigned i(0); (i < 256) && (sum < topn); i++) {
            const size_t c(cnt[i]);
            const size_t l(last[i]);
            if (c) {
                if (c > insert_sort_level) {
                    sum += ShiftBasedRadixSorter<T, GR, GE, SHIFT - 8, continue_after_radix_ends>::RadixSortInternal(R,
                                                                                                                     E,
                                                                                                                     &a[l],
                                                                                                                     c,
                                                                                                                     insert_sort_level,
                                                                                                                     topn - sum);
                } else {
                    std::sort(a + l, a + l + c, E);
                    sum += c;
                }
            }
        }
    }
    return sum;
}

template <typename T, typename GR, typename GE, int SHIFT, bool continue_after_radix_ends>
size_t ShiftBasedRadixSorter<T, GR, GE, SHIFT, continue_after_radix_ends>::RadixSort(GR R,
                                                                                     GE E,
                                                                                     T *a,
                                                                                     size_t n,
                                                                                     unsigned int insert_sort_level,
                                                                                     size_t topn) {
    if (n > insert_sort_level) {
        return RadixSortInternal(R, E, a, n, insert_sort_level, topn);
    } else if (n > 1) {
        std::sort(a, a + n, E);
    }
    return n;
}

template <typename A, typename B, typename C>
class ShiftBasedRadixSorter<A, B, C, -8, false> {
public:
    static size_t RadixSortInternal(B, C, A *, size_t, unsigned int, size_t) { return 0; }
};

template <typename A, typename B, typename C>
class ShiftBasedRadixSorter<A, B, C, -8, true> {
public:
    static size_t RadixSortInternal(B, C E, A *v, size_t sz, unsigned int, size_t) {
        std::sort(v, v + sz, E);
        return sz;
    }
};

template <typename GR, typename T, int IDX>
void RadixFetch2(GR R, size_t cnt[256], const T *a, size_t n) __attribute__((noinline));

template <typename GR, typename T, int IDX>
void RadixFetch2(GR R, size_t cnt[256], const T *a, size_t n) {
    memset(cnt, 0, 256 * sizeof(cnt[0]));
    size_t p(0);
    if (n > 3) {
        for (; p < n - 3; p += 4) {
            cnt[R(a[p + 0], IDX)]++;
            cnt[R(a[p + 1], IDX)]++;
            cnt[R(a[p + 2], IDX)]++;
            cnt[R(a[p + 3], IDX)]++;
        }
    }
    for (; p < n; p++) {
        cnt[R(a[p], IDX)]++;
    }
}

template <typename T, typename GR, typename GE, int LEN, int POS>
void RadixSortInternal(GR R, GE E, T *a, size_t n, unsigned int insert_sort_level, size_t topn) {
    size_t last[257], ptr[256], cnt[256];

    RadixFetch2<GR, T, LEN - POS>(R, cnt, a, n);

    bool sorted = RadixPrepare(n, last, ptr, cnt);

    if (!sorted) {
        // Go through all permutation cycles until all
        // elements are moved or found to be already in place
        size_t i(0), remain(n);
        size_t j, k;
        T temp, swap;

        while (remain > 0) {
            // Find first uncompleted class
            while (ptr[i] == last[i + 1]) {
                i++;
            }

            // Grab first element to move
            j = ptr[i];
            k = R(a[j], LEN - POS);

            // Swap into correct class until cycle completed
            if (i != k) {
                swap = a[j];
                do {
                    temp = a[ptr[k]];
                    a[ptr[k]++] = swap;
                    k = R(swap = temp, LEN - POS);
                    remain--;
                } while (i != k);
                // Place last element in cycle
                a[j] = swap;
            }
            ptr[k]++;
            remain--;
        }
    } else {
        RadixSortInternal<T, GR, GE, LEN, POS - 1>(R, E, a, n, insert_sort_level, topn);
        return;
    }

    if (LEN > 0) {
        // Sort on next key
        for (size_t i(0), sum(0); (i < 256) && (sum < topn); i++) {
            const size_t c(cnt[i]);
            const size_t l(last[i]);
            if (c) {
                if (c > insert_sort_level) {
                    RadixSortInternal<T, GR, GE, LEN, POS - 1>(R, E, &a[l], c, insert_sort_level, topn - sum);
                } else {
                    std::sort(a + l, a + l + c, E);
                }
                sum += c;
            }
        }
    }
}

template <typename T, typename GR, typename GE, int LEN, int POS>
void RadixSort(GR R, GE E, T *a, size_t n, unsigned int insert_sort_level = 10, size_t topn = std::numeric_limits<size_t>::max()) {
    if (n > insert_sort_level) {
        RadixSortInternal<T, GR, GE, LEN, POS>(R, E, a, n, insert_sort_level, topn);
    } else if (n > 1) {
        std::sort(a, a + n, E);
    }
}

template <typename T, typename GR, int SHIFT>
void RadixStableCore(GR R, size_t ptr[256], const T *a, T *b, size_t n) __attribute__((noinline));

template <typename T, typename GR, int SHIFT>
void RadixStableCore(GR R, size_t ptr[256], const T *a, T *b, size_t n) {
    size_t k;
    for (size_t i(0); i < n; i++) {
        k = (R(a[i]) >> SHIFT) & 0xFF;
        b[ptr[k]] = a[i];
        ptr[k]++;
    }
}

template <typename T, typename GR, typename GE, int SHIFT>
T *RadixStableSortInternal(GR R, GE E, T *a, T *b, size_t n, unsigned int insert_sort_level = 10) {
    size_t last[257], ptr[256], cnt[256];

    RadixFetch<GR, T, SHIFT>(R, cnt, a, n);

    bool sorted = RadixPrepare(n, last, ptr, cnt);

    if (!sorted) {
        RadixStableCore<T, R, SHIFT>(R, ptr, a, b, n);
    } else {
        return RadixStableSortInternal<T, GR, GE, SHIFT - 8>(R, E, a, b, n, insert_sort_level);
    }

    if (SHIFT > 0) {
        // Sort on next key
        for (unsigned i(0); i < 256; i++) {
            const size_t c(cnt[i]);
            const size_t l(last[i]);
            if (c > insert_sort_level) {
                const T *r = RadixStableSortInternal<T, GR, GE, SHIFT - 8>(R, E, &b[l], &a[l], c, insert_sort_level);
                if (r != &b[l]) {
                    memcpy(&b[l], &a[l], c * sizeof(*r));
                }
            } else {
                if (c > 1) {
                    std::stable_sort(b + l, b + l + c, E);
                }
            }
        }
    }
    return b;
}

template <typename T, typename GR, typename GE, int SHIFT>
T *RadixStableSort(GR R, GE E, T *a, T *b, size_t n, unsigned int insert_sort_level = 10) {
    if (n > insert_sort_level) {
        return RadixStableSortInternal<T, GR, GE, SHIFT>(R, E, a, b, n, insert_sort_level);
    } else if (n > 1) {
        std::stable_sort(a, a + n, E);
    }
    return a;
}

} // namespace infinity
