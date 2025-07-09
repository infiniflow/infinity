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

#include <algorithm>
#include <cassert>
module infinity_core;

import :stl;
import :index_ivf;
import :ivf_index_storage;
import column_def;
import :index_base;
import embedding_info;
import internal_types;
import :buffer_manager;
import :infinity_exception;
import :status;
import data_type;
import :default_values;
import :column_vector;
import :logger;
import :kmeans_partition;
import logical_type;
import :ivf_index_util_func;

import :column_meta;
import :block_meta;
import :segment_meta;
import :new_catalog;
import :kv_store;

namespace infinity {

class NewIVFDataAccessor : public IVFDataAccessorBase {
public:
    NewIVFDataAccessor(SegmentMeta &segment_meta, ColumnID column_id) : segment_meta_(segment_meta), column_id_(column_id) {}

    const_ptr_t GetEmbedding(SizeT offset) override {
        SizeT block_offset = UpdateColumnVector(offset);
        return cur_column_vector_.data() + block_offset * cur_column_vector_.data_type_size_;
    }

    Pair<Span<const char>, SizeT> GetMultiVector(SizeT offset) override {
        SizeT block_offset = UpdateColumnVector(offset);
        return cur_column_vector_.GetMultiVectorRaw(block_offset);
    }

private:
    SizeT UpdateColumnVector(SizeT offset) {
        SizeT block_offset = offset % DEFAULT_BLOCK_CAPACITY;
        BlockID block_id = offset / DEFAULT_BLOCK_CAPACITY;
        if (block_id != last_block_id_) {
            last_block_id_ = block_id;
            BlockMeta block_meta(block_id, segment_meta_);
            // auto [row_cnt, status] = block_meta.GetRowCnt();
            auto [row_cnt, status] = block_meta.GetRowCnt1();
            if (!status.ok()) {
                UnrecoverableError("Get row count failed");
            }

            ColumnMeta column_meta(column_id_, block_meta);
            status = NewCatalog::GetColumnVector(column_meta, row_cnt, ColumnVectorMode::kReadOnly, cur_column_vector_);
            if (!status.ok()) {
                UnrecoverableError("Get column vector failed");
            }
        }
        return block_offset;
    }

private:
    SegmentMeta &segment_meta_;
    ColumnID column_id_;

    BlockID last_block_id_ = std::numeric_limits<BlockID>::max();
    ColumnVector cur_column_vector_;
};

void IVFIndexInChunk::BuildIVFIndex(SegmentMeta &segment_meta, u32 row_count, SharedPtr<ColumnDef> column_def) {
    RowID base_rowid(segment_meta.segment_id(), 0);
    NewIVFDataAccessor data_accessor(segment_meta, column_def->id());
    BuildIVFIndex(base_rowid, row_count, &data_accessor, column_def);
}

void IVFIndexInChunk::BuildIVFIndex(RowID base_rowid, u32 row_count, IVFDataAccessorBase *data_accessor, const SharedPtr<ColumnDef> &column_def) {
    auto Call = [&]<LogicalType column_t> {
        static_assert(column_t == LogicalType::kEmbedding || column_t == LogicalType::kMultiVector);
        auto CallT = [&]<EmbeddingDataType embedding_t> {
            return BuildIVFIndexT<column_t, embedding_t>(base_rowid, row_count, data_accessor, column_def);
        };
        switch (static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get())->Type()) {
            case EmbeddingDataType::kElemUInt8: {
                return CallT.template operator()<EmbeddingDataType::kElemUInt8>();
            }
            case EmbeddingDataType::kElemInt8: {
                return CallT.template operator()<EmbeddingDataType::kElemInt8>();
            }
            case EmbeddingDataType::kElemDouble: {
                return CallT.template operator()<EmbeddingDataType::kElemDouble>();
            }
            case EmbeddingDataType::kElemFloat: {
                return CallT.template operator()<EmbeddingDataType::kElemFloat>();
            }
            case EmbeddingDataType::kElemFloat16: {
                return CallT.template operator()<EmbeddingDataType::kElemFloat16>();
            }
            case EmbeddingDataType::kElemBFloat16: {
                return CallT.template operator()<EmbeddingDataType::kElemBFloat16>();
            }
            default: {
                UnrecoverableError(std::format("Invalid embedding data type {} for IVF index", column_def->type()->ToString()));
            }
        }
    };
    switch (column_def->type()->type()) {
        case LogicalType::kEmbedding: {
            return Call.template operator()<LogicalType::kEmbedding>();
        }
        case LogicalType::kMultiVector: {
            return Call.template operator()<LogicalType::kMultiVector>();
        }
        default: {
            UnrecoverableError(std::format("Invalid column data type {} for IVF index", column_def->type()->ToString()));
        }
    }
}

template <LogicalType column_t, EmbeddingDataType embedding_t>
void IVFIndexInChunk::BuildIVFIndexT(const RowID base_rowid,
                                     const u32 row_count,
                                     IVFDataAccessorBase *data_accessor,
                                     const SharedPtr<ColumnDef> &column_def) {
    if (row_count <= 0) [[unlikely]] {
        UnrecoverableError("Empty input row count");
    }
    using EmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_t>;
    const SegmentOffset start_segment_offset = base_rowid.segment_offset_;
    u64 embedding_count = 0;
    Deque<Pair<u32, u32>> all_embedding_pos;
    if constexpr (column_t == LogicalType::kEmbedding) {
        embedding_count = row_count;
    } else {
        static_assert(column_t == LogicalType::kMultiVector);
        // read the segment to get total embedding count
        for (u32 i = 0; i < row_count; ++i) {
            auto [raw_data, embedding_num] = data_accessor->GetMultiVector(i);
            embedding_count += embedding_num;
            for (u32 j = 0; j < embedding_num; ++j) {
                all_embedding_pos.emplace_back(i, j);
            }
        }
    }
    // prepare centroid count
    u64 centroid_count = std::min<u64>(std::sqrt(embedding_count) * ivf_option().centroid_option_.centroids_num_ratio_,
                                       embedding_count / ivf_option().centroid_option_.min_points_per_centroid_);
    if (centroid_count == 0) {
        LOG_WARN(std::format("{}: centroid_count is 0. Now change it to 1.", __func__));
        centroid_count = 1;
    }
    // prepare training data
    if (centroid_count > std::numeric_limits<u32>::max()) {
        UnrecoverableError(std::format("{}: centroid_count exceeds u32 limit!", __func__));
    }
    const auto training_embedding_num = std::min<u32>(centroid_count * ivf_option().centroid_option_.min_points_per_centroid_, embedding_count);
    const auto training_data = MakeUniqueForOverwrite<f32[]>(training_embedding_num * embedding_dimension());
    if constexpr (column_t == LogicalType::kEmbedding) {
        Vector<SegmentOffset> all_pos(row_count);
        std::iota(all_pos.begin(), all_pos.end(), start_segment_offset);
        Vector<SegmentOffset> sample_result;
        sample_result.reserve(training_embedding_num);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::ranges::sample(all_pos, std::back_inserter(sample_result), training_embedding_num, gen);
        std::sort(sample_result.begin(), sample_result.end());
        assert(sample_result.size() == training_embedding_num);
        for (u64 i = 0; i < training_embedding_num; ++i) {
            const auto sample_offset = sample_result[i];
            const auto *raw_data = reinterpret_cast<const EmbeddingElementT *>(data_accessor->GetEmbedding(sample_offset));
            if constexpr (std::is_same_v<EmbeddingElementT, f32>) {
                std::copy_n(raw_data, embedding_dimension(), training_data.get() + i * embedding_dimension());
            } else {
                const auto *src_ptr = raw_data;
                auto *target_ptr = training_data.get() + i * embedding_dimension();
                for (u32 j = 0; j < embedding_dimension(); ++j) {
                    if constexpr (std::is_same_v<EmbeddingElementT, f64>) {
                        target_ptr[j] = static_cast<f32>(src_ptr[j]);
                    } else {
                        target_ptr[j] = src_ptr[j];
                    }
                }
            }
        }
    } else if constexpr (column_t == LogicalType::kMultiVector) {
        Vector<Pair<u32, u32>> sample_result;
        sample_result.reserve(training_embedding_num);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::ranges::sample(all_embedding_pos, std::back_inserter(sample_result), training_embedding_num, gen);
        if (sample_result.size() != training_embedding_num) {
            UnrecoverableError(std::format("{}: sample failed to get {} samples.", __func__, training_embedding_num));
        }
        std::sort(sample_result.begin(), sample_result.end());
        assert(sample_result.size() == training_embedding_num);
        for (u64 i = 0; i < training_embedding_num; ++i) {
            const auto [sample_row, sample_id] = sample_result[i];
            auto [raw_data, _] = data_accessor->GetMultiVector(sample_row);
            if constexpr (std::is_same_v<EmbeddingElementT, f32>) {
                std::copy_n(reinterpret_cast<const f32 *>(raw_data.data()) + sample_id * embedding_dimension(),
                            embedding_dimension(),
                            training_data.get() + i * embedding_dimension());
            } else {
                const auto *src_ptr = reinterpret_cast<const EmbeddingElementT *>(raw_data.data()) + sample_id * embedding_dimension();
                auto *target_ptr = training_data.get() + i * embedding_dimension();
                for (u32 j = 0; j < embedding_dimension(); ++j) {
                    if constexpr (std::is_same_v<EmbeddingElementT, f64>) {
                        target_ptr[j] = static_cast<f32>(src_ptr[j]);
                    } else {
                        target_ptr[j] = src_ptr[j];
                    }
                }
            }
        }
    } else {
        static_assert(false);
    }
    // build centroids
    Train(training_embedding_num, training_data.get(), centroid_count);
    // add data
    {
        [[maybe_unused]] BlockID block_id = start_segment_offset / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = start_segment_offset % DEFAULT_BLOCK_CAPACITY;
        u32 segment_row_to_read = row_count;
        SegmentOffset current_segment_offset = start_segment_offset;
        while (segment_row_to_read) {
            const auto block_row_to_read = std::min<u32>(segment_row_to_read, DEFAULT_BLOCK_CAPACITY - block_offset);
            if constexpr (column_t == LogicalType::kEmbedding) {
                for (u32 i = 0; i < block_row_to_read; ++i) {
                    const auto *raw_data = reinterpret_cast<const EmbeddingElementT *>(data_accessor->GetEmbedding(current_segment_offset + i));
                    AddEmbedding(current_segment_offset + i, raw_data);
                }
                // AddEmbeddingBatch(current_segment_offset, column_vector.data(), block_row_to_read);
            } else if constexpr (column_t == LogicalType::kMultiVector) {
                for (u32 i = 0; i < block_row_to_read; ++i) {
                    auto [raw_data, embedding_num] = data_accessor->GetMultiVector(current_segment_offset + i);
                    AddMultiVector(current_segment_offset + i, raw_data.data(), embedding_num);
                }
            } else {
                static_assert(false);
            }
            segment_row_to_read -= block_row_to_read;
            current_segment_offset += block_row_to_read;
            ++block_id;
            block_offset = 0;
        }
    }
}

void IVFIndexInChunk::SaveIndexInner(LocalFileHandle &file_handle) const { IVF_Index_Storage::Save(file_handle); }

void IVFIndexInChunk::ReadIndexInner(LocalFileHandle &file_handle) { IVF_Index_Storage::Load(file_handle); }

IVFIndexInChunk *IVFIndexInChunk::GetNewIVFIndexInChunk(const IndexBase *ivf_index, const ColumnDef *column_def) {
    const auto *ivf_index_ptr = static_cast<const IndexIVF *>(ivf_index);
    const auto *data_type = column_def->type().get();
    const auto logical_type = data_type->type();
    if (logical_type != LogicalType::kEmbedding && logical_type != LogicalType::kMultiVector) {
        UnrecoverableError("Invalid DataType for IVF index");
    }
    const auto *embedding_info_ptr = static_cast<const EmbeddingInfo *>(data_type->type_info().get());
    return new IVFIndexInChunk(ivf_index_ptr->ivf_option_, logical_type, embedding_info_ptr->Type(), embedding_info_ptr->Dimension());
}

} // namespace infinity
