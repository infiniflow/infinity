//
// Created by jinhai on 23-9-25.
//

#pragma once

#include <stdint.h>
#include "new_partitioning.h"
#include "new_heap.h"

namespace infinity {

enum class ResultHandlerType {
    kHeap,
    kReservoir,
    kSingleBest,
    kInvalid,
};

struct ResultHandler {
    explicit
    ResultHandler(ResultHandlerType type) : type_(type) {}

    ResultHandlerType type_{ResultHandlerType::kInvalid};
};

struct SingleResultHandler {
    explicit
    SingleResultHandler(ResultHandlerType type) : type_(type) {}

    ResultHandlerType type_{ResultHandlerType::kInvalid};
};

/*****************************************************************
 * Heap based result handler
 *****************************************************************/

template<class C>
struct NewHeapResultHandler : public ResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    T* heap_dis_tab;
    TI* heap_ids_tab;

    int64_t k; // number of results to keep

    NewHeapResultHandler(size_t nq, T* heap_dis_tab, TI* heap_ids_tab, size_t k)
            : ResultHandler(ResultHandlerType::kHeap),
              nq(nq),
              heap_dis_tab(heap_dis_tab),
              heap_ids_tab(heap_ids_tab),
              k(k) {}

    /******************************************************
     * API for 1 result at a time (each SingleResultHandler is
     * called from 1 thread)
     */

    struct HeapSingleResultHandler : public SingleResultHandler {
        NewHeapResultHandler& hr;
        SizeT k{};

        Vector<T*> heap_dis{};
        Vector<TI*> heap_ids{};
        Vector<T> thresh{};

        explicit
        HeapSingleResultHandler(NewHeapResultHandler& hr, SizeT query_count)
                : SingleResultHandler(ResultHandlerType::kHeap),
                  hr(hr),
                  k(hr.k),
                  heap_dis(query_count),
                  heap_ids(query_count),
                  thresh(query_count) {}

        /// begin results for query # i
        void
        begin(SizeT i) {
            heap_dis[i] = hr.heap_dis_tab + i * k;
            heap_ids[i] = hr.heap_ids_tab + i * k;
            heap_heapify<C>(k, heap_dis[i], heap_ids[i]);
            thresh[i] = *heap_dis[0];
        }

        /// add one result for query i
        void
        add_result(T dis, TI idx, SizeT i) {
            if(C::cmp(*heap_dis[0], dis)) {
                heap_replace_top<C>(k, heap_dis[i], heap_ids[i], dis, idx);
                thresh[i] = *heap_dis[0];
            }
        }

        /// series of results for query i is done
        void
        end(SizeT i) {
            heap_reorder<C>(k, heap_dis[i], heap_ids[i]);
        }
    };

    /******************************************************
     * API for multiple results (called from 1 thread)
     */

    /// begin
    void
    begin_multiple(size_t i_start, size_t i_end) {
        for(size_t i = i_start; i < i_end; i++) {
            heap_heapify<C>(k, heap_dis_tab + i * k, heap_ids_tab + i * k);
        }
    }

    /// add results for query i0..i1 and j0..j1
    void
    add_results(size_t i_start, size_t i_end, size_t j0, size_t j1, const T* dis_tab, i32 segment_id) {
        for(int64_t i = i_start; i < i_end; i++) {
            T* heap_dis = heap_dis_tab + i * k;
            TI* heap_ids = heap_ids_tab + i * k;
            const T* dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;
            T thresh = heap_dis[0];
            for(size_t j = j0; j < j1; j++) {
                T dis = dis_tab_i[j];
                if(C::cmp(thresh, dis)) {
                    heap_replace_top<C>(k, heap_dis, heap_ids, dis, CompoundID(segment_id, (i32)j));
                    thresh = heap_dis[0];
                }
            }
        }
    }

    /// series of results for queries i0..i1 is done
    void
    end_multiple(size_t i_start, size_t i_end) {
        // maybe parallel for
        for(size_t i = i_start; i < i_end; i++) {
            heap_reorder<C>(k, heap_dis_tab + i * k, heap_ids_tab + i * k);
        }
    }
};

/*****************************************************************
 * Reservoir result handler
 *
 * A reservoir is a result array of size capacity > n (number of requested
 * results) all results below a threshold are stored in an arbitrary order. When
 * the capacity is reached, a new threshold is chosen by partitionning the
 * distance array.
 *****************************************************************/

/// Reservoir for a single query
template<class C>
struct ReservoirTopN {
    using T = typename C::T;
    using TI = typename C::TI;

    T* vals;
    TI* ids;

    size_t i;        // number of stored elements
    size_t n;        // number of requested elements
    size_t capacity; // size of storage

    T threshold; // current threshold

    ReservoirTopN() {}

    ReservoirTopN(size_t n, size_t capacity, T* vals, TI* ids)
            : vals(vals), ids(ids), i(0), n(n), capacity(capacity) {
        assert(n < capacity);
        threshold = C::neutral();
    }

    void
    add(T val, TI id) {
        if(C::cmp(threshold, val)) {
            if(i == capacity) {
                shrink_fuzzy();
            }
            vals[i] = val;
            ids[i] = id;
            i++;
        }
    }

    // reduce storage from capacity to anything
    // between n and (capacity + n) / 2
    void
    shrink_fuzzy() {
        assert(i == capacity);

        threshold = partition_fuzzy<C>(
                vals, ids, capacity, n, (capacity + n) / 2, &i);
    }

    void
    to_result(T* heap_dis, TI* heap_ids) const {
        for(int j = 0; j < std::min(i, n); j++) {
            heap_push<C>(j + 1, heap_dis, heap_ids, vals[j], ids[j]);
        }

        if(i < n) {
            heap_reorder<C>(i, heap_dis, heap_ids);
            // add empty results
            heap_heapify<C>(n - i, heap_dis + i, heap_ids + i);
        } else {
            // add remaining elements
            heap_addn<C>(n, heap_dis, heap_ids, vals + n, ids + n, i - n);
            heap_reorder<C>(n, heap_dis, heap_ids);
        }
    }
};

template<class C>
struct NewReservoirResultHandler : public ResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    T* heap_dis_tab;
    TI* heap_ids_tab;

    int64_t k;       // number of results to keep
    size_t capacity; // capacity of the reservoirs

    explicit
    NewReservoirResultHandler(
            size_t nq,
            T* heap_dis_tab,
            TI* heap_ids_tab,
            size_t k)
            : ResultHandler(ResultHandlerType::kReservoir),
              nq(nq),
              heap_dis_tab(heap_dis_tab),
              heap_ids_tab(heap_ids_tab),
              k(k) {
        // double then round up to multiple of 16 (for SIMD alignment)
        capacity = (2 * k + 15) & ~15;
    }

    /******************************************************
     * API for 1 result at a time (each SingleResultHandler is
     * called from 1 thread)
     */

    struct ReservoirSingleResultHandler : public SingleResultHandler {
        NewReservoirResultHandler& hr;

        Vector<Vector<T>> reservoir_dis;
        Vector<Vector<TI>> reservoir_ids;
        Vector<ReservoirTopN<C>> results;

        explicit
        ReservoirSingleResultHandler(NewReservoirResultHandler& hr)
                : SingleResultHandler(ResultHandlerType::kReservoir),
                  hr(hr),
                  reservoir_dis(hr.nq, Vector<T>(hr.capacity)),
                  reservoir_ids(hr.nq, Vector<TI>(hr.capacity)),
                  nq_(hr.nq) {
            results.reserve(nq_);
        }

        size_t nq_{};

        /// begin results for query # i
        void
        begin(SizeT i) {
            results.emplace_back(ReservoirTopN<C>(hr.k,
                                                  hr.capacity,
                                                  reservoir_dis[i].data(),
                                                  reservoir_ids[i].data()));
        }

        /// add one result for query i
        void
        add_result(T dis, TI idx, SizeT i) {
            results[i].add(dis, idx);
        }

        /// series of results for query i is done
        void
        end(SizeT i) {
            T* heap_dis = hr.heap_dis_tab + i * hr.k;
            TI* heap_ids = hr.heap_ids_tab + i * hr.k;
            results[i].to_result(heap_dis, heap_ids);
        }
    };

    /******************************************************
     * API for multiple results (called from 1 thread)
     */

    std::vector<T> reservoir_dis;
    std::vector<TI> reservoir_ids;
    std::vector<ReservoirTopN<C>> reservoirs;

    /// begin
    void
    begin_multiple(size_t i_start, size_t i_end) {
        reservoir_dis.resize((i_end - i_start) * capacity);
        reservoir_ids.resize((i_end - i_start) * capacity);
        reservoirs.clear();
        for(size_t i = i_start; i < i_end; i++) {
            reservoirs.emplace_back(
                    k,
                    capacity,
                    reservoir_dis.data() + (i - i_start) * capacity,
                    reservoir_ids.data() + (i - i_start) * capacity);
        }
    }

    /// add results for query i_start..i_end and j0..j1
    void
    add_results(size_t i_start, size_t i_end, size_t j0, size_t j1, const T* dis_tab, i32 segment_id) {
        // maybe parallel for
//#pragma omp parallel for
        for(int64_t i = i_start; i < i_end; i++) {
            ReservoirTopN<C>& reservoir = reservoirs[i - i_start];
            const T* dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;
            for(size_t j = j0; j < j1; j++) {
                T dis = dis_tab_i[j];
                reservoir.add(dis, CompoundID(segment_id, (i32)j));
            }
        }
    }

    /// series of results for queries i_start..i_end is done
    void
    end_multiple(size_t i_start, size_t i_end) {
        // maybe parallel for
        for(size_t i = i_start; i < i_end; i++) {
            reservoirs[i - i_start].to_result(
                    heap_dis_tab + i * k, heap_ids_tab + i * k);
        }
    }
};


/*****************************************************************
 * Single best result handler.
 * Tracks the only best result, thus avoiding storing
 * some temporary data in memory.
 *****************************************************************/

template<class C>
struct SingleBestResultHandler : public ResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    // contains exactly nq elements
    T* dis_tab;
    // contains exactly nq elements
    TI* ids_tab;

    SingleBestResultHandler(size_t nq, T* dis_tab, TI* ids_tab)
            : ResultHandler(ResultHandlerType::kSingleBest), nq(nq), dis_tab(dis_tab), ids_tab(ids_tab) {}

    struct SingleResultHandler {
        SingleBestResultHandler& hr;

        Vector<T> min_dis;
        Vector<TI> min_idx;

        SingleResultHandler(SingleBestResultHandler& hr) : hr(hr), min_dis(hr.nq), min_idx(hr.nq) {}

        /// begin results for query # i
        void
        begin(const size_t current_idx) {
            min_dis[current_idx] = HUGE_VALF;
            min_idx[current_idx] = 0;
        }

        /// add one result for query i
        void
        add_result(T dis, TI idx, SizeT query_idx) {
            if(C::cmp(min_dis[query_idx], dis)) {
                min_dis[query_idx] = dis;
                min_idx[query_idx] = idx;
            }
        }

        /// series of results for query i is done
        void
        end(SizeT query_idx) {
            hr.dis_tab[query_idx] = min_dis[query_idx];
            hr.ids_tab[query_idx] = min_idx[query_idx];
        }
    };

    /// begin
    void
    begin_multiple(size_t i_start, size_t i_end) {
        for(size_t i = i_start; i < i_end; i++) {
            this->dis_tab[i] = HUGE_VALF;
        }
    }

    /// add results for query i0..i1 and j0..j1
    void
    add_results(size_t i_start, size_t i_end, size_t j0, size_t j1, const T* dis_tab, i32 segment_id) {
        for(int64_t i = i_start; i < i_end; i++) {
            const T* dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;

            auto& min_distance = this->dis_tab[i];
            auto& min_index = this->ids_tab[i];

            for(size_t j = j0; j < j1; j++) {
                const T distance = dis_tab_i[j];

                if(C::cmp(min_distance, distance)) {
                    min_distance = distance;
                    min_index = CompoundID(segment_id, j);
                }
            }
        }
    }

    void
    add_result(const size_t i, const T dis, const TI idx) {
        auto& min_distance = this->dis_tab[i];
        auto& min_index = this->ids_tab[i];

        if(C::cmp(min_distance, dis)) {
            min_distance = dis;
            min_index = idx;
        }
    }

    /// series of results for queries i0..i1 is done
    void
    end_multiple() {}
};

}