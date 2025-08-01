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

#include <vector>
module infinity_core:ivf_index_data_in_mem.impl;

import :ivf_index_data_in_mem;

import :stl;
import :ivf_index_storage;
import internal_types;
import :buffer_manager;
import column_def;
import :index_base;
import :index_ivf;
import embedding_info;
import logical_type;
import data_type;
import :infinity_exception;
import :status;
import :logger;
import :kmeans_partition;
import :search_top_1;
import :column_vector;
import :ivf_index_data;
import :buffer_handle;
import :knn_scan_data;
import :ivf_index_util_func;
import :base_memindex;
import :memindex_tracer;
import :infinity_context;
import :third_party;
import :buffer_obj;

namespace infinity {

IVFIndexInMem::IVFIndexInMem(const RowID begin_row_id,
                             const IndexIVFOption &ivf_option,
                             const LogicalType column_logical_type,
                             const EmbeddingDataType embedding_data_type,
                             const u32 embedding_dimension)
    : begin_row_id_{begin_row_id},
      ivf_index_storage_{new IVF_Index_Storage(ivf_option, column_logical_type, embedding_data_type, embedding_dimension)} {}

IVFIndexInMem::~IVFIndexInMem() {
    if (own_ivf_index_storage_) {
        delete ivf_index_storage_;
    }
    ivf_index_storage_ = nullptr;
}

u32 IVFIndexInMem::GetInputRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return input_row_count_;
}

const ChunkIndexMetaInfo IVFIndexInMem::GetChunkIndexMetaInfo() const { return ChunkIndexMetaInfo{"", begin_row_id_, input_row_count_, 0}; }

template <LogicalType column_logical_type, EmbeddingDataType embedding_data_type>
struct InMemStorage;

template <EmbeddingDataType embedding_data_type>
struct InMemStorage<LogicalType::kEmbedding, embedding_data_type> {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_data_type>;
    Vector<ColumnEmbeddingElementT> raw_source_data_{};
    Vector<SegmentOffset> source_offsets_{};
    SizeT MemoryUsed() const { return sizeof(ColumnEmbeddingElementT) * raw_source_data_.size() + sizeof(SegmentOffset) * source_offsets_.size(); }
};

template <EmbeddingDataType embedding_data_type>
struct InMemStorage<LogicalType::kMultiVector, embedding_data_type> {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_data_type>;
    Vector<ColumnEmbeddingElementT> raw_source_data_{};
    Vector<SegmentOffset> source_offsets_{};
    Vector<u32> multi_vector_data_start_pos_{};
    Vector<u32> multi_vector_embedding_num_{};
    SizeT MemoryUsed() const {
        return sizeof(ColumnEmbeddingElementT) * raw_source_data_.size() + sizeof(SegmentOffset) * source_offsets_.size() +
               sizeof(u32) * (multi_vector_data_start_pos_.size() + multi_vector_embedding_num_.size());
    }
};

template <LogicalType column_logical_type, EmbeddingDataType embedding_data_type>
class IVFIndexInMemT final : public IVFIndexInMem {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_data_type>;
    InMemStorage<column_logical_type, embedding_data_type> in_mem_storage_;
    u32 build_index_bar_embedding_num_ = 0;

public:
    SizeT MemoryUsed() const override {
        if (have_ivf_index_.test(std::memory_order_acquire)) {
            return ivf_index_storage_->MemoryUsed();
        } else {
            return in_mem_storage_.MemoryUsed();
        }
    }

    ~IVFIndexInMemT() override {
        SizeT mem_used = MemoryUsed();
        DecreaseMemoryUsageBase(mem_used);
    }

    MemIndexTracerInfo GetInfo() const override {
        const auto mem = MemoryUsed();
        return MemIndexTracerInfo(MakeShared<String>(index_name_),
                                  MakeShared<String>(table_name_),
                                  MakeShared<String>(db_name_),
                                  mem,
                                  input_row_count_);
    }

    IVFIndexInMemT(const RowID begin_row_id, const IndexIVFOption &ivf_option, const u32 embedding_dimension)
        : IVFIndexInMem(begin_row_id, ivf_option, column_logical_type, embedding_data_type, embedding_dimension) {
        const auto mid = this->ivf_option().centroid_option_.centroids_num_ratio_ * this->ivf_option().centroid_option_.min_points_per_centroid_;
        build_index_bar_embedding_num_ = std::ceil(mid * mid + 3);
    }

    RowID GetBeginRowID() const override { return begin_row_id_; }

    u32 GetRowCount() const override { return input_row_count_; }

    void
    InsertBlockData(const SegmentOffset block_offset, const ColumnVector &column_vector, BlockOffset row_offset, BlockOffset row_count) override {
        std::unique_lock lock(rw_mutex_);
        SizeT mem1 = MemoryUsed();
        if (have_ivf_index_.test(std::memory_order_acquire)) {
            if constexpr (column_logical_type == LogicalType::kEmbedding) {
                const auto *column_embedding_ptr = reinterpret_cast<const ColumnEmbeddingElementT *>(column_vector.data());
                ivf_index_storage_->AddEmbeddingBatch(block_offset + row_offset,
                                                      column_embedding_ptr + row_offset * embedding_dimension(),
                                                      row_count);
                input_embedding_count_ += row_count;
            } else if constexpr (column_logical_type == LogicalType::kMultiVector) {
                for (u32 i = 0; i < row_count; ++i) {
                    auto [raw_data, embedding_num] = column_vector.GetMultiVectorRaw(row_offset + i);
                    ivf_index_storage_->AddMultiVector(block_offset + row_offset + i, raw_data.data(), embedding_num);
                    input_embedding_count_ += embedding_num;
                }
            } else {
                static_assert(false);
            }
            input_row_count_ += row_count;
        } else {
            // no index now
            if constexpr (column_logical_type == LogicalType::kEmbedding) {
                const auto *column_embedding_ptr = reinterpret_cast<const ColumnEmbeddingElementT *>(column_vector.data());
                in_mem_storage_.raw_source_data_.insert(in_mem_storage_.raw_source_data_.end(),
                                                        column_embedding_ptr + row_offset * embedding_dimension(),
                                                        column_embedding_ptr + (row_offset + row_count) * embedding_dimension());
                const auto old_size = in_mem_storage_.source_offsets_.size();
                in_mem_storage_.source_offsets_.resize(old_size + row_count);
                std::iota(in_mem_storage_.source_offsets_.begin() + old_size, in_mem_storage_.source_offsets_.end(), block_offset + row_offset);
                input_embedding_count_ += row_count;
            } else if constexpr (column_logical_type == LogicalType::kMultiVector) {
                for (u32 i = 0; i < row_count; ++i) {
                    auto [raw_data, embedding_num] = column_vector.GetMultiVectorRaw(row_offset + i);
                    const auto *mv_ptr = reinterpret_cast<const ColumnEmbeddingElementT *>(raw_data.data());
                    in_mem_storage_.multi_vector_data_start_pos_.push_back(in_mem_storage_.raw_source_data_.size());
                    in_mem_storage_.raw_source_data_.insert(in_mem_storage_.raw_source_data_.end(),
                                                            mv_ptr,
                                                            mv_ptr + embedding_num * embedding_dimension());
                    in_mem_storage_.source_offsets_.push_back(block_offset + row_offset + i);
                    in_mem_storage_.multi_vector_embedding_num_.push_back(embedding_num);
                    input_embedding_count_ += embedding_num;
                }
            } else {
                static_assert(false);
            }
            input_row_count_ += row_count;
            if (input_embedding_count_ >= build_index_bar_embedding_num_) {
                BuildIndex();
            }
        }
        SizeT mem2 = MemoryUsed();
        LOG_TRACE(fmt::format("ivf mem usage {} -> {}", mem1, mem2));
        IncreaseMemoryUsageBase(mem2 > mem1 ? mem2 - mem1 : 0);
    }

    void BuildIndex() {
        SizeT mem1 = MemoryUsed();
        if (have_ivf_index_.test(std::memory_order_acquire)) {
            UnrecoverableError("Already have index");
        }
        {
            // train by f32
            const auto [train_ptr, _] = GetF32Ptr(in_mem_storage_.raw_source_data_.data(), in_mem_storage_.raw_source_data_.size());
            ivf_index_storage_->Train(input_embedding_count_, train_ptr);
        }
        // insert
        if constexpr (column_logical_type == LogicalType::kEmbedding) {
            ivf_index_storage_->AddEmbeddingBatch(in_mem_storage_.source_offsets_.data(),
                                                  in_mem_storage_.raw_source_data_.data(),
                                                  in_mem_storage_.source_offsets_.size());
        } else if constexpr (column_logical_type == LogicalType::kMultiVector) {
            for (u32 i = 0; i < in_mem_storage_.source_offsets_.size(); ++i) {
                ivf_index_storage_->AddMultiVector(in_mem_storage_.source_offsets_[i],
                                                   in_mem_storage_.raw_source_data_.data() + in_mem_storage_.multi_vector_data_start_pos_[i],
                                                   in_mem_storage_.multi_vector_embedding_num_[i]);
            }
        } else {
            static_assert(false);
        }
        // fin
        have_ivf_index_.test_and_set(std::memory_order_release);
        SizeT mem2 = MemoryUsed();
        LOG_TRACE(fmt::format("ivf mem usage {} -> {}", mem1, mem2));
        IncreaseMemoryUsageBase(mem2 > mem1 ? mem2 - mem1 : 0);
    }

    void Dump(BufferObj *buffer_obj, SizeT *p_dump_size) override {
        std::unique_lock lock(rw_mutex_);
        SizeT dump_size = MemoryUsed();
        if (!have_ivf_index_.test(std::memory_order_acquire)) {
            BuildIndex();
        }
        if (p_dump_size != nullptr) {
            *p_dump_size = dump_size;
        }
        BufferHandle handle = buffer_obj->Load();
        auto *data_ptr = static_cast<IVFIndexInChunk *>(handle.GetDataMut());
        data_ptr->GetMemData(std::move(*ivf_index_storage_));
        delete ivf_index_storage_;
        ivf_index_storage_ = data_ptr->GetIVFIndexStoragePtr();
        own_ivf_index_storage_ = false;
        dump_handle_ = std::move(handle);
    }

    void SearchIndexInMem(const KnnDistanceBase1 *knn_distance,
                          const void *query_ptr,
                          const EmbeddingDataType query_element_type,
                          const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                          const std::function<void(f32, SegmentOffset)> &add_result_func) const override {
        auto ReturnT = [&]<EmbeddingDataType query_element_type> {
            if constexpr ((query_element_type == EmbeddingDataType::kElemFloat && IsAnyOf<ColumnEmbeddingElementT, f64, f32, Float16T, BFloat16T>) ||
                          (query_element_type == embedding_data_type &&
                           (query_element_type == EmbeddingDataType::kElemInt8 || query_element_type == EmbeddingDataType::kElemUInt8))) {
                return SearchIndexInMemT<query_element_type>(knn_distance,
                                                             static_cast<const EmbeddingDataTypeToCppTypeT<query_element_type> *>(query_ptr),
                                                             satisfy_filter_func,
                                                             add_result_func);
            } else {
                UnrecoverableError("Invalid Query EmbeddingDataType");
            }
        };
        switch (query_element_type) {
            case EmbeddingDataType::kElemFloat: {
                return ReturnT.template operator()<EmbeddingDataType::kElemFloat>();
            }
            case EmbeddingDataType::kElemUInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemUInt8>();
            }
            case EmbeddingDataType::kElemInt8: {
                return ReturnT.template operator()<EmbeddingDataType::kElemInt8>();
            }
            default: {
                UnrecoverableError("Invalid EmbeddingDataType");
            }
        }
    }

    template <EmbeddingDataType query_element_type>
    void SearchIndexInMemT(const KnnDistanceBase1 *knn_distance,
                           const EmbeddingDataTypeToCppTypeT<query_element_type> *query_ptr,
                           const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                           const std::function<void(f32, SegmentOffset)> &add_result_func) const {
        using QueryDataType = EmbeddingDataTypeToCppTypeT<query_element_type>;
        auto knn_distance_1 = dynamic_cast<const KnnDistance1<QueryDataType, f32> *>(knn_distance);
        if (!knn_distance_1) [[unlikely]] {
            UnrecoverableError("Invalid KnnDistance1");
        }
        if constexpr (column_logical_type == LogicalType::kEmbedding) {
            auto dist_func = knn_distance_1->dist_func_;
            for (u32 i = 0; i < in_mem_storage_.source_offsets_.size(); ++i) {
                const auto segment_offset = in_mem_storage_.source_offsets_[i];
                if (!satisfy_filter_func(segment_offset)) {
                    continue;
                }
                auto v_ptr = in_mem_storage_.raw_source_data_.data() + i * embedding_dimension();
                auto [calc_ptr, _] = GetSearchCalcPtr<QueryDataType>(v_ptr, embedding_dimension());
                auto d = dist_func(calc_ptr, query_ptr, embedding_dimension());
                add_result_func(d, segment_offset);
            }
        } else if constexpr (column_logical_type == LogicalType::kMultiVector) {
            for (u32 i = 0; i < in_mem_storage_.source_offsets_.size(); ++i) {
                const auto segment_offset = in_mem_storage_.source_offsets_[i];
                if (!satisfy_filter_func(segment_offset)) {
                    continue;
                }
                auto mv_ptr = in_mem_storage_.raw_source_data_.data() + in_mem_storage_.multi_vector_data_start_pos_[i];
                auto mv_num = in_mem_storage_.multi_vector_embedding_num_[i];
                auto [calc_ptr, _] = GetSearchCalcPtr<QueryDataType>(mv_ptr, mv_num * embedding_dimension());
                auto dists = knn_distance_1->Calculate(calc_ptr, mv_num, query_ptr, embedding_dimension());
                for (const auto d : dists) {
                    add_result_func(d, segment_offset);
                }
            }
        } else {
            static_assert(false);
        }
    }
};

template <LogicalType column_logical_type>
SharedPtr<IVFIndexInMem> GetNewIVFIndexInMem(const DataType *column_data_type, const RowID begin_row_id, const IndexIVFOption &index_ivf_option) {
    const auto *embedding_info_ptr = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get());
    auto GetResult = [&]<EmbeddingDataType embedding_data_type> {
        return MakeShared<IVFIndexInMemT<column_logical_type, embedding_data_type>>(begin_row_id, index_ivf_option, embedding_info_ptr->Dimension());
    };
    switch (embedding_info_ptr->Type()) {
        case EmbeddingDataType::kElemInt8: {
            return GetResult.template operator()<EmbeddingDataType::kElemInt8>();
        }
        case EmbeddingDataType::kElemUInt8: {
            return GetResult.template operator()<EmbeddingDataType::kElemUInt8>();
        }
        case EmbeddingDataType::kElemFloat: {
            return GetResult.template operator()<EmbeddingDataType::kElemFloat>();
        }
        case EmbeddingDataType::kElemFloat16: {
            return GetResult.template operator()<EmbeddingDataType::kElemFloat16>();
        }
        case EmbeddingDataType::kElemBFloat16: {
            return GetResult.template operator()<EmbeddingDataType::kElemBFloat16>();
        }
        case EmbeddingDataType::kElemDouble: {
            return GetResult.template operator()<EmbeddingDataType::kElemDouble>();
        }
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64:
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Unsupported embedding element type for IVF");
            return {};
        }
    }
}

SharedPtr<IVFIndexInMem> IVFIndexInMem::NewIVFIndexInMem(const ColumnDef *column_def, const IndexBase *index_base, const RowID begin_row_id) {
    auto *index_ivf_ptr = static_cast<const IndexIVF *>(index_base);
    const auto &index_ivf_option = index_ivf_ptr->ivf_option_;
    const auto *column_data_type = column_def->type().get();
    if (const auto column_logical_type = column_data_type->type(); column_logical_type == LogicalType::kEmbedding) {
        return GetNewIVFIndexInMem<LogicalType::kEmbedding>(column_data_type, begin_row_id, index_ivf_option);
    } else if (column_logical_type == LogicalType::kMultiVector) {
        return GetNewIVFIndexInMem<LogicalType::kMultiVector>(column_data_type, begin_row_id, index_ivf_option);
    }
    UnrecoverableError("IVFIndex can only apply to Embedding and multi-vector column");
    return {};
}

void IVFIndexInMem::SearchIndex(const KnnDistanceBase1 *knn_distance,
                                const void *query_ptr,
                                const EmbeddingDataType query_element_type,
                                const u32 nprobe,
                                const std::function<bool(SegmentOffset)> &satisfy_filter_func,
                                const std::function<void(f32, SegmentOffset)> &add_result_func) const {
    std::shared_lock lock(rw_mutex_);
    if (have_ivf_index_.test(std::memory_order_acquire)) {
        ivf_index_storage_->SearchIndex(knn_distance, query_ptr, query_element_type, nprobe, satisfy_filter_func, add_result_func);
    } else {
        SearchIndexInMem(knn_distance, query_ptr, query_element_type, satisfy_filter_func, add_result_func);
    }
}

} // namespace infinity
