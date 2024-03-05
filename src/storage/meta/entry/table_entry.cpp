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
import compact_segments_task;

namespace infinity {

TableEntry::TableEntry(bool is_delete,
                       const SharedPtr<String> &db_entry_dir,
                       SharedPtr<String> table_collection_name,
                       const Vector<SharedPtr<ColumnDef>> &columns,
                       TableEntryType table_entry_type,
                       TableMeta *table_meta,
                       TransactionID txn_id,
                       TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTable, is_delete), table_meta_(table_meta),
      table_entry_dir_(is_delete ? nullptr : TableEntry::DetermineTableDir(*db_entry_dir, *table_collection_name)),
      table_name_(std::move(table_collection_name)), columns_(columns), table_entry_type_(table_entry_type) {
    SizeT column_count = columns.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        column_name2column_id_[columns[idx]->name()] = idx;
    }

    begin_ts_ = begin_ts;
    txn_id_ = txn_id;

    // SetCompactionAlg(nullptr);
    this->SetCompactionAlg(MakeUnique<DBTCompactionAlg>(DBT_COMPACTION_M, DBT_COMPACTION_C, DBT_COMPACTION_S, DEFAULT_SEGMENT_CAPACITY));
    compaction_alg_->Enable({});
}

SharedPtr<TableEntry> TableEntry::NewTableEntry(bool is_delete,
                                                const SharedPtr<String> &db_entry_dir,
                                                SharedPtr<String> table_collection_name,
                                                const Vector<SharedPtr<ColumnDef>> &columns,
                                                TableEntryType table_entry_type,
                                                TableMeta *table_meta,
                                                TransactionID txn_id,
                                                TxnTimeStamp begin_ts,
                                                TxnManager *txn_mgr) {

    auto table_entry =
        MakeShared<TableEntry>(is_delete, db_entry_dir, table_collection_name, columns, table_entry_type, table_meta, txn_id, begin_ts);
    if (txn_mgr) {
        auto *txn = txn_mgr->GetTxn(txn_id);
        if (is_delete) {
            txn->DropTableStore(table_entry.get());
        } else {
            txn->AddTableStore(table_entry.get());
        }
    }
    return table_entry;
}

SharedPtr<TableEntry> TableEntry::NewReplayTableEntry(TableMeta *table_meta,
                                                      SharedPtr<String> db_entry_dir,
                                                      SharedPtr<String> table_name,
                                                      Vector<SharedPtr<ColumnDef>> &column_defs,
                                                      TableEntryType table_entry_type,
                                                      TransactionID txn_id,
                                                      TxnTimeStamp begin_ts,
                                                      TxnTimeStamp commit_ts,
                                                      bool is_delete,
                                                      SizeT row_count) noexcept {
    auto table_entry = MakeShared<TableEntry>(is_delete, db_entry_dir, table_name, column_defs, table_entry_type, table_meta, txn_id, begin_ts);
    // TODO need to check if commit_ts influence replay catalog delta entry
    table_entry->commit_ts_.store(commit_ts);
    table_entry->row_count_.store(row_count);

    return table_entry;
}

Tuple<TableIndexEntry *, Status> TableEntry::CreateIndex(const SharedPtr<IndexBase> &index_base,
                                                         ConflictType conflict_type,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         TxnManager *txn_mgr,
                                                         bool is_replay,
                                                         String replay_table_index_dir) {
    if (index_base->index_name_->empty()) {
        // Index name shouldn't be empty
        UnrecoverableError("Attempt to create no name index.");
    }
    LOG_TRACE(fmt::format("Creating new index: {}", *index_base->index_name_));
    auto init_index_meta = [&]() { return TableIndexMeta::NewTableIndexMeta(this, index_base->index_name_); };

    auto [table_index_meta, r_lock] = index_meta_map_.GetMeta(*index_base->index_name_, std::move(init_index_meta), txn_id, begin_ts, txn_mgr);

    LOG_TRACE(fmt::format("Creating new index: {}", *index_base->index_name_));
    return table_index_meta
        ->CreateTableIndexEntry(std::move(r_lock), index_base, conflict_type, txn_id, begin_ts, txn_mgr, is_replay, replay_table_index_dir);
}

Tuple<TableIndexEntry *, Status>
TableEntry::DropIndex(const String &index_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    auto [index_meta, status, r_lock] = index_meta_map_.GetExistMeta(index_name, conflict_type);
    if (index_meta == nullptr) {
        return {nullptr, status};
    }
    return index_meta->DropTableIndexEntry(std::move(r_lock), conflict_type, txn_id, begin_ts, txn_mgr);
}

Tuple<TableIndexEntry *, Status> TableEntry::GetIndex(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [index_meta, status, r_lock] = index_meta_map_.GetExistMeta(index_name, ConflictType::kError);
    if (index_meta == nullptr) {
        return {nullptr, status};
    }
    return index_meta->GetEntry(std::move(r_lock), txn_id, begin_ts);
}

void TableEntry::RemoveIndexEntry(const String &index_name, TransactionID txn_id) {
    auto [index_meta, _, r_lock] = index_meta_map_.GetExistMeta(index_name, ConflictType::kError);
    r_lock.unlock();
    LOG_TRACE(fmt::format("Remove index entry: {}", index_name));
    return index_meta->DeleteNewEntry(txn_id);
}

void TableEntry::GetFullTextAnalyzers(TransactionID txn_id,
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

void TableEntry::Append(TransactionID txn_id, void *txn_store, BufferManager *buffer_mgr) {
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
            if (this->unsealed_segment_ == nullptr || unsealed_segment_->Room() <= 0) {
                // unsealed_segment_ is unpopulated or full
                if (unsealed_segment_ != nullptr) {
                    // in wal replay, txn->txn_mgr() will return nullptr, and the sealing task will be skipped
                    // after wal replay, the missing sealing tasks will be created
                    // this function will not change unsealed to sealed (will be changed in CommitAppend)
                    // the background task will skip and put itself into the task queue again if the segment has not been changed to sealing yet
                    unsealed_segment_->CreateTaskSetSegmentStatusSealed(this, txn->txn_mgr());
                    append_state_ptr->set_sealing_segments_.push_back(unsealed_segment_->segment_id());
                }

                SegmentID new_segment_id = this->next_segment_id_++;
                SharedPtr<SegmentEntry> new_segment = SegmentEntry::NewAppendSegmentEntry(this, new_segment_id, txn);

                this->segment_map_.emplace(new_segment_id, new_segment);
                this->unsealed_segment_ = new_segment.get();
                LOG_TRACE(fmt::format("Created a new segment {}", new_segment_id));
            }
        }
        // Append data from app_state_ptr to the buffer in segment. If append all data, then set finish.
        u64 actual_appended = unsealed_segment_->AppendData(txn_id, append_state_ptr, buffer_mgr, txn);
        LOG_TRACE(fmt::format("Segment {} is appended with {} rows", this->unsealed_segment_->segment_id(), actual_appended));
    }
    if (!append_state_ptr->set_sealing_segments_.empty()) {
        // Add a catalog delta operation to set the segment to sealing
        auto operation = MakeUnique<SetSegmentStatusSealingOp>(this, append_state_ptr->set_sealing_segments_);
        txn->AddCatalogDeltaOperation(std::move(operation));
    }
}

void TableEntry::SetSegmentSealingForAppend(const Vector<SegmentID> &set_sealing_segments) {
    for (SegmentID segment_id : set_sealing_segments) {
        SegmentEntry *segment_entry = this->segment_map_[segment_id].get();
        segment_entry->SetSealing();
    }
}

void TableEntry::CommitCreateIndex(HashMap<String, TxnIndexStore> &txn_indexes_store_, bool is_replay) {
    for (auto &[index_name, txn_index_store] : txn_indexes_store_) {
        TableIndexEntry *table_index_entry = txn_index_store.table_index_entry_;
        for (auto &[segment_id, segment_index] : txn_index_store.index_entry_map_) {
            table_index_entry->CommitCreateIndex(segment_id, segment_index, is_replay);
        }
        if (table_index_entry->fulltext_index_entry().get() != nullptr) {
            table_index_entry->CommitCreateIndex(table_index_entry->fulltext_index_entry());
        }
    }
}

Status TableEntry::Delete(TransactionID txn_id, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        SegmentID segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment_entry = GetSegmentByID(segment_id, commit_ts);
        if (segment_entry == nullptr) {
            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Going to delete data in non-exist segment: {}", segment_id));
            return Status(ErrorCode::kTableNotExist, std::move(err_msg));
        }
        const HashMap<BlockID, Vector<BlockOffset>> &block_row_hashmap = to_delete_seg_rows.second;
        segment_entry->DeleteData(txn_id, commit_ts, block_row_hashmap);
    }
    return Status::OK();
}

void TableEntry::CommitAppend(TransactionID txn_id, TxnTimeStamp commit_ts, const AppendState *append_state_ptr) {
    SizeT row_count = 0;
    for (const auto &range : append_state_ptr->append_ranges_) {
        LOG_TRACE(fmt::format("Commit, segment: {}, block: {} start offset: {}, count: {}",
                              range.segment_id_,
                              range.block_id_,
                              range.start_offset_,
                              range.row_count_));
        SegmentEntry *segment_ptr = this->segment_map_[range.segment_id_].get();
        segment_ptr->CommitAppend(txn_id, commit_ts, range.block_id_, range.start_offset_, range.row_count_);
        row_count += range.row_count_;
    }
    this->row_count_ += row_count;
    // finally, set the full unsealed segment to sealing
    SetSegmentSealingForAppend(append_state_ptr->set_sealing_segments_);
}

void TableEntry::RollbackAppend(TransactionID txn_id, TxnTimeStamp commit_ts, void *txn_store) {
    //    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    //    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    UnrecoverableError("TableEntry::RollbackAppend");
}

void TableEntry::CommitDelete(TransactionID txn_id, TxnTimeStamp commit_ts, const DeleteState &delete_state) {
    SizeT row_count = 0;
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment = GetSegmentByID(segment_id, commit_ts);
        if (segment == nullptr) {
            UnrecoverableError(fmt::format("Going to commit delete data in non-exist segment: {}", segment_id));
        }
        const HashMap<u16, Vector<BlockOffset>> &block_row_hashmap = to_delete_seg_rows.second;
        segment->CommitDelete(txn_id, commit_ts, block_row_hashmap);
        row_count += block_row_hashmap.size();
    }
    this->row_count_ -= row_count;
}

Status TableEntry::RollbackDelete(TransactionID txn_id, DeleteState &, BufferManager *) {
    UnrecoverableError("TableEntry::RollbackDelete");
    return Status::OK();
}

Status TableEntry::CommitCompact(TransactionID txn_id, TxnTimeStamp commit_ts, TxnCompactStore &compact_store) {
    if (compact_store.segment_data_.empty()) {
        return Status::OK();
    }
    for (auto &[new_segment, old_segments] : compact_store.segment_data_) {
        auto status = CommitSegment(commit_ts, new_segment);
        if (!status.ok()) {
            // TODO: rollback
            return status;
        }
    }
    {
        {
            String ss = "Commit compact: ";
            for (const auto &[new_segment, old_segments] : compact_store.segment_data_) {
                ss += "new segment: " + std::to_string(new_segment->segment_id_) + " old segment: ";
                for (const auto *old_segment : old_segments) {
                    ss += std::to_string(old_segment->segment_id_) + " ";
                }
            }
            LOG_INFO(ss);
        }
        std::unique_lock lock(this->rw_locker());
        for (const auto &[new_segment, old_segments] : compact_store.segment_data_) {
            auto [_, insert_ok] = this->segment_map_.emplace(new_segment->segment_id_, new_segment);
            if (!insert_ok) {
                UnrecoverableError(fmt::format("Insert new segment {} failed.", new_segment->segment_id()));
            }
            for (const auto &old_segment : old_segments) {
                old_segment->SetDeprecated(commit_ts);
            }
        }
    }

    if (compaction_alg_.get() == nullptr) {
        return Status::OK();
    }

    Vector<SegmentEntry *> new_segments;
    std::transform(compact_store.segment_data_.begin(), compact_store.segment_data_.end(), std::back_inserter(new_segments), [](const auto &pair) {
        return pair.first.get();
    });
    switch (compact_store.task_type_) {
        case CompactSegmentsTaskType::kCompactPickedSegments: {
            compaction_alg_->CommitCompact(new_segments, txn_id);
            break;
        }
        case CompactSegmentsTaskType::kCompactTable: {
            //  reinitialize compaction_alg_ with new segments and enable it
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
    if (compact_store.segment_data_.empty()) {
        return Status::OK();
    }
    for (const auto &[new_segment, old_segments] : compact_store.segment_data_) {
        std::unique_lock lock(this->rw_locker());
        for (const auto &old_segment : old_segments) {
            old_segment->RollbackCompact();
        }
    }
    if (compaction_alg_.get() == nullptr) {
        return Status::OK();
    }

    switch (compact_store.task_type_) {
        case CompactSegmentsTaskType::kCompactPickedSegments: {
            compaction_alg_->RollbackCompact(txn_id);
            break;
        }
        case CompactSegmentsTaskType::kCompactTable: {
            Vector<SegmentEntry *> new_segments;
            std::transform(compact_store.segment_data_.begin(),
                           compact_store.segment_data_.end(),
                           std::back_inserter(new_segments),
                           [](const auto &pair) { return pair.first.get(); });
            compaction_alg_->Enable(new_segments);
            break;
        }
        default: {
            UnrecoverableError("Invalid compact task type");
        }
    }
    return Status::OK();
}

Status TableEntry::CommitImport(TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment) {
    auto status = CommitSegment(commit_ts, segment);
    if (!status.ok()) {
        return status;
    }
    std::unique_lock lock(this->rw_locker());
    row_count_ += segment->row_count();
    this->segment_map_.emplace(segment->segment_id_, std::move(segment));
    return Status::OK();
}

Status TableEntry::CommitSegment(TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> &segment) {
    if (this->deleted_) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Table {} is deleted.", *this->GetTableName()));
        return Status(ErrorCode::kTableNotExist, std::move(err_msg));
    }

    segment->min_row_ts_ = commit_ts;
    // FIXME: max_row_ts is set when the segment is deprecated

    for (auto &block_entry : segment->block_entries_) {
        block_entry->min_row_ts_ = commit_ts;
        block_entry->max_row_ts_ = commit_ts;
        // ATTENTION: Do not modify the block_entry checkpoint_ts_
        block_entry->block_version_->created_.emplace_back(commit_ts, block_entry->row_count());
    }

    return Status::OK();
}

SegmentEntry *TableEntry::GetSegmentByID(SegmentID segment_id, TxnTimeStamp ts) const {
    try {
        std::shared_lock lock(this->rw_locker());
        auto *segment = segment_map_.at(segment_id).get();
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

    UniquePtr<TableEntry> table_entry =
        MakeUnique<TableEntry>(deleted, table_entry_dir, table_name, columns, table_entry_type, table_meta, txn_id, begin_ts);
    table_entry->row_count_ = row_count;
    table_entry->next_segment_id_ = table_entry_json["next_segment_id"];

    if (table_entry_json.contains("segments")) {
        for (const auto &segment_json : table_entry_json["segments"]) {
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
            table_entry->segment_map_.emplace(segment_entry->segment_id(), segment_entry);
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

void TableEntry::MergeFrom(BaseEntry &other) {
    auto table_entry2 = dynamic_cast<TableEntry *>(&other);
    if (table_entry2 == nullptr) {
        UnrecoverableError("MergeFrom requires the same type of BaseEntry");
    }
    // // No locking here since only the load stage needs MergeFrom.
    if (*this->table_name_ != *table_entry2->table_name_) {
        UnrecoverableError("DBEntry::MergeFrom requires table_name_ match");
    }
    if (*this->table_entry_dir_ != *table_entry2->table_entry_dir_) {
        UnrecoverableError("DBEntry::MergeFrom requires table_entry_dir_ match");
    }
    if (this->table_entry_type_ != table_entry2->table_entry_type_) {
        UnrecoverableError("DBEntry::MergeFrom requires table_entry_dir_ match");
    }

    this->next_segment_id_.store(std::max(this->next_segment_id_, table_entry2->next_segment_id_));
    this->row_count_.store(std::max(this->row_count_, table_entry2->row_count_));

    for (auto &[seg_id, sgement_entry2] : table_entry2->segment_map_) {
        auto it = this->segment_map_.find(seg_id);
        if (it == this->segment_map_.end()) {
            sgement_entry2->table_entry_ = this;
            this->segment_map_.emplace(seg_id, sgement_entry2);
        } else {
            it->second->MergeFrom(*sgement_entry2.get());
        }
    }

    for (auto &[index_name, table_index_meta] : table_entry2->index_meta_map()) {
        auto it = this->index_meta_map().find(index_name);
        if (it == this->index_meta_map().end()) {
            this->index_meta_map().emplace(index_name, std::move(table_index_meta));
        } else {
            it->second->MergeFrom(*table_index_meta.get());
        }
    }
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
            SharedPtr<SegmentEntry> &segment = iter->second;
            if (segment->CheckDeprecate(visible_ts)) {
                cleanup_segment_ids.push_back(iter->first);
                scanner->AddEntry(std::move(segment));
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
