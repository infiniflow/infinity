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
import extra_ddl_info;
import cleanup_scanner;
import infinity_context;

namespace infinity {

Vector<std::string_view> DBEntry::DecodeIndex(std::string_view encode) {
    if (encode.empty() || encode[0] != '#') {
        String error_message = fmt::format("Invalid db entry encode: {}", encode);
        UnrecoverableError(error_message);
    }
    return {encode.substr(1)};
}

String DBEntry::EncodeIndex(const String &db_name) { return fmt::format("#{}", db_name); }

DBEntry::DBEntry(DBMeta *db_meta,
                 bool is_delete,
                 const SharedPtr<String> &db_entry_dir,
                 const SharedPtr<String> &db_name,
                 const SharedPtr<String> &comment,
                 TransactionID txn_id,
                 TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kDatabase, is_delete, DBEntry::EncodeIndex(*db_name)), db_meta_(db_meta), db_entry_dir_(db_entry_dir), db_name_(db_name),
      db_comment_(comment) {
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

SharedPtr<DBEntry> DBEntry::NewDBEntry(DBMeta *db_meta,
                                       bool is_delete,
                                       const SharedPtr<String> &db_name,
                                       const SharedPtr<String> &comment,
                                       TransactionID txn_id,
                                       TxnTimeStamp begin_ts) {
    SharedPtr<String> db_entry_dir;
    if (is_delete) {
        db_entry_dir = MakeShared<String>("deleted");
    } else {
        db_entry_dir = DetermineDBDir(*db_name);
    }
    return MakeShared<DBEntry>(db_meta, is_delete, db_entry_dir, db_name, comment, txn_id, begin_ts);
}

SharedPtr<String> DBEntry::DetermineDBDir(const String &db_name) {
    return DetermineRandomString(InfinityContext::instance().config()->DataDir(), fmt::format("db_{}", db_name));
}

SharedPtr<DBEntry> DBEntry::ReplayDBEntry(DBMeta *db_meta,
                                          bool is_delete,
                                          const SharedPtr<String> &db_entry_dir,
                                          const SharedPtr<String> &db_name,
                                          const SharedPtr<String> &comment,
                                          TransactionID txn_id,
                                          TxnTimeStamp begin_ts,
                                          TxnTimeStamp commit_ts) noexcept {
    auto db_entry = MakeShared<DBEntry>(db_meta, is_delete, db_entry_dir, db_name, comment, txn_id, begin_ts);
    db_entry->commit_ts_ = commit_ts;
    return db_entry;
}

SharedPtr<String> DBEntry::AbsoluteDir() const { return MakeShared<String>(Path(InfinityContext::instance().config()->DataDir()) / *db_entry_dir_); }

Tuple<TableEntry *, Status> DBEntry::CreateTable(TableEntryType table_entry_type,
                                                 const SharedPtr<String> &table_name,
                                                 const SharedPtr<String> &table_comment,
                                                 const Vector<SharedPtr<ColumnDef>> &columns,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr,
                                                 ConflictType conflict_type) {
    auto init_table_meta = [&]() { return TableMeta::NewTableMeta(this->db_entry_dir_, table_name, this); };
    LOG_TRACE(fmt::format("Adding new table entry: {}", *table_name));
    auto [table_meta, r_lock] = this->table_meta_map_.GetMeta(*table_name, std::move(init_table_meta));
    return table_meta->CreateEntry(std::move(r_lock), table_entry_type, table_name, table_comment, columns, txn_id, begin_ts, txn_mgr, conflict_type);
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

Tuple<SharedPtr<TableInfo>, Status> DBEntry::GetTableInfo(const String &table_name, Txn *txn) {
    LOG_TRACE(fmt::format("Get a table entry {}", table_name));
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_name, ConflictType::kError);
    if (table_meta == nullptr) {
        return {nullptr, status};
    }

    return table_meta->GetTableInfo(std::move(r_lock), txn);
}

void DBEntry::RemoveTableEntry(const String &table_name, TransactionID txn_id) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(table_name, ConflictType::kError);
    if (!status.ok()) {
        String error_message = fmt::format("error when get table/collection entry: {}", status.message());
        UnrecoverableError(error_message);
    }
    LOG_TRACE(fmt::format("Remove a db entry: {}", table_name));
    table_meta->DeleteEntry(txn_id);
}

Status DBEntry::AddTable(SharedPtr<TableEntry> table_entry, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, bool add_if_found) {
    auto init_table_meta = [&]() { return TableMeta::NewTableMeta(this->db_entry_dir_, table_entry->GetTableName(), this); };
    const String &table_name = *table_entry->GetTableName();
    LOG_TRACE(fmt::format("Adding new table entry: {}", table_name));
    auto [table_meta, r_lock] = this->table_meta_map_.GetMeta(table_name, std::move(init_table_meta));
    return table_meta->AddEntry(std::move(r_lock), table_entry, txn_id, begin_ts, txn_mgr, add_if_found);
}

void DBEntry::CreateTableReplay(
    const SharedPtr<String> &table_name,
    const SharedPtr<String> &table_comment,
    std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    TransactionID txn_id,
    TxnTimeStamp begin_ts) {
    auto init_table_meta = [&]() { return TableMeta::NewTableMeta(this->db_entry_dir_, table_name, this); };
    auto *table_meta = table_meta_map_.GetMetaNoLock(*table_name, std::move(init_table_meta));
    table_meta->CreateEntryReplay(
        [&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(table_meta, table_name, table_comment, txn_id, begin_ts); },
        txn_id,
        begin_ts);
}

void DBEntry::UpdateTableReplay(
    const SharedPtr<String> &table_name,
    const SharedPtr<String> &table_comment,
    std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    TransactionID txn_id,
    TxnTimeStamp begin_ts) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(*table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    table_meta->UpdateEntryReplay(
        [&](SharedPtr<TableEntry> dst_table_entry, TransactionID txn_id, TxnTimeStamp begin_ts) {
            auto src_table_entry = init_entry(table_meta, table_name, table_comment, txn_id, begin_ts);
            dst_table_entry->UpdateEntryReplay(src_table_entry);
        },
        txn_id,
        begin_ts);
}

void DBEntry::DropTableReplay(
    const String &table_name,
    std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
    TransactionID txn_id,
    TxnTimeStamp begin_ts) {
    auto [table_meta, status] = table_meta_map_.GetExistMetaNoLock(table_name, ConflictType::kError);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    table_meta->DropEntryReplay(
        [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
            return init_entry(table_meta, table_meta->table_name_ptr(), MakeShared<String>(), txn_id, begin_ts);
        },
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
                LOG_TRACE(fmt::format("error when get table entry: {} table name: {}", status.message(), *table_meta->table_name_));
            } else {
                results.emplace_back((TableEntry *)table_entry);
            }
        }
    }

    return results;
}

Status DBEntry::GetTablesDetail(Txn *txn, Vector<TableDetail> &output_table_array) {
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp begin_ts = txn->BeginTS();
    Vector<TableEntry *> table_collection_entries = this->TableCollections(txn_id, begin_ts);
    output_table_array.reserve(table_collection_entries.size());
    for (TableEntry *table_entry : table_collection_entries) {
        TableDetail table_detail;
        table_detail.db_name_ = this->db_name_;
        table_detail.table_name_ = table_entry->GetTableName();
        table_detail.table_name_ = table_entry->GetTableComment();
        table_detail.table_entry_type_ = table_entry->EntryType();
        table_detail.column_count_ = table_entry->ColumnCount();
        table_detail.row_count_ = table_entry->row_count();
        table_detail.segment_capacity_ = DEFAULT_SEGMENT_CAPACITY;
        table_detail.block_capacity_ = DEFAULT_BLOCK_CAPACITY;

        SharedPtr<BlockIndex> segment_index = table_entry->GetBlockIndex(txn);

        table_detail.segment_count_ = segment_index->SegmentCount();
        table_detail.block_count_ = segment_index->BlockCount();
        output_table_array.emplace_back(table_detail);
    }
    return Status::OK();
}

Tuple<Vector<String>, Vector<TableMeta *>, std::shared_lock<std::shared_mutex>> DBEntry::GetAllTableMetas() const {
    return table_meta_map_.GetAllMetaGuard();
}

SharedPtr<String> DBEntry::ToString() {
    SharedPtr<String> res =
        MakeShared<String>(fmt::format("DBEntry, db_entry_dir: {}, txn id: {}, table count: {}", *AbsoluteDir(), txn_id_, table_meta_map_.Size()));
    return res;
}

nlohmann::json DBEntry::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;

    json_res["db_name"] = *this->db_name_;

    String& db_comment = *this->db_comment_;
    if(!db_comment.empty()) {
        json_res["db_comment"] = db_comment;
    }

    json_res["txn_id"] = this->txn_id_;
    json_res["begin_ts"] = this->begin_ts_;
    json_res["commit_ts"] = this->commit_ts_.load();
    json_res["deleted"] = this->deleted_;
    if (!this->deleted_) {
        json_res["db_entry_dir"] = *this->db_entry_dir_;
    }
    json_res["entry_type"] = this->entry_type_;

    {
        auto [_, table_meta_ptrs, meta_lock] = table_meta_map_.GetAllMetaGuard();
        for (TableMeta *table_meta : table_meta_ptrs) {
            json_res["tables"].emplace_back(table_meta->Serialize(max_commit_ts));
        }
    }

    return json_res;
}

UniquePtr<DBEntry> DBEntry::Deserialize(const nlohmann::json &db_entry_json, DBMeta *db_meta, BufferManager *buffer_mgr) {
    nlohmann::json json_res;

    bool deleted = db_entry_json["deleted"];
    SharedPtr<String> db_name = MakeShared<String>(db_entry_json["db_name"]);

    SharedPtr<String> db_comment = nullptr;
    if(db_entry_json.contains("db_comment")) {
        db_comment = MakeShared<String>(db_entry_json["db_comment"]);
    } else {
        db_comment = MakeShared<String>();
    }

    TransactionID txn_id = db_entry_json["txn_id"];
    u64 begin_ts = db_entry_json["begin_ts"];

    SharedPtr<String> db_entry_dir{};
    if (!deleted) {
        db_entry_dir = MakeShared<String>(db_entry_json["db_entry_dir"]);
    }
    UniquePtr<DBEntry> db_entry = MakeUnique<DBEntry>(db_meta, deleted, db_entry_dir, db_name, db_comment, txn_id, begin_ts);

    u64 commit_ts = db_entry_json["commit_ts"];
    db_entry->commit_ts_.store(commit_ts);

    if (db_entry_json.contains("tables")) {
        for (const auto &table_meta_json : db_entry_json["tables"]) {
            UniquePtr<TableMeta> table_meta = TableMeta::Deserialize(table_meta_json, db_entry.get(), buffer_mgr);
            db_entry->table_meta_map_.AddNewMetaNoLock(*table_meta->table_name_, std::move(table_meta));
        }
    }

    return db_entry;
}

String DBEntry::GetPathNameTail() const {
    SizeT delimiter_i = db_entry_dir_->rfind('/');
    if (delimiter_i == String::npos) {
        return *db_entry_dir_;
    }
    return db_entry_dir_->substr(delimiter_i + 1);
}

void DBEntry::PickCleanup(CleanupScanner *scanner) { table_meta_map_.PickCleanup(scanner); }

void DBEntry::Cleanup(CleanupInfoTracer *info_tracer, bool dropped) {
    if (this->deleted_) {
        return;
    }
    table_meta_map_.Cleanup(info_tracer, dropped);

    if (dropped) {
        SharedPtr<String> full_db_dir = AbsoluteDir();
        LOG_DEBUG(fmt::format("Cleaning up db dir: {}", *full_db_dir));
        CleanupScanner::CleanupDir(*full_db_dir);
        LOG_DEBUG(fmt::format("Cleaned db dir: {}", *full_db_dir));
        if (info_tracer) {
            info_tracer->AddCleanupInfo(std::move(*full_db_dir));
        }
    }
}

Vector<String> DBEntry::GetFilePath(TransactionID txn_id, TxnTimeStamp begin_ts) const {
    String error_message = "Unimplemented";
    UnrecoverableError(error_message);
    return Vector<String>();
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

void DBEntry::MemIndexRecover(BufferManager *buffer_manager, TxnTimeStamp ts) {
    auto table_meta_map_guard = table_meta_map_.GetMetaMap();
    for (auto &[_, table_meta] : *table_meta_map_guard) {
        auto [table_entry, status] = table_meta->GetEntryNolock(0UL, MAX_TIMESTAMP);
        if (status.ok()) {
            table_entry->MemIndexRecover(buffer_manager, ts);
        }
    }
}

} // namespace infinity
