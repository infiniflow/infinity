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
#include <sstream>
#include <vector>

module segment_index_entry;

import stl;
import buffer_manager;
import buffer_handle;
import buffer_obj;
import logger;
import third_party;
import infinity_exception;
import logical_type;
import index_file_worker;
import status;
import index_base;
import index_hnsw;
import hnsw_common;
import hnsw_alg;
import catalog_delta_entry;
import column_vector;
import secondary_index_data;
import type_info;
import embedding_info;
import create_index_info;
import column_def;
import block_column_entry;
import default_values;
import segment_iter;
import hnsw_file_worker;
import secondary_index_file_worker;
import bmp_index_file_worker;
import sparse_util;
import index_full_text;
import index_defines;
import column_inverter;
import block_entry;
import chunk_index_entry;
import abstract_hnsw;
import abstract_bmp;
import block_column_iter;
import txn_store;
import secondary_index_in_mem;
import ivf_index_data_in_mem;
import ivf_index_data;
import emvb_index;
import emvb_index_in_mem;
import bmp_util;
import hnsw_util;
import wal_entry;
import infinity_context;
import defer_op;

namespace infinity {

Vector<std::string_view> SegmentIndexEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid segment index entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = TableIndexEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String SegmentIndexEntry::EncodeIndex(const SegmentID segment_id, const TableIndexEntry *table_index_entry) {
    if (table_index_entry == nullptr) {
        return ""; // unit test
    }
    return fmt::format("{}#{}", table_index_entry->encode(), segment_id);
}

SegmentIndexEntry::SegmentIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id)
    : BaseEntry(EntryType::kSegmentIndex, false, SegmentIndexEntry::EncodeIndex(segment_id, table_index_entry)),
      table_index_entry_(table_index_entry), segment_id_(segment_id) {
    if (table_index_entry != nullptr)
        index_dir_ = table_index_entry->index_dir();
};

SegmentIndexEntry::SegmentIndexEntry(const SegmentIndexEntry &other)
    : BaseEntry(other), table_index_entry_(other.table_index_entry_), index_dir_(other.index_dir_), segment_id_(other.segment_id_) {
    min_ts_ = other.min_ts_;
    max_ts_ = other.max_ts_;
    checkpoint_ts_ = other.checkpoint_ts_;
    next_chunk_id_ = other.next_chunk_id_.load();

    memory_hnsw_index_ = other.memory_hnsw_index_;
    memory_ivf_index_ = other.memory_ivf_index_;
    memory_indexer_ = other.memory_indexer_;
    memory_secondary_index_ = other.memory_secondary_index_;
    memory_emvb_index_ = other.memory_emvb_index_;
    memory_bmp_index_ = other.memory_bmp_index_;
    ft_column_len_sum_ = other.ft_column_len_sum_;
    ft_column_len_cnt_ = other.ft_column_len_cnt_;
}

SegmentIndexEntry::~SegmentIndexEntry() = default;

UniquePtr<SegmentIndexEntry> SegmentIndexEntry::Clone(TableIndexEntry *table_index_entry) const {
    auto ret = UniquePtr<SegmentIndexEntry>(new SegmentIndexEntry(*this));
    std::shared_lock lock(rw_locker_);
    for (const auto &chunk_index_entry : chunk_index_entries_) {
        ret->chunk_index_entries_.emplace_back(chunk_index_entry->Clone(ret.get()));
    }
    return ret;
}

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::CreateFakeEntry(const String &index_dir) {
    SharedPtr<SegmentIndexEntry> fake_entry;
    fake_entry.reset(new SegmentIndexEntry(static_cast<TableIndexEntry *>(nullptr), SegmentID(0)));
    fake_entry->index_dir_ = MakeShared<String>(index_dir);
    return fake_entry;
}

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::NewIndexEntry(TableIndexEntry *table_index_entry, SegmentID segment_id, Txn *txn) {
    auto segment_index_entry = SharedPtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id));
    auto begin_ts = txn->BeginTS();
    segment_index_entry->min_ts_ = begin_ts;
    segment_index_entry->max_ts_ = begin_ts;
    segment_index_entry->begin_ts_ = begin_ts;

    return segment_index_entry;
}

SharedPtr<SegmentIndexEntry> SegmentIndexEntry::NewReplaySegmentIndexEntry(TableIndexEntry *table_index_entry,
                                                                           TableEntry *table_entry,
                                                                           SegmentID segment_id,
                                                                           TxnTimeStamp min_ts,
                                                                           TxnTimeStamp max_ts,
                                                                           u32 next_chunk_id,
                                                                           TransactionID txn_id,
                                                                           TxnTimeStamp begin_ts,
                                                                           TxnTimeStamp commit_ts) {
    auto [segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    String column_name = table_index_entry->index_base()->column_name();
    auto segment_index_entry = SharedPtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id));
    if (segment_index_entry.get() == nullptr) {
        String error_message = "Failed to load index entry";
        UnrecoverableError(error_message);
    }
    segment_index_entry->min_ts_ = min_ts;
    segment_index_entry->max_ts_ = max_ts;
    segment_index_entry->next_chunk_id_ = next_chunk_id;

    segment_index_entry->commit_ts_.store(commit_ts);
    return segment_index_entry;
}

void SegmentIndexEntry::UpdateSegmentIndexReplay(SharedPtr<SegmentIndexEntry> new_entry) {
    assert(new_entry->segment_id_ == segment_id_);
    assert(new_entry->table_index_entry_ == table_index_entry_);
    assert(new_entry->index_dir_ == index_dir_);
    min_ts_ = new_entry->min_ts_;
    max_ts_ = new_entry->max_ts_;
    next_chunk_id_ = new_entry->next_chunk_id_.load();
}

// String SegmentIndexEntry::IndexFileName(SegmentID segment_id) { return fmt::format("seg{}.idx", segment_id); }

void SegmentIndexEntry::MemIndexInsert(SharedPtr<BlockEntry> block_entry,
                                       u32 row_offset,
                                       u32 row_count,
                                       TxnTimeStamp commit_ts,
                                       BufferManager *buffer_manager) {
    SegmentOffset block_offset = block_entry->segment_offset();
    RowID begin_row_id = block_entry->base_row_id() + row_offset;

    const SharedPtr<IndexBase> &index_base = table_index_entry_->table_index_def();
    const SharedPtr<ColumnDef> &column_def = table_index_entry_->column_def();
    ColumnID column_id = column_def->id();

    auto *table_entry = table_index_entry_->table_index_meta()->GetTableEntry();
    SizeT column_idx = table_entry->GetColumnIdxByID(column_id);

    std::lock_guard lck_m(mem_index_locker_);
    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base.get());
            if (memory_indexer_.get() == nullptr) {
                String base_name = fmt::format("ft_{:016x}", begin_row_id.ToUint64());
                {
                    std::unique_lock<std::shared_mutex> lck(rw_locker_);
                    String full_path = Path(InfinityContext::instance().config()->DataDir()) / *table_index_entry_->index_dir();
                    memory_indexer_ = MakeUnique<MemoryIndexer>(full_path, base_name, begin_row_id, index_fulltext->flag_, index_fulltext->analyzer_);
                }
                table_index_entry_->UpdateFulltextSegmentTs(commit_ts);
            } else {
                RowID exp_begin_row_id = memory_indexer_->GetBaseRowId() + memory_indexer_->GetDocCount();
                assert(begin_row_id >= exp_begin_row_id);
                if (begin_row_id > exp_begin_row_id) {
                    LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                         begin_row_id.ToUint64(),
                                         exp_begin_row_id.ToUint64(),
                                         begin_row_id - exp_begin_row_id));
                    memory_indexer_->InsertGap(begin_row_id - exp_begin_row_id);
                }
            }
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
            SharedPtr<ColumnVector> column_vector = MakeShared<ColumnVector>(block_column_entry->GetConstColumnVector(buffer_manager));
            memory_indexer_->Insert(column_vector, row_offset, row_count, false);
            break;
        }
        case IndexType::kHnsw: {
            if (memory_hnsw_index_.get() == nullptr) {
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_hnsw_index_ = HnswIndexInMem::Make(begin_row_id, index_base.get(), column_def.get(), this, true /*trace*/);
            }
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
            memory_hnsw_index_->InsertVecs(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
            break;
        }
        case IndexType::kSecondary: {
            if (memory_secondary_index_.get() == nullptr) {
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, begin_row_id);
            }
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
            memory_secondary_index_->InsertBlockData(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
            break;
        }
        case IndexType::kIVF: {
            if (!memory_ivf_index_) {
                std::unique_lock lck(rw_locker_);
                memory_ivf_index_ = IVFIndexInMem::NewIVFIndexInMem(column_def.get(), index_base.get(), begin_row_id);
            }
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
            memory_ivf_index_->InsertBlockData(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
            break;
        }
        case IndexType::kEMVB: {
            if (memory_emvb_index_.get() == nullptr) {
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_emvb_index_ = EMVBIndexInMem::NewEMVBIndexInMem(index_base, column_def, begin_row_id);
            }
            auto block_id = block_entry->block_id();
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
            memory_emvb_index_->Insert(block_id, block_column_entry, buffer_manager, row_offset, row_count);
            break;
        }
        case IndexType::kBMP: {
            if (memory_bmp_index_.get() == nullptr) {
                std::unique_lock<std::shared_mutex> lck(rw_locker_);
                memory_bmp_index_ = MakeShared<BMPIndexInMem>(begin_row_id, index_base.get(), column_def.get());
            }
            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
            memory_bmp_index_->AddDocs(block_offset, block_column_entry, buffer_manager, row_offset, row_count);
            break;
        }
        case IndexType::kDiskAnn: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("{} realtime index is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_WARN(*err_msg);
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            UnrecoverableError(*err_msg);
        }
    }
    assert(commit_ts >= min_ts_);
    max_ts_ = commit_ts;
}

void SegmentIndexEntry::MemIndexCommit() {
    const IndexBase *index_base = table_index_entry_->index_base();
    if (index_base->index_type_ != IndexType::kFullText || memory_indexer_.get() == nullptr)
        return;
    memory_indexer_->Commit();
}
void SegmentIndexEntry::MemIndexWaitInflightTasks() {
    const IndexBase *index_base = table_index_entry_->index_base();
    if (index_base->index_type_ != IndexType::kFullText || memory_indexer_.get() == nullptr)
        return;
    memory_indexer_->WaitInflightTasks();
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::MemIndexDump(bool spill, SizeT *dump_size) {
    SharedPtr<ChunkIndexEntry> chunk_index_entry = nullptr;
    const IndexBase *index_base = table_index_entry_->index_base();
    BufferManager *buffer_manager = infinity::InfinityContext::instance().storage()->buffer_manager();
    std::lock_guard lck_m(mem_index_locker_);
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            if (memory_hnsw_index_.get() == nullptr) {
                break;
            }
            chunk_index_entry = memory_hnsw_index_->Dump(this, buffer_manager, dump_size);
            chunk_index_entry->SaveIndexFile();
            std::unique_lock lck2(rw_locker_);
            chunk_index_entries_.push_back(chunk_index_entry);
            memory_hnsw_index_.reset();
            break;
        }
        case IndexType::kFullText: {
            if (memory_indexer_.get() == nullptr) {
                break;
            }
            memory_indexer_->Dump(false, spill);
            if (!spill) {
                chunk_index_entry =
                    AddFtChunkIndexEntry(memory_indexer_->GetBaseName(), memory_indexer_->GetBaseRowId(), memory_indexer_->GetDocCount());
                this->UpdateFulltextColumnLenInfo(memory_indexer_->GetColumnLengthSum(), memory_indexer_->GetDocCount());
                memory_indexer_.reset();
            }
            break;
        }
        case IndexType::kSecondary: {
            if (memory_secondary_index_.get() == nullptr) {
                break;
            }
            chunk_index_entry = memory_secondary_index_->Dump(this, buffer_manager);
            chunk_index_entry->SaveIndexFile();
            std::unique_lock lck(rw_locker_);
            chunk_index_entries_.push_back(chunk_index_entry);
            memory_secondary_index_.reset();
            break;
        }
        case IndexType::kIVF: {
            if (memory_ivf_index_.get() == nullptr) {
                break;
            }
            chunk_index_entry = memory_ivf_index_->Dump(this, buffer_manager);
            chunk_index_entry->SaveIndexFile();
            std::unique_lock lck(rw_locker_);
            chunk_index_entries_.push_back(chunk_index_entry);
            memory_ivf_index_.reset();
            break;
        }
        case IndexType::kEMVB: {
            if (memory_emvb_index_.get() == nullptr) {
                return nullptr;
            }
            std::unique_lock lck(rw_locker_);
            chunk_index_entry = memory_emvb_index_->Dump(this, buffer_manager);
            chunk_index_entry->SaveIndexFile();
            chunk_index_entries_.push_back(chunk_index_entry);
            memory_emvb_index_.reset();
            break;
        }
        case IndexType::kBMP: {
            if (memory_bmp_index_.get() == nullptr) {
                return nullptr;
            }
            chunk_index_entry = memory_bmp_index_->Dump(this, buffer_manager);
            chunk_index_entry->SaveIndexFile();
            std::unique_lock lck(rw_locker_);
            chunk_index_entries_.push_back(chunk_index_entry);
            memory_bmp_index_.reset();
            break;
        }
        default: {
            UnrecoverableError("Not implemented.");
            break;
        }
    }
    return chunk_index_entry;
}

void SegmentIndexEntry::AddWalIndexDump(ChunkIndexEntry *dumped_index_entry, Txn *txn, Vector<ChunkID> deprecate_chunk_ids) {
    Vector<WalChunkIndexInfo> chunk_infos;
    chunk_infos.emplace_back(dumped_index_entry);

    TableEntry *table_entry = table_index_entry_->table_index_meta()->GetTableEntry();
    const auto &db_name = *table_entry->GetDBName();
    const auto &table_name = *table_entry->GetTableName();
    const auto &index_name = *table_index_entry_->GetIndexName();
    txn->AddWalCmd(MakeShared<WalCmdDumpIndex>(db_name, table_name, index_name, segment_id_, std::move(chunk_infos), std::move(deprecate_chunk_ids)));
}

// void SegmentIndexEntry::MemIndexLoad(const String &base_name, RowID base_row_id) {
//     const IndexBase *index_base = table_index_entry_->index_base();
//     if (index_base->index_type_ != IndexType::kFullText)
//         return;
//     // Init the mem index from previously spilled one.
//     assert(memory_indexer_.get() == nullptr);
//     const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
//     String full_path = Path(InfinityContext::instance().config()->DataDir()) / *table_index_entry_->index_dir();
//     memory_indexer_ = MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_);
//     memory_indexer_->Load();
// }

u32 SegmentIndexEntry::MemIndexRowCount() {
    const IndexBase *index_base = table_index_entry_->index_base();
    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            return memory_indexer_.get() ? memory_indexer_->GetDocCount() : 0;
        }
        case IndexType::kHnsw: {
            return memory_hnsw_index_.get() ? memory_hnsw_index_->GetRowCount() : 0;
        }
        case IndexType::kSecondary: {
            return memory_secondary_index_.get() ? memory_secondary_index_->GetRowCount() : 0;
        }
        case IndexType::kIVF: {
            return memory_ivf_index_.get() ? memory_ivf_index_->GetInputRowCount() : 0;
        }
        case IndexType::kEMVB: {
            return memory_emvb_index_.get() ? memory_emvb_index_->GetRowCount() : 0;
        }
        case IndexType::kBMP: {
            return memory_bmp_index_.get() ? memory_bmp_index_->GetRowCount() : 0;
        }
        default: {
            return 0;
        }
    }
}

void SegmentIndexEntry::PopulateEntirely(const SegmentEntry *segment_entry, Txn *txn, const PopulateEntireConfig &config) {
    TxnTimeStamp begin_ts = txn->BeginTS();
    auto *buffer_mgr = txn->buffer_mgr();
    const IndexBase *index_base = table_index_entry_->index_base();
    SharedPtr<ColumnDef> column_def = table_index_entry_->column_def();

    SharedPtr<ChunkIndexEntry> dumped_memindex_entry = nullptr;

    u32 seg_id = segment_entry->segment_id();
    RowID base_row_id(seg_id, 0);
    auto *table_entry = table_index_entry_->table_index_meta()->GetTableEntry();
    BufferManager *buffer_manager = infinity::InfinityContext::instance().storage()->buffer_manager();

    switch (index_base->index_type_) {
        case IndexType::kFullText: {
            const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
            String base_name = fmt::format("ft_{:016x}", base_row_id.ToUint64());
            String full_path = Path(InfinityContext::instance().config()->DataDir()) / *table_index_entry_->index_dir();
            memory_indexer_ = MakeUnique<MemoryIndexer>(full_path, base_name, base_row_id, index_fulltext->flag_, index_fulltext->analyzer_);
            u64 column_id = column_def->id();
            SizeT column_idx = table_entry->GetColumnIdxByID(column_id);
            auto block_entry_iter = BlockEntryIter(segment_entry);
            for (const auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
                RowID begin_row_id = block_entry->base_row_id();
                RowID exp_begin_row_id = memory_indexer_->GetBaseRowId() + memory_indexer_->GetDocCount();
                assert(begin_row_id >= exp_begin_row_id);
                if (begin_row_id > exp_begin_row_id) {
                    LOG_WARN(fmt::format("Begin row id: {}, expect begin row id: {}, insert gap: {}",
                                         begin_row_id.ToUint64(),
                                         exp_begin_row_id.ToUint64(),
                                         begin_row_id - exp_begin_row_id));
                    memory_indexer_->InsertGap(begin_row_id - exp_begin_row_id);
                }

                SharedPtr<ColumnVector> column_vector = MakeShared<ColumnVector>(block_column_entry->GetConstColumnVector(buffer_mgr));
                memory_indexer_->Insert(column_vector, 0, block_entry->row_count(), true);
                memory_indexer_->Commit(true);
            }
            memory_indexer_->Dump(true);
            dumped_memindex_entry =
                AddFtChunkIndexEntry(memory_indexer_->GetBaseName(), memory_indexer_->GetBaseRowId(), memory_indexer_->GetDocCount());
            this->UpdateFulltextColumnLenInfo(memory_indexer_->GetColumnLengthSum(), memory_indexer_->GetDocCount());
            memory_indexer_.reset();
            break;
        }
        case IndexType::kHnsw: {
            auto memory_hnsw_index = HnswIndexInMem::Make(base_row_id, index_base, column_def.get(), this);
            HnswInsertConfig insert_config;
            insert_config.optimize_ = true;
            memory_hnsw_index->InsertVecs(segment_entry, buffer_mgr, column_def->id(), begin_ts, config.check_ts_, insert_config);

            dumped_memindex_entry = memory_hnsw_index->Dump(this, buffer_manager);
            dumped_memindex_entry->SaveIndexFile();
            {
                std::unique_lock lck(rw_locker_);
                chunk_index_entries_.push_back(dumped_memindex_entry);
            }
            break;
        }
        case IndexType::kSecondary: {
            memory_secondary_index_ = SecondaryIndexInMem::NewSecondaryIndexInMem(column_def, base_row_id);
            u64 column_id = column_def->id();
            SizeT column_idx = table_entry->GetColumnIdxByID(column_id);
            auto block_entry_iter = BlockEntryIter(segment_entry);
            for (const auto *block_entry = block_entry_iter.Next(); block_entry != nullptr; block_entry = block_entry_iter.Next()) {
                const auto block_offset = block_entry->segment_offset();
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(column_idx);
                memory_secondary_index_->InsertBlockData(block_offset, block_column_entry, buffer_mgr, 0, block_entry->row_count());
            }
            dumped_memindex_entry = MemIndexDump();
            break;
        }
        case IndexType::kIVF: {
            const u32 row_count = segment_entry->row_count();
            SharedPtr<ChunkIndexEntry> ivf_chunk_index_entry = CreateIVFIndexChunkIndexEntry(base_row_id, row_count, buffer_mgr);
            this->AddChunkIndexEntry(ivf_chunk_index_entry);
            BufferHandle handle = ivf_chunk_index_entry->GetIndex();
            auto data_ptr = static_cast<IVFIndexInChunk *>(handle.GetDataMut());
            data_ptr->BuildIVFIndex(base_row_id, row_count, segment_entry, column_def, buffer_mgr);
            ivf_chunk_index_entry->SaveIndexFile();
            dumped_memindex_entry = std::move(ivf_chunk_index_entry);
            break;
        }
        case IndexType::kEMVB: {
            const u32 row_count = segment_entry->row_count();
            SharedPtr<ChunkIndexEntry> emvb_chunk_index_entry = CreateEMVBIndexChunkIndexEntry(base_row_id, row_count, buffer_mgr);
            this->AddChunkIndexEntry(emvb_chunk_index_entry);
            BufferHandle handle = emvb_chunk_index_entry->GetIndex();
            auto data_ptr = static_cast<EMVBIndex *>(handle.GetDataMut());
            data_ptr->BuildEMVBIndex(base_row_id, row_count, segment_entry, column_def, buffer_mgr);

            emvb_chunk_index_entry->SaveIndexFile();
            dumped_memindex_entry = std::move(emvb_chunk_index_entry);
            break;
        }
        case IndexType::kBMP: {
            memory_bmp_index_ = MakeShared<BMPIndexInMem>(base_row_id, index_base, column_def.get());

            memory_bmp_index_->AddDocs(segment_entry, buffer_mgr, column_def->id(), begin_ts, config.check_ts_);
            dumped_memindex_entry = MemIndexDump();
            break;
        }
        case IndexType::kDiskAnn: { // TODO
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("{} PopulateEntirely is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
            LOG_WARN(*err_msg);
            break;
        }
        default: {
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
            UnrecoverableError(*err_msg);
        }
    }
    // IMPORT invokes this func at upper half at which the txn hasn't been commited yet.
    TxnTimeStamp ts = std::max(txn->BeginTS(), txn->CommitTS());
    assert(ts >= min_ts_);
    max_ts_ = ts;

    if (dumped_memindex_entry.get() != nullptr) {
        AddWalIndexDump(dumped_memindex_entry.get(), txn);
    }
}

Status SegmentIndexEntry::CreateIndexPrepare(const SegmentEntry *segment_entry, Txn *txn, bool prepare, bool check_ts) {
    const IndexBase *index_base = table_index_entry_->index_base();
    PopulateEntireConfig populate_entire_config{.prepare_ = prepare, .check_ts_ = check_ts};
    switch (index_base->index_type_) {
        case IndexType::kIVF:
        case IndexType::kHnsw:
        case IndexType::kFullText:
        case IndexType::kSecondary:
        case IndexType::kEMVB:
        case IndexType::kBMP:
        case IndexType::kDiskAnn: {
            PopulateEntirely(segment_entry, txn, populate_entire_config);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Invalid index type: {}", IndexInfo::IndexTypeToString(index_base->index_type_)));
        }
    }
    return Status::OK();
}

Status SegmentIndexEntry::CreateIndexDo(atomic_u64 &create_index_idx) {
    // const IndexBase *index_base = table_index_entry_->index_base();
    // switch (index_base->index_type_) {
    //     case IndexType::kHnsw: {
    //         Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
    //         GetChunkIndexEntries(chunk_index_entries);
    //         for (const auto &chunk_index_entry : chunk_index_entries) {
    //             RowID base_rowid = chunk_index_entry->base_rowid_;
    //             SegmentOffset offset = base_rowid.segment_offset_;
    //             SegmentOffset row_count = chunk_index_entry->row_count_;
    //             BufferHandle buffer_handle = chunk_index_entry->GetIndex();

    //             auto *abstract_hnsw = reinterpret_cast<AbstractHnsw *>(buffer_handle.GetDataMut());
    //             std::visit(
    //                 [&](auto &&index) {
    //                     using T = std::decay_t<decltype(index)>;
    //                     if constexpr (std::is_same_v<T, std::nullptr_t>) {
    //                         UnrecoverableError("Invalid index type.");
    //                     } else {
    //                         while (true) {
    //                             SizeT idx = create_index_idx.fetch_add(1);
    //                             if (idx >= row_count) {
    //                                 break;
    //                             }
    //                             index->Build(offset + idx);
    //                         }
    //                     }
    //                 },
    //                 *abstract_hnsw);
    //         }
    //         break;
    //     }
    //     default: {
    //         break;
    //     }
    // }
    return Status::OK();
}

void SegmentIndexEntry::GetChunkIndexEntries(Vector<SharedPtr<ChunkIndexEntry>> &chunk_index_entries,
                                             SharedPtr<MemoryIndexer> &memory_indexer,
                                             Txn *txn) {
    std::shared_lock lock(rw_locker_);
    chunk_index_entries.clear();
    SizeT num = chunk_index_entries_.size();
    for (SizeT i = 0; i < num; i++) {
        auto &chunk_index_entry = chunk_index_entries_[i];
        bool add = chunk_index_entry->CheckVisible(txn);
        LOG_INFO(fmt::format("GetChunkIndexEntries, CheckVisible ret: {}, chunk_id: {}, deprecate ts: {}",
                             add,
                             chunk_index_entry->chunk_id_,
                             chunk_index_entry->deprecate_ts_.load()));
        if (add) {
            chunk_index_entries.push_back(chunk_index_entry);
        }
    }
    std::sort(std::begin(chunk_index_entries),
              std::end(chunk_index_entries),
              [](const SharedPtr<ChunkIndexEntry> &lhs, const SharedPtr<ChunkIndexEntry> &rhs) noexcept {
                  return (lhs->base_rowid_ < rhs->base_rowid_ || (lhs->base_rowid_ == rhs->base_rowid_ && lhs->row_count_ < rhs->row_count_));
              });
    memory_indexer = memory_indexer_;
}

void SegmentIndexEntry::RemoveChunkIndexEntry(ChunkIndexEntry *chunk_index_entry) {
    RowID base_rowid = chunk_index_entry->base_rowid_;
    std::unique_lock lock(rw_locker_);
    chunk_index_entries_.erase(std::remove_if(chunk_index_entries_.begin(),
                                              chunk_index_entries_.end(),
                                              [base_rowid](const SharedPtr<ChunkIndexEntry> &entry) { return entry->base_rowid_ == base_rowid; }),
                               chunk_index_entries_.end());
}

void SegmentIndexEntry::CommitSegmentIndex(TransactionID txn_id, TxnTimeStamp commit_ts) {
    std::unique_lock lock(rw_locker_);

    max_ts_ = commit_ts;
    if (!this->Committed()) {
        txn_id_ = txn_id;
        checkpoint_ts_ = commit_ts;
        this->Commit(commit_ts);

        for (auto &chunk_index_entry : chunk_index_entries_) {
            chunk_index_entry->Commit(commit_ts);
        }
    }
}

void SegmentIndexEntry::CommitOptimize(ChunkIndexEntry *new_chunk, const Vector<ChunkIndexEntry *> &old_chunks, TxnTimeStamp commit_ts) {
    std::unique_lock lock(rw_locker_);

    max_ts_ = commit_ts;
    new_chunk->Commit(commit_ts);
    for (auto *old_chunk : old_chunks) {
        old_chunk->DeprecateChunk(commit_ts);
    }

    std::stringstream ss;
    ss << "COMMIT OPTIMIZE: ";
    ss << "Segment ID: " << segment_id_ << ". ";
    ss << "New index chunk: " << new_chunk->chunk_id_ << ". ";
    ss << "Old chunks: ";
    for (auto *old_chunk : old_chunks) {
        ss << old_chunk->chunk_id_ << ", ";
    }
    LOG_INFO(ss.str());

    ResetOptimizing();
}

void SegmentIndexEntry::OptIndex(IndexBase *index_base,
                                 TxnTableStore *txn_table_store,
                                 const Vector<UniquePtr<InitParameter>> &opt_params,
                                 bool replay) {
    SharedPtr<ChunkIndexEntry> dumped_memindex_entry = nullptr;
    switch (table_index_entry_->index_base()->index_type_) {
        case IndexType::kBMP: {
            Optional<BMPOptimizeOptions> ret = BMPUtil::ParseBMPOptimizeOptions(opt_params);
            if (!ret) {
                break;
            }
            const auto &options = ret.value();
            const auto [chunk_index_entries, memory_index_entry] = this->GetBMPIndexSnapshot();

            auto optimize_index = [&](const AbstractBMP &index) {
                std::visit(
                    [&](auto &&index) {
                        using T = std::decay_t<decltype(index)>;
                        if constexpr (std::is_same_v<T, std::nullptr_t>) {
                            UnrecoverableError("Invalid index type.");
                        } else {
                            index->Optimize(options);
                        }
                    },
                    index);
            };
            for (const auto &chunk_index_entry : chunk_index_entries) {
                BufferHandle buffer_handle = chunk_index_entry->GetIndex();
                auto *abstract_bmp = static_cast<AbstractBMP *>(buffer_handle.GetDataMut());
                optimize_index(*abstract_bmp);
                chunk_index_entry->SaveIndexFile();
            }
            if (memory_index_entry.get() != nullptr) {
                optimize_index(memory_index_entry->get());

                dumped_memindex_entry = this->MemIndexDump(false /*spill*/);
            }
            break;
        }
        case IndexType::kHnsw: {
            if (replay) {
                break;
            }
            auto params = HnswUtil::ParseOptimizeOptions(opt_params);
            if (!params) {
                break;
            }
            auto optimize_index = [&](AbstractHnsw *abstract_hnsw) {
                std::visit(
                    [&](auto &&index) {
                        using T = std::decay_t<decltype(index)>;
                        if constexpr (std::is_same_v<T, std::nullptr_t>) {
                            UnrecoverableError("Invalid index type.");
                        } else {
                            using HnswIndexDataType = typename std::remove_pointer_t<T>::DataType;
                            if (params->compress_to_lvq) {
                                if constexpr (IsAnyOf<HnswIndexDataType, i8, u8>) {
                                    UnrecoverableError("Invalid index type.");
                                } else {
                                    auto *p = std::move(*index).CompressToLVQ().release();
                                    delete index;
                                    *abstract_hnsw = p;
                                }
                            }
                            if (params->lvq_avg) {
                                index->Optimize();
                            }
                        }
                    },
                    *abstract_hnsw);
            };

            const auto [chunk_index_entries, memory_index_entry] = this->GetHnswIndexSnapshot();
            for (const auto &chunk_index_entry : chunk_index_entries) {
                BufferHandle buffer_handle = chunk_index_entry->GetIndex();
                auto *abstract_hnsw = reinterpret_cast<AbstractHnsw *>(buffer_handle.GetDataMut());
                optimize_index(abstract_hnsw);
                chunk_index_entry->SaveIndexFile();
            }
            if (memory_index_entry.get() != nullptr) {
                optimize_index(memory_index_entry->get_ptr());
                dumped_memindex_entry = this->MemIndexDump(false /*spill*/);
            }
            break;
        }
        default: {
            LOG_WARN("Not implemented");
        }
    }
    if (dumped_memindex_entry.get() != nullptr) {
        auto *txn = txn_table_store->GetTxn();
        AddWalIndexDump(dumped_memindex_entry.get(), txn);
        txn_table_store->AddChunkIndexStore(table_index_entry_, dumped_memindex_entry.get());
    }
}

bool SegmentIndexEntry::Flush(TxnTimeStamp checkpoint_ts) {
    auto index_type = table_index_entry_->index_base()->index_type_;
    if (index_type == IndexType::kFullText || index_type == IndexType::kHnsw) {
        // Fulltext index, Hnsw doesn't need to be checkpointed.
        return false;
    }

    String &index_name = *table_index_entry_->index_dir();
    u64 segment_id = this->segment_id_;
    if (this->max_ts_ <= this->checkpoint_ts_ || this->min_ts_ > checkpoint_ts) {
        LOG_TRACE(fmt::format("Segment: {}, Index: {} has been flushed at some previous checkpoint, or is not visible at current checkpoint.",
                              segment_id,
                              index_name));
        return false;
    }

    this->checkpoint_ts_ = checkpoint_ts;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} checkpoint is change to {}", segment_id, index_name, this->checkpoint_ts_));
    return true;
}

void SegmentIndexEntry::CommitIndex(TxnTimeStamp commit_ts) {
    this->Commit(commit_ts);
    for (auto &chunk_index_entry : chunk_index_entries_) {
        chunk_index_entry->Commit(commit_ts);
    }
}

void SegmentIndexEntry::Cleanup(CleanupInfoTracer *info_tracer, bool dropped) {
    for (auto &chunk_index_entry : chunk_index_entries_) {
        chunk_index_entry->Cleanup(info_tracer, dropped);
    }
}

Vector<String> SegmentIndexEntry::GetFilePath(TransactionID txn_id, TxnTimeStamp begin_ts) const {
    std::shared_lock lock(rw_locker_);
    Vector<String> res;
    res.reserve(chunk_index_entries_.size());
    for (const auto &chunk_index_entry : chunk_index_entries_) {
        Vector<String> chunk_files = chunk_index_entry->GetFilePath(txn_id, begin_ts);
        res.insert(res.end(), chunk_files.begin(), chunk_files.end());
    }
    return res;
}

void SegmentIndexEntry::PickCleanup(CleanupScanner *scanner) {
    TxnTimeStamp visible_ts = scanner->visible_ts();
    std::unique_lock w_lock(rw_locker_);
    for (auto iter = chunk_index_entries_.begin(); iter != chunk_index_entries_.end();) {
        auto &chunk_index_entry = *iter;
        if (chunk_index_entry->CheckDeprecate(visible_ts)) {
            scanner->AddEntry(std::move(chunk_index_entry));
            iter = chunk_index_entries_.erase(iter);
        } else {
            ++iter;
        }
    }
}

void SegmentIndexEntry::ReplaceChunkIndexEntries(TxnTableStore *txn_table_store,
                                                 SharedPtr<ChunkIndexEntry> merged_chunk_index_entry,
                                                 Vector<ChunkIndexEntry *> old_chunks) {
    TxnIndexStore *txn_index_store = txn_table_store->GetIndexStore(table_index_entry_);
    chunk_index_entries_.push_back(merged_chunk_index_entry);
    txn_index_store->optimize_data_.emplace_back(this, merged_chunk_index_entry.get(), std::move(old_chunks));
}

ChunkIndexEntry *SegmentIndexEntry::RebuildChunkIndexEntries(TxnTableStore *txn_table_store, SegmentEntry *segment_entry) {
    const auto &index_name = *table_index_entry_->GetIndexName();
    if (!TrySetOptimizing()) {
        LOG_INFO(fmt::format("Index {} segment {} is optimizing, skip optimize.", index_name, segment_id_));
        return nullptr;
    }
    bool opt_success = false;
    DeferFn defer_fn([&] {
        if (!opt_success) {
            LOG_WARN(fmt::format("Index {} segment {} optimize fail or skip.", index_name, segment_id_));
            ResetOptimizing();
        }
    });

    Txn *txn = txn_table_store->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();
    const IndexBase *index_base = table_index_entry_->index_base();
    SharedPtr<ColumnDef> column_def = table_index_entry_->column_def();

    BufferManager *buffer_mgr = txn->buffer_mgr();
    Vector<ChunkIndexEntry *> old_chunks;
    Vector<ChunkID> old_ids;
    u32 row_count = 0;
    {
        std::shared_lock lock(rw_locker_);
        for (const auto &chunk_index_entry : chunk_index_entries_) {
            if (chunk_index_entry->CheckVisible(txn)) {
                row_count += chunk_index_entry->row_count_;
                old_chunks.push_back(chunk_index_entry.get());
                old_ids.push_back(chunk_index_entry->chunk_id_);
            }
        }
        if (old_chunks.size() <= 1) { // TODO
            return nullptr;
        }
    }
    RowID base_rowid(segment_id_, 0);
    SharedPtr<ChunkIndexEntry> merged_chunk_index_entry = nullptr;
    switch (index_base->index_type_) {
        case IndexType::kHnsw: {
            auto memory_hnsw_index = HnswIndexInMem::Make(base_rowid, index_base, column_def.get(), this);
            const AbstractHnsw &abstract_hnsw = memory_hnsw_index->get();

            std::visit(
                [&](auto &index) {
                    using T = std::decay_t<decltype(index)>;
                    if constexpr (std::is_same_v<T, std::nullptr_t>) {
                        return;
                    } else {
                        using IndexT = std::decay_t<decltype(*index)>;
                        using DataType = typename IndexT::DataType;
                        CappedOneColumnIterator<DataType, true /*check ts*/> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts, row_count);
                        HnswInsertConfig insert_config;
                        insert_config.optimize_ = true;
                        index->InsertVecs(std::move(iter), insert_config);
                    }
                },
                abstract_hnsw);
            merged_chunk_index_entry = memory_hnsw_index->Dump(this, buffer_mgr);
            break;
        }
        case IndexType::kBMP: {
            auto memory_bmp_index = MakeShared<BMPIndexInMem>(base_rowid, index_base, column_def.get());
            AbstractBMP abstract_bmp = memory_bmp_index->get();

            std::visit(
                [&](auto &index) {
                    using T = std::decay_t<decltype(index)>;
                    if constexpr (std::is_same_v<T, std::nullptr_t>) {
                        UnrecoverableError("Invalid index type.");
                    } else {
                        using IndexT = std::decay_t<decltype(*index)>;
                        using SparseRefT = SparseVecRef<typename IndexT::DataT, typename IndexT::IdxT>;

                        CappedOneColumnIterator<SparseRefT, true /*check_ts*/> iter(segment_entry, buffer_mgr, column_def->id(), begin_ts, row_count);
                        index->AddDocs(std::move(iter));
                    }
                },
                abstract_bmp);
            merged_chunk_index_entry = memory_bmp_index->Dump(this, buffer_mgr);
            break;
        }
        case IndexType::kSecondary: {
            merged_chunk_index_entry = CreateSecondaryIndexChunkIndexEntry(base_rowid, row_count, buffer_mgr);
            BufferHandle handle = merged_chunk_index_entry->GetIndex();
            auto data_ptr = static_cast<SecondaryIndexData *>(handle.GetDataMut());
            data_ptr->InsertMergeData(old_chunks, merged_chunk_index_entry);
            break;
        }
        case IndexType::kEMVB: {
            // TODO: merge
            merged_chunk_index_entry = CreateEMVBIndexChunkIndexEntry(base_rowid, row_count, buffer_mgr);
            BufferHandle handle = merged_chunk_index_entry->GetIndex();
            auto data_ptr = static_cast<EMVBIndex *>(handle.GetDataMut());
            data_ptr->BuildEMVBIndex(base_rowid, row_count, segment_entry, column_def, buffer_mgr);
            break;
        }
        case IndexType::kIVF: {
            // rebuild
            merged_chunk_index_entry = CreateIVFIndexChunkIndexEntry(base_rowid, row_count, buffer_mgr);
            BufferHandle handle = merged_chunk_index_entry->GetIndex();
            auto data_ptr = static_cast<IVFIndexInChunk *>(handle.GetDataMut());
            data_ptr->BuildIVFIndex(base_rowid, row_count, segment_entry, column_def, buffer_mgr);
            break;
        }
        default: {
            String error_message = "RebuildChunkIndexEntries is not supported for this index type.";
            UnrecoverableError(error_message);
            return nullptr;
        }
    }
    ReplaceChunkIndexEntries(txn_table_store, merged_chunk_index_entry, std::move(old_chunks));
    opt_success = true; // set success after record in txn store

    merged_chunk_index_entry->SaveIndexFile();
    AddWalIndexDump(merged_chunk_index_entry.get(), txn, std::move(old_ids));
    return merged_chunk_index_entry.get();
}

BaseMemIndex *SegmentIndexEntry::GetMemIndex() const {
    // only support hnsw index now.
    return static_cast<BaseMemIndex *>(memory_hnsw_index_.get());
}

void SegmentIndexEntry::SaveIndexFile() {
    String &index_name = *table_index_entry_->index_dir();
    u64 segment_id = this->segment_id_;
    LOG_TRACE(fmt::format("Segment: {}, Index: {} is being flushing", segment_id, index_name));
    for (auto &chunk_index_entry : chunk_index_entries_) {
        chunk_index_entry->SaveIndexFile();
    }
}

SharedPtr<ChunkIndexEntry>
SegmentIndexEntry::CreateHnswIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr, SizeT index_size) {
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewHnswIndexChunkIndexEntry(chunk_id, this, "", base_rowid, row_count, buffer_mgr, index_size);
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::CreateSecondaryIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr) {
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewSecondaryIndexChunkIndexEntry(chunk_id, this, "", base_rowid, row_count, buffer_mgr);
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::CreateIVFIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr) {
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewIVFIndexChunkIndexEntry(chunk_id, this, "", base_rowid, row_count, buffer_mgr);
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::CreateEMVBIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr) {
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewEMVBIndexChunkIndexEntry(chunk_id, this, "", base_rowid, row_count, buffer_mgr);
}

SharedPtr<ChunkIndexEntry>
SegmentIndexEntry::CreateBMPIndexChunkIndexEntry(RowID base_rowid, u32 row_count, BufferManager *buffer_mgr, SizeT index_size) {
    ChunkID chunk_id = this->GetNextChunkID();
    return ChunkIndexEntry::NewBMPIndexChunkIndexEntry(chunk_id, this, "", base_rowid, row_count, buffer_mgr, index_size);
}

void SegmentIndexEntry::AddChunkIndexEntry(SharedPtr<ChunkIndexEntry> chunk_index_entry) {
    std::shared_lock lock(rw_locker_);
    chunk_index_entries_.push_back(chunk_index_entry);
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::AddFtChunkIndexEntry(const String &base_name, RowID base_rowid, u32 row_count) {
    std::shared_lock lock(rw_locker_);
    ChunkID chunk_id = this->GetNextChunkID();
    // row range of chunk_index_entries_ may overlop and misorder due to deprecated ones.
    BufferManager *buffer_manager = infinity::InfinityContext::instance().storage()->buffer_manager();
    SharedPtr<ChunkIndexEntry> chunk_index_entry =
        ChunkIndexEntry::NewFtChunkIndexEntry(this, chunk_id, base_name, base_rowid, row_count, buffer_manager);
    chunk_index_entries_.push_back(chunk_index_entry);
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::AddChunkIndexEntryReplayWal(ChunkID chunk_id,
                                                                          TableEntry *table_entry,
                                                                          const String &base_name,
                                                                          RowID base_rowid,
                                                                          u32 row_count,
                                                                          TxnTimeStamp commit_ts,
                                                                          TxnTimeStamp deprecate_ts,
                                                                          BufferManager *buffer_mgr) {
    LOG_INFO(fmt::format("AddChunkIndexEntryReplayWal chunk_id: {} deprecate_ts: {}, base_rowid: {}, row_count: {} to to segment: {}",
                         chunk_id,

                         deprecate_ts,
                         base_rowid.ToUint64(),
                         row_count,
                         segment_id_));
    SharedPtr<ChunkIndexEntry> chunk_index_entry =
        ChunkIndexEntry::NewReplayChunkIndexEntry(chunk_id, this, base_name, base_rowid, row_count, commit_ts, deprecate_ts, buffer_mgr);
    assert(
        std::is_sorted(chunk_index_entries_.begin(),
                       chunk_index_entries_.end(),
                       [](const SharedPtr<ChunkIndexEntry> &lhs, const SharedPtr<ChunkIndexEntry> &rhs) { return lhs->chunk_id_ < rhs->chunk_id_; }));
    auto iter = std::lower_bound(chunk_index_entries_.begin(),
                                 chunk_index_entries_.end(),
                                 chunk_id,
                                 [&](const SharedPtr<ChunkIndexEntry> &entry, ChunkID id) { return entry->chunk_id_ < id; });
    if (iter != chunk_index_entries_.end() && (*iter)->chunk_id_ == chunk_id) {
        UnrecoverableError(fmt::format("Chunk ID: {} already exists in segment: {}", chunk_id, segment_id_));
    }
    chunk_index_entries_.insert(iter, chunk_index_entry);
    ChunkID old_next_chunk_id = next_chunk_id_;
    next_chunk_id_ = std::max(next_chunk_id_.load(), chunk_index_entries_.back()->chunk_id_ + 1);
    LOG_INFO(
        fmt::format("AddChunkIndexEntryReplayWal, old_next_chunk_id: {}, next_chunk_id_, chunk_id: {}", old_next_chunk_id, next_chunk_id_, chunk_id));
    if (table_index_entry_->table_index_def()->index_type_ == IndexType::kFullText) {
        try {
            u64 column_length_sum = chunk_index_entry->GetColumnLengthSum();
            UpdateFulltextColumnLenInfo(column_length_sum, row_count);
        } catch (const UnrecoverableException &e) {
            String msg(e.what());
            if (!msg.find("No such file or directory")) {
                throw e;
            }
            LOG_WARN("Fulltext index file not found, skip update column length info");
        }
    };
    return chunk_index_entry;
}

SharedPtr<ChunkIndexEntry> SegmentIndexEntry::AddChunkIndexEntryReplay(ChunkID chunk_id,
                                                                       TableEntry *table_entry,
                                                                       const String &base_name,
                                                                       RowID base_rowid,
                                                                       u32 row_count,
                                                                       TxnTimeStamp commit_ts,
                                                                       TxnTimeStamp deprecate_ts,
                                                                       BufferManager *buffer_mgr) {
    if (chunk_id >= next_chunk_id_) {
        UnrecoverableError(fmt::format("Chunk ID: {} is greater than next chunk ID: {}", chunk_id, next_chunk_id_));
    }
    LOG_INFO(fmt::format("AddChunkIndexEntryReplay chunk_id: {} deprecate_ts: {}, base_rowid: {}, row_count: {} to to segment: {}",
                         chunk_id,

                         deprecate_ts,
                         base_rowid.ToUint64(),
                         row_count,
                         segment_id_));
    SharedPtr<ChunkIndexEntry> chunk_index_entry =
        ChunkIndexEntry::NewReplayChunkIndexEntry(chunk_id, this, base_name, base_rowid, row_count, commit_ts, deprecate_ts, buffer_mgr);
    assert(
        std::is_sorted(chunk_index_entries_.begin(),
                       chunk_index_entries_.end(),
                       [](const SharedPtr<ChunkIndexEntry> &lhs, const SharedPtr<ChunkIndexEntry> &rhs) { return lhs->chunk_id_ < rhs->chunk_id_; }));
    auto iter = std::lower_bound(chunk_index_entries_.begin(),
                                 chunk_index_entries_.end(),
                                 chunk_id,
                                 [&](const SharedPtr<ChunkIndexEntry> &entry, ChunkID id) { return entry->chunk_id_ < id; });
    if (iter != chunk_index_entries_.end() && (*iter)->chunk_id_ == chunk_id) {
        *iter = chunk_index_entry;
    } else {
        chunk_index_entries_.insert(iter, chunk_index_entry);
    }
    if (table_index_entry_->table_index_def()->index_type_ == IndexType::kFullText) {
        try {
            u64 column_length_sum = chunk_index_entry->GetColumnLengthSum();
            UpdateFulltextColumnLenInfo(column_length_sum, row_count);
        } catch (const UnrecoverableException &e) {
            String msg(e.what());
            if (!msg.find("No such file or directory")) {
                throw e;
            }
            LOG_WARN("Fulltext index file not found, skip update column length info");
        }
    };
    return chunk_index_entry;
}

ChunkIndexEntry *SegmentIndexEntry::GetChunkIndexEntry(ChunkID chunk_id) {
    std::shared_lock lock(rw_locker_);
    for (auto &chunk_index_entry : chunk_index_entries_) {
        if (chunk_index_entry->chunk_id_ == chunk_id) {
            return chunk_index_entry.get();
        }
    }
    return nullptr;
}

nlohmann::json SegmentIndexEntry::Serialize(TxnTimeStamp max_commit_ts) {
    if (this->deleted_) {
        String error_message = "Segment Column index entry can't be deleted.";
        UnrecoverableError(error_message);
    }

    nlohmann::json index_entry_json;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        index_entry_json["segment_id"] = this->segment_id_;
        index_entry_json["commit_ts"] = this->commit_ts_.load();
        index_entry_json["min_ts"] = this->min_ts_;
        index_entry_json["max_ts"] = this->max_ts_;
        index_entry_json["next_chunk_id"] = this->next_chunk_id_.load();
        index_entry_json["checkpoint_ts"] = this->checkpoint_ts_; // TODO shenyushi:: use fields in BaseEntry

        for (auto &chunk_index_entry : chunk_index_entries_) {
            if (chunk_index_entry->commit_ts_ <= max_commit_ts) {
                index_entry_json["chunk_index_entries"].push_back(chunk_index_entry->Serialize());
            }
        }
        index_entry_json["ft_column_len_sum"] = this->ft_column_len_sum_;
        index_entry_json["ft_column_len_cnt"] = this->ft_column_len_cnt_;
    }

    return index_entry_json;
}

UniquePtr<SegmentIndexEntry>
SegmentIndexEntry::Deserialize(const nlohmann::json &index_entry_json, TableIndexEntry *table_index_entry, TableEntry *table_entry) {
    SegmentID segment_id = index_entry_json["segment_id"];
    auto [segment_row_count, status] = table_entry->GetSegmentRowCountBySegmentID(segment_id);

    if (!status.ok()) {
        UnrecoverableError(status.message());
        return nullptr;
    }
    const IndexBase *index_base = table_index_entry->index_base();
    String column_name = index_base->column_name();
    auto segment_index_entry = UniquePtr<SegmentIndexEntry>(new SegmentIndexEntry(table_index_entry, segment_id));
    segment_index_entry->min_ts_ = index_entry_json["min_ts"];
    segment_index_entry->max_ts_ = index_entry_json["max_ts"];
    segment_index_entry->commit_ts_.store(index_entry_json["commit_ts"]);
    segment_index_entry->next_chunk_id_ = index_entry_json["next_chunk_id"];
    segment_index_entry->checkpoint_ts_ = index_entry_json["checkpoint_ts"]; // TODO shenyushi:: use fields in BaseEntry
    if (index_entry_json.contains("chunk_index_entries")) {
        for (const auto &chunk_index_entry_json : index_entry_json["chunk_index_entries"]) {
            auto chunk_index_entry = ChunkIndexEntry::Deserialize(chunk_index_entry_json, segment_index_entry.get());
            segment_index_entry->chunk_index_entries_.push_back(std::move(chunk_index_entry));
        }
    }

    segment_index_entry->ft_column_len_sum_ = index_entry_json["ft_column_len_sum"];
    segment_index_entry->ft_column_len_cnt_ = index_entry_json["ft_column_len_cnt"];

    return segment_index_entry;
}

bool SegmentIndexEntry::TrySetOptimizing() {
    bool expected = false;
    return optimizing_.compare_exchange_strong(expected, true);
}

void SegmentIndexEntry::ResetOptimizing() {
    bool expected = true;
    optimizing_.compare_exchange_strong(expected, false);
}

} // namespace infinity
