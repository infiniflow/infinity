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

module;

#include <cassert>
#include <cstring>

import faiss;
import stl;

export module knn_heap;

namespace infinity {

/*******************************************************************
 * Basic heap ops: push and pop
 *******************************************************************/

/** Pops the top element from the heap defined by bh_val[0..k-1] and
 * bh_ids[0..k-1].  on output the element at k-1 is undefined.
 */
template <class C>
inline void heap_pop(SizeT k, typename C::T *bh_val, typename C::TI *bh_ids) {
    bh_val--; /* Use 1-based indexing for easier node->child translation */
    bh_ids--;
    typename C::T val = bh_val[k];
    typename C::TI id = bh_ids[k];
    SizeT i = 1, i1, i2;
    while (1) {
        i1 = i << 1;
        i2 = i1 + 1;
        if (i1 > k)
            break;
        if ((i2 == k + 1) || C::cmp2(bh_val[i1], bh_val[i2], bh_ids[i1], bh_ids[i2])) {
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
export template <class C>
inline void heap_push(SizeT k, typename C::T *bh_val, typename C::TI *bh_ids, typename C::T val, typename C::TI id) {
    bh_val--; /* Use 1-based indexing for easier node->child translation */
    bh_ids--;
    SizeT i = k, i_father;
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
export template <class C>
inline void heap_replace_top(SizeT k, typename C::T *bh_val, typename C::TI *bh_ids, typename C::T val, typename C::TI id) {
    bh_val--; /* Use 1-based indexing for easier node->child translation */
    bh_ids--;
    SizeT i = 1, i1, i2;
    while (1) {
        i1 = i << 1;
        i2 = i1 + 1;
        if (i1 > k) {
            break;
        }

        // Note that C::cmp2() is a bool function answering
        // `(a1 > b1) || ((a1 == b1) && (a2 > b2))` for max
        // heap and same with the `<` sign for min heap.
        if ((i2 == k + 1) || C::cmp2(bh_val[i1], bh_val[i2], bh_ids[i1], bh_ids[i2])) {
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

/* Partial instanciation for heaps with TI = i64 */

template <typename T>
inline void minheap_pop(SizeT k, T *bh_val, i64 *bh_ids) {
    heap_pop<FaissCMin<T, i64>>(k, bh_val, bh_ids);
}

template <typename T>
inline void minheap_push(SizeT k, T *bh_val, i64 *bh_ids, T val, i64 ids) {
    heap_push<FaissCMin<T, i64>>(k, bh_val, bh_ids, val, ids);
}

template <typename T>
inline void minheap_replace_top(SizeT k, T *bh_val, i64 *bh_ids, T val, i64 ids) {
    heap_replace_top<FaissCMin<T, i64>>(k, bh_val, bh_ids, val, ids);
}

template <typename T>
inline void maxheap_pop(SizeT k, T *bh_val, i64 *bh_ids) {
    heap_pop<FaissCMax<T, i64>>(k, bh_val, bh_ids);
}

template <typename T>
inline void maxheap_push(SizeT k, T *bh_val, i64 *bh_ids, T val, i64 ids) {
    heap_push<FaissCMax<T, i64>>(k, bh_val, bh_ids, val, ids);
}

template <typename T>
inline void maxheap_replace_top(SizeT k, T *bh_val, i64 *bh_ids, T val, i64 ids) {
    heap_replace_top<FaissCMax<T, i64>>(k, bh_val, bh_ids, val, ids);
}

/*******************************************************************
 * Heap initialization
 *******************************************************************/

/* Initialization phase for the heap (with unconditionnal pushes).
 * Store k0 elements in a heap containing up to k values. Note that
 * (bh_val, bh_ids) can be the same as (x, ids) */
export template <class C>
inline void heap_heapify(SizeT k,
                         typename C::T *bh_val,
                         typename C::TI *bh_ids,
                         const typename C::T *x = nullptr,
                         const typename C::TI *ids = nullptr,
                         SizeT k0 = 0) {
    if (k0 > 0 && ids != nullptr)
        assert(x);

    for (SizeT i = 0; i < k0; i++) {
        heap_push<C>(i + 1, bh_val, bh_ids, x[i], ids[i]);
    }

    for (SizeT i = k0; i < k; i++) {
        bh_val[i] = C::neutral();
        // bh_ids[i] = -1;
    }
}

/*******************************************************************
 * Add n elements to the heap
 *******************************************************************/

/* Add some elements to the heap  */
export template <class C>
inline void heap_addn(SizeT k, typename C::T *bh_val, typename C::TI *bh_ids, const typename C::T *x, const typename C::TI *ids, SizeT n) {
    SizeT i;
    if (ids) {
        for (i = 0; i < n; i++) {
            if (C::cmp(bh_val[0], x[i])) {
                heap_replace_top<C>(k, bh_val, bh_ids, x[i], ids[i]);
            }
        }
    }
}

/* Partial instanciation for heaps with TI = i64 */

template <typename T>
inline void minheap_addn(SizeT k, T *bh_val, i64 *bh_ids, const T *x, const i64 *ids, SizeT n) {
    heap_addn<FaissCMin<T, i64>>(k, bh_val, bh_ids, x, ids, n);
}

template <typename T>
inline void maxheap_addn(SizeT k, T *bh_val, i64 *bh_ids, const T *x, const i64 *ids, SizeT n) {
    heap_addn<FaissCMax<T, i64>>(k, bh_val, bh_ids, x, ids, n);
}

/*******************************************************************
 * Heap finalization (reorder elements)
 *******************************************************************/

/* This function maps a binary heap into a sorted structure.
   It returns the number  */
export template <typename C>
inline SizeT heap_reorder(SizeT k, typename C::T *bh_val, typename C::TI *bh_ids) {
    SizeT i, ii;

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
    SizeT nel = ii;

    memmove(bh_val, bh_val + k - ii, ii * sizeof(*bh_val));
    memmove(bh_ids, bh_ids + k - ii, ii * sizeof(*bh_ids));

    for (; ii < k; ii++) {
        bh_val[ii] = C::neutral();
        bh_ids[ii] = -1;
    }
    return nel;
}

template <typename T>
inline SizeT minheap_reorder(SizeT k, T *bh_val, i64 *bh_ids) {
    return heap_reorder<FaissCMin<T, i64>>(k, bh_val, bh_ids);
}

template <typename T>
inline SizeT maxheap_reorder(SizeT k, T *bh_val, i64 *bh_ids) {
    return heap_reorder<FaissCMax<T, i64>>(k, bh_val, bh_ids);
}

} // namespace infinity
