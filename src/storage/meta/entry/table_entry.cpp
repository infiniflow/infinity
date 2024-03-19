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

#include <stdexcept>
#include <string>

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
import iresearch_datastore;
import index_base;
import index_full_text;
import catalog_delta_entry;
import extra_ddl_info;
import create_index_info;
import column_def;
import data_type;
import default_values;
import DBT_compaction_alg;
import compact_segments_task;
import local_file_system;
import build_fast_rough_filter_task;
import block_entry;

namespace infinity {

TableEntry::TableEntry(bool is_delete,
                       const SharedPtr<String> &table_entry_dir,
                       SharedPtr<String> table_name,
                       const Vector<SharedPtr<ColumnDef>> &columns,
                       TableEntryType table_entry_type,
                       TableMeta *table_meta,
                       TransactionID txn_id,
                       TxnTimeStamp begin_ts,
                       SegmentID unsealed_id)
    : BaseEntry(EntryType::kTable, is_delete), table_meta_(table_meta), table_entry_dir_(std::move(table_entry_dir)),
      table_name_(std::move(table_name)), columns_(columns), table_entry_type_(table_entry_type), unsealed_id_(unsealed_id) {
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;

    SizeT column_count = columns.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        column_name2column_id_[columns[idx]->name()] = idx;
    }

    // this->SetCompactionAlg(nullptr);
    if (!is_delete) {
        this->SetCompactionAlg(MakeUnique<DBTCompactionAlg>(DBT_COMPACTION_M, DBT_COMPACTION_C, DBT_COMPACTION_S, DEFAULT_SEGMENT_CAPACITY, this));
        compaction_alg_->Enable({});
    }
}

SharedPtr<TableEntry> TableEntry::NewTableEntry(bool is_delete,
                                                const SharedPtr<String> &db_entry_dir,
                                                SharedPtr<String> table_name,
                                                const Vector<SharedPtr<ColumnDef>> &columns,
                                                TableEntryType table_entry_type,
                                                TableMeta *table_meta,
                                                TransactionID txn_id,
                                                TxnTimeStamp begin_ts) {
    SharedPtr<String> table_entry_dir = is_delete ? MakeShared<String>("deleted") : TableEntry::DetermineTableDir(*db_entry_dir, *table_name);
    return MakeShared<TableEntry>(is_delete,
                                  std::move(table_entry_dir),
                                  std::move(table_name),
                                  columns,
                                  table_entry_type,
                                  table_meta,
                                  txn_id,
                                  begin_ts,
                                  0);
}

SharedPtr<TableEntry> TableEntry::ReplayTableEntry(TableMeta *table_meta,
                                                   SharedPtr<String> table_entry_dir,
                                                   SharedPtr<String> table_name,
                                                   Vector<SharedPtr<ColumnDef>> &column_defs,
                                                   TableEntryType table_entry_type,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnTimeStamp commit_ts,
                                                   bool is_delete,
                                                   SizeT row_count,
                                                   SegmentID unsealed_id) noexcept {
    auto table_entry = MakeShared<TableEntry>(is_delete,
                                              std::move(table_entry_dir),
                                              std::move(table_name),
                                              column_defs,
                                              table_entry_type,
                                              table_meta,
                                              txn_id,
                                              begin_ts,
                                              unsealed_id);
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
        UnrecoverableError("Attempt to create no name index.");
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
    return index_meta->DropTableIndexEntry(std::move(r_lock), conflict_type, txn_id, begin_ts, txn_mgr);
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

TableIndexEntry *TableEntry::CreateIndexReplay(
    const SharedPtr<String> &index_name,
    std::function<SharedPtr<TableIndexEntry>(TableIndexMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    TransactionID txn_id,
    TxnTimeStamp begin_ts) {
    auto init_index_meta = [&]() { return TableIndexMeta::NewTableIndexMeta(this, index_name); };
    auto *index_meta = index_meta_map_.GetMetaNoLock(*index_name, std::move(init_index_meta));
    return index_meta->CreateEntryReplay(std::move(init_entry), txn_id, begin_ts);
}

void TableEntry::DropIndexReplay(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [index_meta, status] = index_meta_map_.GetExistMetaNoLock(index_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    index_meta->DropEntryReplay(txn_id, begin_ts);
}

TableIndexEntry *TableEntry::GetIndexReplay(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [index_meta, status] = index_meta_map_.GetExistMetaNoLock(index_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return index_meta->GetEntryReplay(txn_id, begin_ts);
}

void TableEntry::AddSegmentReplay(std::function<SharedPtr<SegmentEntry>()> &&init_segment, SegmentID segment_id) {
    SharedPtr<SegmentEntry> new_segment = init_segment();
    if (new_segment->status() == SegmentStatus::kDeprecated) {
        auto iter = segment_map_.find(segment_id);
        if (iter == segment_map_.end()) {
            return;
        }
        iter->second->Cleanup();
        segment_map_.erase(iter);
        return;
    }
    segment_map_[segment_id] = new_segment;
    if (segment_id == unsealed_id_) {
        unsealed_segment_ = std::move(new_segment);
    }
}

void TableEntry::GetFulltextAnalyzers(TransactionID txn_id,
                                      TxnTimeStamp begin_ts,
                                      SharedPtr<FulltextIndexEntry> &fulltext_index_entry,
                                      Map<String, String> &column2analyzer) {
    column2analyzer.clear();
    {
        auto index_meta_map_guard = index_meta_map_.GetMetaMap();
        for (auto &[_, table_index_meta] : *index_meta_map_guard) {
            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (status.ok()) {
                fulltext_index_entry = table_index_entry->fulltext_index_entry();
                const IndexBase *index_base = table_index_entry->index_base();
                if (index_base->index_type_ != IndexType::kFullText)
                    continue;
                auto index_full_text = static_cast<const IndexFullText *>(index_base);
                if (index_full_text->homebrewed_) {
                    // set fulltext_index_entry to nullptr to indicate that the index is homebrewed
                    fulltext_index_entry = nullptr;
                    column2analyzer.clear();
                    return;
                }
                for (auto &column_name : index_full_text->column_names_) {
                    column2analyzer[column_name] = index_full_text->analyzer_;
                }
                if (!column2analyzer.empty()) {
                    // iresearch requires there is exactly one full index per table.
                    break;
                }
            }
        }
    }
}

void TableEntry::Import(SharedPtr<SegmentEntry> segment_entry) {
    std::unique_lock lock(this->rw_locker());
    SegmentID segment_id = segment_entry->segment_id();
    SizeT row_count = segment_entry->row_count();
    auto [_, insert_ok] = this->segment_map_.emplace(segment_id, std::move(segment_entry));
    if (!insert_ok) {
        UnrecoverableError(fmt::format("Insert segment {} failed.", segment_id));
    }
    this->row_count_ += row_count;
}

void TableEntry::AddCompactNew(SharedPtr<SegmentEntry> segment_entry) {
    std::unique_lock lock(this->rw_locker());
    SegmentID segment_id = segment_entry->segment_id();
    auto [_, insert_ok] = this->segment_map_.emplace(segment_id, std::move(segment_entry));
    if (!insert_ok) {
        UnrecoverableError(fmt::format("Insert segment {} failed.", segment_id));
    }
}

void TableEntry::Append(TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, BufferManager *buffer_mgr) {
    SizeT row_count = 0;

    if (this->deleted_) {
        UnrecoverableError("table is deleted");
        return;
    }
    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    Txn *txn = txn_store_ptr->txn_;
    if (append_state_ptr->Finished()) {
        LOG_TRACE("No append is done.");
        return;
    }

    while (!append_state_ptr->Finished()) {
        {
            std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker()); // prevent another read conflict with this append operation
            if (unsealed_segment_.get() == nullptr || unsealed_segment_->Room() <= 0) {
                // unsealed_segment_ is unpopulated or full
                if (unsealed_segment_) {
                    txn_store_ptr->AddSealedSegment(unsealed_segment_.get());
                }

                SegmentID new_segment_id = this->next_segment_id_++;

                this->unsealed_segment_ = SegmentEntry::NewSegmentEntry(this, new_segment_id, txn);
                unsealed_id_ = unsealed_segment_->segment_id();
                this->segment_map_.emplace(new_segment_id, this->unsealed_segment_);
                LOG_TRACE(fmt::format("Created a new segment {}", new_segment_id));
            }
        }
        u64 actual_appended = unsealed_segment_->AppendData(txn_id, commit_ts, append_state_ptr, buffer_mgr, txn);

        LOG_TRACE(fmt::format("Segment {} is appended with {} rows", this->unsealed_segment_->segment_id(), actual_appended));
        row_count += actual_appended;
    }

    this->row_count_ += row_count;
}

Status TableEntry::Delete(TransactionID txn_id, void *txn_store, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    SizeT row_count = 0;

    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    Txn *txn = txn_store_ptr->txn_;
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        SegmentID segment_id = to_delete_seg_rows.first;
        SharedPtr<SegmentEntry> segment_entry = GetSegmentByID(segment_id, commit_ts);
        if (!segment_entry) {
            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Going to delete data in non-exist segment: {}", segment_id));
            return Status(ErrorCode::kTableNotExist, std::move(err_msg));
        }
        const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap = to_delete_seg_rows.second;
        segment_entry->DeleteData(txn_id, commit_ts, block_row_hashmap, txn);

        row_count += block_row_hashmap.size();
    }
    this->row_count_ -= row_count;
    return Status::OK();
}

void TableEntry::RollbackAppend(TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store) {
    //    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    //    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    UnrecoverableError("TableEntry::RollbackAppend");
}

Status TableEntry::RollbackDelete(TransactionID txn_id, DeleteState &, BufferManager *) {
    UnrecoverableError("TableEntry::RollbackDelete");
    return Status::OK();
}

Status TableEntry::CommitCompact(TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_store) {
    if (compact_store.task_type_ == CompactSegmentsTaskType::kInvalid) {
        return Status::OK();
    }

    Vector<SegmentEntry *> new_segments;
    {
        {
            String ss = "Compact commit: " + *this->GetTableName();
            for (const auto &[segment_store, old_segments] : compact_store.compact_data_) {
                auto *new_segment = segment_store.segment_entry_;
                ss += ", new segment: " + std::to_string(new_segment->segment_id()) + ", old segment: ";
                for (const auto *old_segment : old_segments) {
                    ss += std::to_string(old_segment->segment_id_) + " ";
                }
            }
            LOG_INFO(ss);
        }
        std::unique_lock lock(this->rw_locker());
        for (const auto &[segment_store, old_segments] : compact_store.compact_data_) {

            auto *segment_entry = segment_store.segment_entry_;
            new_segments.push_back(segment_entry);

            segment_entry->CommitSegment(txn_id, commit_ts);
            for (auto *block_entry : segment_store.block_entries_) {
                block_entry->CommitBlock(txn_id, commit_ts);
            }

            for (const auto &old_segment : old_segments) {
                // old_segment->TrySetDeprecated(commit_ts);
                old_segment->SetDeprecated(commit_ts);
            }
        }
    }

    if (compaction_alg_.get() == nullptr) {
        return Status::OK();
    }

    switch (compact_store.task_type_) {
        case CompactSegmentsTaskType::kCompactPickedSegments: {
            compaction_alg_->CommitCompact(txn_id);
            LOG_INFO(fmt::format("Compact commit picked, tablename: {}", *this->GetTableName()));
            break;
        }
        case CompactSegmentsTaskType::kCompactTable: {
            //  reinitialize compaction_alg_ with new segments and enable it
            LOG_INFO(fmt::format("Compact commit whole, tablename: {}", *this->GetTableName()));
            compaction_alg_->Enable(new_segments);
            break;
        }
        default: {
            UnrecoverableError("Invalid compact task type");
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
                UnrecoverableError(fmt::format("RollbackCompact: segment {} is committed", segment_entry->segment_id()));
            }
            {
                std::unique_lock lock(this->rw_locker());
                if (auto iter = segment_map_.find(segment_entry->segment_id()); iter != segment_map_.end()) {
                    segment = std::move(iter->second);
                    segment_map_.erase(iter);
                } else {
                    UnrecoverableError(fmt::format("RollbackCompact: segment {} not found", segment_entry->segment_id()));
                }

                for (auto *old_segment : old_segments) {
                    old_segment->RollbackCompact();
                }
            }
            segment->Cleanup();
        }
    }
    if (compaction_alg_.get() != nullptr) {
        switch (compact_store.task_type_) {
            case CompactSegmentsTaskType::kCompactPickedSegments: {
                compaction_alg_->RollbackCompact(txn_id);
                break;
            }
            case CompactSegmentsTaskType::kCompactTable: {
                Vector<SegmentEntry *> old_segments;
                for (const auto &[_, old_segs] : compact_store.compact_data_) {
                    old_segments.insert(old_segments.end(), old_segs.begin(), old_segs.end());
                }
                compaction_alg_->Enable(old_segments);
                break;
            }
            default: {
                UnrecoverableError("Invalid compact task type");
            }
        }
    }

    return Status::OK();
}

Status TableEntry::CommitWrite(TransactionID txn_id, TxnTimeStamp commit_ts, const HashMap<SegmentID, TxnSegmentStore> &segment_stores) {
    {
        std::unique_lock w_lock(this->rw_locker());
        for (const auto &[segment_id, segment_store] : segment_stores) {
            auto *segment_entry = segment_store.segment_entry_;
            segment_entry->CommitSegment(txn_id, commit_ts);
            for (auto *block_entry : segment_store.block_entries_) {
                block_entry->CommitBlock(txn_id, commit_ts);
            }
        }
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
                std::unique_lock w_lock(this->rw_locker());
                if (auto iter = segment_map_.find(segment_entry->segment_id()); iter != segment_map_.end()) {
                    segment = std::move(iter->second);
                    segment_map_.erase(iter);
                } else {
                    UnrecoverableError(fmt::format("RollbackWrite: segment {} not found", segment_entry->segment_id()));
                }
            }
            segment->Cleanup();
        }
    }
    return Status::OK();
}

void TableEntry::MemIndexInsert(Txn *txn, SharedPtr<BlockEntry> block_entry, u32 row_offset, u32 row_count) {
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
        if (status.ok()) {
            table_index_entry->MemIndexInsert(txn, block_entry, row_offset, row_count);
        }
    }
}

void TableEntry::MemIndexDump(Txn *txn, bool spill) {
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn->TxnID(), txn->BeginTS());
        if (status.ok()) {
            table_index_entry->MemIndexDump(spill);
        }
    }
}

void TableEntry::MemIndexCommit() {
    auto index_meta_map_guard = index_meta_map_.GetMetaMap();
    for (auto &[_, table_index_meta] : *index_meta_map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(0UL, 0UL);
        if (status.ok()) {
            table_index_entry->MemIndexCommit();
        }
    }
}

SharedPtr<SegmentEntry> TableEntry::GetSegmentByID(SegmentID segment_id, TxnTimeStamp ts) const {
    try {
        std::shared_lock lock(this->rw_locker());
        auto segment = segment_map_.at(segment_id);
        if ( // TODO: read deprecate segment is allowed
             // segment->deprecate_ts() < ts ||
            segment->min_row_ts() > ts) {
            return nullptr;
        }
        return segment;
    } catch (const std::out_of_range &e) {
        return nullptr;
    }
}

Pair<SizeT, Status> TableEntry::GetSegmentRowCountBySegmentID(u32 seg_id) {
    auto iter = this->segment_map_.find(seg_id);
    if (iter != this->segment_map_.end()) {
        return {iter->second->row_count(), Status::OK()};
    } else {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("No segment id: {}.", seg_id));
        LOG_ERROR(*err_msg);
        UnrecoverableError(*err_msg);
        return {0, Status(ErrorCode::kUnexpectedError, std::move(err_msg))};
    }
}

const SharedPtr<String> &TableEntry::GetDBName() const { return table_meta_->db_name_ptr(); }

SharedPtr<BlockIndex> TableEntry::GetBlockIndex(TxnTimeStamp begin_ts) {
    //    SharedPtr<MultiIndex<u64, u64, SegmentEntry*>> result = MakeShared<MultiIndex<u64, u64, SegmentEntry*>>();
    SharedPtr<BlockIndex> result = MakeShared<BlockIndex>();
    std::shared_lock<std::shared_mutex> rw_locker(this->rw_locker());
    result->Reserve(this->segment_map_.size());

    // Add segment that is not deprecated
    for (const auto &segment_pair : this->segment_map_) {
        result->Insert(segment_pair.second.get(), begin_ts);
    }

    return result;
}

nlohmann::json TableEntry::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;

    Vector<SegmentEntry *> segment_candidates;
    Vector<TableIndexMeta *> table_index_meta_candidates;
    Vector<String> table_index_name_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["table_name"] = *this->GetTableName();
        json_res["table_entry_type"] = this->table_entry_type_;
        json_res["row_count"] = this->row_count_.load();
        json_res["begin_ts"] = this->begin_ts_;
        json_res["commit_ts"] = this->commit_ts_.load();
        json_res["txn_id"] = this->txn_id_.load();
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

                json_res["column_definition"].emplace_back(column_def_json);
            }
        }
        u32 next_segment_id = this->next_segment_id_;
        json_res["next_segment_id"] = next_segment_id;

        segment_candidates.reserve(this->segment_map_.size());
        for (const auto &[segment_id, segment_entry] : this->segment_map_) {
            segment_candidates.emplace_back(segment_entry.get());
        }

        table_index_meta_candidates.reserve(this->index_meta_map().size());
        table_index_name_candidates.reserve(this->index_meta_map().size());
        for (const auto &[index_name, table_index_meta] : this->index_meta_map()) {
            table_index_meta_candidates.emplace_back(table_index_meta.get());
            table_index_name_candidates.emplace_back(index_name);
        }
    }

    // Serialize segments
    for (const auto &segment_entry : segment_candidates) {
        json_res["segments"].emplace_back(segment_entry->Serialize(max_commit_ts, is_full_checkpoint));
    }
    json_res["unsealed_id"] = unsealed_id_;

    // Serialize indexes
    SizeT table_index_count = table_index_meta_candidates.size();
    for (SizeT idx = 0; idx < table_index_count; ++idx) {
        TableIndexMeta *table_index_meta = table_index_meta_candidates[idx];
        nlohmann::json index_def_meta_json = table_index_meta->Serialize(max_commit_ts);
        index_def_meta_json["index_name"] = table_index_name_candidates[idx];
        json_res["table_indexes"].emplace_back(index_def_meta_json);
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

            HashSet<ConstraintType> constraints;
            if (column_def_json.contains("constraints")) {
                for (const auto &column_constraint : column_def_json["constraints"]) {
                    ConstraintType constraint = column_constraint;
                    constraints.emplace(constraint);
                }
            }

            SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints);
            columns.emplace_back(column_def);
        }
        row_count = table_entry_json["row_count"];
    }

    TransactionID txn_id = table_entry_json["txn_id"];
    TxnTimeStamp begin_ts = table_entry_json["begin_ts"];
    SegmentID unsealed_id = table_entry_json["unsealed_id"];

    UniquePtr<TableEntry> table_entry =
        MakeUnique<TableEntry>(deleted, table_entry_dir, table_name, columns, table_entry_type, table_meta, txn_id, begin_ts, unsealed_id);
    table_entry->row_count_ = row_count;
    table_entry->next_segment_id_ = table_entry_json["next_segment_id"];

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
    table_entry->deleted_ = deleted;

    if (table_entry->deleted_) {
        if (!table_entry->segment_map_.empty()) {
            UnrecoverableError("deleted table should have no segment");
        }
    }

    if (table_entry_json.contains("table_indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["table_indexes"]) {

            UniquePtr<TableIndexMeta> table_index_meta = TableIndexMeta::Deserialize(index_def_meta_json, table_entry.get(), buffer_mgr);
            String index_name = index_def_meta_json["index_name"];
            table_entry->index_meta_map().emplace(std::move(index_name), std::move(table_index_meta));
        }
    }

    return table_entry;
}

u64 TableEntry::GetColumnIdByName(const String &column_name) const {
    auto it = column_name2column_id_.find(column_name);
    if (it == column_name2column_id_.end()) {
        RecoverableError(Status::ColumnNotExist(column_name));
    }
    return it->second;
}

bool TableEntry::CheckDeleteConflict(const Vector<RowID> &delete_row_ids, TransactionID txn_id) {
    HashMap<SegmentID, Vector<SegmentOffset>> delete_row_map;
    for (const auto row_id : delete_row_ids) {
        delete_row_map[row_id.segment_id_].emplace_back(row_id.segment_offset_);
    }
    Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> check_segments;
    std::shared_lock lock(this->rw_locker());
    for (const auto &[segment_id, segment_offsets] : delete_row_map) {
        check_segments.emplace_back(this->segment_map_.at(segment_id).get(), std::move(segment_offsets));
    }

    return SegmentEntry::CheckDeleteConflict(std::move(check_segments), txn_id);
}

void TableEntry::WalReplaySegment(SharedPtr<SegmentEntry> segment_entry) {
    this->DeltaReplaySegment(std::move(segment_entry));
    // ATTENTION: focusing on the segment id
    next_segment_id_++;
}

void TableEntry::DeltaReplaySegment(SharedPtr<SegmentEntry> segment_entry) {
    if (compaction_alg_.get() != nullptr) {
        compaction_alg_->AddSegmentNoCheck(segment_entry.get());
    }
    segment_map_.emplace(segment_entry->segment_id(), std::move(segment_entry));
}

Optional<Pair<Vector<SegmentEntry *>, Txn *>> TableEntry::TryCompactAddSegment(SegmentEntry *new_segment, std::function<Txn *()> generate_txn) {
    if (compaction_alg_.get() == nullptr) {
        return None;
    }
    return compaction_alg_->AddSegment(new_segment, generate_txn);
}

Optional<Pair<Vector<SegmentEntry *>, Txn *>> TableEntry::TryCompactDeleteRow(SegmentID segment_id, std::function<Txn *()> generate_txn) {
    if (compaction_alg_.get() == nullptr) {
        return None;
    }
    return compaction_alg_->DeleteInSegment(segment_id, generate_txn);
}

Vector<SegmentEntry *> TableEntry::PickCompactSegments() const {
    if (compaction_alg_.get() != nullptr) {
        compaction_alg_->Disable(); // wait for current compaction to finish
    }
    Vector<SegmentEntry *> result;
    std::shared_lock lock(this->rw_locker());
    for (const auto &[segment_id, segment] : this->segment_map_) {
        auto status = segment->status();
        if (status == SegmentStatus::kSealed) {
            result.emplace_back(segment.get());
        } else if (status == SegmentStatus::kCompacting || status == SegmentStatus::kNoDelete) {
            UnrecoverableError("Segment should not be compacting or no delete when picking manually");
        }
    }
    return result;
}

void TableEntry::PickCleanup(CleanupScanner *scanner) {
    index_meta_map_.PickCleanup(scanner);
    Vector<SegmentID> cleanup_segment_ids;
    {
        std::unique_lock lock(this->rw_locker());
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

void TableEntry::Cleanup() {
    if (this->deleted_) {
        return;
    }
    for (auto &[segment_id, segment] : segment_map_) {
        segment->Cleanup();
    }
    index_meta_map_.Cleanup();

    LOG_INFO(fmt::format("Cleanup dir: {}", *table_entry_dir_));
    LocalFileSystem fs;
    fs.DeleteEmptyDirectory(*table_entry_dir_);
}

} // namespace infinity
