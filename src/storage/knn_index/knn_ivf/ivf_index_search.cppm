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
#include <functional>
export module ivf_index_search;

import stl;
import internal_types;
import data_type;
import knn_expr;
import knn_scan_data;
import logical_type;
import knn_result_handler;
import multivector_result_handler;
import infinity_exception;
import status;
import third_party;
import roaring_bitmap;
import knn_filter;
import ivf_index_data;
import ivf_index_data_in_mem;
import ivf_index_storage;
import search_top_1;
import search_top_k;

namespace infinity {

export struct IVF_Search_Params {
    const KnnDistanceBase1 *knn_distance_{};
    const KnnScanSharedData *knn_scan_shared_data_{};
    i64 topk_{};
    const void *query_embedding_{};
    EmbeddingDataType query_elem_type_{EmbeddingDataType::kElemInvalid};
    KnnDistanceType knn_distance_type_{KnnDistanceType::kInvalid};
    i32 nprobe_{1};

    static IVF_Search_Params Make(const KnnScanFunctionData *knn_scan_function_data);
};

export template <typename DistanceDataType>
class IVF_Search_Handler {
protected:
    IVF_Search_Params ivf_params_;
    UniquePtr<DistanceDataType[]> distance_output_ptr_{};
    UniquePtr<SegmentOffset[]> segment_offset_output_ptr_{};

    explicit IVF_Search_Handler(const IVF_Search_Params &ivf_params) : ivf_params_(ivf_params) {
        distance_output_ptr_ = MakeUniqueForOverwrite<DistanceDataType[]>(ivf_params_.topk_);
        segment_offset_output_ptr_ = MakeUniqueForOverwrite<SegmentOffset[]>(ivf_params_.topk_);
    }
    virtual SizeT EndWithoutSortAndGetResultSize() = 0;

public:
    virtual ~IVF_Search_Handler() = default;
    virtual void Begin() = 0;
    virtual void Search(const IVFIndexInChunk *ivf_index_in_chunk) = 0;
    virtual void Search(const IVFIndexInMem *ivf_index_in_mem) = 0;
    Tuple<SizeT, UniquePtr<DistanceDataType[]>, UniquePtr<SegmentOffset[]>> EndWithoutSort() {
        const auto result_cnt = EndWithoutSortAndGetResultSize();
        return {result_cnt, std::move(distance_output_ptr_), std::move(segment_offset_output_ptr_)};
    }
};

template <bool use_bitmask>
struct IVF_Filter;

template <>
struct IVF_Filter<true> {
    BitmaskFilter<SegmentOffset> filter_;
    IVF_Filter(const Bitmask &bitmask, const SegmentOffset max_segment_offset) : filter_(bitmask) {}
    bool operator()(const SegmentOffset segment_offset) const { return filter_(segment_offset); }
};

template <>
struct IVF_Filter<false> {
    AppendFilter filter_;
    IVF_Filter(const Bitmask &bitmask, const SegmentOffset max_segment_offset) : filter_(max_segment_offset) {}
    bool operator()(const SegmentOffset segment_offset) const { return filter_(segment_offset); }
};

template <LogicalType t,
          template <typename, typename>
          typename C,
          typename DistanceDataType,
          bool use_bitmask,
          typename MultiVectorInnerTopnIndexType = void>
class IVF_Search_HandlerT final : public IVF_Search_Handler<DistanceDataType> {
    static_assert(std::is_same_v<DistanceDataType, f32>); // KnnDistanceBase1 type?
    static_assert(t == LogicalType::kEmbedding || t == LogicalType::kMultiVector);
    static constexpr bool NEED_FLIP = !std::is_same_v<CompareMax<DistanceDataType, SegmentOffset>, C<DistanceDataType, SegmentOffset>>;
    using ResultHandler = std::conditional_t<t == LogicalType::kEmbedding,
                                             HeapResultHandler<CompareMax<DistanceDataType, SegmentOffset>>,
                                             MultiVectorResultHandler<DistanceDataType, SegmentOffset, MultiVectorInnerTopnIndexType>>;
    IVF_Filter<use_bitmask> filter_;
    ResultHandler result_handler_;

public:
    IVF_Search_HandlerT(const IVF_Search_Params &ivf_params, const Bitmask &bitmask, SegmentOffset max_segment_offset)
        : IVF_Search_Handler<DistanceDataType>(ivf_params), filter_(bitmask, max_segment_offset),
          result_handler_(1, this->ivf_params_.topk_, this->distance_output_ptr_.get(), this->segment_offset_output_ptr_.get()) {}
    void Begin() override { result_handler_.Begin(); }
    void Search(const IVFIndexInChunk *ivf_index_in_chunk) override {
        const auto *ivf_index_storage = ivf_index_in_chunk->GetIVFIndexStoragePtr();
        ivf_index_storage->SearchIndex(this->ivf_params_.knn_distance_,
                                       this->ivf_params_.query_embedding_,
                                       this->ivf_params_.query_elem_type_,
                                       this->ivf_params_.nprobe_,
                                       std::bind(&IVF_Search_HandlerT::SatisfyFilter, this, std::placeholders::_1),
                                       std::bind(&IVF_Search_HandlerT::AddResult, this, std::placeholders::_1, std::placeholders::_2));
    }
    void Search(const IVFIndexInMem *ivf_index_in_mem) override {
        ivf_index_in_mem->SearchIndex(this->ivf_params_.knn_distance_,
                                      this->ivf_params_.query_embedding_,
                                      this->ivf_params_.query_elem_type_,
                                      this->ivf_params_.nprobe_,
                                      std::bind(&IVF_Search_HandlerT::SatisfyFilter, this, std::placeholders::_1),
                                      std::bind(&IVF_Search_HandlerT::AddResult, this, std::placeholders::_1, std::placeholders::_2));
    }
    bool SatisfyFilter(SegmentOffset i) { return filter_(i); }
    void AddResult(DistanceDataType d, SegmentOffset i) {
        assert(SatisfyFilter(i));
        if constexpr (NEED_FLIP) {
            d = -d;
        }
        if constexpr (t == LogicalType::kEmbedding) {
            result_handler_.AddResult(0, d, i);
        } else {
            static_assert(t == LogicalType::kMultiVector);
            result_handler_.AddResult(d, i);
        }
    }
    SizeT EndWithoutSortAndGetResultSize() override {
        result_handler_.EndWithoutSort();
        const auto result_cnt = result_handler_.GetSize(0);
        if constexpr (NEED_FLIP) {
            for (u32 i = 0; i < result_cnt; ++i) {
                this->distance_output_ptr_[i] = -(this->distance_output_ptr_[i]);
            }
        }
        return result_cnt;
    }
};

export template <LogicalType t, template <typename, typename> typename C, typename DistanceDataType, bool use_bitmask>
UniquePtr<IVF_Search_Handler<DistanceDataType>>
GetIVFSearchHandler(const IVF_Search_Params &ivf_params, const Bitmask &bitmask, const SegmentOffset max_segment_offset) {
    if constexpr (t == LogicalType::kEmbedding) {
        return MakeUnique<IVF_Search_HandlerT<t, C, DistanceDataType, use_bitmask>>(ivf_params, bitmask, max_segment_offset);
    } else if constexpr (t == LogicalType::kMultiVector) {
        const auto top_k = ivf_params.topk_;
        if (top_k <= 0) {
            RecoverableError(Status::SyntaxError(fmt::format("Invalid topk: {}", top_k)));
            return nullptr;
        }
        if (top_k <= std::numeric_limits<u8>::max()) {
            return MakeUnique<IVF_Search_HandlerT<t, C, DistanceDataType, use_bitmask, u8>>(ivf_params, bitmask, max_segment_offset);
        }
        if (top_k <= std::numeric_limits<u16>::max()) {
            return MakeUnique<IVF_Search_HandlerT<t, C, DistanceDataType, use_bitmask, u16>>(ivf_params, bitmask, max_segment_offset);
        }
        if (top_k <= std::numeric_limits<u32>::max()) {
            return MakeUnique<IVF_Search_HandlerT<t, C, DistanceDataType, use_bitmask, u32>>(ivf_params, bitmask, max_segment_offset);
        }
        RecoverableError(Status::SyntaxError(fmt::format("Unsupported topk : {}, which is larger than u32::max()", top_k)));
        return nullptr;
    } else {
        static_assert(false, "Invalid LogicalType for KNN");
        return nullptr;
    }
}

export template <LogicalType t, template <typename, typename> typename C, typename DistanceDataType>
UniquePtr<IVF_Search_Handler<DistanceDataType>>
GetIVFSearchHandler(const IVF_Search_Params &ivf_params, const bool use_bitmask, const Bitmask &bitmask, const SegmentOffset max_segment_offset) {
    if (use_bitmask) {
        return GetIVFSearchHandler<t, C, DistanceDataType, true>(ivf_params, bitmask, max_segment_offset);
    }
    return GetIVFSearchHandler<t, C, DistanceDataType, false>(ivf_params, bitmask, max_segment_offset);
}

} // namespace infinity
