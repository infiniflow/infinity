//
// Created by jinhai on 23-9-25.
//

#pragma once

#include "faiss/utils/ordered_key_value.h"
#include <cassert>

namespace infinity {

/*******************************************************************
 * Basic heap ops: push and pop
 *******************************************************************/

/** Pops the top element from the heap defined by bh_val[0..k-1] and
 * bh_ids[0..k-1].  on output the element at k-1 is undefined.
 */
template <class C>
inline void heap_pop(size_t k, typename C::T* bh_val, typename C::TI* bh_ids) {
    bh_val--; /* Use 1-based indexing for easier node->child translation */
    bh_ids--;
    typename C::T val = bh_val[k];
    typename C::TI id = bh_ids[k];
    size_t i = 1, i1, i2;
    while (1) {
        i1 = i << 1;
        i2 = i1 + 1;
        if (i1 > k)
            break;
        if ((i2 == k + 1) ||
            C::cmp2(bh_val[i1], bh_val[i2], bh_ids[i1], bh_ids[i2])) {
            if (C::cmp2(val, bh_val[i1], id, bh_ids[i1])) {
                break;
            }
            bh_val[i] = bh_val[i1];
            bh_ids[i] = bh_ids[i1];
            i = i1;
        } else {
            if (C::cmp2(val, bh_val[i2], id, bh_ids[i2])) {
                break;
            }
            bh_val[i] = bh_val[i2];
            bh_ids[i] = bh_ids[i2];
            i = i2;
        }
    }
    bh_val[i] = bh_val[k];
    bh_ids[i] = bh_ids[k];
}

/** Pushes the element (val, ids) into the heap bh_val[0..k-2] and
* bh_ids[0..k-2].  on output the element at k-1 is defined.
*/
template <class C>
inline void heap_push(
        size_t k,
        typename C::T* bh_val,
        typename C::TI* bh_ids,
        typename C::T val,
        typename C::TI id) {
    bh_val--; /* Use 1-based indexing for easier node->child translation */
    bh_ids--;
    size_t i = k, i_father;
    while (i > 1) {
        i_father = i >> 1;
        if (!C::cmp2(val, bh_val[i_father], id, bh_ids[i_father])) {
            /* the heap structure is ok */
            break;
        }
        bh_val[i] = bh_val[i_father];
        bh_ids[i] = bh_ids[i_father];
        i = i_father;
    }
    bh_val[i] = val;
    bh_ids[i] = id;
}

/**
* Replaces the top element from the heap defined by bh_val[0..k-1] and
* bh_ids[0..k-1], and for identical bh_val[] values also sorts by bh_ids[]
* values.
*/
template <class C>
inline void heap_replace_top(
        size_t k,
        typename C::T* bh_val,
        typename C::TI* bh_ids,
        typename C::T val,
        typename C::TI id) {
    bh_val--; /* Use 1-based indexing for easier node->child translation */
    bh_ids--;
    size_t i = 1, i1, i2;
    while (1) {
        i1 = i << 1;
        i2 = i1 + 1;
        if (i1 > k) {
            break;
        }

        // Note that C::cmp2() is a bool function answering
        // `(a1 > b1) || ((a1 == b1) && (a2 > b2))` for max
        // heap and same with the `<` sign for min heap.
        if ((i2 == k + 1) ||
            C::cmp2(bh_val[i1], bh_val[i2], bh_ids[i1], bh_ids[i2])) {
            if (C::cmp2(val, bh_val[i1], id, bh_ids[i1])) {
                break;
            }
            bh_val[i] = bh_val[i1];
            bh_ids[i] = bh_ids[i1];
            i = i1;
        } else {
            if (C::cmp2(val, bh_val[i2], id, bh_ids[i2])) {
                break;
            }
            bh_val[i] = bh_val[i2];
            bh_ids[i] = bh_ids[i2];
            i = i2;
        }
    }
    bh_val[i] = val;
    bh_ids[i] = id;
}

/* Partial instanciation for heaps with TI = int64_t */

template <typename T>
inline void minheap_pop(size_t k, T* bh_val, int64_t* bh_ids) {
    heap_pop<faiss::CMin<T, int64_t>>(k, bh_val, bh_ids);
}

template <typename T>
inline void minheap_push(
        size_t k,
        T* bh_val,
        int64_t* bh_ids,
        T val,
        int64_t ids) {
    heap_push<faiss::CMin<T, int64_t>>(k, bh_val, bh_ids, val, ids);
}

template <typename T>
inline void minheap_replace_top(
        size_t k,
        T* bh_val,
        int64_t* bh_ids,
        T val,
        int64_t ids) {
    heap_replace_top<faiss::CMin<T, int64_t>>(k, bh_val, bh_ids, val, ids);
}

template <typename T>
inline void maxheap_pop(size_t k, T* bh_val, int64_t* bh_ids) {
    heap_pop<faiss::CMax<T, int64_t>>(k, bh_val, bh_ids);
}

template <typename T>
inline void maxheap_push(
        size_t k,
        T* bh_val,
        int64_t* bh_ids,
        T val,
        int64_t ids) {
    heap_push<faiss::CMax<T, int64_t>>(k, bh_val, bh_ids, val, ids);
}

template <typename T>
inline void maxheap_replace_top(
        size_t k,
        T* bh_val,
        int64_t* bh_ids,
        T val,
        int64_t ids) {
    heap_replace_top<faiss::CMax<T, int64_t>>(k, bh_val, bh_ids, val, ids);
}

/*******************************************************************
 * Heap initialization
 *******************************************************************/

/* Initialization phase for the heap (with unconditionnal pushes).
 * Store k0 elements in a heap containing up to k values. Note that
 * (bh_val, bh_ids) can be the same as (x, ids) */
template <class C>
inline void heap_heapify(
        size_t k,
        typename C::T* bh_val,
        typename C::TI* bh_ids,
        const typename C::T* x = nullptr,
        const typename C::TI* ids = nullptr,
        size_t k0 = 0) {
    if (k0 > 0 && ids != nullptr)
        assert(x);

    for (size_t i = 0; i < k0; i++) {
        heap_push<C>(i + 1, bh_val, bh_ids, x[i], ids[i]);
    }

    for (size_t i = k0; i < k; i++) {
        bh_val[i] = C::neutral();
        bh_ids[i] = -1;
    }
}


/*******************************************************************
 * Add n elements to the heap
 *******************************************************************/

/* Add some elements to the heap  */
template <class C>
inline void heap_addn(
        size_t k,
        typename C::T* bh_val,
        typename C::TI* bh_ids,
        const typename C::T* x,
        const typename C::TI* ids,
        size_t n) {
    size_t i;
    if (ids) {
        for (i = 0; i < n; i++) {
            if (C::cmp(bh_val[0], x[i])) {
                heap_replace_top<C>(k, bh_val, bh_ids, x[i], ids[i]);
            }
        }
    }
}

/* Partial instanciation for heaps with TI = int64_t */

template <typename T>
inline void minheap_addn(
        size_t k,
        T* bh_val,
        int64_t* bh_ids,
        const T* x,
        const int64_t* ids,
        size_t n) {
    heap_addn<faiss::CMin<T, int64_t>>(k, bh_val, bh_ids, x, ids, n);
}

template <typename T>
inline void maxheap_addn(
        size_t k,
        T* bh_val,
        int64_t* bh_ids,
        const T* x,
        const int64_t* ids,
        size_t n) {
    heap_addn<faiss::CMax<T, int64_t>>(k, bh_val, bh_ids, x, ids, n);
}

/*******************************************************************
 * Heap finalization (reorder elements)
 *******************************************************************/

/* This function maps a binary heap into a sorted structure.
   It returns the number  */
template <typename C>
inline size_t heap_reorder(
        size_t k,
        typename C::T* bh_val,
        typename C::TI* bh_ids) {
    size_t i, ii;

    for (i = 0, ii = 0; i < k; i++) {
        /* top element should be put at the end of the list */
        typename C::T val = bh_val[0];
        typename C::TI id = bh_ids[0];

        /* boundary case: we will over-ride this value if not a true element */
        heap_pop<C>(k - i, bh_val, bh_ids);
        bh_val[k - ii - 1] = val;
        bh_ids[k - ii - 1] = id;
        if (id != -1)
            ii++;
    }
    /* Count the number of elements which are effectively returned */
    size_t nel = ii;

    memmove(bh_val, bh_val + k - ii, ii * sizeof(*bh_val));
    memmove(bh_ids, bh_ids + k - ii, ii * sizeof(*bh_ids));

    for (; ii < k; ii++) {
        bh_val[ii] = C::neutral();
        bh_ids[ii] = -1;
    }
    return nel;
}

template <typename T>
inline size_t minheap_reorder(size_t k, T* bh_val, int64_t* bh_ids) {
    return heap_reorder<faiss::CMin<T, int64_t>>(k, bh_val, bh_ids);
}

template <typename T>
inline size_t maxheap_reorder(size_t k, T* bh_val, int64_t* bh_ids) {
    return heap_reorder<faiss::CMax<T, int64_t>>(k, bh_val, bh_ids);
}

}
