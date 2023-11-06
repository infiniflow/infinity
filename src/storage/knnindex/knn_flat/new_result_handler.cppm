//
// Created by jinhai on 23-9-25.
//

module;

import stl;
import parser;
import knn_heap;
import knn_partition;
import infinity_assert;
import infinity_exception;

export module knn_result_handler;

namespace infinity {

enum class ResultHandlerType {
    kHeap,
    kReservoir,
    kSingleBest,
    kInvalid,
};

struct ResultHandler {
    explicit ResultHandler(ResultHandlerType type) : type_(type) {}

    ResultHandlerType type_{ResultHandlerType::kInvalid};
};

struct SingleResultHandler {
    explicit SingleResultHandler(ResultHandlerType type) : type_(type) {}

    ResultHandlerType type_{ResultHandlerType::kInvalid};
};

/*****************************************************************
 * Heap based result handler
 *****************************************************************/

export template <class C>
struct NewHeapResultHandler : public ResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    T *heap_dis_tab;
    TI *heap_ids_tab;

    i64 k; // number of results to keep

    NewHeapResultHandler(SizeT nq, T *heap_dis_tab, TI *heap_ids_tab, SizeT k)
        : ResultHandler(ResultHandlerType::kHeap), nq(nq), heap_dis_tab(heap_dis_tab), heap_ids_tab(heap_ids_tab), k(k) {}

    /******************************************************
     * API for 1 result at a time (each SingleResultHandler is
     * called from 1 thread)
     */

    struct HeapSingleResultHandler : public SingleResultHandler {
        NewHeapResultHandler &hr;
        SizeT k{};

        Vector<T *> heap_dis{};
        Vector<TI *> heap_ids{};
        Vector<T> thresh{};

        explicit HeapSingleResultHandler(NewHeapResultHandler &hr, SizeT query_count)
            : SingleResultHandler(ResultHandlerType::kHeap), hr(hr), k(hr.k), heap_dis(query_count), heap_ids(query_count), thresh(query_count) {}

        /// begin results for query # i
        void begin(SizeT i) {
            heap_dis[i] = hr.heap_dis_tab + i * k;
            heap_ids[i] = hr.heap_ids_tab + i * k;
            heap_heapify<C>(k, heap_dis[i], heap_ids[i]);
            // thresh[i] = *heap_dis[0];
            thresh[i] = *heap_dis[i];
        }

        /// add one result for query i
        void add_result(T dis, TI idx, SizeT i) {
            // if (C::cmp(*heap_dis[0], dis)) {
            if (C::cmp(*heap_dis[i], dis)) {
                heap_replace_top<C>(k, heap_dis[i], heap_ids[i], dis, idx);
                // thresh[i] = *heap_dis[0];
                thresh[i] = *heap_dis[i];
            }
        }

        /// series of results for query i is done
        void end(SizeT i) { heap_reorder<C>(k, heap_dis[i], heap_ids[i]); }
    };

    /******************************************************
     * API for multiple results (called from 1 thread)
     */

    /// begin
    void begin_multiple(SizeT i_start, SizeT i_end) {
        for (SizeT i = i_start; i < i_end; i++) {
            heap_heapify<C>(k, heap_dis_tab + i * k, heap_ids_tab + i * k);
        }
    }

    /// add results for query i0..i1 and j0..j1
    void add_results(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const T *dis_tab, u32 segment_id, u32 segment_offset_start) {
        for (i64 i = i_start; i < i_end; i++) {
            T *heap_dis = heap_dis_tab + i * k;
            TI *heap_ids = heap_ids_tab + i * k;
            const T *dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;
            T thresh = heap_dis[0];
            for (u32 j = j0; j < j1; j++) {
                T dis = dis_tab_i[j];
                if (C::cmp(thresh, dis)) {
                    heap_replace_top<C>(k, heap_dis, heap_ids, dis, RowID(segment_id, segment_offset_start + j));
                    thresh = heap_dis[0];
                }
            }
        }
    }

    /// series of results for queries i0..i1 is done
    void end_multiple(SizeT i_start, SizeT i_end) {
        // maybe parallel for
        for (SizeT i = i_start; i < i_end; i++) {
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
template <class C>
struct ReservoirTopN {
    using T = typename C::T;
    using TI = typename C::TI;

    T *vals;
    TI *ids;

    SizeT i;        // number of stored elements
    SizeT n;        // number of requested elements
    SizeT capacity; // size of storage

    T threshold; // current threshold

    ReservoirTopN() {}

    ReservoirTopN(SizeT n, SizeT capacity, T *vals, TI *ids) : vals(vals), ids(ids), i(0), n(n), capacity(capacity) {
        Assert<ExecutorException>(n < capacity, "n >= capacity", __FILE_NAME__, __LINE__);
        threshold = C::neutral();
    }

    void add(T val, TI id) {
        if (C::cmp(threshold, val)) {
            if (i == capacity) {
                shrink_fuzzy();
            }
            vals[i] = val;
            ids[i] = id;
            i++;
        }
    }

    // reduce storage from capacity to anything
    // between n and (capacity + n) / 2
    void shrink_fuzzy() {
        Assert<ExecutorException>(i == capacity, "n != capacity", __FILE_NAME__, __LINE__);

        threshold = partition_fuzzy<C>(vals, ids, capacity, n, (capacity + n) / 2, &i);
    }

    void to_result(T *heap_dis, TI *heap_ids) const {
        for (int j = 0; j < Min(i, n); j++) {
            heap_push<C>(j + 1, heap_dis, heap_ids, vals[j], ids[j]);
        }

        if (i < n) {
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

export template <class C>
struct NewReservoirResultHandler : public ResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    T *heap_dis_tab;
    TI *heap_ids_tab;

    i64 k;       // number of results to keep
    SizeT capacity; // capacity of the reservoirs

    explicit NewReservoirResultHandler(SizeT nq, T *heap_dis_tab, TI *heap_ids_tab, SizeT k)
        : ResultHandler(ResultHandlerType::kReservoir), nq(nq), heap_dis_tab(heap_dis_tab), heap_ids_tab(heap_ids_tab), k(k) {
        // double then round up to multiple of 16 (for SIMD alignment)
        capacity = (2 * k + 15) & ~15;
    }

    /******************************************************
     * API for 1 result at a time (each SingleResultHandler is
     * called from 1 thread)
     */

    struct ReservoirSingleResultHandler : public SingleResultHandler {
        NewReservoirResultHandler &hr;

        Vector<Vector<T>> reservoir_dis;
        Vector<Vector<TI>> reservoir_ids;
        Vector<ReservoirTopN<C>> results;

        explicit ReservoirSingleResultHandler(NewReservoirResultHandler &hr)
            : SingleResultHandler(ResultHandlerType::kReservoir), hr(hr), reservoir_dis(hr.nq, Vector<T>(hr.capacity)),
              reservoir_ids(hr.nq, Vector<TI>(hr.capacity)), nq_(hr.nq) {
            results.reserve(nq_);
        }

        SizeT nq_{};

        /// begin results for query # i
        void begin(SizeT i) { results.emplace_back(ReservoirTopN<C>(hr.k, hr.capacity, reservoir_dis[i].data(), reservoir_ids[i].data())); }

        /// add one result for query i
        void add_result(T dis, TI idx, SizeT i) { results[i].add(dis, idx); }

        /// series of results for query i is done
        void end(SizeT i) {
            T *heap_dis = hr.heap_dis_tab + i * hr.k;
            TI *heap_ids = hr.heap_ids_tab + i * hr.k;
            results[i].to_result(heap_dis, heap_ids);
        }
    };

    /******************************************************
     * API for multiple results (called from 1 thread)
     */

    Vector<T> reservoir_dis;
    Vector<TI> reservoir_ids;
    Vector<ReservoirTopN<C>> reservoirs;

    /// begin
    void begin_multiple(SizeT i_start, SizeT i_end) {
        reservoir_dis.resize((i_end - i_start) * capacity);
        reservoir_ids.resize((i_end - i_start) * capacity);
        reservoirs.clear();
        for (SizeT i = i_start; i < i_end; i++) {
            reservoirs.emplace_back(k, capacity, reservoir_dis.data() + (i - i_start) * capacity, reservoir_ids.data() + (i - i_start) * capacity);
        }
    }

    /// add results for query i_start..i_end and j0..j1
    void add_results(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const T *dis_tab, u32 segment_id, u32 segment_offset_start) {
        // maybe parallel for
        // #pragma omp parallel for
        for (i64 i = i_start; i < i_end; i++) {
            ReservoirTopN<C> &reservoir = reservoirs[i - i_start];
            const T *dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;
            for (u32 j = j0; j < j1; j++) {
                T dis = dis_tab_i[j];
                reservoir.add(dis, RowID(segment_id, segment_offset_start + j));
            }
        }
    }

    /// series of results for queries i_start..i_end is done
    void end_multiple(SizeT i_start, SizeT i_end) {
        // maybe parallel for
        for (SizeT i = i_start; i < i_end; i++) {
            reservoirs[i - i_start].to_result(heap_dis_tab + i * k, heap_ids_tab + i * k);
        }
    }
};

/*****************************************************************
 * Single best result handler.
 * Tracks the only best result, thus avoiding storing
 * some temporary data in memory.
 *****************************************************************/

export template <class C>
struct SingleBestResultHandler : public ResultHandler {
    using T = typename C::T;
    using TI = typename C::TI;

    int nq;
    // contains exactly nq elements
    T *dis_tab;
    // contains exactly nq elements
    TI *ids_tab;

    SingleBestResultHandler(SizeT nq, T *dis_tab, TI *ids_tab)
        : ResultHandler(ResultHandlerType::kSingleBest), nq(nq), dis_tab(dis_tab), ids_tab(ids_tab) {}

    struct SingleResultHandler {
        SingleBestResultHandler &hr;

        Vector<T> min_dis;
        Vector<TI> min_idx;

        SingleResultHandler(SingleBestResultHandler &hr) : hr(hr), min_dis(hr.nq), min_idx(hr.nq) {}

        /// begin results for query # i
        void begin(const SizeT current_idx) {
            min_dis[current_idx] = HugeValf();
            min_idx[current_idx] = 0;
        }

        /// add one result for query i
        void add_result(T dis, TI idx, SizeT query_idx) {
            if (C::cmp(min_dis[query_idx], dis)) {
                min_dis[query_idx] = dis;
                min_idx[query_idx] = idx;
            }
        }

        /// series of results for query i is done
        void end(SizeT query_idx) {
            hr.dis_tab[query_idx] = min_dis[query_idx];
            hr.ids_tab[query_idx] = min_idx[query_idx];
        }
    };

    /// begin
    void begin_multiple(SizeT i_start, SizeT i_end) {
        for (SizeT i = i_start; i < i_end; i++) {
            this->dis_tab[i] = HugeValf();
        }
    }

    /// add results for query i0..i1 and j0..j1
    void add_results(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const T *dis_tab, u32 segment_id, u32 segment_offset_start) {
        for (i64 i = i_start; i < i_end; i++) {
            const T *dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;

            auto &min_distance = this->dis_tab[i];
            auto &min_index = this->ids_tab[i];

            for (u32 j = j0; j < j1; j++) {
                const T distance = dis_tab_i[j];

                if (C::cmp(min_distance, distance)) {
                    min_distance = distance;
                    min_index = RowID(segment_id, segment_offset_start + j);
                }
            }
        }
    }

    void add_result(const SizeT i, const T dis, const TI idx) {
        auto &min_distance = this->dis_tab[i];
        auto &min_index = this->ids_tab[i];

        if (C::cmp(min_distance, dis)) {
            min_distance = dis;
            min_index = idx;
        }
    }

    /// series of results for queries i0..i1 is done
    void end_multiple() {}
};

} // namespace infinity