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

module ivf_index_data_in_mem;

import stl;
import ivf_index_storage;
import internal_types;
import block_column_entry;
import buffer_manager;
import column_def;
import chunk_index_entry;
import segment_index_entry;
import index_base;
import index_ivf;
import embedding_info;
import logical_type;
import data_type;
import infinity_exception;
import status;
import logger;
import kmeans_partition;
import search_top_1;
import column_vector;
import ivf_index_data;
import buffer_handle;

namespace infinity {
template <IsAnyOf<u8, i8, f64, f32, Float16T, BFloat16T> ColumnEmbeddingElementT>
Pair<const f32 *, UniquePtr<f32[]>> GetF32Ptr(const ColumnEmbeddingElementT *src_data_ptr, const u32 src_data_cnt) {
    Pair<const f32 *, UniquePtr<f32[]>> dst_data_ptr;
    if constexpr (std::is_same_v<f32, ColumnEmbeddingElementT>) {
        dst_data_ptr.first = src_data_ptr;
    } else {
        dst_data_ptr.second = MakeUniqueForOverwrite<f32[]>(src_data_cnt);
        dst_data_ptr.first = dst_data_ptr.second.get();
        for (u32 i = 0; i < src_data_cnt; ++i) {
            if constexpr (std::is_same_v<f64, ColumnEmbeddingElementT>) {
                dst_data_ptr.second[i] = static_cast<f32>(src_data_ptr[i]);
            } else {
                dst_data_ptr.second[i] = src_data_ptr[i];
            }
        }
    }
    return dst_data_ptr;
}

IVFIndexInMem::IVFIndexInMem(const RowID begin_row_id,
                             const IndexIVFOption &ivf_option,
                             const LogicalType column_logical_type,
                             const EmbeddingDataType embedding_data_type,
                             const u32 embedding_dimension)
    : begin_row_id_{begin_row_id},
      ivf_index_storage_{new IVF_Index_Storage(ivf_option, column_logical_type, embedding_data_type, embedding_dimension)} {}

IVFIndexInMem::~IVFIndexInMem() {
    std::unique_lock lock(rw_mutex_);
    if (own_ivf_index_storage_) {
        delete ivf_index_storage_;
    }
    ivf_index_storage_ = nullptr;
}

u32 IVFIndexInMem::GetInputRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return input_row_count_;
}

template <LogicalType column_logical_type, EmbeddingDataType embedding_data_type>
struct InMemStorage;

template <EmbeddingDataType embedding_data_type>
struct InMemStorage<LogicalType::kEmbedding, embedding_data_type> {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_data_type>;
    Vector<ColumnEmbeddingElementT> raw_source_data_{};
    Vector<SegmentOffset> source_offsets_{};
};

template <EmbeddingDataType embedding_data_type>
struct InMemStorage<LogicalType::kMultiVector, embedding_data_type> {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_data_type>;
    Vector<ColumnEmbeddingElementT> raw_source_data_{};
    Vector<SegmentOffset> source_offsets_{};
    Vector<u32> multi_vector_data_start_pos_{};
    Vector<u32> multi_vector_embedding_num_{};
};

template <LogicalType column_logical_type, EmbeddingDataType embedding_data_type>
class IVFIndexInMemT final : public IVFIndexInMem {
    using ColumnEmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_data_type>;
    InMemStorage<column_logical_type, embedding_data_type> in_mem_storage_;
    u32 build_index_bar_embedding_num_ = 0;

public:
    IVFIndexInMemT(const RowID begin_row_id, const IndexIVFOption &ivf_option, const u32 embedding_dimension)
        : IVFIndexInMem(begin_row_id, ivf_option, column_logical_type, embedding_data_type, embedding_dimension) {
        const auto mid = this->ivf_option().centroid_option_.centroids_num_ratio_ * this->ivf_option().centroid_option_.min_points_per_centroid_;
        build_index_bar_embedding_num_ = std::ceil(mid * mid + 3);
    }

    void InsertBlockData(const SegmentOffset block_offset,
                         BlockColumnEntry *block_column_entry,
                         BufferManager *buffer_manager,
                         const u32 row_offset,
                         const u32 row_count) override {
        const auto column_vector = block_column_entry->GetConstColumnVector(buffer_manager);
        std::unique_lock lock(rw_mutex_);
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
    }

    void BuildIndex() {
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
    }

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) override {
        std::unique_lock lock(rw_mutex_);
        if (!have_ivf_index_.test(std::memory_order_acquire)) {
            BuildIndex();
        }
        auto new_chunk_index_entry = segment_index_entry->CreateIVFIndexChunkIndexEntry(begin_row_id_, input_row_count_, buffer_mgr);
        BufferHandle handle = new_chunk_index_entry->GetIndex();
        auto *data_ptr = static_cast<IVFIndexInChunk *>(handle.GetDataMut());
        data_ptr->GetMemData(std::move(*ivf_index_storage_));
        ivf_index_storage_ = data_ptr->BasePtr();
        own_ivf_index_storage_ = false;
        dump_handle_ = std::move(handle);
        return new_chunk_index_entry;
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

} // namespace infinity