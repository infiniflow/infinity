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

#include <iostream>
#include <memory>
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

namespace infinity {

TableEntry::TableEntry(const SharedPtr<String> &db_entry_dir,
                       SharedPtr<String> table_collection_name,
                       const Vector<SharedPtr<ColumnDef>> &columns,
                       TableEntryType table_entry_type,
                       TableMeta *table_meta,
                       u64 txn_id,
                       TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTable), table_entry_dir_(MakeShared<String>(Format("{}/{}/txn_{}", *db_entry_dir, *table_collection_name, txn_id))),
      table_name_(Move(table_collection_name)), columns_(columns), table_entry_type_(table_entry_type), table_meta_(table_meta) {
    SizeT column_count = columns.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        column_name2column_id_[columns[idx]->name()] = idx;
    }

    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

Tuple<TableIndexEntry *, Status>
TableEntry::CreateIndex(const SharedPtr<IndexDef> &index_def, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    if (index_def->index_name_->empty()) {
        // Index name shouldn't be empty
        Error<StorageException>("Attempt to create no name index.");
    }

    TableIndexMeta *table_index_meta{nullptr};

    // TODO: lock granularity can be narrowed.
    this->rw_locker_.lock_shared();
    if (auto iter = this->index_meta_map_.find(*index_def->index_name_); iter != this->index_meta_map_.end()) {
        table_index_meta = iter->second.get();
    }
    this->rw_locker_.unlock_shared();

    if (table_index_meta == nullptr) {

        auto new_table_index_meta = MakeUnique<TableIndexMeta>(this, index_def->index_name_);
        table_index_meta = new_table_index_meta.get();

        this->rw_locker_.lock();

        if (auto iter = this->index_meta_map_.find(*index_def->index_name_); iter != this->index_meta_map_.end()) {
            table_index_meta = iter->second.get();
        } else {

            this->index_meta_map_[*index_def->index_name_] = Move(new_table_index_meta);
        }
        this->rw_locker_.unlock();
    }

    LOG_TRACE(Format("Creating new index: {}", *index_def->index_name_));
    return table_index_meta->CreateTableIndexEntry(index_def, conflict_type, txn_id, begin_ts, txn_mgr);
}

Tuple<TableIndexEntry *, Status>
TableEntry::DropIndex(const String &index_name, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
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
                String error_message = Format("Attempt to drop not existed index entry {}", index_name);
                LOG_TRACE(error_message);
                return {nullptr, Status(ErrorCode::kNotFound, error_message.c_str())};
            }
            default: {
                Error<StorageException>("Invalid conflict type.");
            }
        }
        Error<StorageException>("Should not reach here.");
    }
    LOG_TRACE(Format("Drop index entry {}", index_name));
    return index_meta->DropTableIndexEntry(conflict_type, txn_id, begin_ts, txn_mgr);
}

Tuple<TableIndexEntry *, Status> TableEntry::GetIndex(const String &index_name, u64 txn_id, TxnTimeStamp begin_ts) {
    if (auto iter = this->index_meta_map_.find(index_name); iter != this->index_meta_map_.end()) {
        return iter->second->GetEntry(txn_id, begin_ts);
    }
    UniquePtr<String> err_msg = MakeUnique<String>("Cannot find index def");
    LOG_ERROR(*err_msg);
    return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
}

void TableEntry::RemoveIndexEntry(const String &index_name, u64 txn_id, TxnManager *txn_mgr) {
    this->rw_locker_.lock_shared();

    TableIndexMeta *table_index_meta{nullptr};
    if (auto iter = this->index_meta_map_.find(index_name); iter != this->index_meta_map_.end()) {
        table_index_meta = iter->second.get();
    }
    this->rw_locker_.unlock_shared();

    LOG_TRACE(Format("Remove index entry: {}", index_name));
    table_index_meta->DeleteNewEntry(txn_id, txn_mgr);
}

void TableEntry::GetFullTextAnalyzers(u64 txn_id,
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

void TableEntry::Append(u64 txn_id, void *txn_store, BufferManager *buffer_mgr) {
    if (this->deleted_) {
        Error<StorageException>("table is deleted");
        return;
    }
    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    if (append_state_ptr->Finished()) {
        LOG_TRACE("No append is done.");
        return;
    }

    while (!append_state_ptr->Finished()) {
        {
            UniqueLock<RWMutex> rw_locker(this->rw_locker_); // prevent another read conflict with this append operation
            if (this->unsealed_segment_ == nullptr || unsealed_segment_->Room() <= 0) {
                // unsealed_segment_ is unpopulated or full
                u32 new_segment_id = this->next_segment_id_++;
                SharedPtr<SegmentEntry> new_segment = SegmentEntry::MakeNewSegmentEntry(this, new_segment_id, buffer_mgr);
                this->segment_map_.emplace(new_segment_id, new_segment);
                this->unsealed_segment_ = new_segment.get();
                LOG_TRACE(Format("Created a new segment {}", new_segment_id));
            }
        }
        // Append data from app_state_ptr to the buffer in segment. If append all data, then set finish.
        u64 actual_appended = SegmentEntry::AppendData(this->unsealed_segment_, txn_id, append_state_ptr, buffer_mgr);
        LOG_TRACE(Format("Segment {} is appended with {} rows", this->unsealed_segment_->segment_id_, actual_appended));
    }
}

void TableEntry::CreateIndexFile(void *txn_store, TableIndexEntry *table_index_entry, TxnTimeStamp begin_ts, BufferManager *buffer_mgr) {
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
                SharedPtr<SegmentColumnIndexEntry> segment_column_index_entry =
                    SegmentEntry::CreateIndexFile(segment_entry.get(), column_index_entry, column_def, begin_ts, buffer_mgr, txn_store_ptr);
                column_index_entry->index_by_segment_.emplace(segment_id, segment_column_index_entry);
            }
        } else if (base_entry->entry_type_ == EntryType::kIRSIndex) {
            continue;
        } else {
            Error<StorageException>("Invalid entry type");
        }
    }
}

void TableEntry::CommitCreateIndex(HashMap<String, TxnIndexStore> &txn_indexes_store_) {
    for (auto &[index_name, txn_index_store] : txn_indexes_store_) {
        TableIndexEntry *table_index_entry = txn_index_store.table_index_entry_;
        for (auto &[column_id, segment_index_map] : txn_index_store.index_entry_map_) {
            for (auto &[segment_id, segment_column_index] : segment_index_map) {
                table_index_entry->CommitCreateIndex(column_id, segment_id, segment_column_index);
            }
        }
        if (table_index_entry->irs_index_entry().get() != nullptr) {
            table_index_entry->CommitCreateIndex(table_index_entry->irs_index_entry());
        }
    }
}

Status TableEntry::Delete(u64 txn_id, TxnTimeStamp commit_ts, DeleteState &delete_state) {
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment_entry = TableEntry::GetSegmentByID(this, segment_id);
        if (segment_entry == nullptr) {
            UniquePtr<String> err_msg = MakeUnique<String>(Format("Going to delete data in non-exist segment: {}", segment_id));
            Error<ExecutorException>(*err_msg);
            return Status(ErrorCode::kNotFound, Move(err_msg));
        }
        const HashMap<u16, Vector<RowID>> &block_row_hashmap = to_delete_seg_rows.second;
        SegmentEntry::DeleteData(segment_entry, txn_id, commit_ts, block_row_hashmap);
    }
    return Status::OK();
}

void TableEntry::CommitAppend(u64 txn_id, TxnTimeStamp commit_ts, const AppendState *append_state_ptr) {
    SizeT row_count = 0;
    for (const auto &range : append_state_ptr->append_ranges_) {
        LOG_TRACE(Format("Commit, segment: {}, block: {} start offset: {}, count: {}",
                         range.segment_id_,
                         range.block_id_,
                         range.start_offset_,
                         range.row_count_));
        SegmentEntry *segment_ptr = this->segment_map_[range.segment_id_].get();
        SegmentEntry::CommitAppend(segment_ptr, txn_id, commit_ts, range.block_id_, range.start_offset_, range.row_count_);
        row_count += range.row_count_;
    }
    this->row_count_ += row_count;
}

void TableEntry::RollbackAppend(u64 txn_id, TxnTimeStamp commit_ts, void *txn_store) {
    //    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    //    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    Error<NotImplementException>("TableEntry::RollbackAppend");
}

void TableEntry::CommitDelete(u64 txn_id, TxnTimeStamp commit_ts, const DeleteState &delete_state) {
    SizeT row_count = 0;
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment = TableEntry::GetSegmentByID(this, segment_id);
        if (segment == nullptr) {
            Error<ExecutorException>(Format("Going to commit delete data in non-exist segment: {}", segment_id));
        }
        const HashMap<u16, Vector<RowID>> &block_row_hashmap = to_delete_seg_rows.second;
        SegmentEntry::CommitDelete(segment, txn_id, commit_ts, block_row_hashmap);
        row_count += block_row_hashmap.size();
    }
    this->row_count_ += row_count;
}

Status TableEntry::RollbackDelete(u64 txn_id, DeleteState &, BufferManager *) {
    Error<NotImplementException>("TableEntry::RollbackDelete");
    return Status::OK();
}

Status TableEntry::ImportSegment(TxnTimeStamp commit_ts, SharedPtr<SegmentEntry> segment) {
    if (this->deleted_) {
        UniquePtr<String> err_msg = MakeUnique<String>(Format("Table {} is deleted.", *this->GetTableName()));
        Error<ExecutorException>(*err_msg);
        return Status(ErrorCode::kNotFound, Move(err_msg));
    }

    segment->min_row_ts_ = commit_ts;
    segment->max_row_ts_ = commit_ts;

    SizeT row_count = 0;
    for (auto &block_entry : segment->block_entries_) {
        block_entry->min_row_ts_ = commit_ts;
        block_entry->max_row_ts_ = commit_ts;
        // ATTENTION: Do not modify the block_entry checkpoint_ts_
        row_count += block_entry->row_count();
        block_entry->block_version_->created_.emplace_back(commit_ts, block_entry->row_count());
    }

    UniqueLock<RWMutex> rw_locker(this->rw_locker_);
    this->row_count_ += row_count;
    this->segment_map_.emplace(segment->segment_id_, Move(segment));
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
        throw ExecutorException(Format("Cannot find segment, segment id: {}", seg_id));
    }

    BlockEntry *block_entry = segment_entry->GetBlockEntryByID(block_id);
    if (block_entry == nullptr) {
        throw ExecutorException(Format("Cannot find block, segment id: {}, block id: {}", seg_id, block_id));
    }
    return block_entry;
}

Pair<SizeT, Status> TableEntry::GetSegmentRowCountBySegmentID(u32 seg_id) {
    auto iter = this->segment_map_.find(seg_id);
    if (iter != this->segment_map_.end()) {
        return {iter->second->row_count(), Status::OK()};
    } else {
        UniquePtr<String> err_msg = MakeUnique<String>(Format("No segment id: {}.", seg_id));
        LOG_ERROR(*err_msg);
        return {0, Status(ErrorCode::kNotFound, Move(err_msg))};
    }
}

// DBEntry *TableEntry::GetDBEntry(const TableEntry *table_entry) {
//     TableMeta *table_meta = (TableMeta *)table_entry->table_meta_;
//     return (DBEntry *)table_meta->db_entry_;
// }

const SharedPtr<String> &TableEntry::GetDBName() const { return table_meta_->db_name_ptr(); }

SharedPtr<BlockIndex> TableEntry::GetBlockIndex(u64, TxnTimeStamp begin_ts) {
    //    SharedPtr<MultiIndex<u64, u64, SegmentEntry*>> result = MakeShared<MultiIndex<u64, u64, SegmentEntry*>>();
    SharedPtr<BlockIndex> result = MakeShared<BlockIndex>();
    SharedLock<RWMutex> rw_locker(this->rw_locker_);
    result->Reserve(this->segment_map_.size());

    for (const auto &segment_pair : this->segment_map_) {
        result->Insert(segment_pair.second.get(), begin_ts);
    }

    return result;
}

Json TableEntry::Serialize(TableEntry *table_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;

    Vector<SegmentEntry *> segment_candidates;
    Vector<TableIndexMeta *> table_index_meta_candidates;
    Vector<String> table_index_name_candidates;
    {
        SharedLock<RWMutex> lck(table_entry->rw_locker_);
        json_res["table_entry_dir"] = *table_entry->table_entry_dir_;
        json_res["table_name"] = *table_entry->GetTableName();
        json_res["table_entry_type"] = table_entry->table_entry_type_;
        json_res["row_count"] = table_entry->row_count_.load();
        json_res["begin_ts"] = table_entry->begin_ts_;
        json_res["commit_ts"] = table_entry->commit_ts_.load();
        json_res["txn_id"] = table_entry->txn_id_.load();
        json_res["deleted"] = table_entry->deleted_;
        for (const auto &column_def : table_entry->columns_) {
            Json column_def_json;
            column_def_json["column_type"] = column_def->type()->Serialize();
            column_def_json["column_id"] = column_def->id();
            column_def_json["column_name"] = column_def->name();

            for (const auto &column_constraint : column_def->constraints_) {
                column_def_json["constraints"].emplace_back(column_constraint);
            }

            json_res["column_definition"].emplace_back(column_def_json);
        }
        u32 next_segment_id = table_entry->next_segment_id_;
        json_res["next_segment_id"] = next_segment_id;

        segment_candidates.reserve(table_entry->segment_map_.size());
        for (const auto &[segment_id, segment_entry] : table_entry->segment_map_) {
            //            if(segment_entry->commit_ts_ <= max_commit_ts) {
            segment_candidates.emplace_back(segment_entry.get());
            //            }
        }

        table_index_meta_candidates.reserve(table_entry->index_meta_map_.size());
        table_index_name_candidates.reserve(table_entry->index_meta_map_.size());
        for (const auto &[index_name, table_index_meta] : table_entry->index_meta_map_) {
            table_index_meta_candidates.emplace_back(table_index_meta.get());
            table_index_name_candidates.emplace_back(index_name);
        }
    }

    // Serialize segments
    for (const auto &segment_entry : segment_candidates) {
        json_res["segments"].emplace_back(SegmentEntry::Serialize(segment_entry, max_commit_ts, is_full_checkpoint));
    }

    // Serialize indexes
    SizeT table_index_count = table_index_meta_candidates.size();
    for (SizeT idx = 0; idx < table_index_count; ++idx) {
        TableIndexMeta *table_index_meta = table_index_meta_candidates[idx];
        Json index_def_meta_json = TableIndexMeta::Serialize(table_index_meta, max_commit_ts);
        index_def_meta_json["index_name"] = table_index_name_candidates[idx];
        json_res["table_indexes"].emplace_back(index_def_meta_json);
    }

    return json_res;
}

UniquePtr<TableEntry> TableEntry::Deserialize(const Json &table_entry_json, TableMeta *table_meta, BufferManager *buffer_mgr) {
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

    u64 txn_id = table_entry_json["txn_id"];
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
            max_segment_id = Max(max_segment_id, segment_entry->segment_id_);
        }
        table_entry->unsealed_segment_ = table_entry->segment_map_[max_segment_id].get();
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];
    table_entry->deleted_ = deleted;

    if (table_entry->deleted_) {
        if (!table_entry->segment_map_.empty()) {
            Error<StorageException>("deleted table should have no segment");
        }
    } else if (!table_entry->segment_map_.empty() && table_entry->segment_map_[0].get() == nullptr) {
        Error<StorageException>("table segment 0 should be valid");
    }

    if (table_entry_json.contains("table_indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["table_indexes"]) {

            UniquePtr<TableIndexMeta> table_index_meta = TableIndexMeta::Deserialize(index_def_meta_json, table_entry.get(), buffer_mgr);
            String index_name = index_def_meta_json["index_name"];
            table_entry->index_meta_map_.emplace(Move(index_name), Move(table_index_meta));
        }
    }

    return table_entry;
}

u64 TableEntry::GetColumnIdByName(const String &column_name) const {
    auto it = column_name2column_id_.find(column_name);
    if (it == column_name2column_id_.end()) {
        Error<NotImplementException>(Format("No column name: {}", column_name));
    }
    return it->second;
}

void TableEntry::MergeFrom(BaseEntry &other) {
    auto table_entry2 = dynamic_cast<TableEntry *>(&other);
    if (table_entry2 == nullptr) {
        Error<StorageException>("MergeFrom requires the same type of BaseEntry");
    }
    // // No locking here since only the load stage needs MergeFrom.
    if (*this->table_name_ != *table_entry2->table_name_) {
        Error<StorageException>("DBEntry::MergeFrom requires table_name_ match");
    }
    if (*this->table_entry_dir_ != *table_entry2->table_entry_dir_) {
        Error<StorageException>("DBEntry::MergeFrom requires table_entry_dir_ match");
    }
    if (this->table_entry_type_ != table_entry2->table_entry_type_) {
        Error<StorageException>("DBEntry::MergeFrom requires table_entry_dir_ match");
    }

    this->next_segment_id_.store(Max(this->next_segment_id_, table_entry2->next_segment_id_));
    this->row_count_.store(Max(this->row_count_, table_entry2->row_count_));
    u32 max_segment_id = 0;
    for (auto &[seg_id, sgement_entry] : this->segment_map_) {
        max_segment_id = Max(max_segment_id, seg_id);
    }
    for (auto &[seg_id, sgement_entry2] : table_entry2->segment_map_) {
        max_segment_id = Max(max_segment_id, seg_id);
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
            Error<StorageException>(Format("max_segment_id {} is invalid", max_segment_id));
        }
        this->unsealed_segment_ = seg_it->second.get();
    }

    for (auto &[index_name, table_index_meta] : table_entry2->index_meta_map_) {
        auto it = this->index_meta_map_.find(index_name);
        if (it == this->index_meta_map_.end()) {
            this->index_meta_map_.emplace(index_name, Move(table_index_meta));
        } else {
            it->second->MergeFrom(*table_index_meta.get());
        }
    }
}

} // namespace infinity
