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

module abstract_hnsw;

import segment_index_entry;
import chunk_index_entry;
import buffer_manager;
import buffer_handle;
import block_column_iter;
import segment_iter;
import segment_entry;

namespace infinity {

AbstractHnsw HnswIndexInMem::InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def) {
    const auto *index_hnsw = static_cast<const IndexHnsw *>(index_base);
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(column_def->type()->type_info().get());

    switch (embedding_info->Type()) {
        case EmbeddingDataType::kElemFloat: {
            return InitAbstractIndex<float>(index_hnsw, embedding_info);
        }
        default: {
            return nullptr;
        }
    }
}

HnswIndexInMem::~HnswIndexInMem() {
    std::visit(
        [](auto &&arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                if (arg != nullptr) {
                    delete arg;
                }
            }
        },
        hnsw_);
}

SizeT HnswIndexInMem::GetRowCount() const {
    return std::visit(
        [](auto &&index) {
            using IndexType = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                return SizeT(0);
            } else {
                return index->GetVecNum();
            }
        },
        hnsw_);
}

void HnswIndexInMem::InsertVecs(SizeT block_offset,
                                BlockColumnEntry *block_column_entry,
                                BufferManager *buffer_manager,
                                SizeT row_offset,
                                SizeT row_count,
                                const HnswInsertConfig &config) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                using DataType = typename IndexT::DataType;
                MemIndexInserterIter<DataType> iter(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
                InsertVecs(index, std::move(iter), config);
            }
        },
        hnsw_);
}

void HnswIndexInMem::InsertVecs(const SegmentEntry *segment_entry,
                                BufferManager *buffer_mgr,
                                SizeT column_id,
                                TxnTimeStamp begin_ts,
                                bool check_ts,
                                const HnswInsertConfig &config) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                using DataType = typename IndexT::DataType;
                if (check_ts) {
                    OneColumnIterator<DataType> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                    InsertVecs(index, std::move(iter), config);
                } else {
                    OneColumnIterator<DataType, false> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                    InsertVecs(index, std::move(iter), config);
                }
            }
        },
        hnsw_);
}

SharedPtr<ChunkIndexEntry> HnswIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) {
    SizeT row_count = 0;
    SizeT index_size = 0;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                row_count = index->GetVecNum();
                index_size = index->GetSizeInBytes();
            }
        },
        hnsw_);
    auto new_chunk_indey_entry = segment_index_entry->CreateHnswIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);

    BufferHandle handle = new_chunk_indey_entry->GetIndex();
    auto *data_ptr = static_cast<AbstractHnsw *>(handle.GetDataMut());
    *data_ptr = hnsw_;
    hnsw_ = nullptr;
    return new_chunk_indey_entry;
}

} // namespace infinity