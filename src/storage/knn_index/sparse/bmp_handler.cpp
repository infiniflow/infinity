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

module bmp_handler;

import segment_index_entry;
import chunk_index_entry;
import buffer_manager;
import buffer_handle;
import block_column_iter;
import sparse_util;
import segment_iter;
import segment_entry;
import third_party;
import logger;
import buffer_obj;
import local_file_handle;

namespace infinity {

BMPHandler::~BMPHandler() {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                delete index;
            }
        },
        bmp_);
}

BMPHandler::BMPHandler(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem)
    : bmp_(InitAbstractIndex(index_base, column_def, own_mem)) {
    const auto *index_bmp = static_cast<const IndexBMP *>(index_base);
    const auto *sparse_info = static_cast<SparseInfo *>(column_def->type()->type_info().get());
    if (!own_mem) {
        return;
    }
    SizeT term_num = sparse_info->Dimension();
    SizeT block_size = index_bmp->block_size_;
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    bmp_ = new IndexT(term_num, block_size);
                } else {
                    UnrecoverableError("BMPHandler::BMPHandler: index does not own memory");
                }
            }
        },
        bmp_);
}

UniquePtr<BMPHandler> BMPHandler::Make(const IndexBase *index_base, const ColumnDef *column_def, bool own_mem) {
    return MakeUnique<BMPHandler>(index_base, column_def, own_mem);
}

SizeT BMPHandler::AddDocs(SizeT block_offset, BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr, SizeT row_offset, SizeT row_count) {
    SizeT mem_usage{};
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;
                    SizeT mem_before = index->MemoryUsage();
                    MemIndexInserterIter<SparseRefT> iter(block_offset, block_column_entry, buffer_mgr, row_offset, row_count);
                    index->AddDocs(std::move(iter));
                    SizeT mem_after = index->MemoryUsage();
                    mem_usage = mem_after - mem_before;
                    LOG_INFO(fmt::format("before : {} -> after : {}, add mem_used : {}", mem_before, mem_after, mem_after - mem_before));
                } else {
                    UnrecoverableError("BMPHandler::AddDocs: index does not own memory");
                }
            }
        },
        bmp_);
    return mem_usage;
}

SizeT BMPHandler::AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count) {
    SizeT mem_usage{};
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;
                    SizeT mem_before = index->MemoryUsage();
                    MemIndexInserterIter1<SparseRefT> iter(block_offset, col, offset, row_count);
                    index->AddDocs(std::move(iter));
                    SizeT mem_after = index->MemoryUsage();
                    mem_usage = mem_after - mem_before;
                    LOG_INFO(fmt::format("before : {} -> after : {}, add mem_used : {}", mem_before, mem_after, mem_after - mem_before));
                } else {
                    UnrecoverableError("BMPHandler::AddDocs: index does not own memory");
                }
            }
        },
        bmp_);
    return mem_usage;
}

void BMPHandler::AddDocs(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (!IndexT::kOwnMem) {
                    UnrecoverableError("BMPHandler::AddDocs: index does not own memory");
                } else {
                    using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;

                    if (check_ts) {
                        OneColumnIterator<SparseRefT> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                        index->AddDocs(std::move(iter));
                    } else {
                        OneColumnIterator<SparseRefT, false> iter(segment_entry, buffer_mgr, column_id, begin_ts);
                        index->AddDocs(std::move(iter));
                    }
                }
            }
        },
        bmp_);
}

void BMPHandler::AddDocs(int row_count, const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts) {
    std::visit(
        [&](auto &index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;
                    CappedOneColumnIterator<SparseRefT, true /*check_ts*/> iter(segment_entry, buffer_mgr, column_id, begin_ts, row_count);
                    index->AddDocs(std::move(iter));
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        bmp_);
}

SizeT BMPHandler::MemUsage() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return SizeT(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->MemoryUsage();
                } else {
                    UnrecoverableError("BMPHandler::MemUsage: index does not own memory");
                    return SizeT(0);
                }
            }
        },
        bmp_);
}

SizeT BMPHandler::DocNum() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return SizeT(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->DocNum();
                } else {
                    UnrecoverableError("BMPHandler::DocNum: index does not own memory");
                    return SizeT(0);
                }
            }
        },
        bmp_);
}

SizeT BMPHandler::GetSizeInBytes() const {
    return std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                return SizeT(0);
            } else {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    return index->GetSizeInBytes();
                } else {
                    UnrecoverableError("BMPHandler::GetSizeInBytes: index does not own memory");
                    return SizeT(0);
                }
            }
        },
        bmp_);
}

void BMPHandler::Save(LocalFileHandle &file_handle) const {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->Save(file_handle);
                } else {
                    UnrecoverableError("BMPHandler::Save: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::SaveToPtr(LocalFileHandle &file_handle) const {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index->SaveToPtr(file_handle);
                } else {
                    UnrecoverableError("BMPHandler::SaveToPtr: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::Load(LocalFileHandle &file_handle) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = new IndexT(IndexT::Load(file_handle));
                } else {
                    UnrecoverableError("BMPHandler::Load: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::LoadFromPtr(LocalFileHandle &file_handle, SizeT file_size) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    index = new IndexT(IndexT::LoadFromPtr(file_handle, file_size));
                } else {
                    UnrecoverableError("BMPHandler::LoadFromPtr: index does not own memory");
                }
            }
        },
        bmp_);
}

void BMPHandler::LoadFromPtr(const char *ptr, SizeT file_size) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (!std::is_same_v<T, std::nullptr_t>) {
                using IndexT = std::decay_t<decltype(*index)>;
                if constexpr (IndexT::kOwnMem) {
                    UnrecoverableError("BMPHandler::LoadFromPtr: index own memory");
                } else {
                    index = new IndexT(IndexT::LoadFromPtr(ptr, file_size));
                }
            }
        },
        bmp_);
}

void BMPHandler::Optimize(const BMPOptimizeOptions &options) {
    std::visit(
        [&](auto &&index) {
            using T = std::decay_t<decltype(index)>;
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                UnrecoverableError("Invalid index type.");
            } else {
                using IndexT = typename std::remove_pointer_t<T>;
                if constexpr (IndexT::kOwnMem) {
                    index->Optimize(options);
                } else {
                    UnrecoverableError("Invalid index type.");
                }
            }
        },
        bmp_);
}

BMPIndexInMem::~BMPIndexInMem() {
    if (!own_memory_) {
        return;
    }
    SizeT mem_used = bmp_handler_->MemUsage();
    if (bmp_handler_ != nullptr) {
        delete bmp_handler_;
    }
    DecreaseMemoryUsageBase(mem_used);
}

// realtime insert, trace this
void BMPIndexInMem::AddDocs(SizeT block_offset, BlockColumnEntry *block_column_entry, BufferManager *buffer_mgr, SizeT row_offset, SizeT row_count) {
    SizeT mem_used = bmp_handler_->AddDocs(block_offset, block_column_entry, buffer_mgr, row_offset, row_count);
    IncreaseMemoryUsageBase(mem_used);
}

void BMPIndexInMem::AddDocs(SegmentOffset block_offset, const ColumnVector &col, BlockOffset offset, BlockOffset row_count) {
    SizeT mem_used = bmp_handler_->AddDocs(block_offset, col, offset, row_count);
    IncreaseMemoryUsageBase(mem_used);
}

void BMPIndexInMem::AddDocs(const SegmentEntry *segment_entry, BufferManager *buffer_mgr, SizeT column_id, TxnTimeStamp begin_ts, bool check_ts) {
    bmp_handler_->AddDocs(segment_entry, buffer_mgr, column_id, begin_ts, check_ts);
}

SharedPtr<ChunkIndexEntry> BMPIndexInMem::Dump(SegmentIndexEntry *segment_index_entry, BufferManager *buffer_mgr, SizeT *dump_size) {
    if (!own_memory_) {
        UnrecoverableError("BMPIndexInMem::Dump() called with own_memory_ = false.");
    }
    SizeT row_count = bmp_handler_->DocNum();
    SizeT index_size = bmp_handler_->GetSizeInBytes();
    if (dump_size != nullptr) {
        *dump_size = bmp_handler_->MemUsage();
    }
    auto new_chunk_index_entry = segment_index_entry->CreateBMPIndexChunkIndexEntry(begin_row_id_, row_count, buffer_mgr, index_size);

    BufferHandle handle = new_chunk_index_entry->GetIndex();
    auto *data_ptr = static_cast<BMPHandlerPtr *>(handle.GetDataMut());
    *data_ptr = bmp_handler_;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);
    return new_chunk_index_entry;
}

void BMPIndexInMem::Dump(BufferObj *buffer_obj, SizeT *dump_size_ptr) {
    if (!own_memory_) {
        UnrecoverableError("BMPIndexInMem::Dump() called with own_memory_ = false.");
    }
    if (dump_size_ptr) {
        *dump_size_ptr = bmp_handler_->MemUsage();
    }

    BufferHandle handle = buffer_obj->Load();
    auto *data_ptr = static_cast<BMPHandlerPtr *>(handle.GetDataMut());
    *data_ptr = bmp_handler_;
    own_memory_ = false;
    chunk_handle_ = std::move(handle);
}

SizeT BMPIndexInMem::GetRowCount() const { return bmp_handler_->DocNum(); }

SizeT BMPIndexInMem::GetSizeInBytes() const { return bmp_handler_->GetSizeInBytes(); }

MemIndexTracerInfo BMPIndexInMem::GetInfo() const {
    SizeT mem_used = bmp_handler_->MemUsage();
    SizeT row_cnt = bmp_handler_->DocNum();

    if (segment_index_entry_ == nullptr) {
        return MemIndexTracerInfo(MakeShared<String>(index_name_), MakeShared<String>(table_name_), MakeShared<String>(db_name_), mem_used, row_cnt);
    }

    auto *table_index_entry = segment_index_entry_->table_index_entry();
    SharedPtr<String> index_name = table_index_entry->GetIndexName();
    auto *table_entry = table_index_entry->table_index_meta()->GetTableEntry();
    SharedPtr<String> table_name = table_entry->GetTableName();
    SharedPtr<String> db_name = table_entry->GetDBName();
    return MemIndexTracerInfo(index_name, table_name, db_name, mem_used, row_cnt);
}

TableIndexEntry *BMPIndexInMem::table_index_entry() const { return segment_index_entry_->table_index_entry(); }

} // namespace infinity
