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

#include <vector>

module catalog;

import :base_entry;

import table_entry_type;
import stl;
import parser;
import txn_manager;
import table_detail;
import buffer_manager;
import default_values;
import block_index;
import logger;
import third_party;
import infinity_exception;
import status;

namespace infinity {

Tuple<TableEntry *, Status> DBEntry::CreateTable(TableEntryType table_entry_type,
                                                 const SharedPtr<String> &table_collection_name,
                                                 const Vector<SharedPtr<ColumnDef>> &columns,
                                                 u64 txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr) {
    const String &table_name = *table_collection_name;

    // Check if there is table_meta with the table_name
    TableMeta *table_meta{nullptr};
    this->rw_locker_.lock_shared();
    auto table_iter = this->tables_.find(table_name);
    if (table_iter != this->tables_.end()) {
        table_meta = table_iter->second.get();
        this->rw_locker_.unlock_shared();

        LOG_TRACE(Format("Add new table entry for {} in existed table meta of db_entry {}", table_name, *this->db_entry_dir_));

    } else {
        this->rw_locker_.unlock_shared();

        LOG_TRACE(Format("Create new table/collection: {}", table_name));
        UniquePtr<TableMeta> new_table_meta = MakeUnique<TableMeta>(this->db_entry_dir_, table_collection_name, this);
        table_meta = new_table_meta.get();

        this->rw_locker_.lock();
        auto table_iter2 = this->tables_.find(table_name);
        if (table_iter2 != this->tables_.end()) {
            table_meta = table_iter2->second.get();
        } else {
            this->tables_[table_name] = Move(new_table_meta);
        }
        this->rw_locker_.unlock();

        LOG_TRACE(Format("Add new table entry for {} in new table meta of db_entry {} ", table_name, *this->db_entry_dir_));
    }

    return table_meta->CreateNewEntry(table_entry_type, table_collection_name, columns, txn_id, begin_ts, txn_mgr);
}

Tuple<TableEntry *, Status>
DBEntry::DropTable(const String &table_collection_name, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    this->rw_locker_.lock_shared();

    TableMeta *table_meta{nullptr};
    if (this->tables_.find(table_collection_name) != this->tables_.end()) {
        table_meta = this->tables_[table_collection_name].get();
    }
    this->rw_locker_.unlock_shared();
    if (table_meta == nullptr) {
        if (conflict_type == ConflictType::kIgnore) {
            LOG_TRACE(Format("Ignore drop a not existed table/collection entry {}", table_collection_name));
            return {nullptr, Status::OK()};
        }

        UniquePtr<String> err_msg = MakeUnique<String>(Format("Attempt to drop not existed table/collection entry {}", table_collection_name));
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }

    LOG_TRACE(Format("Drop a table/collection entry {}", table_collection_name));
    return table_meta->DropNewEntry(txn_id, begin_ts, txn_mgr, table_collection_name, conflict_type);
}

Tuple<TableEntry *, Status> DBEntry::GetTableCollection(const String &table_collection_name, u64 txn_id, TxnTimeStamp begin_ts) {
    this->rw_locker_.lock_shared();

    TableMeta *table_meta{nullptr};
    if (this->tables_.find(table_collection_name) != this->tables_.end()) {
        table_meta = this->tables_[table_collection_name].get();
    }
    this->rw_locker_.unlock_shared();

    //    LOG_TRACE("Get a table entry {}", table_name);
    if (table_meta == nullptr) {
        UniquePtr<String> err_msg = MakeUnique<String>("No valid db meta.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kNotFound, Move(err_msg))};
    }
    return table_meta->GetEntry(txn_id, begin_ts);
}

void DBEntry::RemoveTableEntry(const String &table_collection_name, u64 txn_id, TxnManager *txn_mgr) {
    this->rw_locker_.lock_shared();

    TableMeta *table_meta{nullptr};
    if (this->tables_.find(table_collection_name) != this->tables_.end()) {
        table_meta = this->tables_[table_collection_name].get();
    }
    this->rw_locker_.unlock_shared();

    LOG_TRACE(Format("Remove a table/collection entry: {}", table_collection_name));
    table_meta->DeleteNewEntry(txn_id, txn_mgr);
}

Vector<TableEntry *> DBEntry::TableCollections(u64 txn_id, TxnTimeStamp begin_ts) {
    Vector<TableEntry *> results;

    this->rw_locker_.lock_shared();

    results.reserve(this->tables_.size());
    for (auto &table_collection_meta_pair : this->tables_) {
        TableMeta *table_meta = table_collection_meta_pair.second.get();
        auto [table_entry, status] = table_meta->GetEntry(txn_id, begin_ts);
        if (!status.ok()) {
            LOG_TRACE(Format("error when get table/collection entry: {}", status.message()));
        } else {
            results.emplace_back((TableEntry *)table_entry);
        }
    }
    this->rw_locker_.unlock_shared();

    return results;
}

Status DBEntry::GetTablesDetail(u64 txn_id, TxnTimeStamp begin_ts, Vector<TableDetail> &output_table_array) {
    Vector<TableEntry *> table_collection_entries = this->TableCollections(txn_id, begin_ts);
    output_table_array.reserve(table_collection_entries.size());
    for (TableEntry *table_entry : table_collection_entries) {
        TableDetail table_detail;
        table_detail.db_name_ = this->db_name_;
        table_detail.table_name_ = table_entry->GetTableName();
        table_detail.table_entry_type_ = table_entry->EntryType();
        table_detail.column_count_ = table_entry->ColumnCount();
        table_detail.row_count_ = table_entry->RowCount();
        table_detail.segment_capacity_ = DEFAULT_SEGMENT_CAPACITY;

        SharedPtr<BlockIndex> segment_index = table_entry->GetBlockIndex(txn_id, begin_ts);

        table_detail.segment_count_ = segment_index->SegmentCount();
        table_detail.block_count_ = segment_index->BlockCount();
        output_table_array.emplace_back(table_detail);
    }
    return Status::OK();
}

SharedPtr<String> DBEntry::ToString(DBEntry *db_entry) {
    SharedLock<RWMutex> r_locker(db_entry->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(
        Format("DBEntry, db_entry_dir: {}, txn id: {}, table count: ", *db_entry->db_entry_dir_, db_entry->txn_id_, db_entry->tables_.size()));
    return res;
}

Json DBEntry::Serialize(DBEntry *db_entry, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;

    Vector<TableMeta *> table_metas;
    {
        SharedLock<RWMutex> lck(db_entry->rw_locker_);
        json_res["db_entry_dir"] = *db_entry->db_entry_dir_;
        json_res["db_name"] = *db_entry->db_name_;
        json_res["txn_id"] = db_entry->txn_id_.load();
        json_res["begin_ts"] = db_entry->begin_ts_;
        json_res["commit_ts"] = db_entry->commit_ts_.load();
        json_res["deleted"] = db_entry->deleted_;
        json_res["entry_type"] = db_entry->entry_type_;
        table_metas.reserve(db_entry->tables_.size());
        for (auto &table_meta_pair : db_entry->tables_) {
            table_metas.push_back(table_meta_pair.second.get());
        }
    }
    for (TableMeta *table_meta : table_metas) {
        json_res["tables"].emplace_back(TableMeta::Serialize(table_meta, max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<DBEntry> DBEntry::Deserialize(const Json &db_entry_json, BufferManager *buffer_mgr) {
    Json json_res;

    SharedPtr<String> db_entry_dir = MakeShared<String>(db_entry_json["db_entry_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_entry_json["db_name"]);
    u64 txn_id = db_entry_json["txn_id"];
    u64 begin_ts = db_entry_json["begin_ts"];
    UniquePtr<DBEntry> res = MakeUnique<DBEntry>(db_entry_dir, db_name, txn_id, begin_ts);

    u64 commit_ts = db_entry_json["commit_ts"];
    bool deleted = db_entry_json["deleted"];
    EntryType entry_type = db_entry_json["entry_type"];
    res->commit_ts_ = commit_ts;
    res->deleted_ = deleted;
    res->entry_type_ = entry_type;
    res->db_entry_dir_ = db_entry_dir;

    if (db_entry_json.contains("tables")) {
        for (const auto &table_meta_json : db_entry_json["tables"]) {
            UniquePtr<TableMeta> table_meta = TableMeta::Deserialize(table_meta_json, res.get(), buffer_mgr);
            res->tables_.emplace(*table_meta->table_name_, Move(table_meta));
        }
    }

    return res;
}

void DBEntry::MergeFrom(BaseEntry &other) {
    if (other.entry_type_ != EntryType::kDatabase) {
        Error<StorageException>("MergeFrom requires the same type of BaseEntry");
    }
    DBEntry *db_entry2 = static_cast<DBEntry *>(&other);

    // No locking here since only the load stage needs MergeFrom.
    if (!IsEqual(*this->db_name_, *db_entry2->db_name_)) {
        Error<StorageException>("DBEntry::MergeFrom requires db_name_ match");
    }
    if (!IsEqual(*this->db_entry_dir_, *db_entry2->db_entry_dir_)) {
        Error<StorageException>("DBEntry::MergeFrom requires db_entry_dir_ match");
    }
    for (auto &[table_name, table_meta2] : db_entry2->tables_) {
        auto it = this->tables_.find(table_name);
        if (it == this->tables_.end()) {
            table_meta2->db_entry_ = this;
            this->tables_.emplace(table_name, Move(table_meta2));
        } else {
            it->second->MergeFrom(*table_meta2.get());
        }
    }
}

} // namespace infinity
