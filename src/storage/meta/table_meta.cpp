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
Tuple<TableEntry *, Status> TableMeta::CreateNewEntry(TableEntryType table_entry_type,
                                                      const SharedPtr<String> &table_collection_name_ptr,
                                                      const Vector<SharedPtr<ColumnDef>> &columns,
                                                      TransactionID txn_id,
                                                      TxnTimeStamp begin_ts,
                                                      TxnManager *txn_mgr,
                                                      ConflictType conflict_type) {
    SharedPtr<TableEntry> table_entry =
        TableEntry::NewTableEntry(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);
    return table_entry_list_.AddEntry(table_entry, txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<TableEntry *, Status>
TableMeta::DropNewEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, const String &table_name, ConflictType conflict_type) {
    Vector<SharedPtr<ColumnDef>> dummy_columns;
    SharedPtr<TableEntry> drop_entry =
        TableEntry::NewTableEntry(this->db_entry_dir_, this->table_name_, dummy_columns, TableEntryType::kTableEntry, this, txn_id, begin_ts);
    return table_entry_list_.DropEntry(drop_entry, txn_id, begin_ts, txn_mgr, conflict_type);
}

/**
 * @brief Get the table entry object
 *        LIST: [👇(a new entry).... table_entry2 , table_entry1 , dummy_entry]
 *        Get the first valid table entry from the front.
 *        The table entry is valid if:
 *          1. committed and commit_ts < begin_ts.
 *          2. not dummy entry.
 * @param table_meta
 * @param txn_id
 * @param begin_ts
 * @return Status
 */
Tuple<TableEntry *, Status> TableMeta::GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts) { return table_entry_list_.GetEntry(txn_id, begin_ts); }

void TableMeta::DeleteNewEntry(TransactionID txn_id) { table_entry_list_.DeleteEntry(txn_id); }

const SharedPtr<String> &TableMeta::db_name_ptr() const { return db_entry_->db_name_ptr(); }

const String &TableMeta::db_name() const { return db_entry_->db_name(); }

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
            if (table_entry->entry_type_ == EntryType::kTable) {
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
    auto dummy_entry = MakeUnique<TableEntry>();
    dummy_entry->deleted_ = true;
    res->table_entry_list().emplace_back(std::move(dummy_entry));

    return res;
}

void TableMeta::MergeFrom(TableMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    if (!IsEqual(*this->table_name_, *other.table_name_) || !IsEqual(*this->db_entry_dir_, *other.db_entry_dir_)) {
        UnrecoverableError("DBEntry::MergeFrom requires table_name_ and db_entry_dir_ match");
    }
    this->table_entry_list_.MergeWith(other.table_entry_list_);
}

void TableMeta::Cleanup() && {
    std::move(table_entry_list_).Cleanup();

    String table_meta_dir = fmt::format("{}/{}", *db_entry_dir_, *table_name_);
    // LocalFileSystem fs;
    // fs.DeleteEmptyDirectory(table_meta_dir);
}

bool TableMeta::PickCleanup(CleanupScanner *scanner) { return table_entry_list_.PickCleanup(scanner); }

} // namespace infinity
