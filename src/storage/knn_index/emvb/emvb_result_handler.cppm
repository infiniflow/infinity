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

export module emvb_result_handler;
import stl;
import infinity_exception;

namespace infinity {

// EMVB needs move-only ID support
// EMVB only use min heap
template <typename DistType, typename ID>
struct EMVBCompareMin {
    using DistanceType = DistType;
    using IDType = ID;
    inline static bool Compare(DistanceType a, DistanceType b) { return a < b; }
    static constexpr DistanceType InitialValue() { return std::numeric_limits<DistanceType>::lowest(); }
    struct CompareReverse {
        static constexpr DistanceType InitialValue() { return std::numeric_limits<DistanceType>::max(); }
    };
};

template <typename Compare>
inline void HeapifyDown(typename Compare::DistanceType *distance, typename Compare::IDType *id, const u32 size, u32 index) {
    if (index == 0 || (index << 1) > size) {
        return;
    }
    auto tmp_d = distance[index];
    auto tmp_i = std::move(id[index]);
    for (u32 sub; (sub = (index << 1)) <= size; index = sub) {
        if (sub + 1 <= size && Compare::Compare(distance[sub + 1], distance[sub])) {
            ++sub;
        }
        if (!Compare::Compare(distance[sub], tmp_d)) {
            break;
        }
        distance[index] = distance[sub];
        id[index] = std::move(id[sub]);
    }
    distance[index] = tmp_d;
    id[index] = std::move(tmp_i);
}

template <typename DistType>
inline DistType median3(DistType a, DistType b, DistType c) {
    if (a > b) {
        std::swap(a, b);
    }
    return b <= c ? b : std::max(a, c);
}

template <typename Compare, typename DistType>
inline void count_lt_and_eq(const DistType *vals, const u32 n, DistType thresh, u32 &n_lt, u32 &n_eq) {
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

template <typename Compare, typename DistType>
inline DistType sample_threshold_median3(const DistType *vals, const u32 n, DistType thresh_inf, DistType thresh_sup) {
    DistType val3[3];
    u32 vi = 0;
    for (u32 i = 0; i < n; ++i) {
        DistType v = vals[(i * 6700417ull) % n];
        if (Compare::Compare(v, thresh_inf) && Compare::Compare(thresh_sup, v)) {
            val3[vi++] = v;
            if (vi == 3) {
                break;
            }
        }
    }
    if (vi == 3) {
        return median3(val3[0], val3[1], val3[2]);
    }
    if (vi != 0) {
        return val3[0];
    }
    for (SizeT i = 0; i < n; i++) {
        if (const DistType v = vals[i]; Compare::Compare(v, thresh_inf) && Compare::Compare(thresh_sup, v)) {
            return v;
        }
    }
    return thresh_inf;
}

template <typename Compare, typename DistType, typename ID>
inline u32 compress_array(DistType *vals, ID *ids, const u32 n, DistType thresh, u32 n_eq) {
    u32 wp = 0;
    for (u32 i = 0; i < n; ++i) {
        if (Compare::Compare(thresh, vals[i])) {
            if (wp != i) {
                vals[wp] = vals[i];
                ids[wp] = std::move(ids[i]);
            }
            ++wp;
        } else if (n_eq > 0 && vals[i] == thresh) {
            if (wp != i) {
                vals[wp] = vals[i];
                ids[wp] = std::move(ids[i]);
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

template <typename Compare, typename DistType, typename ID>
inline DistType partition_median3(DistType *vals, ID *ids, const u32 n, const u32 q_min, const u32 q_max, u32 &q_out) {
    if (n < 3) {
        UnrecoverableError("partition_median3 error: n < 3");
    }
    DistType thresh_inf = Compare::CompareReverse::InitialValue();
    DistType thresh_sup = Compare::InitialValue();
    DistType thresh = median3(vals[0], vals[n / 2], vals[n - 1]);
    u32 n_eq = 0;
    u32 n_lt = 0;
    u32 q = 0;
    for (int it = 0; it < 200; ++it) {
        count_lt_and_eq<Compare>(vals, n, thresh, n_lt, n_eq);
        if (n_lt <= q_min) {
            if (n_lt + n_eq >= q_min) {
                q = q_min;
                break;
            }
            thresh_inf = thresh;
        } else if (n_lt <= q_max) {
            q = n_lt;
            break;
        } else {
            thresh_sup = thresh;
        }
        DistType new_thresh = sample_threshold_median3<Compare>(vals, n, thresh_inf, thresh_sup);
        if (new_thresh == thresh_inf) {
            UnrecoverableError("partition_median3 error: new_thresh == thresh_inf");
        }
        thresh = new_thresh;
    }
    if (q < n_lt) {
        UnrecoverableError("partition_median3 error: q < n_lt");
    }
    const u32 n_eq_extra = q - n_lt;
    auto wp = compress_array<Compare>(vals, ids, n, thresh, n_eq_extra);
    if (wp != q) {
        UnrecoverableError("partition_median3 error: wp != q");
    }
    q_out = q;
    return thresh;
}

template <class Compare>
class EMVBReservoirResultHandlerT {
    using DistType = typename Compare::DistanceType;
    using ID = typename Compare::IDType;
    u32 top_k_;
    u32 capacity_;
    u32 size_;
    DistType threshold_;
    UniquePtr<DistType[]> reservoir_distance_ptr_;
    UniquePtr<ID[]> reservoir_id_ptr_;

public:
    explicit EMVBReservoirResultHandlerT(const u32 top_k) : top_k_{top_k}, capacity_{2 * top_k}, size_{0}, threshold_{Compare::InitialValue()} {
        if (capacity_ < 8) {
            capacity_ = 8;
        }
        reservoir_distance_ptr_ = MakeUniqueForOverwrite<DistType[]>(capacity_);
        reservoir_id_ptr_ = MakeUniqueForOverwrite<ID[]>(capacity_);
    }

    [[nodiscard]] auto GetThreshold() const { return threshold_; }

    [[nodiscard]] auto GetSize() const { return size_; }

    [[nodiscard]] auto GetDistancePtr() { return std::move(reservoir_distance_ptr_); }

    [[nodiscard]] auto GetIdPtr() { return std::move(reservoir_id_ptr_); }

    void Add(DistType distance, auto &&id) {
        auto q_id_distance = reservoir_distance_ptr_.get();
        auto q_id_id = reservoir_id_ptr_.get();
        if (Compare::Compare(threshold_, distance)) {
            if (size_ == capacity_) {
                threshold_ = partition_median3<Compare>(q_id_distance, q_id_id, capacity_, top_k_, (capacity_ + top_k_) / 2, size_);
            }
            q_id_distance[size_] = distance;
            q_id_id[size_] = std::move(id);
            ++size_;
        }
    }

    void EndWithoutSort() {
        if (size_ > top_k_) {
            const auto size = size_;
            const auto result_size = top_k_;
            const auto distance_ptr = reservoir_distance_ptr_.get();
            const auto id_ptr = reservoir_id_ptr_.get();
            const auto dis_result = distance_ptr - 1;
            const auto id_result = id_ptr - 1;
            for (u32 index = result_size / 2; index > 0; --index) {
                HeapifyDown<Compare>(dis_result, id_result, result_size, index);
            }
            for (u32 j = result_size; j < size; ++j) {
                if (Compare::Compare(distance_ptr[0], distance_ptr[j])) {
                    distance_ptr[0] = distance_ptr[j];
                    id_ptr[0] = std::move(id_ptr[j]);
                    HeapifyDown<Compare>(dis_result, id_result, result_size, 1);
                }
            }
            size_ = result_size;
        }
    }

    void EndSort() {
        EndWithoutSort();
        auto result_size = size_;
        const auto dis_result = reservoir_distance_ptr_.get() - 1;
        const auto id_result = reservoir_id_ptr_.get() - 1;
        while (result_size > 1) {
            std::swap(dis_result[result_size], dis_result[1]);
            std::swap(id_result[result_size], id_result[1]);
            --result_size;
            HeapifyDown<Compare>(dis_result, id_result, result_size, 1);
        }
    }
};

export template <typename DistType, typename ID>
using EMVBReservoirResultHandler = EMVBReservoirResultHandlerT<EMVBCompareMin<DistType, ID>>;

} // namespace infinity