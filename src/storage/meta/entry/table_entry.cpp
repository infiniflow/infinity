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
#include <stdexcept>
#include <string>
#include <vector>

module table_entry;

import stl;

import table_entry_type;
import third_party;
import txn;
import buffer_manager;
import block_index;
import data_access_state;
import internal_types;
import logger;
import txn_store;
import status;
import infinity_exception;
import index_ivfflat;
import txn_manager;
import index_base;
import index_full_text;
import catalog_delta_entry;
import extra_ddl_info;
import create_index_info;
import column_def;
import data_type;
import default_values;
import DBT_compaction_alg;
import compact_statement;
import local_file_system;
import build_fast_rough_filter_task;
import block_entry;
import segment_index_entry;
import chunk_index_entry;
import memory_indexer;
import cleanup_scanner;
import column_index_merger;
import parsed_expr;
import constant_expr;
import infinity_context;
import persistence_manager;
import bg_task;
import defer_op;

namespace infinity {

SharedPtr<String> TableEntry::DetermineTableDir(const String &parent_dir, const String &table_name) {
    auto abs_parent_dir = Path(InfinityContext::instance().config()->DataDir()) / parent_dir;
    SharedPtr<String> temp_dir = DetermineRandomString(abs_parent_dir, fmt::format("table_{}", table_name));
    return MakeShared<String>(Path(parent_dir) / *temp_dir);
}

Vector<std::string_view> TableEntry::DecodeIndex(std::string_view encode) {
    SizeT delimiter_i = encode.rfind('#');
    if (delimiter_i == String::npos) {
        String error_message = fmt::format("Invalid table entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    auto decodes = DBEntry::DecodeIndex(encode.substr(0, delimiter_i));
    decodes.push_back(encode.substr(delimiter_i + 1));
    return decodes;
}

String TableEntry::EncodeIndex(const String &table_name, TableMeta *table_meta) {
    if (table_meta == nullptr) {
        return ""; // unit test
    }
    return fmt::format("{}#{}", table_meta->db_entry()->encode(), table_name);
}

TableEntry::TableEntry(bool is_delete,
                       const SharedPtr<String> &table_entry_dir,
                       SharedPtr<String> table_name,
                       const Vector<SharedPtr<ColumnDef>> &columns,
                       TableEntryType table_entry_type,
                       TableMeta *table_meta,
                       TransactionID txn_id,
                       TxnTimeStamp begin_ts,
                       SegmentID unsealed_id,
                       SegmentID next_segment_id,
                       ColumnID next_column_id)
    : BaseEntry(EntryType::kTable, is_delete, TableEntry::EncodeIndex(*table_name, table_meta)), table_meta_(table_meta),
      table_entry_dir_(std::move(table_entry_dir)), table_name_(std::move(table_name)), columns_(columns), next_column_id_(next_column_id),
      table_entry_type_(table_entry_type), unsealed_id_(unsealed_id), next_segment_id_(next_segment_id),
      fulltext_column_index_cache_(MakeShared<TableIndexReaderCache>(this)) {
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;

    // this->SetCompactionAlg(nullptr);
    if (!is_delete) {
        this->SetCompactionAlg(MakeUnique<DBTCompactionAlg>(DBT_COMPACTION_M, DBT_COMPACTION_C, DBT_COMPACTION_S, DEFAULT_SEGMENT_CAPACITY, this));
        compaction_alg_->Enable({});
    }
}

TableEntry::TableEntry(const TableEntry &other)
    : BaseEntry(other), table_meta_(other.table_meta_), table_entry_dir_(other.table_entry_dir_), table_name_(other.table_name_),
      columns_(other.columns_), next_column_id_(other.next_column_id_), table_entry_type_(other.table_entry_type_) {
    row_count_ = other.row_count_.load();
    unsealed_id_ = other.unsealed_id_;
    next_segment_id_ = other.next_segment_id_.load();
    fulltext_column_index_cache_ = other.fulltext_column_index_cache_;
}

UniquePtr<TableEntry> TableEntry::Clone(TableMeta *meta) const {
    auto ret = UniquePtr<TableEntry>(new TableEntry(*this));
    std::shared_lock lock(rw_locker_);
    if (meta != nullptr) {
        ret->table_meta_ = meta;
    }
    for (const auto &[segment_id, segment_entry] : segment_map_) {
        ret->segment_map_[segment_id] = segment_entry->Clone(ret.get());
    }
    if (unsealed_segment_.get() != nullptr) {
        ret->unsealed_segment_ = ret->segment_map_[ret->unsealed_id_];
    }
    return ret;
}

SharedPtr<TableEntry> TableEntry::NewTableEntry(bool is_delete,
                                                const SharedPtr<String> &db_entry_dir,
                                                SharedPtr<String> table_name,
                                                const Vector<SharedPtr<ColumnDef>> &columns,
                                                TableEntryType table_entry_type,
                                                TableMeta *table_meta,
                                                TransactionID txn_id,
                                                TxnTimeStamp begin_ts) {
    SharedPtr<String> table_entry_dir;
    if (is_delete) {
        table_entry_dir = MakeShared<String>("deleted");
    } else {
        table_entry_dir = TableEntry::DetermineTableDir(*db_entry_dir, *table_name);
    }
    ColumnID next_column_id = columns.size();
    return MakeShared<TableEntry>(is_delete,
                                  std::move(table_entry_dir),
                                  std::move(table_name),
                                  columns,
                                  table_entry_type,
                                  table_meta,
                                  txn_id,
                                  begin_ts,
                                  INVALID_SEGMENT_ID,
                                  0 /*next_segment_id*/,
                                  next_column_id);
}

SharedPtr<TableEntry> TableEntry::ReplayTableEntry(bool is_delete,
                                                   TableMeta *table_meta,
                                                   SharedPtr<String> table_entry_dir,
                                                   SharedPtr<String> table_name,
                                                   const Vector<SharedPtr<ColumnDef>> &column_defs,
                                                   TableEntryType table_entry_type,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnTimeStamp commit_ts,
                                                   SizeT row_count,
                                                   SegmentID unsealed_id,
                                                   SegmentID next_segment_id,
                                                   ColumnID next_column_id) noexcept {
    auto table_entry = MakeShared<TableEntry>(is_delete,
                                              std::move(table_entry_dir),
                                              std::move(table_name),
                                              column_defs,
                                              table_entry_type,
                                              table_meta,
                                              txn_id,
                                              begin_ts,
                                              unsealed_id,
                                              next_segment_id,
                                              next_column_id);
    // TODO need to check if commit_ts influence replay catalog delta entry
    table_entry->commit_ts_.store(commit_ts);
    table_entry->row_count_.store(row_count);

    return table_entry;
}

Tuple<TableIndexEntry *, Status> TableEntry::CreateIndex(const SharedPtr<IndexBase> &index_base,
                                                         ConflictType conflict_type,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         TxnManager *txn_mgr) {
    if (index_base->index_name_->empty()) {
        // Index name shouldn't be empty
        String error_message = "Attempt to create no name index.";
        UnrecoverableError(error_message);
    }
    LOG_TRACE(fmt::format("Creating new index: {}", *index_base->index_name_));
    auto init_index_meta = [&]() { return TableIndexMeta::NewTableIndexMeta(this, index_base->index_name_); };

    auto [table_index_meta, r_lock] = index_meta_map_.GetMeta(*index_base->index_name_, std::move(init_index_meta));

    LOG_TRACE(fmt::format("Creating new index: {}", *index_base->index_name_));
    return table_index_meta->CreateTableIndexEntry(std::move(r_lock), index_base, table_entry_dir_, conflict_type, txn_id, begin_ts, txn_mgr);
}

Tuple<SharedPtr<TableIndexEntry>, Status>
TableEntry::DropIndex(const String &index_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    auto [index_meta, status, r_lock] = index_meta_map_.GetExistMeta(index_name, conflict_type);
    if (!status.ok()) {
        return {nullptr, status};
    }
    SharedPtr<String> index_name_ptr = index_meta->index_name();
    return index_meta->DropTableIndexEntry(std::move(r_lock), conflict_type, index_name_ptr, txn_id, begin_ts, txn_mgr);
}

Tuple<TableIndexEntry *, Status> TableEntry::GetIndex(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [index_meta, status, r_lock] = index_meta_map_.GetExistMeta(index_name, ConflictType::kError);
    if (!status.ok()) {
        return {nullptr, status};
    }
    return index_meta->GetEntry(std::move(r_lock), txn_id, begin_ts);
}

Tuple<SharedPtr<TableIndexInfo>, Status> TableEntry::GetTableIndexInfo(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [index_meta, status, r_lock] = index_meta_map_.GetExistMeta(index_name, ConflictType::kError);
    if (!status.ok()) {
        return {nullptr, status};
    }
    return index_meta->GetTableIndexInfo(std::move(r_lock), txn_id, begin_ts);
}

void TableEntry::RemoveIndexEntry(const String &index_name, TransactionID txn_id) {
    auto [index_meta, status] = index_meta_map_.GetExistMetaNoLock(index_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    LOG_TRACE(fmt::format("Remove index entry: {}", index_name));
    return index_meta->DeleteEntry(txn_id);
}

void TableEntry::AddIndexMetaNoLock(const String &table_meta_name, UniquePtr<TableIndexMeta> table_index_meta) {
    index_meta_map_.AddNewMetaNoLock(table_meta_name, std::move(table_index_meta));
}

/// replay
void TableEntry::UpdateEntryReplay(const SharedPtr<TableEntry> &table_entry) {
    txn_id_ = table_entry->txn_id_;
    begin_ts_ = table_entry->begin_ts_;
    commit_ts_.store(table_entry->commit_ts_);
    row_count_ = table_entry->row_count();
    unsealed_id_ = table_entry->unsealed_id();
    next_segment_id_ = table_entry->next_segment_id();
}

TableIndexEntry *TableEntry::CreateIndexReplay(const SharedPtr<String> &index_name,
                                               std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                                               TransactionID txn_id,
                                               TxnTimeStamp begin_ts) {
    auto init_index_meta = [&]() { return TableIndexMeta::NewTableIndexMeta(this, index_name); };
    auto *index_meta = index_meta_map_.GetMetaNoLock(*index_name, std::move(init_index_meta));
    return index_meta->CreateEntryReplay(std::move(init_entry), txn_id, begin_ts);
}

void TableEntry::UpdateIndexReplay(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    auto [index_meta, status] = index_meta_map_.GetExistMetaNoLock(index_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    index_meta->UpdateEntryReplay(txn_id, begin_ts, commit_ts);
}

void TableEntry::DropIndexReplay(const String &index_name,
                                 std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, TransactionID, TxnTimeStamp)> &&init_entry,
                                 TransactionID txn_id,
                                 TxnTimeStamp begin_ts) {
    auto [index_meta, status] = index_meta_map_.GetExistMetaNoLock(index_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    index_meta->DropEntryReplay(std::move(init_entry), txn_id, begin_ts);
}

TableIndexEntry *TableEntry::GetIndexReplay(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [index_meta, status] = index_meta_map_.GetExistMetaNoLock(index_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return index_meta->GetEntryReplay(txn_id, begin_ts);
}

Vector<TableIndexEntry *> TableEntry::TableIndexes(TransactionID txn_id, TxnTimeStamp begin_ts) {
    Vector<TableIndexEntry *> results;
    auto map_guard = index_meta_map_.GetMetaMap();
    results.reserve((*map_guard).size());
    for (auto &[index_name, index_meta] : *map_guard) {
        auto [index_entry, status] = index_meta->GetEntryNolock(txn_id, begin_ts);
        if (!status.ok()) {
            LOG_TRACE(fmt::format("error when get table index entry: {} index name: {}", status.message(), *index_meta->index_name()));
        } else {
            results.emplace_back(static_cast<TableIndexEntry *>(index_entry));
        }
    }
    return results;
}

void TableEntry::AddSegmentReplayWalImport(SharedPtr<SegmentEntry> segment_entry) {
    this->AddSegmentReplayWal(segment_entry);
    row_count_ += segment_entry->actual_row_count();
}

void TableEntry::AddSegmentReplayWalCompact(SharedPtr<SegmentEntry> segment_entry) { this->AddSegmentReplayWal(segment_entry); }

void TableEntry::AddSegmentReplayWal(SharedPtr<SegmentEntry> new_segment) {
    SegmentID segment_id = new_segment->segment_id();
    segment_map_[segment_id] = new_segment;
    if (compaction_alg_.get() != nullptr) {
        compaction_alg_->AddSegment(new_segment.get());
    }
    next_segment_id_++;
}

void TableEntry::AddSegmentReplay(SharedPtr<SegmentEntry> new_segment) {
    SegmentID segment_id = new_segment->segment_id();

    auto [iter, insert_ok] = segment_map_.emplace(segment_id, new_segment);
    if (!insert_ok) {
        String error_message = fmt::format("Segment {} already exists.", segment_id);
        UnrecoverableError(error_message);
    }
    if (compaction_alg_.get() != nullptr) {
        compaction_alg_->AddSegment(new_segment.get());
    }
    if (segment_id == unsealed_id_) {
        unsealed_segment_ = std::move(new_segment);
    }
}

void TableEntry::UpdateSegmentReplay(SharedPtr<SegmentEntry> new_segment, String segment_filter_binary_data) {
    SegmentID segment_id = new_segment->segment_id();

    auto iter = segment_map_.find(segment_id);
    if (iter == segment_map_.end()) {
        String error_message = fmt::format("Segment {} not found.", segment_id);
        UnrecoverableError(error_message);
    }
    iter->second->UpdateSegmentReplay(new_segment, std::move(segment_filter_binary_data));
}

void TableEntry::GetFulltextAnalyzers(TransactionID txn_id, TxnTimeStamp begin_ts, Map<String, String> &column2analyzer) {
    column2analyzer.clear();
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
        if (status.ok()) {
            const IndexBase *index_base = table_index_entry->index_base();
            if (index_base->index_type_ != IndexType::kFullText)
                continue;
            auto index_full_text = static_cast<const IndexFullText *>(index_base);
            for (auto &column_name : index_full_text->column_names_) {
                column2analyzer[column_name] = index_full_text->analyzer_;
            }
        }
    }
}

void TableEntry::Import(SharedPtr<SegmentEntry> segment_entry, Txn *txn) {
    {
        std::unique_lock lock(this->rw_locker_);
        SegmentID segment_id = segment_entry->segment_id();
        auto [_, insert_ok] = this->segment_map_.emplace(segment_id, segment_entry);
        if (!insert_ok) {
            String error_message = fmt::format("Insert segment {} failed.", segment_id);
            UnrecoverableError(error_message);
        }
    }
    // Populate index entirely for the segment

    // create txn_table_store in Txn::Import
    TxnTableStore *txn_table_store = txn->GetExistTxnTableStore(this);
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(0UL, txn->BeginTS());
        if (!status.ok())
            continue;
        const IndexBase *index_base = table_index_entry->index_base();
        switch (index_base->index_type_) {
            case IndexType::kFullText:
            case IndexType::kSecondary:
            case IndexType::kEMVB:
            case IndexType::kHnsw:
            case IndexType::kBMP: {
                // support realtime index
                break;
            }
            default: {
                UniquePtr<String> err_msg =
                    MakeUnique<String>(fmt::format("{} realtime index is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
                LOG_WARN(*err_msg);
                continue;
            }
        }
        PopulateEntireConfig populate_entire_config{.prepare_ = false, .check_ts_ = false};
        SharedPtr<SegmentIndexEntry> segment_index_entry = table_index_entry->PopulateEntirely(segment_entry.get(), txn, populate_entire_config);
        if (segment_index_entry.get() != nullptr) {
            Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
            SharedPtr<MemoryIndexer> memory_indexer;
            segment_index_entry->GetChunkIndexEntries(chunk_index_entries, memory_indexer, txn);
            for (auto &chunk_index_entry : chunk_index_entries) {
                txn_table_store->AddChunkIndexStore(table_index_entry, chunk_index_entry.get());
            }
        }
    }
}

void TableEntry::AddCompactNew(SharedPtr<SegmentEntry> segment_entry) {
    std::unique_lock lock(this->rw_locker_);
    SegmentID segment_id = segment_entry->segment_id();
    auto [_, insert_ok] = this->segment_map_.emplace(segment_id, std::move(segment_entry));
    if (!insert_ok) {
        String error_message = fmt::format("Insert segment {} failed.", segment_id);
        UnrecoverableError(error_message);
    }
}

void TableEntry::AppendData(TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, BufferManager *buffer_mgr, bool is_replay) {
    SizeT row_count = 0;

    // Read-only no lock needed.
    if (this->Deleted()) {
        String error_message = "Table is deleted";
        UnrecoverableError(error_message);
        return;
    }
    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->GetAppendState();
    Txn *txn = txn_store_ptr->GetTxn();
    if (append_state_ptr->Finished()) {
        // Import update row count
        if (append_state_ptr->blocks_.empty()) {
            for (auto &segment : txn_store_ptr->flushed_segments()) {
                this->row_count_ += segment->row_count();
            }
        }
        LOG_TRACE("No append is done.");
        return;
    }

    while (!append_state_ptr->Finished()) {
        {
            std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_); // prevent another read conflict with this append operation
            if (unsealed_segment_.get() == nullptr || unsealed_segment_->Room() <= 0) {
                // unsealed_segment_ is unpopulated or full
                if (unsealed_segment_.get() != nullptr) {
                    // The segment is full, need to be set sealed.
                    txn_store_ptr->AddSealedSegment(unsealed_segment_.get());
                }

                SegmentID new_segment_id = this->next_segment_id_++;

                this->unsealed_segment_ = SegmentEntry::NewSegmentEntry(this, new_segment_id, txn);
                // FIXME: Why not use new_segment_id directly?
                unsealed_id_ = unsealed_segment_->segment_id();
                this->segment_map_.emplace(new_segment_id, this->unsealed_segment_);
                LOG_TRACE(fmt::format("Created a new segment {}", new_segment_id));
            }
        }
        u64 actual_appended = unsealed_segment_->AppendData(txn_id, commit_ts, append_state_ptr, buffer_mgr, txn);

        LOG_TRACE(fmt::format("Segment {} is appended with {} rows", this->unsealed_segment_->segment_id(), actual_appended));
        row_count += actual_appended;
    }

    // Needn't inserting into MemIndex since MemIndexRecover is responsible for recovering MemIndex
    if (!is_replay) {
        // Realtime index insertion.
        MemIndexInsert(txn, append_state_ptr->append_ranges_);
    }

    this->row_count_ += row_count;
}

Status TableEntry::Delete(TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    SizeT row_count = 0;

    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    Txn *txn = txn_store_ptr->GetTxn();
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        SegmentID segment_id = to_delete_seg_rows.first;
        SharedPtr<SegmentEntry> segment_entry = GetSegmentByID(segment_id, commit_ts);
        if (!segment_entry) {
            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Going to delete data in non-exist segment: {}", segment_id));
            return Status(ErrorCode::kTableNotExist, std::move(err_msg));
        }
        const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap = to_delete_seg_rows.second;
        SizeT delete_row_n = segment_entry->DeleteData(txn_id, commit_ts, block_row_hashmap, txn);

        row_count += delete_row_n;
    }
    this->row_count_ -= row_count;
    return Status::OK();
}

void TableEntry::RollbackAppend(TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store) {
    //    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    //    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    String error_message = "TableEntry::RollbackAppend";
    UnrecoverableError(error_message);
}

Status TableEntry::RollbackDelete(TransactionID txn_id, DeleteState &, BufferManager *) {
    String error_message = "TableEntry::RollbackDelete";
    UnrecoverableError(error_message);
    return Status::OK();
}

Status TableEntry::CommitCompact(TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_store) {
    if (compact_store.type_ == CompactStatementType::kInvalid) {
        return Status::OK();
    }

    {
        // {
        //     String ss = "Compact commit: " + *this->GetTableName();
        //     for (const auto &[segment_store, old_segments] : compact_store.compact_data_) {
        //         auto *new_segment = segment_store.segment_entry_;
        //         ss += ", new segment: " + std::to_string(new_segment->segment_id()) + ", old segment: ";
        //         for (const auto *old_segment : old_segments) {
        //             ss += std::to_string(old_segment->segment_id_) + " ";
        //         }
        //     }
        //     LOG_INFO(ss);
        // }
        std::unique_lock lock(this->rw_locker_);
        for (const auto &[segment_store, old_segments] : compact_store.compact_data_) {

            auto *segment_entry = segment_store.segment_entry_;

            segment_entry->CommitSegment(txn_id, commit_ts, segment_store, nullptr);

            for (const auto &old_segment : old_segments) {
                // old_segment->TrySetDeprecated(commit_ts);
                old_segment->SetDeprecated(commit_ts);
            }
        }
    }

    // Update fulltext ts so that TableIndexReaderCache::GetIndexReader will update the cache
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, commit_ts);
        if (!status.ok())
            continue;
        const IndexBase *index_base = table_index_entry->index_base();
        switch (index_base->index_type_) {
            case IndexType::kFullText: {
                table_index_entry->UpdateFulltextSegmentTs(commit_ts);
                break;
            }
            default: {
            }
        }
    }

    if (compaction_alg_.get() == nullptr) {
        return Status::OK();
    }

    switch (compact_store.type_) {
        case CompactStatementType::kAuto: {
            compaction_alg_->CommitCompact(txn_id);
            LOG_DEBUG(fmt::format("Compact commit picked, table name: {}", *this->GetTableName()));
            break;
        }
        case CompactStatementType::kManual: {
            //  reinitialize compaction_alg_ with new segments and enable it
            LOG_DEBUG(fmt::format("Compact commit whole, table name: {}", *this->GetTableName()));
            compaction_alg_->Enable({});
            break;
        }
        default: {
            String error_message = "Invalid compact task type";
            UnrecoverableError(error_message);
        }
    }
    return Status::OK();
}

Status TableEntry::RollbackCompact(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store) {
    if (compact_store.compact_data_.empty()) {
        return Status::OK();
    }
    {
        for (const auto &[segment_store, old_segments] : compact_store.compact_data_) {
            SharedPtr<SegmentEntry> segment;

            auto *segment_entry = segment_store.segment_entry_;
            segment_entry->RollbackBlocks(commit_ts, segment_store.block_entries_);
            if (segment_entry->Committed()) {
                String error_message = fmt::format("RollbackCompact: segment {} is committed", segment_entry->segment_id());
                UnrecoverableError(error_message);
            }
            {
                std::unique_lock lock(this->rw_locker_);
                if (auto iter = segment_map_.find(segment_entry->segment_id()); iter != segment_map_.end()) {
                    segment = std::move(iter->second);
                    segment_map_.erase(iter);
                } else {
                    String error_message = fmt::format("RollbackCompact: segment {} not found", segment_entry->segment_id());
                    UnrecoverableError(error_message);
                }

                for (auto *old_segment : old_segments) {
                    old_segment->RollbackCompact();
                }
            }
            segment->Cleanup();
        }
    }
    if (compaction_alg_.get() != nullptr) {
        switch (compact_store.type_) {
            case CompactStatementType::kAuto: {
                compaction_alg_->RollbackCompact(txn_id);
                break;
            }
            case CompactStatementType::kManual: {
                Vector<SegmentEntry *> old_segments;
                for (const auto &[_, old_segs] : compact_store.compact_data_) {
                    old_segments.insert(old_segments.end(), old_segs.begin(), old_segs.end());
                }
                compaction_alg_->Enable(old_segments);
                break;
            }
            default: {
                String error_message = "Invalid compact task type";
                UnrecoverableError(error_message);
            }
        }
    }

    return Status::OK();
}

Status TableEntry::CommitWrite(TransactionID txn_id,
                               TxnTimeStamp commit_ts,
                               const HashMap<SegmentID, TxnSegmentStore> &segment_stores,
                               const DeleteState *delete_state) {
    for (const auto &[segment_id, segment_store] : segment_stores) {
        auto *segment_entry = segment_store.segment_entry_;
        segment_entry->CommitSegment(txn_id, commit_ts, segment_store, delete_state);
    }
    return Status::OK();
}

Status TableEntry::RollbackWrite(TxnTimeStamp commit_ts, const Vector<TxnSegmentStore> &segment_stores) {
    for (auto &segment_store : segment_stores) {
        auto *segment_entry = segment_store.segment_entry_;
        segment_entry->RollbackBlocks(commit_ts, segment_store.block_entries_);

        if (!segment_entry->Committed()) {
            SharedPtr<SegmentEntry> segment;
            {
                std::unique_lock w_lock(this->rw_locker_);
                if (auto iter = segment_map_.find(segment_entry->segment_id()); iter != segment_map_.end()) {
                    segment = std::move(iter->second);
                    segment_map_.erase(iter);
                } else {
                    String error_message = fmt::format("RollbackWrite: segment {} not found", segment_entry->segment_id());
                    UnrecoverableError(error_message);
                }
            }
            segment->Cleanup();
        }
    }
    return Status::OK();
}

void TableEntry::MemIndexInsert(Txn *txn, Vector<AppendRange> &append_ranges) {
    Map<SegmentID, Vector<AppendRange>> seg_append_ranges;
    SizeT num_ranges = append_ranges.size();
    for (SizeT i = 0; i < num_ranges; i++) {
        AppendRange &range = append_ranges[i];
        seg_append_ranges[range.segment_id_].push_back(range);
    }
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
        if (!status.ok())
            continue;
        const IndexBase *index_base = table_index_entry->index_base();
        switch (index_base->index_type_) {
            case IndexType::kHnsw:
            case IndexType::kFullText:
            case IndexType::kEMVB:
            case IndexType::kSecondary:
            case IndexType::kBMP: {
                for (auto &[seg_id, ranges] : seg_append_ranges) {
                    MemIndexInsertInner(table_index_entry, txn, seg_id, ranges);
                }
                break;
            }
            default: {
                UniquePtr<String> err_msg =
                    MakeUnique<String>(fmt::format("{} realtime index is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
                LOG_WARN(*err_msg);
            }
        }
    }
}

void TableEntry::MemIndexInsertInner(TableIndexEntry *table_index_entry, Txn *txn, SegmentID seg_id, Vector<AppendRange> &append_ranges) {
    const IndexBase *index_base = table_index_entry->index_base();

    SharedPtr<SegmentEntry> segment_entry = GetSegmentByID(seg_id, MAX_TIMESTAMP);
    SharedPtr<SegmentIndexEntry> segment_index_entry;
    // create txn_table_store in `Txn::Append`
    TxnTableStore *txn_table_store = txn->GetExistTxnTableStore(this);
    bool created = table_index_entry->GetOrCreateSegment(seg_id, txn, segment_index_entry);
    if (created) {
        Vector<SegmentIndexEntry *> segment_index_entries{segment_index_entry.get()};
        txn_table_store->AddSegmentIndexesStore(table_index_entry, segment_index_entries);

        if (index_base->index_type_ == IndexType::kFullText) {
            table_index_entry->UpdateFulltextSegmentTs(txn->CommitTS());
        }
    }
    table_index_entry->last_segment_ = segment_index_entry;
    Vector<SharedPtr<BlockEntry>> block_entries;
    SizeT num_ranges = append_ranges.size();
    SizeT dump_idx = SizeT(-1);
    // Determine the last sealed BlockEntry
    for (SizeT i = 0; i < num_ranges; i++) {
        AppendRange &range = append_ranges[i];
        SharedPtr<BlockEntry> block_entry = segment_entry->GetBlockEntryByID(range.block_id_);
        block_entries.push_back(block_entry);
        if (block_entry->GetAvailableCapacity() <= 0)
            dump_idx = i;
    }

    for (SizeT i = 0; i < num_ranges; i++) {
        AppendRange &range = append_ranges[i];
        SharedPtr<BlockEntry> block_entry = block_entries[i];
        segment_index_entry->MemIndexInsert(block_entry, range.start_offset_, range.row_count_, txn->CommitTS(), txn->buffer_mgr());
        if ((i == dump_idx && segment_index_entry->MemIndexRowCount() >= infinity::InfinityContext::instance().config()->MemIndexCapacity()) ||
            (i == num_ranges - 1 && segment_entry->Room() <= 0)) {
            SharedPtr<ChunkIndexEntry> chunk_index_entry = segment_index_entry->MemIndexDump();
            String *index_name = index_base->index_name_.get();
            String message = fmt::format("Table {}.{} index {} segment {} MemIndex dumped.", *GetDBName(), *table_name_, *index_name, seg_id);
            LOG_INFO(message);
            if (chunk_index_entry.get() != nullptr) {
                chunk_index_entry->Commit(txn->CommitTS());

                auto *compaction_process = InfinityContext::instance().storage()->compaction_processor();

                compaction_process->Submit(
                    MakeShared<DumpIndexBylineTask>(GetDBName(), GetTableName(), table_index_entry->GetIndexName(), seg_id, chunk_index_entry));

                if (index_base->index_type_ == IndexType::kFullText) {
                    table_index_entry->UpdateFulltextSegmentTs(txn->CommitTS());
                }
            } else {
                LOG_WARN("MemIndexDump failed.");
            }
        }
    }
}

bool TableEntry::CheckIfIndexColumn(ColumnID column_id, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (const auto &[_, table_index_meta] : *index_meta_map_guard) {
        if (table_index_meta->CheckIfIndexColumn(column_id, txn_id, begin_ts)) {
            return true;
        }
    }
    return false;
}

// void TableEntry::MemIndexDump(Txn *txn, bool spill) {
//     TxnTableStore *txn_table_store = txn->GetTxnTableStore(this);
//     auto index_meta_map_guard = index_meta_map_.GetMetaMap();
//     for (auto &[_, table_index_meta] : *index_meta_map_guard) {
//         auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
//         if (!status.ok())
//             continue;
//         TxnIndexStore *txn_index_store = txn_table_store->GetIndexStore(table_index_entry);
//         SharedPtr<ChunkIndexEntry> chunk_index_entry = table_index_entry->MemIndexDump(txn, txn_index_store, spill);
//         if (chunk_index_entry.get() != nullptr) {
//             chunk_index_entry->Commit(txn->CommitTS());
//             txn_table_store->AddChunkIndexStore(table_index_entry, chunk_index_entry.get());
//             table_index_entry->UpdateFulltextSegmentTs(txn->CommitTS());
//         }
//     }
// }

void TableEntry::MemIndexCommit() {
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            table_index_entry->MemIndexCommit();
        }
    }
}

void TableEntry::MemIndexRecover(BufferManager *buffer_manager, TxnTimeStamp ts) {
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[index_name, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (!status.ok())
            continue;
        for (const auto &[segment_id, segment_entry] : segment_map_) {
            auto iter = table_index_entry->index_by_segment().find(segment_id);
            SharedPtr<SegmentIndexEntry> segment_index_entry = nullptr;
            if (iter == table_index_entry->index_by_segment().end()) {
                segment_index_entry = SegmentIndexEntry::NewReplaySegmentIndexEntry(table_index_entry,
                                                                                    this,
                                                                                    segment_id,
                                                                                    buffer_manager,
                                                                                    ts /*min_ts*/,
                                                                                    ts /*max_ts*/,
                                                                                    0 /*next_chunk_id*/,
                                                                                    0 /*txn_id*/,
                                                                                    ts /*begin_ts*/,
                                                                                    ts /*commit_ts*/);
                table_index_entry->index_by_segment().emplace(segment_id, segment_index_entry);
            } else {
                segment_index_entry = iter->second;
            }
            Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
            SharedPtr<MemoryIndexer> memory_indexer;
            segment_index_entry->GetChunkIndexEntries(chunk_index_entries, memory_indexer, nullptr);

            // Determine block entries need to insert into MemIndexer
            Vector<AppendRange> append_ranges;
            Vector<SharedPtr<BlockEntry>> &block_entries = segment_entry->block_entries();
            if (chunk_index_entries.empty()) {
                for (SizeT i = 0; i < block_entries.size(); i++) {
                    append_ranges.emplace_back(segment_id, i, 0, block_entries[i]->row_count());
                }
            } else {
                SizeT block_capacity = block_entries[0]->row_capacity();
                SharedPtr<ChunkIndexEntry> &chunk_index_entry = chunk_index_entries.back();
                RowID base_rowid = chunk_index_entry->base_rowid_ + chunk_index_entry->row_count_;
                SizeT block_id = base_rowid.segment_offset_ / block_capacity;
                assert(block_id <= block_entries.size());
                if (block_id >= block_entries.size())
                    continue;
                SizeT start_offset = base_rowid.segment_offset_ % block_capacity;
                SizeT last_block_row_count = block_entries[block_id]->row_count();
                assert(last_block_row_count >= start_offset);
                SizeT row_count = last_block_row_count - start_offset;
                if (row_count == 0)
                    continue;
                append_ranges.emplace_back(segment_id, block_id, start_offset, row_count);
                for (SizeT i = block_id + 1; i < block_entries.size(); i++) {
                    assert(block_entries[i - 1]->row_capacity() == block_capacity);
                    if (block_entries[i - 1]->GetAvailableCapacity() > 0) {
                        LOG_ERROR(fmt::format("MemIndexRecover got a non-full BlockEntry. block_id {}, row_count {}, block_entries {}",
                                              block_entries[i - 1]->block_id(),
                                              block_entries[i - 1]->row_count(),
                                              block_entries.size()));
                    }
                    append_ranges.emplace_back(segment_id, i, 0, block_entries[i]->row_count());
                }
            }

            SizeT num_ranges = append_ranges.size();
            if (num_ranges > 0) {
                assert(segment_entry->Room() > 0);
                // Insert block entries into MemIndexer
                String message = fmt::format("Table {}.{} index {} segment {} MemIndex recovering from {} block entries.",
                                             *GetDBName(),
                                             *table_name_,
                                             index_name,
                                             segment_id,
                                             num_ranges);
                LOG_INFO(message);
                for (SizeT i = 0; i < num_ranges; i++) {
                    AppendRange &range = append_ranges[i];
                    segment_index_entry->MemIndexInsert(block_entries[range.block_id_],
                                                        range.start_offset_,
                                                        range.row_count_,
                                                        segment_index_entry->max_ts(),
                                                        buffer_manager);
                }
                segment_index_entry->MemIndexWaitInflightTasks();
                message = fmt::format("Table {}.{} index {} segment {} MemIndex recovered.", *GetDBName(), *table_name_, index_name, segment_id);
                LOG_INFO(message);
            }
            if (segment_id == unsealed_id_) {
                table_index_entry->last_segment_ = segment_index_entry;
            }
        }
    }
}

void TableEntry::OptimizeIndex(Txn *txn) {
    TxnTableStore *txn_table_store = txn->GetTxnTableStore(this);
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[index_name, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
        if (!status.ok()) {
            continue;
        }
        const IndexBase *index_base = table_index_entry->index_base();
        switch (index_base->index_type_) {
            case IndexType::kFullText: {
                const IndexFullText *index_fulltext = static_cast<const IndexFullText *>(index_base);
                Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment = table_index_entry->GetIndexBySegmentSnapshot(this, txn);
                for (auto &[segment_id, segment_index_entry] : index_by_segment) {
                    if (!segment_index_entry->TrySetOptimizing()) {
                        LOG_INFO(fmt::format("Index {} segment {} is optimizing, skip optimize.", index_name, segment_id));
                        continue;
                    }
                    bool opt_success = false;
                    DeferFn defer_fn([&] {
                        if (!opt_success) {
                            LOG_WARN(fmt::format("Index {} segment {} optimize fail or skip.", index_name, segment_id));
                            segment_index_entry->ResetOptimizing();
                        }
                    });
                    Vector<SharedPtr<ChunkIndexEntry>> chunk_index_entries;
                    SharedPtr<MemoryIndexer> memory_indexer;
                    Vector<ChunkIndexEntry *> old_chunks;
                    segment_index_entry->GetChunkIndexEntries(chunk_index_entries, memory_indexer, txn);
                    if (chunk_index_entries.size() <= 1) {
                        continue;
                    }

                    String msg = fmt::format("merging {}", index_name);
                    Vector<String> base_names;
                    Vector<RowID> base_rowids;
                    RowID base_rowid = chunk_index_entries[0]->base_rowid_;
                    u32 total_row_count = 0;
                    for (SizeT i = 0; i < chunk_index_entries.size(); i++) {
                        auto &chunk_index_entry = chunk_index_entries[i];
                        msg += " " + chunk_index_entry->base_name_;
                        if (chunk_index_entry->base_rowid_ != chunk_index_entries[0]->base_rowid_ + total_row_count) {
                            String error_msg = fmt::format("{}... chunk_index_entry {} base_rowid expects to be {:016x}",
                                                           msg,
                                                           chunk_index_entry->base_name_,
                                                           (chunk_index_entries[0]->base_rowid_ + total_row_count).ToUint64());
                            UnrecoverableError(error_msg);
                        }
                        base_names.push_back(chunk_index_entry->base_name_);
                        base_rowids.push_back(chunk_index_entry->base_rowid_);
                        total_row_count += chunk_index_entry->row_count_;
                    }
                    String dst_base_name = fmt::format("ft_{:016x}_{:x}", base_rowid.ToUint64(), total_row_count);
                    msg += " -> " + dst_base_name;
                    LOG_INFO(msg);
                    ColumnIndexMerger column_index_merger(*table_index_entry->index_dir_, index_fulltext->flag_);
                    column_index_merger.Merge(base_names, base_rowids, dst_base_name);

                    for (SizeT i = 0; i < chunk_index_entries.size(); i++) {
                        auto &chunk_index_entry = chunk_index_entries[i];
                        old_chunks.push_back(chunk_index_entry.get());
                    }
                    ChunkID chunk_id = segment_index_entry->GetNextChunkID();
                    SharedPtr<ChunkIndexEntry> merged_chunk_index_entry = ChunkIndexEntry::NewFtChunkIndexEntry(segment_index_entry.get(),
                                                                                                                chunk_id, /*chunk_id*/
                                                                                                                dst_base_name,
                                                                                                                base_rowid,
                                                                                                                total_row_count,
                                                                                                                txn->buffer_mgr());
                    segment_index_entry->ReplaceChunkIndexEntries(txn_table_store, merged_chunk_index_entry, std::move(old_chunks));
                    opt_success = true; // set success after record in txn store

                    // OPTIMIZE invoke this func at which the txn hasn't been commited yet.
                    TxnTimeStamp ts = std::max(txn->BeginTS(), txn->CommitTS());
                    table_index_entry->UpdateFulltextSegmentTs(ts);
                    LOG_INFO(fmt::format("done merging {} {}", index_name, dst_base_name));
                }
                break;
            }
            case IndexType::kHnsw:
            case IndexType::kEMVB:
            case IndexType::kSecondary:
            case IndexType::kBMP: {
                TxnTimeStamp begin_ts = txn->BeginTS();
                Map<SegmentID, SharedPtr<SegmentIndexEntry>> index_by_segment = table_index_entry->GetIndexBySegmentSnapshot(this, txn);
                for (auto &[segment_id, segment_index_entry] : index_by_segment) {
                    SegmentEntry *segment_entry = GetSegmentByID(segment_id, begin_ts).get();
                    if (segment_entry != nullptr) {
                        [[maybe_unused]] auto *merged_chunk_entry = segment_index_entry->RebuildChunkIndexEntries(txn_table_store, segment_entry);
                    }
                }
                break;
            }
            default: {
                UniquePtr<String> err_msg =
                    MakeUnique<String>(fmt::format("{} realtime index is not supported yet", IndexInfo::IndexTypeToString(index_base->index_type_)));
                LOG_WARN(*err_msg);
            }
        }
    }
}

SharedPtr<SegmentEntry> TableEntry::GetSegmentByID(SegmentID segment_id, TxnTimeStamp ts) const {
    std::shared_lock lock(this->rw_locker_);
    auto iter = segment_map_.find(segment_id);
    if (iter == segment_map_.end()) {
        return nullptr;
    }
    const auto &segment = iter->second;
    if (segment->min_row_ts() > ts) {
        return nullptr;
    }
    return segment;
}

SharedPtr<SegmentEntry> TableEntry::GetSegmentByID(SegmentID seg_id, Txn *txn) const {
    std::shared_lock lock(this->rw_locker_);
    auto iter = segment_map_.find(seg_id);
    if (iter == segment_map_.end()) {
        return nullptr;
    }
    const auto &segment = iter->second;
    if (!segment->CheckVisible(txn)) {
        return nullptr;
    }
    return segment;
}

const ColumnDef *TableEntry::GetColumnDefByID(ColumnID column_id) const {
    auto iter = std::find_if(columns_.begin(), columns_.end(), [column_id](const SharedPtr<ColumnDef> &column_def) {
        return static_cast<ColumnID>(column_def->id()) == column_id;
    });
    if (iter == columns_.end()) {
        return nullptr;
    }
    return iter->get();
}

SharedPtr<ColumnDef> TableEntry::GetColumnDefByName(const String &column_name) const {
    ColumnID column_id = GetColumnIdByName(column_name);
    auto iter = std::find_if(columns_.begin(), columns_.end(), [column_id](const SharedPtr<ColumnDef> &column_def) {
        return static_cast<ColumnID>(column_def->id()) == column_id;
    });
    if (iter == columns_.end()) {
        return nullptr;
    }
    return *iter;
}

SizeT TableEntry::GetColumnIdxByID(ColumnID column_id) const {
    auto iter = std::find_if(columns_.begin(), columns_.end(), [column_id](const SharedPtr<ColumnDef> &column_def) {
        return static_cast<ColumnID>(column_def->id()) == column_id;
    });
    if (iter == columns_.end()) {
        return -1;
    }
    return std::distance(columns_.begin(), iter);
}

Pair<SizeT, Status> TableEntry::GetSegmentRowCountBySegmentID(u32 seg_id) {
    auto iter = this->segment_map_.find(seg_id);
    if (iter != this->segment_map_.end()) {
        return {iter->second->row_count(), Status::OK()};
    } else {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("No segment id: {}.", seg_id));
        UnrecoverableError(*err_msg);
        return {0, Status(ErrorCode::kUnexpectedError, std::move(err_msg))};
    }
}

const SharedPtr<String> &TableEntry::GetDBName() const { return table_meta_->db_name_ptr(); }

String TableEntry::GetPathNameTail() const {
    SizeT delimiter_i = table_entry_dir_->rfind('/');
    if (delimiter_i == String::npos) {
        return *table_entry_dir_;
    }
    return table_entry_dir_->substr(delimiter_i + 1);
}

SharedPtr<BlockIndex> TableEntry::GetBlockIndex(Txn *txn) {
    //    SharedPtr<MultiIndex<u64, u64, SegmentEntry*>> result = MakeShared<MultiIndex<u64, u64, SegmentEntry*>>();
    SharedPtr<BlockIndex> result = MakeShared<BlockIndex>();
    std::shared_lock<std::shared_mutex> rw_locker(this->rw_locker_);

    // Add segment that is not deprecated
    for (const auto &segment_pair : this->segment_map_) {
        result->Insert(segment_pair.second.get(), txn);
    }

    return result;
}

SharedPtr<IndexIndex> TableEntry::GetIndexIndex(Txn *txn) {
    SharedPtr<IndexIndex> result = MakeShared<IndexIndex>();
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[index_name, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
        if (!status.ok()) {
            continue;
        }
        result->Insert(table_index_entry, txn);
    }
    return result;
}

nlohmann::json TableEntry::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;

    Vector<SegmentEntry *> segment_candidates;
    SizeT checkpoint_row_count = 0;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json_res["table_name"] = *this->GetTableName();
        json_res["table_entry_type"] = this->table_entry_type_;
        json_res["begin_ts"] = this->begin_ts_;
        json_res["commit_ts"] = this->commit_ts_.load();
        json_res["txn_id"] = this->txn_id_;
        json_res["deleted"] = this->deleted_;
        if (!this->deleted_) {
            json_res["table_entry_dir"] = *this->table_entry_dir_;
            for (const auto &column_def : this->columns_) {
                nlohmann::json column_def_json;
                column_def_json["column_type"] = column_def->type()->Serialize();
                column_def_json["column_id"] = column_def->id();
                column_def_json["column_name"] = column_def->name();

                for (const auto &column_constraint : column_def->constraints_) {
                    column_def_json["constraints"].emplace_back(column_constraint);
                }

                if (column_def->has_default_value()) {
                    auto default_expr = dynamic_pointer_cast<ConstantExpr>(column_def->default_expr_);
                    column_def_json["default"] = default_expr->Serialize();
                }

                json_res["column_definition"].emplace_back(column_def_json);
            }
        }
        u32 next_segment_id = this->next_segment_id_;
        json_res["next_segment_id"] = next_segment_id;
        json_res["next_column_id"] = next_column_id_;

        segment_candidates.reserve(this->segment_map_.size());
        for (const auto &[segment_id, segment_entry] : this->segment_map_) {
            if (segment_entry->commit_ts_ > max_commit_ts or segment_entry->deprecate_ts() <= max_commit_ts) {
                continue;
            }
            segment_candidates.emplace_back(segment_entry.get());
        }
    }

    {
        auto [table_index_name_candidates, table_index_meta_candidates, meta_lock] = index_meta_map_.GetAllMetaGuard();

        // Serialize segments
        for (const auto &segment_entry : segment_candidates) {
            json_res["segments"].emplace_back(segment_entry->Serialize(max_commit_ts));
            checkpoint_row_count += segment_entry->checkpoint_row_count();
        }
        json_res["row_count"] = checkpoint_row_count;
        json_res["unsealed_id"] = unsealed_id_;

        // Serialize indexes
        SizeT table_index_count = table_index_meta_candidates.size();
        for (SizeT idx = 0; idx < table_index_count; ++idx) {
            TableIndexMeta *table_index_meta = table_index_meta_candidates[idx];
            nlohmann::json index_def_meta_json = table_index_meta->Serialize(max_commit_ts);
            index_def_meta_json["index_name"] = table_index_name_candidates[idx];
            json_res["table_indexes"].emplace_back(index_def_meta_json);
        }
    }

    return json_res;
}

UniquePtr<TableEntry> TableEntry::Deserialize(const nlohmann::json &table_entry_json, TableMeta *table_meta, BufferManager *buffer_mgr) {
    SharedPtr<String> table_name = MakeShared<String>(table_entry_json["table_name"]);
    TableEntryType table_entry_type = table_entry_json["table_entry_type"];

    bool deleted = table_entry_json["deleted"];

    Vector<SharedPtr<ColumnDef>> columns;
    u64 row_count = 0;
    SharedPtr<String> table_entry_dir;
    if (!deleted) {
        table_entry_dir = MakeShared<String>(table_entry_json["table_entry_dir"]);
        for (const auto &column_def_json : table_entry_json["column_definition"]) {
            SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"]);
            i64 column_id = column_def_json["column_id"];
            String column_name = column_def_json["column_name"];

            std::set<ConstraintType> constraints;
            if (column_def_json.contains("constraints")) {
                for (const auto &column_constraint : column_def_json["constraints"]) {
                    ConstraintType constraint = column_constraint;
                    constraints.emplace(constraint);
                }
            }

            SharedPtr<ParsedExpr> default_expr = nullptr;
            if (column_def_json.contains("default")) {
                default_expr = ConstantExpr::Deserialize(column_def_json["default"]);
            }

            SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints, default_expr);
            columns.emplace_back(column_def);
        }
        row_count = table_entry_json["row_count"];
    }

    TransactionID txn_id = table_entry_json["txn_id"];
    TxnTimeStamp begin_ts = table_entry_json["begin_ts"];
    SegmentID unsealed_id = table_entry_json["unsealed_id"];
    SegmentID next_segment_id = table_entry_json["next_segment_id"];
    ColumnID next_column_id = table_entry_json["next_column_id"];

    UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(deleted,
                                                               table_entry_dir,
                                                               table_name,
                                                               columns,
                                                               table_entry_type,
                                                               table_meta,
                                                               txn_id,
                                                               begin_ts,
                                                               unsealed_id,
                                                               next_segment_id,
                                                               next_column_id);
    table_entry->row_count_ = row_count;

    if (table_entry_json.contains("segments")) {
        for (const auto &segment_json : table_entry_json["segments"]) {
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
            table_entry->segment_map_.emplace(segment_entry->segment_id(), segment_entry);
        }
        // here the unsealed_segment_ may be nullptr
        if (table_entry->segment_map_.find(unsealed_id) != table_entry->segment_map_.end()) {
            table_entry->unsealed_segment_ = table_entry->segment_map_.at(unsealed_id);
        }
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];

    if (table_entry->deleted_) {
        if (!table_entry->segment_map_.empty()) {
            String error_message = "deleted table should have no segment";
            UnrecoverableError(error_message);
        }
    }

    if (table_entry_json.contains("table_indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["table_indexes"]) {

            UniquePtr<TableIndexMeta> table_index_meta = TableIndexMeta::Deserialize(index_def_meta_json, table_entry.get(), buffer_mgr);
            String index_name = index_def_meta_json["index_name"];
            table_entry->AddIndexMetaNoLock(index_name, std::move(table_index_meta));
        }
    }

    return table_entry;
}

u64 TableEntry::GetColumnIdByName(const String &column_name) const {
    auto iter = std::find_if(columns_.begin(), columns_.end(), [column_name](const SharedPtr<ColumnDef> &column_def) {
        return column_def->name() == column_name;
    });
    if (iter == columns_.end()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    }
    return (*iter)->id();
}

bool TableEntry::CheckDeleteConflict(const Vector<RowID> &delete_row_ids, TransactionID txn_id) {
    HashMap<SegmentID, Vector<SegmentOffset>> delete_row_map;
    for (const auto row_id : delete_row_ids) {
        delete_row_map[row_id.segment_id_].emplace_back(row_id.segment_offset_);
    }
    Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> check_segments;
    std::shared_lock lock(this->rw_locker_);
    for (const auto &[segment_id, segment_offsets] : delete_row_map) {
        check_segments.emplace_back(this->segment_map_.at(segment_id).get(), std::move(segment_offsets));
    }

    return SegmentEntry::CheckDeleteConflict(std::move(check_segments), txn_id);
}

void TableEntry::AddSegmentToCompactionAlg(SegmentEntry *segment_entry) {
    if (compaction_alg_.get() == nullptr) {
        return;
    }
    compaction_alg_->AddSegment(segment_entry);
}

void TableEntry::AddDeleteToCompactionAlg(SegmentID segment_id) {
    if (compaction_alg_.get() == nullptr) {
        return;
    }
    compaction_alg_->DeleteInSegment(segment_id);
}

Vector<SegmentEntry *> TableEntry::CheckCompaction(TransactionID txn_id) {
    if (compaction_alg_.get() == nullptr) {
        return {};
    }
    return compaction_alg_->CheckCompaction(txn_id);
}

bool TableEntry::CompactPrepare() const {
    if (compaction_alg_.get() == nullptr) {
        LOG_WARN(fmt::format("Table {} compaction algorithm not set", *this->GetTableName()));
        return false;
    }
    compaction_alg_->Disable(); // wait for current compaction to finish
    return true;
}

void TableEntry::PickCleanup(CleanupScanner *scanner) {
    index_meta_map_.PickCleanup(scanner);
    Vector<SegmentID> cleanup_segment_ids;
    {
        std::unique_lock lock(this->rw_locker_);
        TxnTimeStamp visible_ts = scanner->visible_ts();
        for (auto iter = segment_map_.begin(); iter != segment_map_.end();) {
            SegmentEntry *segment = iter->second.get();
            // If segment is visible by txn, txn.begin_ts < segment.deprecate_ts
            // If segment can be cleaned up, segment.deprecate_ts > visible_ts, and visible_ts must > txn.begin_ts
            // So the used segment will not be cleaned up.
            if (segment->CheckDeprecate(visible_ts)) {
                cleanup_segment_ids.push_back(iter->first);
                scanner->AddEntry(std::move(iter->second));
                iter = segment_map_.erase(iter);
            } else {
                ++iter;
            }
        }
    }
    std::sort(cleanup_segment_ids.begin(), cleanup_segment_ids.end());
    {
        auto map_guard = index_meta_map_.GetMetaMap();
        for (auto &[_, table_index_meta] : *map_guard) {
            table_index_meta->PickCleanupBySegments(cleanup_segment_ids, scanner);
        }
    }
}

void TableEntry::Cleanup(CleanupInfoTracer *info_tracer, bool dropped) {
    if (this->deleted_) {
        return;
    }
    fulltext_column_index_cache_->Invalidate();
    for (auto &[segment_id, segment] : segment_map_) {
        segment->Cleanup(info_tracer, dropped);
    }
    index_meta_map_.Cleanup(info_tracer);

    if (dropped) {
        String full_table_dir = Path(InfinityContext::instance().config()->DataDir()) / *table_entry_dir_;
        LOG_DEBUG(fmt::format("Cleaning up dir: {}", full_table_dir));
        CleanupScanner::CleanupDir(full_table_dir);
        LOG_DEBUG(fmt::format("Cleaned dir: {}", full_table_dir));
        if (info_tracer != nullptr) {
            info_tracer->AddCleanupInfo(std::move(full_table_dir));
        }
    }
}

IndexReader TableEntry::GetFullTextIndexReader(Txn *txn) { return fulltext_column_index_cache_->GetIndexReader(txn); }

Tuple<Vector<String>, Vector<TableIndexMeta *>, std::shared_lock<std::shared_mutex>> TableEntry::GetAllIndexMapGuard() const {
    return index_meta_map_.GetAllMetaGuard();
}

TableIndexMeta *TableEntry::GetIndexMetaPtrByName(const String &name) const { return index_meta_map_.GetMetaPtrByName(name); }

bool TableEntry::CheckAnyDelete(TxnTimeStamp check_ts) const {
    std::shared_lock lock(this->rw_locker_);
    for (const auto &[_, segment] : segment_map_) {
        if (segment->CheckAnyDelete(check_ts)) {
            return true;
        }
    }
    return false;
}

Status TableEntry::AddWriteTxnNum(Txn *txn) {
    std::lock_guard lock(mtx_);
    if (locked_ || wait_lock_) {
        String error_msg = fmt::format("Table {} is locked or waiting for lock", *GetTableName());
        LOG_WARN(error_msg);
        return Status::TxnRollback(txn->TxnID(), error_msg);
    }
    ++write_txn_num_;
    txn->AddWriteTxnNum(this);
    return Status::OK();
}

void TableEntry::DecWriteTxnNum() {
    std::lock_guard lock(mtx_);
    if (write_txn_num_ == 0) {
        UnrecoverableError(fmt::format("Table {} has mismatch txn num", *GetTableName()));
    }
    --write_txn_num_;
    if (write_txn_num_ == 0 && wait_lock_) {
        cv_.notify_one();
    }
}

void TableEntry::SetLocked() {
    std::unique_lock lock(mtx_);
    if (locked_) {
        LOG_WARN(fmt::format("Table {} has already been locked", *GetTableName()));
        return;
    }
    if (write_txn_num_ > 0) {
        wait_lock_ = true;
        cv_.wait(lock, [&] { return write_txn_num_ == 0; });
        wait_lock_ = false;
    }
    locked_ = true;
}

void TableEntry::SetUnlock() {
    std::lock_guard lock(mtx_);
    if (!locked_) {
        LOG_WARN(fmt::format("Table {} is not locked", *GetTableName()));
        return;
    }
    locked_ = false;
}

void TableEntry::AddColumns(const Vector<SharedPtr<ColumnDef>> &column_defs, const Vector<Value> &default_values, TxnTableStore *txn_table_store) {
    Vector<Pair<ColumnID, const Value *>> columns_info;
    for (SizeT idx = 0; idx < column_defs.size(); ++idx) {
        const auto &column_def = column_defs[idx];
        column_def->id_ = next_column_id_++;
        columns_.push_back(column_def);
        columns_info.emplace_back(column_defs[idx]->id(), &default_values[idx]);
    }
    for (auto &[segment_id, segment_entry] : segment_map_) {
        segment_entry->AddColumns(columns_info, txn_table_store);
    }
}

void TableEntry::DropColumns(const Vector<String> &column_names, TxnTableStore *txn_table_store) {
    Vector<ColumnID> column_ids;
    for (const String &column_name : column_names) {
        ColumnID column_id = GetColumnIdByName(column_name);
        column_ids.push_back(column_id);
        std::erase_if(columns_, [&](const SharedPtr<ColumnDef> &column_def) { return static_cast<ColumnID>(column_def->id()) == column_id; });
    }
    for (auto &[segment_id, segment_entry] : segment_map_) {
        segment_entry->DropColumns(column_ids, txn_table_store);
    }
}

} // namespace infinity
