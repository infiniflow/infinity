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
import table_detail;
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

namespace infinity {

DBEntry::DBEntry(DBMeta *db_meta,
                 bool is_delete,
                 const SharedPtr<String> &data_dir,
                 const SharedPtr<String> &db_name,
                 TransactionID txn_id,
                 TxnTimeStamp begin_ts)
    // "data_dir": "/tmp/infinity/data"
    // "db_entry_dir": "/tmp/infinity/data/db1/txn_6"
    : BaseEntry(EntryType::kDatabase, is_delete), db_meta_(db_meta), db_entry_dir_(is_delete ? nullptr : DetermineDBDir(*data_dir, *db_name)),
      db_name_(db_name) {
    //    atomic_u64 txn_id_{0};
    //    TxnTimeStamp begin_ts_{0};
    //    atomic_u64 commit_ts_{UNCOMMIT_TS};
    //    bool deleted_{false};
    begin_ts_ = begin_ts;
    txn_id_.store(txn_id);
}

SharedPtr<DBEntry> DBEntry::NewDBEntry(DBMeta *db_meta,
                                       bool is_delete,
                                       const SharedPtr<String> &data_dir,
                                       const SharedPtr<String> &db_name,
                                       TransactionID txn_id,
                                       TxnTimeStamp begin_ts,
                                       TxnManager *txn_mgr) {
    auto db_entry = MakeShared<DBEntry>(db_meta, is_delete, data_dir, db_name, txn_id, begin_ts);
    if (txn_mgr) {
        auto *txn = txn_mgr->GetTxn(txn_id);
        if (is_delete) {
            txn->DropDBStore(db_entry.get());
        } else {
            txn->AddDBStore(db_entry.get());
        }
    }
    return db_entry;
}

SharedPtr<DBEntry> DBEntry::NewReplayDBEntry(DBMeta *db_meta,
                                             const SharedPtr<String> &data_dir,
                                             const SharedPtr<String> &db_name,
                                             TransactionID txn_id,
                                             TxnTimeStamp begin_ts,
                                             TxnTimeStamp commit_ts,
                                             bool is_delete) noexcept {
    auto db_entry = MakeShared<DBEntry>(db_meta, is_delete, data_dir, db_name, txn_id, begin_ts);
    db_entry->commit_ts_ = commit_ts;
    return db_entry;
}

Tuple<TableEntry *, Status> DBEntry::CreateTable(TableEntryType table_entry_type,
                                                 const SharedPtr<String> &table_collection_name,
                                                 const Vector<SharedPtr<ColumnDef>> &columns,
                                                 TransactionID txn_id,
                                                 TxnTimeStamp begin_ts,
                                                 TxnManager *txn_mgr,
                                                 ConflictType conflict_type) {
    const String &table_name = *table_collection_name;
    auto init_table_meta = [&]() { return TableMeta::NewTableMeta(this->db_entry_dir_, table_collection_name, this); };
    LOG_TRACE(fmt::format("Adding new table entry: {}", table_name));
    auto [table_meta, r_lock] = this->table_meta_map_.GetMeta(table_name, std::move(init_table_meta), txn_id, begin_ts, txn_mgr);

    return table_meta->CreateNewEntry(std::move(r_lock), table_entry_type, table_collection_name, columns, txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<TableEntry *, Status> DBEntry::DropTable(const String &table_collection_name,
                                               ConflictType conflict_type,
                                               TransactionID txn_id,
                                               TxnTimeStamp begin_ts,
                                               TxnManager *txn_mgr) {
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_collection_name, conflict_type);
    if (table_meta == nullptr) {
        return {nullptr, status};
    }
    return table_meta->DropNewEntry(std::move(r_lock), txn_id, begin_ts, txn_mgr, table_collection_name, conflict_type);
}

Tuple<TableEntry *, Status> DBEntry::GetTableCollection(const String &table_name, TransactionID txn_id, TxnTimeStamp begin_ts) {
    LOG_TRACE(fmt::format("Get a table entry {}", table_name));
    auto [table_meta, status, r_lock] = table_meta_map_.GetExistMeta(table_name, ConflictType::kError);
    if (table_meta == nullptr) {
        return {nullptr, status};
    }
    return table_meta->GetEntry(std::move(r_lock), txn_id, begin_ts);
}

void DBEntry::RemoveTableEntry(const String &table_name, TransactionID txn_id) {
    auto [table_meta, _1, r_lock] = table_meta_map_.GetExistMeta(table_name, ConflictType::kError);
    LOG_TRACE(fmt::format("Remove a db entry: {}", table_name));
    table_meta->DeleteNewEntry(txn_id);
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

nlohmann::json DBEntry::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
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
        json_res["tables"].emplace_back(table_meta->Serialize(max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<DBEntry> DBEntry::Deserialize(const nlohmann::json &db_entry_json, DBMeta *db_meta, BufferManager *buffer_mgr) {
    nlohmann::json json_res;

    SharedPtr<String> db_entry_dir = MakeShared<String>(db_entry_json["db_entry_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_entry_json["db_name"]);
    TransactionID txn_id = db_entry_json["txn_id"];
    u64 begin_ts = db_entry_json["begin_ts"];
    bool deleted = db_entry_json["deleted"];
    UniquePtr<DBEntry> res = MakeUnique<DBEntry>(db_meta, deleted, db_entry_dir, db_name, txn_id, begin_ts);

    u64 commit_ts = db_entry_json["commit_ts"];
//    EntryType entry_type = db_entry_json["entry_type"];
    res->commit_ts_.store(commit_ts);
    res->deleted_ = deleted;

    if (db_entry_json.contains("tables")) {
        for (const auto &table_meta_json : db_entry_json["tables"]) {
            UniquePtr<TableMeta> table_meta = TableMeta::Deserialize(table_meta_json, res.get(), buffer_mgr);
            res->table_meta_map().emplace(*table_meta->table_name_, std::move(table_meta));
        }
    }

    return res;
}

void DBEntry::PickCleanup(CleanupScanner *scanner) { table_meta_map_.PickCleanup(scanner); }

void DBEntry::Cleanup() {
    if (this->deleted_) {
        return;
    }
    table_meta_map_.Cleanup();

    LOG_INFO(fmt::format("Cleanup dir: {}", *db_entry_dir_));
    LocalFileSystem fs;
    fs.DeleteEmptyDirectory(*db_entry_dir_);
}

} // namespace infinity
