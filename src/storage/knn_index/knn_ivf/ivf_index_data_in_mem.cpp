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

#include <boost/asio/detail/mutex.hpp>
#include <cassert>
#include <mutex>
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
import multivector_util;
import block_column_iter;
import data_type;
import infinity_exception;
import status;
import logger;

namespace infinity {

u32 IVFIndexInMem::GetInputRowCount() const {
    std::shared_lock lock(rw_mutex_);
    return input_row_count_;
}

template <LogicalType column_logical_type, typename ColumnEmbeddingElementT>
class IVFIndexInMemT final : public IVFIndexInMem {
    const u32 source_embedding_dimension_ = 0;
    Vector<ColumnEmbeddingElementT> raw_source_data_{};
    Vector<SegmentOffset> source_offsets_{};

public:
    IVFIndexInMemT(const RowID begin_row_id, const IndexIVFOption &ivf_option, const u32 source_embedding_dimension)
        : IVFIndexInMem(begin_row_id, ivf_option), source_embedding_dimension_(source_embedding_dimension) {}

    void InsertBlockData(const SegmentOffset block_offset,
                         BlockColumnEntry *block_column_entry,
                         BufferManager *buffer_manager,
                         const u32 row_offset,
                         const u32 row_count) override {
        if constexpr (column_logical_type == LogicalType::kEmbedding) {
            MemIndexInserterIter<ColumnEmbeddingElementT> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
            InsertVecs(iter);
        } else if constexpr (column_logical_type == LogicalType::kMultiVector) {
            MemIndexInserterIter<MultiVectorRef<ColumnEmbeddingElementT>> iter(block_offset,
                                                                               block_column_entry,
                                                                               buffer_manager,
                                                                               row_offset,
                                                                               row_count);
            InsertVecs(iter);
        } else {
            static_assert(false, "Wrong logical type!");
        }
        {
            std::unique_lock lock(rw_mutex_);
            input_row_count_ += row_count;
        }
    }

    void InsertVecs(auto &iter) {
        if (have_ivf_index_.test()) {
            InsertVecsToIndex(iter);
        } else {
            InsertVecsToRawData(iter);
        }
    }

    void InsertVecsToIndex(auto &iter) {
        for (auto ret = iter.Next(); ret; ret = iter.Next()) {
            auto &[embedding_raw_data_ptr, segment_offset] = *ret;
            static_assert(std::is_same_v<ColumnEmbeddingElementT, std::decay_t<std::remove_pointer_t<decltype(embedding_raw_data_ptr)>>>);
            std::unique_lock lock(rw_mutex_);
            assert(source_offsets_.empty());
            assert(raw_source_data_.empty());
            // TODO
        }
    }

    void InsertVecsToRawData(auto &iter) {
        for (auto ret = iter.Next(); ret; ret = iter.Next()) {
            auto &[embedding_raw_data_ptr, segment_offset] = *ret;
            static_assert(std::is_same_v<ColumnEmbeddingElementT, std::decay_t<std::remove_pointer_t<decltype(embedding_raw_data_ptr)>>>);
            std::unique_lock lock(rw_mutex_);
            assert(source_offsets_.size() == input_embedding_count_);
            assert(raw_source_data_.size() == input_embedding_count_ * source_embedding_dimension_);
            source_offsets_.push_back(segment_offset);
            raw_source_data_.insert(raw_source_data_.end(), embedding_raw_data_ptr, embedding_raw_data_ptr + source_embedding_dimension_);
            ++input_embedding_count_;
        }
    }

    SharedPtr<ChunkIndexEntry> Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) const override {
        // TODO
        return {};
    }
};

template <LogicalType column_logical_type>
SharedPtr<IVFIndexInMem> GetNewIVFIndexInMem(const DataType *column_data_type, const RowID begin_row_id, const IndexIVFOption &index_ivf_option) {
    const auto *embedding_info_ptr = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get());
    auto GetResult = [&]<typename ColumnEmbeddingElementT> {
        return MakeShared<IVFIndexInMemT<column_logical_type, ColumnEmbeddingElementT>>(begin_row_id,
                                                                                        index_ivf_option,
                                                                                        embedding_info_ptr->Dimension());
    };
    switch (embedding_info_ptr->Type()) {
        case EmbeddingDataType::kElemInt8: {
            return GetResult.template operator()<i8>();
        }
        case EmbeddingDataType::kElemUInt8: {
            return GetResult.template operator()<u8>();
        }
        case EmbeddingDataType::kElemFloat: {
            return GetResult.template operator()<f32>();
        }
        case EmbeddingDataType::kElemFloat16: {
            return GetResult.template operator()<Float16T>();
        }
        case EmbeddingDataType::kElemBFloat16: {
            return GetResult.template operator()<BFloat16T>();
        }
        case EmbeddingDataType::kElemDouble: {
            return GetResult.template operator()<f64>();
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