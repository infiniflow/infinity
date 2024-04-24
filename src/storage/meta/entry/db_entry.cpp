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

module db_entry;

import table_entry_type;
import stl;

import txn_manager;
import meta_info;
import buffer_manager;
import default_values;
import block_index;
import logger;
import third_party;
import infinity_exception;
import status;
import catalog_delta_entry;
import column_def;
import local_file_system;
import extra_ddl_info;
import cleanup_scanner;

namespace infinity {

String DBEntry::EncodeIndex(const String &db_name) { return fmt::format("#{}", db_name); }

DBEntry::DBEntry(DBMeta *db_meta,
                 bool is_delete,
                 const SharedPtr<String> &db_entry_dir,
                 const SharedPtr<String> &db_name,
                 TransactionID txn_id,
                 TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kDatabase, is_delete, DBEntry::EncodeIndex(*db_name)), db_meta_(db_meta), db_entry_dir_(db_entry_dir), db_name_(db_name) {
    begin_ts_ = begin_ts;
    txn_id_.store(txn_id);
}

SharedPtr<DBEntry> DBEntry::NewDBEntry(DBMeta *db_meta,
                                       bool is_delete,
                                       const SharedPtr<String> &data_dir,
                                       const SharedPtr<String> &db_name,
                                       TransactionID txn_id,
                                       TxnTimeStamp begin_ts) {
    SharedPtr<String> db_entry_dir = is_delete ? MakeShared<String>("deleted") : DetermineDBDir(*data_dir, *db_name);
    return MakeShared<DBEntry>(db_meta, is_delete, db_entry_dir, db_name, txn_id, begin_ts);
}

SharedPtr<DBEntry> DBEntry::ReplayDBEntry(DBMeta *db_meta,
                                          bool is_delete,
                                          const SharedPtr<String> &db_entry_dir,
                                          const SharedPtr<String> &db_name,
                                          TransactionID txn_id,
                                          TxnTimeStamp begin_ts,
                                          TxnTimeStamp commit_ts) noexcept {
    auto db_entry = MakeShared<DBEntry>(db_meta, is_delete, db_entry_dir, db_name, txn_id, begin_ts);
    db_entry->commit_ts_ = commit_ts;
    return db_entry;
}

Tuple<TableEntry *, Status> DBEntry::CreateTable(TableEntryType table_entry_type,
                                                 const SharedPtr<String> &table_name,
                                                 const Vector<SharedPtr<ColumnDef>> &columns,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr,
                                                 ConflictType conflict_type) {
    auto init_table_meta = [&]() { return TableMeta::NewTableMeta(this->db_entry_dir_, table_name, this); };
    LOG_TRACE(fmt::format("Adding new table entry: {}", *table_name));
    auto [table_meta, r_lock] = this->table_meta_map_.GetMeta(*table_name, std::move(init_table_meta));
    return table_meta->CreateEntry(std::move(r_lock), table_entry_type, table_name, columns, txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<TableEntry>, Status>
DBEntry::DropTable(const String &table_name, ConflictType conflict_type, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_name, conflict_type);
    if (!status.ok()) {
        return {nullptr, status};
    }
    return table_meta->DropEntry(std::move(r_lock), txn_id, begin_ts, txn_mgr, table_name, conflict_type);
}

Tuple<TableEntry *, Status> DBEntry::GetTableCollection(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    LOG_TRACE(fmt::format("Get a table entry {}", table_name));
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_name, ConflictType::kError);
    if (table_meta == nullptr) {
        return {nullptr, status};
    }
    return table_meta->GetEntry(std::move(r_lock), txn_id, begin_ts);
}

bool DBEntry::CheckConflict(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts, TableEntry *&table_entry) {
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError("Check conflict if not for non-exist table meta");
    }
    return table_meta->CheckConflict(std::move(r_lock), txn_id, begin_ts, table_entry);
}

Tuple<SharedPtr<TableInfo>, Status> DBEntry::GetTableInfo(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    LOG_TRACE(fmt::format("Get a table entry {}", table_name));
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_name, ConflictType::kError);
    if (table_meta == nullptr) {
        return {nullptr, status};
    }

    return table_meta->GetTableInfo(std::move(r_lock), txn_id, begin_ts);
}

void DBEntry::RemoveTableEntry(const String &table_name, TransactionID txn_id) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("error when get table/collection entry: {}", status.message()));
    }
    LOG_TRACE(fmt::format("Remove a db entry: {}", table_name));
    table_meta->DeleteEntry(txn_id);
}

void DBEntry::CreateTableReplay(const SharedPtr<String> &table_name,
                                std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                                TransactionID txn_id,
                                TxnTimeStamp begin_ts) {
    auto init_table_meta = [&]() { return TableMeta::NewTableMeta(this->db_entry_dir_, table_name, this); };
    auto *table_meta = table_meta_map_.GetMetaNoLock(*table_name, std::move(init_table_meta));
    table_meta->CreateEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(table_meta, table_name, txn_id, begin_ts); },
                                  txn_id,
                                  begin_ts);
}

void DBEntry::UpdateTableReplay(const SharedPtr<String> &table_name,
                                std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                                TransactionID txn_id,
                                TxnTimeStamp begin_ts) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(*table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    table_meta->UpdateEntryReplay(
        [&](SharedPtr<TableEntry> dst_table_entry, TransactionID txn_id, TxnTimeStamp begin_ts) {
            auto src_table_entry = init_entry(table_meta, table_name, txn_id, begin_ts);
            dst_table_entry->UpdateEntryReplay(src_table_entry);
        },
        txn_id,
        begin_ts);
}

void DBEntry::DropTableReplay(const String &table_name,
                              std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                              TransactionID txn_id,
                              TxnTimeStamp begin_ts) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    table_meta->DropEntryReplay(
        [&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(table_meta, table_meta->table_name_ptr(), txn_id, begin_ts); },
        txn_id,
        begin_ts);
}

TableEntry *DBEntry::GetTableReplay(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return table_meta->GetEntryReplay(txn_id, begin_ts);
}

Vector<TableEntry *> DBEntry::TableCollections(TransactionID txn_id, TxnTimeStamp begin_ts) {
    Vector<TableEntry *> results;

    {
        auto map_guard = table_meta_map_.GetMetaMap();
        results.reserve((*map_guard).size());
        for (auto &table_collection_meta_pair : *map_guard) {
            TableMeta *table_meta = table_collection_meta_pair.second.get();
            auto [table_entry, status] = table_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                LOG_TRACE(fmt::format("error when get table/collection entry: {}", status.message()));
            } else {
                results.emplace_back((TableEntry *)table_entry);
            }
        }
    }

    return results;
}

Status DBEntry::GetTablesDetail(TransactionID txn_id, TxnTimeStamp begin_ts, Vector<TableDetail> &output_table_array) {
    Vector<TableEntry *> table_collection_entries = this->TableCollections(txn_id, begin_ts);
    output_table_array.reserve(table_collection_entries.size());
    for (TableEntry *table_entry : table_collection_entries) {
        TableDetail table_detail;
        table_detail.db_name_ = this->db_name_;
        table_detail.table_name_ = table_entry->GetTableName();
        table_detail.table_entry_type_ = table_entry->EntryType();
        table_detail.column_count_ = table_entry->ColumnCount();
        table_detail.row_count_ = table_entry->row_count();
        table_detail.segment_capacity_ = DEFAULT_SEGMENT_CAPACITY;
        table_detail.block_capacity_ = DEFAULT_BLOCK_CAPACITY;

        SharedPtr<BlockIndex> segment_index = table_entry->GetBlockIndex(begin_ts);

        table_detail.segment_count_ = segment_index->SegmentCount();
        table_detail.block_count_ = segment_index->BlockCount();
        output_table_array.emplace_back(table_detail);
    }
    return Status::OK();
}

SharedPtr<String> DBEntry::ToString() {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker());
    SharedPtr<String> res = MakeShared<String>(
        fmt::format("DBEntry, db_entry_dir: {}, txn id: {}, table count: ", *db_entry_dir_, txn_id_, this->table_meta_map().size()));
    return res;
}

nlohmann::json DBEntry::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;

    Vector<TableMeta *> table_metas;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["db_name"] = *this->db_name_;
        json_res["txn_id"] = this->txn_id_.load();
        json_res["begin_ts"] = this->begin_ts_;
        json_res["commit_ts"] = this->commit_ts_.load();
        json_res["deleted"] = this->deleted_;
        if (!this->deleted_) {
            json_res["db_entry_dir"] = *this->db_entry_dir_;
        }
        json_res["entry_type"] = this->entry_type_;
        table_metas.reserve(this->table_meta_map().size());
        for (auto &table_meta_pair : this->table_meta_map()) {
            table_metas.push_back(table_meta_pair.second.get());
        }
    }
    for (TableMeta *table_meta : table_metas) {
        json_res["tables"].emplace_back(table_meta->Serialize(max_commit_ts));
    }
    return json_res;
}

UniquePtr<DBEntry> DBEntry::Deserialize(const nlohmann::json &db_entry_json, DBMeta *db_meta, BufferManager *buffer_mgr) {
    nlohmann::json json_res;

    bool deleted = db_entry_json["deleted"];
    SharedPtr<String> db_name = MakeShared<String>(db_entry_json["db_name"]);
    TransactionID txn_id = db_entry_json["txn_id"];
    u64 begin_ts = db_entry_json["begin_ts"];

    SharedPtr<String> db_entry_dir{};
    if (!deleted) {
        db_entry_dir = MakeShared<String>(db_entry_json["db_entry_dir"]);
    }
    UniquePtr<DBEntry> res = MakeUnique<DBEntry>(db_meta, deleted, db_entry_dir, db_name, txn_id, begin_ts);

    u64 commit_ts = db_entry_json["commit_ts"];
    res->commit_ts_.store(commit_ts);

    if (db_entry_json.contains("tables")) {
        for (const auto &table_meta_json : db_entry_json["tables"]) {
            UniquePtr<TableMeta> table_meta = TableMeta::Deserialize(table_meta_json, res.get(), buffer_mgr);
            res->table_meta_map().emplace(*table_meta->table_name_, std::move(table_meta));
        }
    }

    return res;
}

String DBEntry::GetPathNameTail() const {
    SizeT delimiter_i = db_entry_dir_->rfind('/');
    if (delimiter_i == String::npos) {
        return *db_entry_dir_;
    }
    return db_entry_dir_->substr(delimiter_i + 1);
}

void DBEntry::PickCleanup(CleanupScanner *scanner) { table_meta_map_.PickCleanup(scanner); }

void DBEntry::Cleanup() {
    if (this->deleted_) {
        return;
    }
    table_meta_map_.Cleanup();

    LOG_TRACE(fmt::format("Cleaning up dir: {}", *db_entry_dir_));
    CleanupScanner::CleanupDir(*db_entry_dir_);
    LOG_TRACE(fmt::format("Cleaned dir: {}", *db_entry_dir_));
}

void DBEntry::MemIndexCommit() {
    auto table_meta_map_guard = table_meta_map_.GetMetaMap();
    for (auto &[_, table_meta] : *table_meta_map_guard) {
        auto [table_entry, status] = table_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            table_entry->MemIndexCommit();
        }
    }
}

void DBEntry::MemIndexRecover(BufferManager *buffer_manager) {
    auto table_meta_map_guard = table_meta_map_.GetMetaMap();
    for (auto &[_, table_meta] : *table_meta_map_guard) {
        auto [table_entry, status] = table_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            table_entry->MemIndexRecover(buffer_manager);
        }
    }
}

} // namespace infinity
