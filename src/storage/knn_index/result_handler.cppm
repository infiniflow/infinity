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

#include <cstring>

export module knn_result_handler;

import stl;

import infinity_exception;
import bitmask;
import internal_types;

namespace infinity {

export template <typename DistType, typename ID>
struct CompareMax;

export template <typename DistType, typename ID>
struct CompareMin {
    using DistanceType = DistType;
    using IDType = ID;
    using CompareReverse = CompareMax<DistanceType, IDType>;
    static bool Compare(DistanceType a, DistanceType b) { return a < b; }
    static bool Compare(DistanceType a, DistanceType b, IDType i, IDType j) { return (a < b) || ((a == b) && (i > j)); }
    static constexpr DistanceType InitialValue() { return std::numeric_limits<DistanceType>::lowest(); }
    static constexpr bool IsMax = false;
};

template <typename DistType, typename ID>
struct CompareMax {
    using DistanceType = DistType;
    using IDType = ID;
    using CompareReverse = CompareMin<DistanceType, IDType>;
    static bool Compare(DistanceType a, DistanceType b) { return a > b; }
    static bool Compare(DistanceType a, DistanceType b, IDType i, IDType j) { return (a > b) || ((a == b) && (i > j)); }
    static constexpr DistanceType InitialValue() { return std::numeric_limits<DistanceType>::max(); }
    static constexpr bool IsMax = true;
};

enum class ResultHandlerType {
    kHeap,
    kReservoir,
    kSingleBest,
    kInvalid,
};

class ResultHandlerBase {
public:
    explicit ResultHandlerBase(ResultHandlerType type) : type_(type) {}
    ~ResultHandlerBase() = default;
    ResultHandlerType type_{ResultHandlerType::kInvalid};
};

template <class Compare>
inline void HeapifyDown(typename Compare::DistanceType *distance, typename Compare::IDType *id, u32 size, u32 index) {
    if (index == 0 || (index << 1) > size) {
        return;
    }
    auto tmp_d = distance[index];
    auto tmp_i = id[index];
    for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
        if (sub + 1 <= size && Compare::Compare(distance[sub + 1], distance[sub], id[sub + 1], id[sub])) {
            ++sub;
        }
        if (!Compare::Compare(distance[sub], tmp_d, id[sub], tmp_i)) {
            break;
        }
        distance[index] = distance[sub];
        id[index] = id[sub];
    }
    distance[index] = tmp_d;
    id[index] = tmp_i;
}

export template <class Compare>
class HeapResultHandler : public ResultHandlerBase {
    using DistType = typename Compare::DistanceType;
    using ID = typename Compare::IDType;
    SizeT n_queries{};
    u32 top_k{};
    DistType *distance_ptr = nullptr;
    ID *id_ptr = nullptr;
    Vector<u32> sizes;

public:
    explicit HeapResultHandler(SizeT n_queries, SizeT top_k, DistType *distance, ID *id)
        : ResultHandlerBase(ResultHandlerType::kHeap), n_queries(n_queries), top_k(top_k), distance_ptr(distance), id_ptr(id), sizes(n_queries) {}

    ~HeapResultHandler() = default;

    void Begin(SizeT i) {}

    void Begin(SizeT i_start, SizeT i_end) {}

    void Begin() {}

    [[nodiscard]] DistType GetDistance0(SizeT q_id) const { return distance_ptr[q_id * top_k]; }

    [[nodiscard]] u32 GetSize(SizeT q_id) const { return sizes[q_id]; }

    void ReInitialize() { std::fill(sizes.begin(), sizes.end(), 0); }

    void AddResult(SizeT q_id, DistType d, ID i) {
        u32 &size = sizes[q_id];
        auto distance = distance_ptr + q_id * top_k - 1;
        auto id = id_ptr + q_id * top_k - 1;
        if (size == top_k) {
            if (Compare::Compare(distance[1], d, id[1], i)) {
                distance[1] = d;
                id[1] = i;
                HeapifyDown<Compare>(distance, id, size, 1);
            }
        } else {
            ++size;
            distance[size] = d;
            id[size] = i;
            if (size == top_k) {
                for (u32 index = size / 2; index > 0; --index) {
                    HeapifyDown<Compare>(distance, id, size, index);
                }
            }
        }
    }

    void AddResults(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const DistType *dis_tab, u32 segment_id, u32 segment_offset_start) {
        for (SizeT q_id = i_start; q_id < i_end; ++q_id) {
            u32 &size = sizes[q_id];
            auto distance = distance_ptr + q_id * top_k - 1;
            auto id = id_ptr + q_id * top_k - 1;
            auto dis_tab_i = dis_tab + (j1 - j0) * (q_id - i_start) - j0;
            for (SizeT j = j0; j < j1; ++j) {
                auto dis = dis_tab_i[j];
                if (size == top_k) {
                    const auto add_row_id = RowID(segment_id, segment_offset_start + j);
                    if (Compare::Compare(distance[1], dis, id[1], add_row_id)) {
                        distance[1] = dis;
                        id[1] = add_row_id;
                        HeapifyDown<Compare>(distance, id, size, 1);
                    }
                } else {
                    ++size;
                    distance[size] = dis;
                    id[size] = RowID(segment_id, segment_offset_start + j);
                    if (size == top_k) {
                        for (u32 index = size / 2; index > 0; --index) {
                            HeapifyDown<Compare>(distance, id, size, index);
                        }
                    }
                }
            }
        }
    }

    // bitmask is for the segment
    void
    AddResults(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const DistType *dis_tab, u32 segment_id, u32 segment_offset_start, Bitmask &bitmask) {
        for (SizeT q_id = i_start; q_id < i_end; ++q_id) {
            u32 &size = sizes[q_id];
            auto distance = distance_ptr + q_id * top_k - 1;
            auto id = id_ptr + q_id * top_k - 1;
            auto dis_tab_i = dis_tab + (j1 - j0) * (q_id - i_start) - j0;
            for (SizeT j = j0; j < j1; ++j) {
                auto segment_offset = segment_offset_start + j;
                if (bitmask.IsTrue(segment_offset)) {
                    auto dis = dis_tab_i[j];
                    if (size == top_k) {
                        const auto add_row_id = RowID(segment_id, segment_offset);
                        if (Compare::Compare(distance[1], dis, id[1], add_row_id)) {
                            distance[1] = dis;
                            id[1] = add_row_id;
                            HeapifyDown<Compare>(distance, id, size, 1);
                        }
                    } else {
                        ++size;
                        distance[size] = dis;
                        id[size] = RowID(segment_id, segment_offset);
                        if (size == top_k) {
                            for (u32 index = size / 2; index > 0; --index) {
                                HeapifyDown<Compare>(distance, id, size, index);
                            }
                        }
                    }
                }
            }
        }
    }

    void End(SizeT q_id) {
        u32 &size = sizes[q_id];
        DistType *distance = distance_ptr + q_id * top_k - 1;
        ID *id = id_ptr + q_id * top_k - 1;
        if (size < top_k) {
            std::fill_n(distance + size + 1, top_k - size, Compare::InitialValue());
            for (u32 index = size / 2; index > 0; --index) {
                HeapifyDown<Compare>(distance, id, size, index);
            }
        }
        while (size > 1) {
            std::swap(distance[size], distance[1]);
            std::swap(id[size], id[1]);
            --size;
            HeapifyDown<Compare>(distance, id, size, 1);
        }
        size = 0;
    }

    void End(SizeT i_start, SizeT i_end) {
        for (u64 q_id = i_start; q_id < i_end; ++q_id) {
            End(q_id);
        }
    }

    void End() {
        for (u64 q_id = 0; q_id < n_queries; ++q_id) {
            End(q_id);
        }
    }

    void EndWithoutSort(SizeT q_id) {
        if (u32 size = sizes[q_id]; size < top_k) {
            std::fill_n(distance_ptr + q_id * top_k + size, top_k - size, Compare::InitialValue());
        }
    }

    void EndWithoutSort() {
        for (u64 q_id = 0; q_id < n_queries; ++q_id) {
            EndWithoutSort(q_id);
        }
    }
};

export template <class Compare>
class SingleBestResultHandler : public ResultHandlerBase {
    using DistType = typename Compare::DistanceType;
    using ID = typename Compare::IDType;
    SizeT n_queries{};
    DistType *distance_ptr = nullptr;
    ID *id_ptr = nullptr;

public:
    explicit SingleBestResultHandler(SizeT n_queries, DistType *distance, ID *id)
        : ResultHandlerBase(ResultHandlerType::kSingleBest), n_queries(n_queries), distance_ptr(distance), id_ptr(id) {}

    ~SingleBestResultHandler() = default;

    void Begin(const SizeT i) { distance_ptr[i] = Compare::InitialValue(); }

    void Begin(SizeT i_start, SizeT i_end) { std::fill(distance_ptr + i_start, distance_ptr + i_end, Compare::InitialValue()); }

    void Begin() { std::fill_n(distance_ptr, n_queries, Compare::InitialValue()); }

    void ReInitialize() { std::fill_n(distance_ptr, n_queries, Compare::InitialValue()); }

    void AddResult(const SizeT i, const DistType dis, const ID idx) {
        auto &min_distance = this->distance_ptr[i];
        auto &min_index = this->id_ptr[i];
        if (Compare::Compare(min_distance, dis)) {
            min_distance = dis;
            min_index = idx;
        }
    }

    void AddResults(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const DistType *dis_tab_input, u32 segment_id, u32 segment_offset_start) {
        for (SizeT i = i_start; i < i_end; ++i) {
            auto dis_tab_i = dis_tab_input + (j1 - j0) * (i - i_start) - j0;

            auto &min_distance = this->distance_ptr[i];
            auto &min_index = this->id_ptr[i];
            auto tmp_min_distance = min_distance;
            auto tmp_min_j = std::numeric_limits<SizeT>::max();
            for (u32 j = j0; j < j1; ++j) {
                if (const DistType distance = dis_tab_i[j]; Compare::Compare(tmp_min_distance, distance)) {
                    tmp_min_distance = distance;
                    tmp_min_j = j;
                }
            }
            if (tmp_min_j != std::numeric_limits<SizeT>::max()) {
                min_distance = tmp_min_distance;
                min_index = RowID(segment_id, segment_offset_start + tmp_min_j);
            }
        }
    }

    void End(SizeT query_idx) {}

    void End(SizeT i_start, SizeT i_end) {}

    void End() {}

    void EndWithoutSort() {}
};

export template <class Compare>
class ReservoirResultHandler : public ResultHandlerBase {
    using DistType = typename Compare::DistanceType;
    using ID = typename Compare::IDType;
    SizeT n_queries;
    SizeT top_k;
    SizeT capacity;
    DistType *distance_ptr;
    ID *id_ptr;
    Vector<SizeT> sizes;
    Vector<DistType> thresholds;
    UniquePtr<DistType[]> reservoir_distance_ptr;
    UniquePtr<ID[]> reservoir_id_ptr;

    static DistType median3(DistType a, DistType b, DistType c) {
        if (a > b) {
            std::swap(a, b);
        }
        return b <= c ? b : std::max(a, c);
    }

    static void count_lt_and_eq(const DistType *vals, SizeT n, DistType thresh, SizeT &n_lt, SizeT &n_eq) {
        n_lt = n_eq = 0;
        for (SizeT i = 0; i < n; ++i) {
            auto v = *(vals++);
            if (Compare::Compare(thresh, v)) {
                n_lt++;
            } else if (v == thresh) {
                n_eq++;
            }
        }
    }

    static DistType sample_threshold_median3(const DistType *vals, SizeT n, DistType thresh_inf, DistType thresh_sup) {
        SizeT big_prime = 6700417;
        DistType val3[3];
        int vi = 0;
        for (SizeT i = 0; i < n; i++) {
            DistType v = vals[(i * big_prime) % n];
            if (Compare::Compare(v, thresh_inf) && Compare::Compare(thresh_sup, v)) {
                val3[vi++] = v;
                if (vi == 3) {
                    break;
                }
            }
        }
        if (vi == 3) {
            return median3(val3[0], val3[1], val3[2]);
        } else if (vi != 0) {
            return val3[0];
        } else {
            for (SizeT i = 0; i < n; i++) {
                DistType v = vals[i];
                if (Compare::Compare(v, thresh_inf) && Compare::Compare(thresh_sup, v)) {
                    return v;
                }
            }
            return thresh_inf;
        }
    }

    static SizeT compress_array(DistType *vals, ID *ids, SizeT n, DistType thresh, SizeT n_eq) {
        SizeT wp = 0;
        for (SizeT i = 0; i < n; ++i) {
            if (Compare::Compare(thresh, vals[i])) {
                if (wp != i) {
                    vals[wp] = vals[i];
                    ids[wp] = ids[i];
                }
                ++wp;
            } else if (n_eq > 0 && vals[i] == thresh) {
                if (wp != i) {
                    vals[wp] = vals[i];
                    ids[wp] = ids[i];
                }
                ++wp;
                --n_eq;
            }
        }
        if (n_eq != 0) {
            UnrecoverableError("compress_array error: n_eq != 0");
        }
        return wp;
    }

    static DistType partition_median3(DistType *vals, ID *ids, SizeT n, SizeT q_min, SizeT q_max, SizeT &q_out) {
        if (n < 3) {
            UnrecoverableError("partition_median3 error: n < 3");
        }
        DistType thresh_inf = Compare::CompareReverse::InitialValue();
        DistType thresh_sup = Compare::InitialValue();
        DistType thresh = median3(vals[0], vals[n / 2], vals[n - 1]);
        SizeT n_eq = 0, n_lt = 0;
        SizeT q = 0;
        for (int it = 0; it < 200; it++) {
            count_lt_and_eq(vals, n, thresh, n_lt, n_eq);
            if (n_lt <= q_min) {
                if (n_lt + n_eq >= q_min) {
                    q = q_min;
                    break;
                } else {
                    thresh_inf = thresh;
                }
            } else if (n_lt <= q_max) {
                q = n_lt;
                break;
            } else {
                thresh_sup = thresh;
            }
            DistType new_thresh = sample_threshold_median3(vals, n, thresh_inf, thresh_sup);
            if (new_thresh == thresh_inf) {
                UnrecoverableError("partition_median3 error: new_thresh == thresh_inf");
            }
            thresh = new_thresh;
        }
        if (q < n_lt) {
            UnrecoverableError("partition_median3 error: q < n_lt");
        }
        SizeT n_eq_extra = q - n_lt;
        auto wp = compress_array(vals, ids, n, thresh, n_eq_extra);
        if (wp != q) {
            UnrecoverableError("partition_median3 error: wp != q");
        }
        q_out = q;
        return thresh;
    }

public:
    explicit ReservoirResultHandler(SizeT n_queries, SizeT top_k, DistType *distance, ID *id)
        : ResultHandlerBase(ResultHandlerType::kReservoir), n_queries(n_queries), top_k(top_k), capacity(2 * top_k), distance_ptr(distance),
          id_ptr(id), sizes(n_queries), thresholds(n_queries, Compare::InitialValue()) {
        if (capacity < 8) {
            capacity = 8;
        }
        reservoir_distance_ptr = MakeUniqueForOverwrite<DistType[]>(n_queries * capacity);
        reservoir_id_ptr = MakeUniqueForOverwrite<ID[]>(n_queries * capacity);
    }

    ~ReservoirResultHandler() = default;

    void Begin() {}

    void ReInitialize() {
        std::fill(sizes.begin(), sizes.end(), 0);
        std::fill(thresholds.begin(), thresholds.end(), Compare::InitialValue());
    }

    [[nodiscard]] DistType GetThreshold(SizeT q_id) const { return thresholds[q_id]; }

    [[nodiscard]] SizeT GetSize(SizeT q_id) const { return sizes[q_id]; }

    void AddResult(SizeT q_id, DistType distance, ID id) {
        auto q_id_distance = reservoir_distance_ptr.get() + q_id * capacity;
        auto q_id_id = reservoir_id_ptr.get() + q_id * capacity;
        auto &size = sizes[q_id];
        auto &bar = thresholds[q_id];
        if (Compare::Compare(bar, distance)) {
            if (size == capacity) {
                bar = partition_median3(q_id_distance, q_id_id, capacity, top_k, (capacity + top_k) / 2, size);
            }
            q_id_distance[size] = distance;
            q_id_id[size] = id;
            ++size;
        }
    }

    void AddResults(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const DistType *dis_tab, u32 segment_id, u32 segment_offset_start) {
        for (SizeT i = i_start; i < i_end; i++) {
            auto dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;
            for (SizeT j = j0; j < j1; j++) {
                AddResult(i, dis_tab_i[j], RowID(segment_id, segment_offset_start + j));
            }
        }
    }

    void
    AddResults(SizeT i_start, SizeT i_end, SizeT j0, SizeT j1, const DistType *dis_tab, u32 segment_id, u32 segment_offset_start, Bitmask &bitmask) {
        for (SizeT i = i_start; i < i_end; i++) {
            auto dis_tab_i = dis_tab + (j1 - j0) * (i - i_start) - j0;
            for (SizeT j = j0; j < j1; j++) {
                auto segment_offset = segment_offset_start + j;
                if (bitmask.IsTrue(segment_offset)) {
                    AddResult(i, dis_tab_i[j], RowID(segment_id, segment_offset));
                }
            }
        }
    }

    void End(SizeT q_id) {
        auto dis_result = distance_ptr + q_id * top_k;
        auto id_result = id_ptr + q_id * top_k;
        auto q_id_distance = reservoir_distance_ptr.get() + q_id * capacity;
        auto q_id_id = reservoir_id_ptr.get() + q_id * capacity;
        auto size = sizes[q_id];
        auto result_size = std::min(size, top_k);
        memcpy(dis_result, q_id_distance, result_size * sizeof(DistType));
        memcpy(id_result, q_id_id, result_size * sizeof(ID));
        --dis_result;
        --id_result;
        for (u32 index = result_size / 2; index > 0; --index) {
            HeapifyDown<Compare>(dis_result, id_result, result_size, index);
        }
        if (size < top_k) {
            std::fill_n(dis_result + size + 1, top_k - size, Compare::InitialValue());
        } else {
            for (SizeT j = top_k; j < size; ++j) {
                if (Compare::Compare(dis_result[1], q_id_distance[j], id_result[1], q_id_id[j])) {
                    dis_result[1] = q_id_distance[j];
                    id_result[1] = q_id_id[j];
                    HeapifyDown<Compare>(dis_result, id_result, top_k, 1);
                }
            }
        }
        sizes[q_id] = result_size;
        while (result_size > 1) {
            std::swap(dis_result[result_size], dis_result[1]);
            std::swap(id_result[result_size], id_result[1]);
            --result_size;
            HeapifyDown<Compare>(dis_result, id_result, result_size, 1);
        }
    }

    void End() {
        for (SizeT i = 0; i < n_queries; ++i) {
            End(i);
        }
    }

    void EndWithoutSort(SizeT q_id) {
        auto dis_result = distance_ptr + q_id * top_k;
        auto id_result = id_ptr + q_id * top_k;
        auto q_id_distance = reservoir_distance_ptr.get() + q_id * capacity;
        auto q_id_id = reservoir_id_ptr.get() + q_id * capacity;
        auto size = sizes[q_id];
        auto result_size = std::min(size, top_k);
        memcpy(dis_result, q_id_distance, result_size * sizeof(DistType));
        memcpy(id_result, q_id_id, result_size * sizeof(ID));
        --dis_result;
        --id_result;
        if (size < top_k) {
            std::fill_n(dis_result + size + 1, top_k - size, Compare::InitialValue());
        } else if (size > top_k) {
            for (u32 index = result_size / 2; index > 0; --index) {
                HeapifyDown<Compare>(dis_result, id_result, result_size, index);
            }
            for (SizeT j = top_k; j < size; ++j) {
                if (Compare::Compare(dis_result[1], q_id_distance[j])) {
                    dis_result[1] = q_id_distance[j];
                    id_result[1] = q_id_id[j];
                    HeapifyDown<Compare>(dis_result, id_result, top_k, 1);
                }
            }
        }
        sizes[q_id] = result_size;
    }

    void EndWithoutSort() {
        for (u64 q_id = 0; q_id < n_queries; ++q_id) {
            EndWithoutSort(q_id);
        }
    }
};

} // namespace infinity