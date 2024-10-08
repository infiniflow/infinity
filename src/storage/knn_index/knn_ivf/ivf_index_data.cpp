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
module ivf_index_data;

import stl;
import index_ivf;
import ivf_index_storage;
import column_def;
import index_base;
import embedding_info;
import internal_types;
import segment_entry;
import buffer_manager;
import infinity_exception;
import status;
import block_entry;
import data_type;
import default_values;
import column_vector;
import logger;
import kmeans_partition;
import logical_type;

namespace infinity {

void IVFIndexInChunk::BuildIVFIndex(const RowID base_rowid,
                                    const u32 row_count,
                                    const SegmentEntry *segment_entry,
                                    const SharedPtr<ColumnDef> &column_def,
                                    BufferManager *buffer_mgr) {
    if (segment_entry->segment_id() != base_rowid.segment_id_) {
        UnrecoverableError(std::format("{}: segment_id mismatch: segment_entry_id: {}, row_id_segment_id: {}.",
                                       __func__,
                                       segment_entry->segment_id(),
                                       base_rowid.segment_id_));
    }
    auto Call = [&]<LogicalType column_t> {
        static_assert(column_t == LogicalType::kEmbedding || column_t == LogicalType::kMultiVector);
        auto CallT = [&]<EmbeddingDataType embedding_t> {
            return BuildIVFIndexT<column_t, embedding_t>(base_rowid, row_count, segment_entry, column_def, buffer_mgr);
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
                                     const SegmentEntry *segment_entry,
                                     const SharedPtr<ColumnDef> &column_def,
                                     BufferManager *buffer_mgr) {
    if (row_count <= 0) [[unlikely]] {
        UnrecoverableError("Empty input row count");
    }
    using EmbeddingElementT = EmbeddingDataTypeToCppTypeT<embedding_t>;
    Vector<SharedPtr<BlockEntry>> block_entries;
    {
        const BlocksGuard blocks_guard = segment_entry->GetBlocksGuard();
        block_entries = blocks_guard.block_entries_;
    }
    const SegmentOffset start_segment_offset = base_rowid.segment_offset_;
    const ColumnID column_id = column_def->id();
    u64 embedding_count = 0;
    Deque<Pair<u32, u32>> all_embedding_pos;
    if constexpr (column_t == LogicalType::kEmbedding) {
        embedding_count = row_count;
    } else {
        static_assert(column_t == LogicalType::kMultiVector);
        // read the segment to get total embedding count
        BlockID block_id = std::numeric_limits<BlockID>::max();
        BlockOffset block_offset = 0;
        UniquePtr<ColumnVector> column_vector;
        const MultiVectorT *mv_ptr = nullptr;
        auto update_cv_mv_ptr = [&] {
            column_vector = MakeUnique<ColumnVector>(block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr));
            mv_ptr = reinterpret_cast<const MultiVectorT *>(column_vector->data());
        };
        for (u32 i = 0; i < row_count; ++i) {
            {
                const SegmentOffset new_segment_offset = start_segment_offset + i;
                block_offset = new_segment_offset % DEFAULT_BLOCK_CAPACITY;
                if (const BlockID new_block_id = new_segment_offset / DEFAULT_BLOCK_CAPACITY; new_block_id != block_id) {
                    block_id = new_block_id;
                    update_cv_mv_ptr();
                }
            }
            const auto embedding_num = mv_ptr[block_offset].embedding_num_;
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
            const BlockID block_id = sample_offset / DEFAULT_BLOCK_CAPACITY;
            const BlockOffset block_offset = sample_offset % DEFAULT_BLOCK_CAPACITY;
            auto column_vector = block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr);
            const EmbeddingElementT *raw_data = reinterpret_cast<const EmbeddingElementT *>(column_vector.data());
            if constexpr (std::is_same_v<EmbeddingElementT, f32>) {
                std::copy_n(raw_data + block_offset * embedding_dimension(), embedding_dimension(), training_data.get() + i * embedding_dimension());
            } else {
                const auto *src_ptr = raw_data + block_offset * embedding_dimension();
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
            const SegmentOffset new_segment_offset = start_segment_offset + sample_row;
            const BlockID block_id = new_segment_offset / DEFAULT_BLOCK_CAPACITY;
            const BlockOffset block_offset = new_segment_offset % DEFAULT_BLOCK_CAPACITY;
            auto column_vector = block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr);
            auto [raw_data, _] = column_vector.GetMultiVectorRaw(block_offset);
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
        BlockID block_id = start_segment_offset / DEFAULT_BLOCK_CAPACITY;
        BlockOffset block_offset = start_segment_offset % DEFAULT_BLOCK_CAPACITY;
        u32 segment_row_to_read = row_count;
        SegmentOffset current_segment_offset = start_segment_offset;
        while (segment_row_to_read) {
            const auto block_row_to_read = std::min<u32>(segment_row_to_read, DEFAULT_BLOCK_CAPACITY - block_offset);
            ColumnVector column_vector(block_entries[block_id]->GetColumnBlockEntry(column_id)->GetConstColumnVector(buffer_mgr));
            if constexpr (column_t == LogicalType::kEmbedding) {
                AddEmbeddingBatch(current_segment_offset, column_vector.data(), block_row_to_read);
            } else if constexpr (column_t == LogicalType::kMultiVector) {
                for (u32 i = 0; i < block_row_to_read; ++i) {
                    auto [raw_data, embedding_num] = column_vector.GetMultiVectorRaw(block_offset + i);
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
