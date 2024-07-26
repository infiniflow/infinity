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

module abstract_bmp;

import segment_index_entry;
import chunk_index_entry;
import buffer_manager;
import buffer_handle;
import block_column_iter;
import sparse_util;
import segment_iter;
import segment_entry;

namespace infinity {

BMPIndexInMem::BMPIndexInMem(RowID begin_row_id, const IndexBase *index_base, const ColumnDef *column_def)
    : begin_row_id_(begin_row_id), bmp_(InitAbstractIndex(index_base, column_def)) {
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
    const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());
    SizeT term_num = sparse_info->Dimension();
    SizeT block_size = index_bmp->block_size_;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                bmp_ = new IndexT(term_num, block_size);
            }
        },
        bmp_);
}

AbstractBMP BMPIndexInMem::InitAbstractIndex(const IndexBase *index_base, const ColumnDef *column_def) {
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
    const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());

    switch (sparse_info->DataType()) {
        case EmbeddingDataType::kElemFloat: {
            return InitAbstractIndex<f32>(index_bmp, sparse_info);
        }
        case EmbeddingDataType::kElemDouble: {
            return InitAbstractIndex<f64>(index_bmp, sparse_info);
        }
        default: {
            return nullptr;
        }
    }
}

BMPIndexInMem::~BMPIndexInMem() {
    std::visit(
        [](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                if (index != nullptr) {
                    delete index;
                }
            }
        },
        bmp_);
}

SizeT BMPIndexInMem::GetRowCount() const {
    return std::visit(
        [](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return SizeT(0);
            } else {
                return index->DocNum();
            }
        },
        bmp_);
}

void BMPIndexInMem::AddDocs(SizeT block_offset, BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr, SizeT row_offset, SizeT row_count) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;

                MemIndexInserterIter<SparseRefT> iter(block_offset, block_column_entry, buffer_mgr, row_offset, row_count);
                index->AddDocs(std::move(iter));
            }
        },
        bmp_);
}

void BMPIndexInMem::AddDocs(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return;
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;

                if (check_ts) {
                    OneColumnIterator<SparseRefT> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                    index->AddDocs(std::move(iter));
                } else {
                    OneColumnIterator<SparseRefT, false> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                    index->AddDocs(std::move(iter));
                }
            }
        },
        bmp_);
}

SharedPtr<ChunkIndexEntry> BMPIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr) {
    SizeT row_count = 0;
    SizeT index_size = 0;
    std::visit(
        [&](auto &&index) {
            using IndexType = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<IndexType, std::nullptr_t>) {
                return;
            } else {
                row_count = index->DocNum();
                index_size = index->GetSizeInBytes();
            }
        },
        bmp_);
    auto new_chunk_index_entry = segment_index_entry->CreateBMPIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);

    BufferHandle handle = new_chunk_index_entry->GetIndex();
    auto *data_ptr = static_cast<AbstractBMP *>(handle.GetDataMut());
    *data_ptr = bmp_;
    bmp_ = nullptr;
    return new_chunk_index_entry;
}

} // namespace infinity