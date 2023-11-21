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

import stl;
import parser;
import base_entry;
import table_collection_meta;
import table_collection_type;
import third_party;
import txn;
import buffer_manager;
import segment_entry;
import index_def;
import block_index;
import data_access_state;
import db_entry;
import logger;
import txn_store;
import status;
import infinity_exception;
import ivfflat_index_def;
import index_def_meta;
import txn_manager;
import index_entry;
import index_def_entry;

module table_collection_entry;

namespace infinity {

TableCollectionEntry::TableCollectionEntry(const SharedPtr<String> &db_entry_dir,
                                           SharedPtr<String> table_collection_name,
                                           const Vector<SharedPtr<ColumnDef>> &columns,
                                           TableCollectionType table_collection_type,
                                           TableCollectionMeta *table_collection_meta,
                                           u64 txn_id,
                                           TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kTable), table_entry_dir_(MakeShared<String>(Format("{}/{}/txn_{}", *db_entry_dir, *table_collection_name, txn_id))),
      table_collection_name_(Move(table_collection_name)), columns_(columns), table_collection_type_(table_collection_type),
      table_collection_meta_(table_collection_meta) {
    SizeT column_count = columns.size();
    for (SizeT idx = 0; idx < column_count; ++idx) {
        column_name2column_id_[columns[idx]->name()] = idx;
    }

    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

Status TableCollectionEntry::CreateIndex(TableCollectionEntry *table_entry,
                                         SharedPtr<IndexDef> index_def,
                                         ConflictType conflict_type,
                                         u64 txn_id,
                                         TxnTimeStamp begin_ts,
                                         TxnManager *txn_mgr,
                                         BaseEntry *&new_index_entry) {
    // TODO: lock granularity can be narrowed.
    table_entry->rw_locker_.lock_shared();
    IndexDefMeta *index_def_meta{nullptr};
    if (index_def->index_name_->empty()) {
        // set default index name
        i32 index_id = 0;
        String index_name_base = "idx";
        for (const auto &column_name : index_def->column_names_) {
            index_name_base += "_" + column_name;
        }
        while (true) {
            auto index_name = index_id > 0 ? (index_name_base + "_" + ToStr(index_id)) : index_name_base;
            if (auto iter = table_entry->indexes_.find(index_name); iter == table_entry->indexes_.end()) {
                index_def->index_name_ = MakeShared<String>(Move(index_name));
                break;
            }
            ++index_id;
        }
    } else if (auto iter = table_entry->indexes_.find(*index_def->index_name_); iter != table_entry->indexes_.end()) {
        index_def_meta = iter->second.get();
    }
    table_entry->rw_locker_.unlock_shared();

    if (index_def_meta == nullptr) {
        LOG_TRACE(Format("Create new index: {}", *index_def->index_name_));
        auto new_index_def_meta = MakeUnique<IndexDefMeta>(table_entry);
        index_def_meta = new_index_def_meta.get();

        table_entry->rw_locker_.lock();
        // Insert index_def meta here
        table_entry->indexes_[*index_def->index_name_] = Move(new_index_def_meta);
        table_entry->rw_locker_.unlock();

        LOG_TRACE(Format("Add new index entry for {} in new index meta of table_entry {} ", *index_def->index_name_, *table_entry->table_entry_dir_));
    } else {
        LOG_TRACE(
            Format("Add new index entry for {} in existed index meta of table_entry {}", *index_def->index_name_, *table_entry->table_entry_dir_));
    }
    IndexDef *index_def_ptr = index_def.get();
    return IndexDefMeta::CreateNewEntry(index_def_meta, Move(index_def), conflict_type, txn_id, begin_ts, txn_mgr, new_index_entry);
}

Status TableCollectionEntry::DropIndex(TableCollectionEntry *table_entry,
                                       const String &index_name,
                                       ConflictType conflict_type,
                                       u64 txn_id,
                                       TxnTimeStamp begin_ts,
                                       TxnManager *txn_mgr,
                                       BaseEntry *&new_index_entry) {
    table_entry->rw_locker_.lock_shared();

    IndexDefMeta *index_meta{nullptr};
    if (auto iter = table_entry->indexes_.find(index_name); iter != table_entry->indexes_.end()) {
        index_meta = iter->second.get();
    }
    table_entry->rw_locker_.unlock_shared();
    if (index_meta == nullptr) {
        switch (conflict_type) {

            case ConflictType::kIgnore: {
                return Status::OK();
            }
            case ConflictType::kError: {
                String error_message = Format("Attempt to drop not existed index entry {}", index_name);
                LOG_TRACE(error_message);
                return Status(ErrorCode::kNotFound, error_message.c_str());
            }
            default: {
                Error<StorageException>("Invalid conflict type.");
            }
        }
        Error<StorageException>("Should not reach here.");
    }
    LOG_TRACE(Format("Drop index entry {}", index_name));
    return IndexDefMeta::DropNewEntry(index_meta, conflict_type, txn_id, begin_ts, txn_mgr, new_index_entry);
}

Status TableCollectionEntry::GetIndex(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry*& base_entry) {
    if (auto iter = table_entry->indexes_.find(index_name); iter != table_entry->indexes_.end()) {
        return IndexDefMeta::GetEntry(iter->second.get(), txn_id, begin_ts, base_entry);
    }
    UniquePtr<String> err_msg = MakeUnique<String>("Cannot find index def");
    LOG_ERROR(*err_msg);
    return Status(ErrorCode::kNotFound, Move(err_msg));
}

void TableCollectionEntry::RemoveIndexEntry(TableCollectionEntry *table_entry, const String &index_name, u64 txn_id, TxnManager *txn_mgr) {
    table_entry->rw_locker_.lock_shared();

    IndexDefMeta *index_def_meta{nullptr};
    if (auto iter = table_entry->indexes_.find(index_name); iter != table_entry->indexes_.end()) {
        index_def_meta = iter->second.get();
    }
    table_entry->rw_locker_.unlock_shared();

    LOG_TRACE(Format("Remove index entry: {}", index_name));
    IndexDefMeta::DeleteNewEntry(index_def_meta, txn_id, txn_mgr);
}

void TableCollectionEntry::Append(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store, BufferManager *buffer_mgr) {
    Assert<StorageException>(!table_entry->deleted_, "table is deleted");
    TxnTableStore *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    if (append_state_ptr->Finished()) {
        LOG_TRACE("No append is done.");
        return;
    }

    while (!append_state_ptr->Finished()) {
        {
            UniqueLock<RWMutex> rw_locker(table_entry->rw_locker_); // prevent another read conflict with this append operation
            if (table_entry->unsealed_segment_ == nullptr || SegmentEntry::Room(table_entry->unsealed_segment_) <= 0) {
                // unsealed_segment_ is unpopulated or full
                SharedPtr<SegmentEntry> new_segment =
                    SegmentEntry::MakeNewSegmentEntry(table_entry, TableCollectionEntry::GetNextSegmentID(table_entry), buffer_mgr);
                table_entry->segments_.emplace(new_segment->segment_id_, new_segment);
                table_entry->unsealed_segment_ = new_segment.get();
                LOG_TRACE(Format("Created a new segment {}", new_segment->segment_id_));
            }
        }
        // Append data from app_state_ptr to the buffer in segment. If append all data, then set finish.
        u64 actual_appended = SegmentEntry::AppendData(table_entry->unsealed_segment_, txn_ptr, append_state_ptr, buffer_mgr);
        LOG_TRACE(Format("Segment {} is appended with {} rows", table_entry->unsealed_segment_->segment_id_, actual_appended));
    }
}

void TableCollectionEntry::CreateIndexFile(TableCollectionEntry *table_entry,
                                           void *txn_store,
                                           IndexDefEntry *index_def_entry,
                                           TxnTimeStamp begin_ts,
                                           BufferManager *buffer_mgr) {
    SharedPtr<IndexDef> index_def = index_def_entry->index_def_;
    auto txn_store_ptr = static_cast<TxnTableStore *>(txn_store);
    if (index_def->column_names_.size() != 1) {
        StorageException("Not implemented");
    }
    const String &column_name = index_def->column_names_[0];
    u64 column_id = table_entry->GetColumnIdByName(column_name);
    SharedPtr<ColumnDef> column_def = table_entry->columns_[column_id];
    for (const auto &[_segment_id, segment_entry] : table_entry->segments_) {
        SegmentEntry::CreateIndexFile(segment_entry.get(), index_def_entry, column_def, begin_ts, buffer_mgr, txn_store_ptr);
    }
}

UniquePtr<String> TableCollectionEntry::Delete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &delete_state) {
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment_entry = TableCollectionEntry::GetSegmentByID(table_entry, segment_id);
        if (segment_entry == nullptr) {
            Error<ExecutorException>(Format("Going to delete data in non-exist segment: {}", segment_id));
        }
        const HashMap<u16, Vector<RowID>> &block_row_hashmap = to_delete_seg_rows.second;
        SegmentEntry::DeleteData(segment_entry, txn_ptr, block_row_hashmap);
    }
    return nullptr;
}

void TableCollectionEntry::CommitAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, const AppendState *append_state_ptr) {
    SizeT row_count = 0;
    for (const auto &range : append_state_ptr->append_ranges_) {
        LOG_TRACE(Format("Commit, segment: {}, block: {} start offset: {}, count: {}",
                         range.segment_id_,
                         range.block_id_,
                         range.start_offset_,
                         range.row_count_));
        SegmentEntry *segment_ptr = table_entry->segments_[range.segment_id_].get();
        SegmentEntry::CommitAppend(segment_ptr, txn_ptr, range.block_id_, range.start_offset_, range.row_count_);
        row_count += range.row_count_;
    }
    table_entry->row_count_ += row_count;
}

void TableCollectionEntry::CommitCreateIndex(TableCollectionEntry *table_entry, HashMap<String, TxnIndexStore> &txn_indexes_store_) {
    for (auto &[index_name, txn_index_store] : txn_indexes_store_) {
        IndexDefEntry *index_def_entry = txn_index_store.index_def_entry_;
        for (auto &[segment_id, index_entry] : txn_index_store.index_entry_map_) {
            IndexDefEntry::CommitCreatedIndex(index_def_entry, segment_id, index_entry);
        }
    }
}

void TableCollectionEntry::RollbackAppend(TableCollectionEntry *table_entry, Txn *txn_ptr, void *txn_store) {
    auto *txn_store_ptr = (TxnTableStore *)txn_store;
    AppendState *append_state_ptr = txn_store_ptr->append_state_.get();
    Error<NotImplementException>("TableCollectionEntry::RollbackAppend");
}

void TableCollectionEntry::CommitDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, const DeleteState &delete_state) {
    SizeT row_count = 0;
    for (const auto &to_delete_seg_rows : delete_state.rows_) {
        u32 segment_id = to_delete_seg_rows.first;
        SegmentEntry *segment = TableCollectionEntry::GetSegmentByID(table_entry, segment_id);
        if (segment == nullptr) {
            Error<ExecutorException>(Format("Going to commit delete data in non-exist segment: {}", segment_id));
        }
        const HashMap<u16, Vector<RowID>> &block_row_hashmap = to_delete_seg_rows.second;
        SegmentEntry::CommitDelete(segment, txn_ptr, block_row_hashmap);
        row_count += block_row_hashmap.size();
    }
    table_entry->row_count_ += row_count;
}

UniquePtr<String>
TableCollectionEntry::RollbackDelete(TableCollectionEntry *table_entry, Txn *txn_ptr, DeleteState &append_state, BufferManager *buffer_mgr) {
    Error<NotImplementException>("TableCollectionEntry::RollbackDelete");
    return nullptr;
}

UniquePtr<String> TableCollectionEntry::ImportSegment(TableCollectionEntry *table_entry, Txn *txn_ptr, SharedPtr<SegmentEntry> segment) {
    Assert<StorageException>(!table_entry->deleted_, "table is deleted");
    TxnTimeStamp commit_ts = txn_ptr->CommitTS();
    segment->min_row_ts_ = commit_ts;
    segment->max_row_ts_ = commit_ts;

    SizeT row_count = 0;
    for (auto &block_entry : segment->block_entries_) {
        block_entry->min_row_ts_ = commit_ts;
        block_entry->max_row_ts_ = commit_ts;
        // ATTENTION: Do not modify the block_entry checkpoint_ts_
        row_count += block_entry->row_count_;
        block_entry->block_version_->created_.emplace_back(commit_ts, block_entry->row_count_);
    }
    table_entry->row_count_ += row_count;

    UniqueLock<RWMutex> rw_locker(table_entry->rw_locker_);
    table_entry->segments_.emplace(segment->segment_id_, Move(segment));
    return nullptr;
}

SegmentEntry *TableCollectionEntry::GetSegmentByID(const TableCollectionEntry *table_entry, u32 segment_id) {
    auto iter = table_entry->segments_.find(segment_id);
    if (iter != table_entry->segments_.end()) {
        return iter->second.get();
    } else {
        return nullptr;
    }
}

DBEntry *TableCollectionEntry::GetDBEntry(const TableCollectionEntry *table_entry) {
    TableCollectionMeta *table_meta = (TableCollectionMeta *)table_entry->table_collection_meta_;
    return (DBEntry *)table_meta->db_entry_;
}

SharedPtr<String> TableCollectionEntry::GetDBName(const TableCollectionEntry *table_entry) {
    TableCollectionMeta *table_meta = (TableCollectionMeta *)table_entry->table_collection_meta_;
    return table_meta->db_entry_->db_name_;
}

SharedPtr<BlockIndex> TableCollectionEntry::GetBlockIndex(TableCollectionEntry *table_collection_entry, u64 txn_id, TxnTimeStamp begin_ts) {
    //    SharedPtr<MultiIndex<u64, u64, SegmentEntry*>> result = MakeShared<MultiIndex<u64, u64, SegmentEntry*>>();
    SharedPtr<BlockIndex> result = MakeShared<BlockIndex>();
    SharedLock<RWMutex> rw_locker(table_collection_entry->rw_locker_);
    result->Reserve(table_collection_entry->segments_.size());

    for (const auto &segment_pair : table_collection_entry->segments_) {
        result->Insert(segment_pair.second.get(), begin_ts);
    }

    return result;
}

Json TableCollectionEntry::Serialize(TableCollectionEntry *table_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;

    {
        SharedLock<RWMutex> lck(table_entry->rw_locker_);
        json_res["table_entry_dir"] = *table_entry->table_entry_dir_;
        json_res["table_name"] = *table_entry->table_collection_name_;
        json_res["table_entry_type"] = table_entry->table_collection_type_;
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
    }

    for (const auto &[segment_id, segment_entry] : table_entry->segments_) {
        json_res["segments"].emplace_back(SegmentEntry::Serialize(segment_entry.get(), max_commit_ts, is_full_checkpoint));
    }

    for (const auto &[index_name, index_entry] : table_entry->indexes_) {
        Json index_def_meta_json = IndexDefMeta::Serialize(index_entry.get(), max_commit_ts);
        index_def_meta_json["index_name"] = index_name;
        json_res["indexes"].emplace_back(index_def_meta_json);
    }

    return json_res;
}

UniquePtr<TableCollectionEntry>
TableCollectionEntry::Deserialize(const Json &table_entry_json, TableCollectionMeta *table_meta, BufferManager *buffer_mgr) {
    SharedPtr<String> table_entry_dir = MakeShared<String>(table_entry_json["table_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_entry_json["table_name"]);
    TableCollectionType table_entry_type = table_entry_json["table_entry_type"];
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

    UniquePtr<TableCollectionEntry> table_entry =
        MakeUnique<TableCollectionEntry>(table_entry_dir, table_name, columns, table_entry_type, table_meta, txn_id, begin_ts);
    table_entry->row_count_ = row_count;
    table_entry->next_segment_id_ = table_entry_json["next_segment_id"];
    table_entry->table_entry_dir_ = table_entry_dir;

    if (table_entry_json.contains("segments")) {
        u32 max_segment_id = 0;
        for (const auto &segment_json : table_entry_json["segments"]) {
            SharedPtr<SegmentEntry> segment_entry = SegmentEntry::Deserialize(segment_json, table_entry.get(), buffer_mgr);
            table_entry->segments_.emplace(segment_entry->segment_id_, segment_entry);
            max_segment_id = Max(max_segment_id, segment_entry->segment_id_);
        }
        table_entry->unsealed_segment_ = table_entry->segments_[max_segment_id].get();
    }

    table_entry->commit_ts_ = table_entry_json["commit_ts"];
    table_entry->deleted_ = deleted;

    if (table_entry->deleted_)
        Assert<StorageException>(table_entry->segments_.empty(), "deleted table should have no segment");
    else
        Assert<StorageException>(table_entry->segments_.empty() || table_entry->segments_[0].get() != nullptr, "table segment 0 should be valid");

    if (table_entry_json.contains("indexes")) {
        for (const auto &index_def_meta_json : table_entry_json["indexes"]) {

            UniquePtr<IndexDefMeta> index_def_meta = IndexDefMeta::Deserialize(index_def_meta_json, table_entry.get(), buffer_mgr);
            String index_name = index_def_meta_json["index_name"];
            table_entry->indexes_.emplace(Move(index_name), Move(index_def_meta));
        }
    }

    return table_entry;
}

u64 TableCollectionEntry::GetColumnIdByName(const String &column_name) {
    auto it = column_name2column_id_.find(column_name);
    if (it == column_name2column_id_.end()) {
        Error<NotImplementException>(Format("No column name: {}", column_name));
    }
    return it->second;
}

void TableCollectionEntry::MergeFrom(BaseEntry &other) {
    auto table_entry2 = dynamic_cast<TableCollectionEntry *>(&other);
    Assert<StorageException>(table_entry2 != nullptr, "MergeFrom requires the same type of BaseEntry");
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(*this->table_collection_name_ == *table_entry2->table_collection_name_,
                             "DBEntry::MergeFrom requires table_collection_name_ match");
    Assert<StorageException>(*this->table_entry_dir_ == *table_entry2->table_entry_dir_, "DBEntry::MergeFrom requires table_entry_dir_ match");
    Assert<StorageException>(this->table_collection_type_ == table_entry2->table_collection_type_,
                             "DBEntry::MergeFrom requires table_entry_dir_ match");

    this->next_segment_id_.store(Max(this->next_segment_id_, table_entry2->next_segment_id_));
    u32 max_segment_id = 0;
    for (auto &[seg_id, sgement_entry] : this->segments_) {
        max_segment_id = Max(max_segment_id, seg_id);
    }
    for (auto &[seg_id, sgement_entry2] : table_entry2->segments_) {
        max_segment_id = Max(max_segment_id, seg_id);
        auto it = this->segments_.find(seg_id);
        if (it == this->segments_.end()) {
            sgement_entry2->table_entry_ = this;
            this->segments_.emplace(seg_id, sgement_entry2);
        } else {
            it->second->MergeFrom(*sgement_entry2.get());
        }
    }
    if (this->unsealed_segment_ == nullptr && !this->segments_.empty()) {
        auto seg_it = this->segments_.find(max_segment_id);
        Assert<StorageException>(seg_it != this->segments_.end(), Format("max_segment_id {} is invalid", max_segment_id));
        this->unsealed_segment_ = seg_it->second.get();
    }
}

} // namespace infinity
