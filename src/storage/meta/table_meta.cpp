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

module table_meta;

import stl;
import table_entry_type;

import logger;
import default_values;
import third_party;
import txn_state;
import txn_manager;
import buffer_manager;
import catalog_delta_entry;
import local_file_system;
import third_party;
import status;
import infinity_exception;
import column_def;
import block_index;

namespace infinity {

UniquePtr<TableMeta> TableMeta::NewTableMeta(const SharedPtr<String> &db_entry_dir, const SharedPtr<String> &table_name, DBEntry *db_entry) {
    auto table_meta = MakeUnique<TableMeta>(db_entry_dir, table_name, db_entry);

    return table_meta;
}
/**
 * @brief Create a new table entry.
 *        LIST: [👇(insert a new).... table_entry2 , table_entry1 , dummy_entry] insert new table entry from front.
 * @param table_meta
 * @param table_entry_type
 * @param table_collection_name_ptr
 * @param columns
 * @param txn_id
 * @param begin_ts
 * @param txn_mgr
 * @return Status
 */
Tuple<TableEntry *, Status> TableMeta::CreateEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                   TableEntryType table_entry_type,
                                                   const SharedPtr<String> &table_name,
                                                   const Vector<SharedPtr<ColumnDef>> &columns,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnManager *txn_mgr,
                                                   ConflictType conflict_type) {
    auto init_table_entry = [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
        return TableEntry::NewTableEntry(false, this->db_entry_dir_, table_name, columns, table_entry_type, this, txn_id, begin_ts);
    };
    return table_entry_list_.AddEntry(std::move(r_lock), std::move(init_table_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<TableEntry>, Status> TableMeta::DropEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                          TransactionID txn_id,
                                                          TxnTimeStamp begin_ts,
                                                          TxnManager *txn_mgr,
                                                          const String &table_name,
                                                          ConflictType conflict_type) {
    auto init_drop_entry = [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
        Vector<SharedPtr<ColumnDef>> dummy_columns;
        return TableEntry::NewTableEntry(true,
                                         this->db_entry_dir_,
                                         this->table_name_,
                                         dummy_columns,
                                         TableEntryType::kTableEntry,
                                         this,
                                         txn_id,
                                         begin_ts);
    };
    return table_entry_list_.DropEntry(std::move(r_lock), std::move(init_drop_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<TableInfo>, Status>
TableMeta::GetTableInfo(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_entry, status] = table_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SharedPtr<TableInfo> table_info = MakeShared<TableInfo>();
    table_info->table_name_ = table_name_;
    table_info->table_entry_dir_ = table_entry->TableEntryDir();
    table_info->column_count_ = table_entry->ColumnCount();
    table_info->row_count_ = table_entry->row_count();

    SharedPtr<BlockIndex> segment_index = table_entry->GetBlockIndex(begin_ts);
    table_info->segment_count_ = segment_index->SegmentCount();

    return {table_info, status};
}

void TableMeta::DeleteEntry(TransactionID txn_id) { auto erase_list = table_entry_list_.DeleteEntry(txn_id); }

void TableMeta::CreateEntryReplay(std::function<SharedPtr<TableEntry>(TableMeta *, SharedPtr<String>, TransactionID, TxnTimeStamp)> &&init_entry,
                                  TransactionID txn_id,
                                  TxnTimeStamp begin_ts) {
    auto [entry, status] =
        table_entry_list_.AddEntryReplay([&](TransactionID txn_id, TxnTimeStamp begin_ts) { return init_entry(this, table_name_, txn_id, begin_ts); },
                                         txn_id,
                                         begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

void TableMeta::DropEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [table_entry, status] = table_entry_list_.DropEntryReplay(txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    table_entry->Cleanup();
}

TableEntry *TableMeta::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [entry, status] = table_entry_list_.GetEntryReplay(txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return entry;
}

const SharedPtr<String> &TableMeta::db_name_ptr() const { return db_entry_->db_name_ptr(); }

SharedPtr<String> TableMeta::ToString() {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker());
    SharedPtr<String> res = MakeShared<String>(
        fmt::format("TableMeta, db_entry_dir: {}, table name: {}, entry count: ", *db_entry_dir_, *table_name_, table_entry_list().size()));
    return res;
}

nlohmann::json TableMeta::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<TableEntry *> table_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["db_entry_dir"] = *this->db_entry_dir_;
        json_res["table_name"] = *this->table_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        table_candidates.reserve(this->table_entry_list().size());
        for (auto &table_entry : this->table_entry_list()) {
            if (table_entry->entry_type_ == EntryType::kTable && table_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                table_candidates.push_back((TableEntry *)table_entry.get());
            }
        }
    }
    for (TableEntry *table_entry : table_candidates) {
        json_res["table_entries"].emplace_back(table_entry->Serialize(max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

/**
 * @brief Deserialize the table meta from json.
 *        The table meta is a list of table entries in reverse order.
 *        Same as CreateNewEntry, the last entry is a dummy entry.
 *        LIST: [👇(a new entry).... table_entry2 , table_entry1 , dummy_entry]
 *        The raw catalog is json, so the dummy entry is not included.
 *        The dummy entry is added during the deserialization.
 * @param table_meta_json
 * @param db_entry
 * @param buffer_mgr
 * @return UniquePtr<TableMeta>
 */
UniquePtr<TableMeta> TableMeta::Deserialize(const nlohmann::json &table_meta_json, DBEntry *db_entry, BufferManager *buffer_mgr) {
    SharedPtr<String> db_entry_dir = MakeShared<String>(table_meta_json["db_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_meta_json["table_name"]);
    LOG_TRACE(fmt::format("load table {}", *table_name));
    UniquePtr<TableMeta> res = MakeUnique<TableMeta>(db_entry_dir, table_name, db_entry);
    if (table_meta_json.contains("table_entries")) {
        for (const auto &table_entry_json : table_meta_json["table_entries"]) {
            UniquePtr<TableEntry> table_entry = TableEntry::Deserialize(table_entry_json, res.get(), buffer_mgr);
            res->table_entry_list().emplace_back(std::move(table_entry));
        }
    }
    res->table_entry_list().sort(
        [](const SharedPtr<BaseEntry> &ent1, const SharedPtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });

    return res;
}

void TableMeta::Cleanup() { table_entry_list_.Cleanup(); }

bool TableMeta::PickCleanup(CleanupScanner *scanner) { return table_entry_list_.PickCleanup(scanner); }

} // namespace infinity
