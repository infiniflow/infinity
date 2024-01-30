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

#include <string>

module catalog;

import stl;
import parser;
import table_entry_type;
import third_party;
import txn;
import buffer_manager;
import index_def;
import block_index;
import data_access_state;

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

namespace infinity {

TableEntry::TableEntry(const SharedPtr<String> &db_entry_dir,
                       SharedPtr<String> table_collection_name,
                       const Vector<SharedPtr<ColumnDef>> &columns,
                       TableEntryType table_entry_type,
                       TableMeta *table_meta,
                       TransactionID txn_id,
                       TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTable), table_meta_(table_meta),
      table_entry_dir_(MakeShared<String>(fmt::format("{}/{}/txn_{}", *db_entry_dir, *table_collection_name, txn_id))),
      table_name_(std::move(table_collection_name)), columns_(columns), table_entry_type_(table_entry_type) {
    SizeT column_count = columns.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        column_name2column_id_[columns[idx]->name()] = idx;
    }

    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

SharedPtr<TableEntry> TableEntry::NewTableEntry(const SharedPtr<String> &db_entry_dir,
                                                SharedPtr<String> table_collection_name,
                                                const Vector<SharedPtr<ColumnDef>> &columns,
                                                TableEntryType table_entry_type,
                                                TableMeta *table_meta,
                                                TransactionID txn_id,
                                                TxnTimeStamp begin_ts) {

    auto table_entry = MakeShared<TableEntry>(db_entry_dir, table_collection_name, columns, table_entry_type, table_meta, txn_id, begin_ts);
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
                                                      SizeT row_count) {
    auto table_entry = MakeShared<TableEntry>(db_entry_dir, table_name, column_defs, table_entry_type, table_meta, txn_id, begin_ts);
    table_entry->deleted_ = is_delete;
    // TODO need to check if commit_ts influence replay catalog delta entry
    table_entry->commit_ts_.store(commit_ts);
    table_entry->row_count_.store(row_count);

    return table_entry;
}

Tuple<TableIndexEntry *, Status> TableEntry::CreateIndex(const SharedPtr<IndexDef> &index_def,
                                                         ConflictType conflict_type,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts,
                                                         TxnManager *txn_mgr,
                                                         bool is_replay,
                                                         String replay_table_index_dir) {
    if (index_def->index_name_->empty()) {
        // Index name shouldn't be empty
        UnrecoverableError("Attempt to create no name index.");
    }

    TableIndexMeta *table_index_meta{nullptr};

    // TODO: lock granularity can be narrowed.
    this->rw_locker_.lock_shared();
    if (auto iter = this->index_meta_map_.find(*index_def->index_name_); iter != this->index_meta_map_.end()) {
        table_index_meta = iter->second.get();
    }
    this->rw_locker_.unlock_shared();

    if (table_index_meta == nullptr) {

        UniquePtr<TableIndexMeta> new_table_index_meta = TableIndexMeta::NewTableIndexMeta(this, index_def->index_name_);

        { //
            if (txn_mgr != nullptr) {
                auto operation = MakeUnique<AddIndexMetaOp>(new_table_index_meta.get(), begin_ts);
                txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
            }
        }

        table_index_meta = new_table_index_meta.get();

        this->rw_locker_.lock();

        if (auto iter = this->index_meta_map_.find(*index_def->index_name_); iter != this->index_meta_map_.end()) {
            table_index_meta = iter->second.get();
        } else {

            this->index_meta_map_[*index_def->index_name_] = std::move(new_table_index_meta);
        }
        this->rw_locker_.unlock();
    }

    LOG_TRACE(fmt::format("Creating new index: {}", *index_def->index_name_));
    return table_index_meta->CreateTableIndexEntry(index_def, conflict_type, txn_id, begin_ts, txn_mgr, is_replay, replay_table_index_dir);
}

Tuple<TableIndexEntry *, Status>
TableEntry::DropIndex(const String &index_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    this->rw_locker_.lock_shared();

    TableIndexMeta *index_meta{nullptr};
    if (auto iter = this->index_meta_map_.find(index_name); iter != this->index_meta_map_.end()) {
        index_meta = iter->second.get();
    }
    this->rw_locker_.unlock_shared();
    if (index_meta == nullptr) {
        switch (conflict_type) {

            case ConflictType::kIgnore: {
                return {nullptr, Status::OK()};
            }
            case ConflictType::kError: {
                String error_message = fmt::format("Attempt to drop not existed index entry {}", index_name);
                LOG_TRACE(error_message);
                return {nullptr, Status(ErrorCode::kIndexNotExist, error_message.c_str())};
            }
            default: {
                UnrecoverableError("Invalid conflict type.");
            }
        }
        UnrecoverableError("Should not reach here.");
    }
    LOG_TRACE(fmt::format("Drop index entry {}", index_name));
    return index_meta->DropTableIndexEntry(conflict_type, txn_id, begin_ts, txn_mgr);
}

Tuple<TableIndexEntry *, Status> TableEntry::GetIndex(const String &index_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    if (auto iter = this->index_meta_map_.find(index_name); iter != this->index_meta_map_.end()) {
        return iter->second->GetEntry(txn_id, begin_ts);
    }
    UniquePtr<String> err_msg = MakeUnique<String>("Cannot find index def");
    LOG_ERROR(*err_msg);
    return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg))};
}

void TableEntry::RemoveIndexEntry(const String &index_name, TransactionID txn_id, TxnManager *txn_mgr) {
    this->rw_locker_.lock_shared();

    TableIndexMeta *table_index_meta{nullptr};
    if (auto iter = this->index_meta_map_.find(index_name); iter != this->index_meta_map_.end()) {
        table_index_meta = iter->second.get();
    }
    this->rw_locker_.unlock_shared();

    LOG_TRACE(fmt::format("Remove index entry: {}", index_name));
    table_index_meta->DeleteNewEntry(txn_id, txn_mgr);
}

void TableEntry::GetFullTextAnalyzers(TransactionID txn_id,
                                      TxnTimeStamp begin_ts,
                                      SharedPtr<IrsIndexEntry> &irs_index_entry,
                                      Map<String, String> &column2analyzer) {
    column2analyzer.clear();
    for (auto &[_, table_index_meta] : this->index_meta_map_) {
        auto [table_index_entry, status] = table_index_meta->GetEntry(txn_id, begin_ts);
        if (status.ok()) {
            irs_index_entry = table_index_entry->irs_index_entry();
            for (const SharedPtr<IndexBase> &index_base : table_index_entry->index_def()->index_array_) {
                if (index_base->index_type_ != IndexType::kIRSFullText)
                    continue;
                IndexFullText *index_full_text = static_cast<IndexFullText *>(index_base.get());
                for (auto &column_name : index_full_text->column_names_) {
                    column2analyzer[column_name] = index_full_text->analyzer_;
                }
            }
            if (!column2analyzer.empty()) {
                // iresearch requires there is exactly one full index per table.
                break;
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
            std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_); // prevent another read conflict with this append operation
            if (this->unsealed_segment_ == nullptr || unsealed_segment_->Room() <= 0) {
                // unsealed_segment_ is unpopulated or full
                SegmentID new_segment_id = this->next_segment_id_++;
                SharedPtr<SegmentEntry> new_segment = SegmentEntry::NewSegmentEntry(this, new_segment_id, txn);

                this->segment_map_.emplace(new_segment_id, new_segment);
                this->unsealed_segment_ = new_segment.get();
                LOG_TRACE(fmt::format("Created a new segment {}", new_segment_id));
            }
        }
        // Append data from app_state_ptr to the buffer in segment. If append all data, then set finish.
        u64 actual_appended = unsealed_segment_->AppendData(txn_id, append_state_ptr, buffer_mgr, txn);
        LOG_TRACE(fmt::format("Segment {} is appended with {} rows", this->unsealed_segment_->segment_id_, actual_appended));
    }
}

void TableEntry::CreateIndexFile(void *txn_store,
                                 TableIndexEntry *table_index_entry,
                                 TxnTimeStamp begin_ts,
                                 BufferManager *buffer_mgr,
                                 bool prepare,
                                 bool is_replay) {
    IrsIndexEntry *irs_index_entry = table_index_entry->irs_index_entry().get();
    if (irs_index_entry != nullptr) {

        for (const auto &[_segment_id, segment_entry] : this->segment_map_) {
            irs_index_entry->irs_index_->BatchInsert(this, table_index_entry->index_def(), segment_entry.get(), buffer_mgr);
        }
        irs_index_entry->irs_index_->Commit();
        irs_index_entry->irs_index_->StopSchedule();
    }
    auto txn_store_ptr = static_cast<TxnTableStore *>(txn_store);
    for (const auto &base_index_pair : table_index_entry->column_index_map_) {
        u64 column_id = base_index_pair.first;
        BaseEntry *base_entry = base_index_pair.second.get();
        if (base_entry->entry_type_ == EntryType::kColumnIndex) {
            ColumnIndexEntry *column_index_entry = (ColumnIndexEntry *)(base_entry);
            SharedPtr<ColumnDef> column_def = this->columns_[column_id];
            for (const auto &[segment_id, segment_entry] : this->segment_map_) {
                // TODO: Check the segment min/max_row_ts
                SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry =
                    segment_entry->CreateIndexFile(column_index_entry, column_def, begin_ts, buffer_mgr, txn_store_ptr, prepare, is_replay);

                column_index_entry->index_by_segment_.emplace(segment_id, segment_column_index_entry);
            }
        } else if (base_entry->entry_type_ == EntryType::kIRSIndex) {
            continue;
        } else {
            UnrecoverableError("Invalid entry type");
        }
    }
}

void TableEntry::CommitCreateIndex(HashMap<String, TxnIndexStore> &txn_indexes_store_, bool is_replay) {
    for (auto &[index_name, txn_index_store] : txn_indexes_store_) {
        TableIndexEntry *table_index_entry = txn_index_store.table_index_entry_;
        for (auto &[column_id, segment_index_map] : txn_index_store.index_entry_map_) {
            for (auto &[segment_id, segment_column_index] : segment_index_map) {
                table_index_entry->CommitCreateIndex(column_id, segment_id, segment_column_index, is_replay);
            }
        }
        if (table_index_entry->irs_index_entry().get() != nullptr) {
            table_index_entry->CommitCreateIndex(table_index_entry->irs_index_entry());
        }
    }
}

Status TableEntry::Delete(TransactionID txn_id, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        SegmentID segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment_entry = TableEntry::GetSegmentByID(this, segment_id);
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
        SegmentEntry *segment = TableEntry::GetSegmentByID(this, segment_id);
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

Status TableEntry::CommitCompact(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store) {
    for (const auto &[new_segment, old_segments] : compact_store.segment_data_) {
        std::unique_lock lock(this->rw_locker_);
        for (const auto &old_segment : old_segments) {
            old_segment->SetDeprecated(commit_ts);
        }
        segment_map_.emplace(new_segment->segment_id(), new_segment);
    }
    return Status::OK();
}

Status TableEntry::RollbackCompact(TransactionID txn_id, TxnTimeStamp commit_ts, const TxnCompactStore &compact_store) {
    for (const auto &[new_segment, old_segments] : compact_store.segment_data_) {
        std::unique_lock lock(this->rw_locker_);
        for (const auto &old_segment : old_segments) {
            old_segment->RollbackCompact();
        }
    }
    return Status::OK();
}

Status TableEntry::ImportSegment(TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment) {
    if (this->deleted_) {
        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Table {} is deleted.", *this->GetTableName()));
        return Status(ErrorCode::kTableNotExist, std::move(err_msg));
    }

    segment->min_row_ts_ = commit_ts;
    // FIXME: max_row_ts is set when the segment is deprecated

    SizeT row_count = 0;
    for (auto &block_entry : segment->block_entries_) {
        block_entry->min_row_ts_ = commit_ts;
        block_entry->max_row_ts_ = commit_ts;
        // ATTENTION: Do not modify the block_entry checkpoint_ts_
        row_count += block_entry->row_count();
        block_entry->block_version_->created_.emplace_back(commit_ts, block_entry->row_count());
    }

    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker_);
    this->row_count_ = row_count;
    this->segment_map_.emplace(segment->segment_id_, std::move(segment));
    return Status::OK();
}

SegmentEntry *TableEntry::GetSegmentByID(const TableEntry *table_entry, u32 segment_id) {
    auto iter = table_entry->segment_map_.find(segment_id);
    if (iter != table_entry->segment_map_.end()) {
        return iter->second.get();
    } else {
        return nullptr;
    }
}

const BlockEntry *TableEntry::GetBlockEntryByID(u32 seg_id, u16 block_id) const {
    SegmentEntry *segment_entry = TableEntry::GetSegmentByID(this, seg_id);
    if (segment_entry == nullptr) {
        UnrecoverableError(fmt::format("Cannot find segment, segment id: {}", seg_id));
    }

    BlockEntry *block_entry = segment_entry->GetBlockEntryByID(block_id);
    if (block_entry == nullptr) {
        UnrecoverableError(fmt::format("Cannot find block, segment id: {}, block id: {}", seg_id, block_id));
    }
    return block_entry;
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

SharedPtr<BlockIndex> TableEntry::GetBlockIndex(u64, TxnTimeStamp begin_ts) {
    //    SharedPtr<MultiIndex<u64, u64, SegmentEntry*>> result = MakeShared<MultiIndex<u64, u64, SegmentEntry*>>();
    SharedPtr<BlockIndex> result = MakeShared<BlockIndex>();
    std::shared_lock<std::shared_mutex> rw_locker(this->rw_locker_);
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
        std::shared_lock<std::shared_mutex> lck(this->rw_locker_);
        json_res["table_entry_dir"] = *this->table_entry_dir_;
        json_res["table_name"] = *this->GetTableName();
        json_res["table_entry_type"] = this->table_entry_type_;
        json_res["row_count"] = this->row_count_.load();
        json_res["begin_ts"] = this->begin_ts_;
        json_res["commit_ts"] = this->commit_ts_.load();
        json_res["txn_id"] = this->txn_id_.load();
        json_res["deleted"] = this->deleted_;
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
        u32 next_segment_id = this->next_segment_id_;
        json_res["next_segment_id"] = next_segment_id;

        segment_candidates.reserve(this->segment_map_.size());
        for (const auto &[segment_id, segment_entry] : this->segment_map_) {
            segment_candidates.emplace_back(segment_entry.get());
        }

        table_index_meta_candidates.reserve(this->index_meta_map_.size());
        table_index_name_candidates.reserve(this->index_meta_map_.size());
        for (const auto &[index_name, table_index_meta] : this->index_meta_map_) {
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
    SharedPtr<String> table_entry_dir = MakeShared<String>(table_entry_json["table_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_entry_json["table_name"]);
    TableEntryType table_entry_type = table_entry_json["table_entry_type"];
    u64 row_count = table_entry_json["row_count"];

    bool deleted = table_entry_json["deleted"];

    Vector<SharedPtr<ColumnDef>> columns;

    if (table_entry_json.contains("column_definition")) {
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
    }

    TransactionID txn_id = table_entry_json["txn_id"];
    TxnTimeStamp begin_ts = table_entry_json["begin_ts"];

    UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(table_entry_dir, table_name, columns, table_entry_type, table_meta, txn_id, begin_ts);
    table_entry->row_count_ = row_count;
    table_entry->next_segment_id_ = table_entry_json["next_segment_id"];
    table_entry->table_entry_dir_ = table_entry_dir;

    if (table_entry_json.contains("segments")) {
        u32 max_segment_id = 0;
        for (const auto &segment_json : table_entry_json["segments"]) {
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
            table_entry->segment_map_.emplace(segment_entry->segment_id_, segment_entry);
            max_segment_id = std::max(max_segment_id, segment_entry->segment_id_);
        }
        table_entry->unsealed_segment_ = table_entry->segment_map_[max_segment_id].get();
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];
    table_entry->deleted_ = deleted;

    if (table_entry->deleted_) {
        if (!table_entry->segment_map_.empty()) {
            UnrecoverableError("deleted table should have no segment");
        }
    } else if (!table_entry->segment_map_.empty() && table_entry->segment_map_[0].get() == nullptr) {
        UnrecoverableError("table segment 0 should be valid");
    }

    if (table_entry_json.contains("table_indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["table_indexes"]) {

            UniquePtr<TableIndexMeta> table_index_meta = TableIndexMeta::Deserialize(index_def_meta_json, table_entry.get(), buffer_mgr);
            String index_name = index_def_meta_json["index_name"];
            table_entry->index_meta_map_.emplace(std::move(index_name), std::move(table_index_meta));
        }
    }

    return table_entry;
}

u64 TableEntry::GetColumnIdByName(const String &column_name) const {
    auto it = column_name2column_id_.find(column_name);
    if (it == column_name2column_id_.end()) {
        UnrecoverableError(fmt::format("No column name: {}", column_name));
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
    u32 max_segment_id = 0;
    for (auto &[seg_id, sgement_entry] : this->segment_map_) {
        max_segment_id = std::max(max_segment_id, seg_id);
    }
    for (auto &[seg_id, sgement_entry2] : table_entry2->segment_map_) {
        max_segment_id = std::max(max_segment_id, seg_id);
        auto it = this->segment_map_.find(seg_id);
        if (it == this->segment_map_.end()) {
            sgement_entry2->table_entry_ = this;
            this->segment_map_.emplace(seg_id, sgement_entry2);
        } else {
            it->second->MergeFrom(*sgement_entry2.get());
        }
    }

    if (this->unsealed_segment_ == nullptr && !this->segment_map_.empty()) {
        auto seg_it = this->segment_map_.find(max_segment_id);
        if (seg_it == this->segment_map_.end()) {
            UnrecoverableError(fmt::format("max_segment_id {} is invalid", max_segment_id));
        }
        this->unsealed_segment_ = seg_it->second.get();
    }

    for (auto &[index_name, table_index_meta] : table_entry2->index_meta_map_) {
        auto it = this->index_meta_map_.find(index_name);
        if (it == this->index_meta_map_.end()) {
            this->index_meta_map_.emplace(index_name, std::move(table_index_meta));
        } else {
            it->second->MergeFrom(*table_index_meta.get());
        }
    }
}

bool TableEntry::CheckDeleteConflict(const Vector<RowID> &delete_row_ids, Txn *delete_txn) {
    HashMap<SegmentID, Vector<SegmentOffset>> delete_row_map;
    for (const auto row_id : delete_row_ids) {
        delete_row_map[row_id.segment_id_].emplace_back(row_id.segment_offset_);
    }
    Vector<Pair<SegmentEntry *, Vector<SegmentOffset>>> check_segments;
    for (const auto &[segment_id, segment_offsets] : delete_row_map) {
        check_segments.emplace_back(this->segment_map_.at(segment_id).get(), std::move(segment_offsets));
    }

    return SegmentEntry::CheckDeleteConflict(std::move(check_segments), delete_txn);
}

} // namespace infinity
