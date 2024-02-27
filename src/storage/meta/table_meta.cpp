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
                                                      TxnManager *txn_mgr) {
    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker());
    const String &table_collection_name = *table_collection_name_ptr;

    TableEntry *table_entry_ptr{nullptr};

    if (this->table_entry_list().empty()) {
        auto dummy_entry = MakeUnique<TableEntry>();
        this->table_entry_list().emplace_back(std::move(dummy_entry));

        SharedPtr<TableEntry> table_entry =
            TableEntry::NewTableEntry(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);

        {
            if (txn_mgr != nullptr) {
                auto operation = MakeUnique<AddTableEntryOp>(table_entry);
                txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
            }
        }

        table_entry_ptr = table_entry.get();
        this->table_entry_list().emplace_front(std::move(table_entry));

        LOG_TRACE(fmt::format("New table entry is added: {}.", table_collection_name));
        return {table_entry_ptr, Status::OK()};
    } else {
        // Already have a table entry, check if the table entry is valid here.
        BaseEntry *header_base_entry = this->table_entry_list().front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            // Dummy entry in the header of the list, insert the new table entry to the front.
            SharedPtr<TableEntry> table_entry =
                TableEntry::NewTableEntry(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);

            // physical wal log
            {
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<AddTableEntryOp>(table_entry);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
            }

            table_entry_ptr = table_entry.get();
            this->table_entry_list().emplace_front(std::move(table_entry));
            return {table_entry_ptr, Status::OK()};
        }

        TableEntry *header_table_entry = (TableEntry *)header_base_entry;
        if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if (begin_ts > header_table_entry->commit_ts_) {
                if (header_table_entry->deleted_) {
                    // No conflict
                    SharedPtr<TableEntry> table_entry =
                        TableEntry::NewTableEntry(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);

                    // physical wal log
                    {
                        if (txn_mgr != nullptr) {
                            auto operation = MakeUnique<AddTableEntryOp>(table_entry);
                            txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                        }
                    }

                    table_entry_ptr = table_entry.get();
                    this->table_entry_list().emplace_front(std::move(table_entry));
                    return {table_entry_ptr, Status::OK()};
                } else {
                    // Duplicated table
                    UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Duplicated table: {}.", table_collection_name));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kDuplicateTableName, std::move(err_msg))};
                }
            } else {
                // Write-Write conflict
                UniquePtr<String> err_msg =
                    MakeUnique<String>(fmt::format("Write-write conflict: There is a committed table: {} which is later than current transaction.",
                                                   table_collection_name));
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
            }
        } else {

            TxnState head_table_entry_state = txn_mgr->GetTxnState(header_table_entry->txn_id_);

            switch (head_table_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_table_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_table_entry->deleted_) {
                            SharedPtr<TableEntry> table_entry = TableEntry::NewTableEntry(this->db_entry_dir_,
                                                                                          table_collection_name_ptr,
                                                                                          columns,
                                                                                          table_entry_type,
                                                                                          this,
                                                                                          txn_id,
                                                                                          begin_ts);
                            // physical wal log
                            {
                                if (txn_mgr != nullptr) {
                                    auto operation = MakeUnique<AddTableEntryOp>(table_entry);
                                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                                }
                            }

                            table_entry_ptr = table_entry.get();
                            this->table_entry_list().emplace_front(std::move(table_entry));
                            return {table_entry_ptr, Status::OK()};
                        } else {
                            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Create a duplicated table {}.", table_collection_name));
                            LOG_ERROR(*err_msg);
                            return {nullptr, Status(ErrorCode::kDuplicateTableName, std::move(err_msg))};
                        }
                    } else {
                        UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Write-write conflict: There is a uncommitted transaction."));
                        LOG_ERROR(*err_msg);
                        return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    UniquePtr<String> err_msg = MakeUnique<String>(
                        fmt::format("Write-write conflict: There is a committing/committed table which is later than current transaction."));
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    this->table_entry_list().erase(this->table_entry_list().begin());

                    // Append new one
                    SharedPtr<TableEntry> table_entry =
                        TableEntry::NewTableEntry(this->db_entry_dir_, table_collection_name_ptr, columns, table_entry_type, this, txn_id, begin_ts);

                    // physical wal log
                    {
                        if (txn_mgr != nullptr) {
                            auto operation = MakeUnique<AddTableEntryOp>(table_entry);
                            txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                        }
                    }

                    table_entry_ptr = table_entry.get();
                    this->table_entry_list().emplace_front(std::move(table_entry));
                    return {table_entry_ptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Invalid table entry txn state");
                    LOG_ERROR(*err_msg);
                    UnrecoverableError(*err_msg);
                    return {nullptr, Status(ErrorCode::kUnexpectedError, std::move(err_msg))};
                }
            }
        }
    }
}

Tuple<TableEntry *, Status>
TableMeta::DropNewEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, const String &table_name, ConflictType conflict_type) {

    TableEntry *table_entry_ptr{nullptr};

    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker());
    if (this->table_entry_list().empty()) {
        if (conflict_type == ConflictType::kIgnore) {
            LOG_TRACE(fmt::format("Ignore drop a not existed table entry {}", table_name));
            return {nullptr, Status::OK()};
        }
        UniquePtr<String> err_msg = MakeUnique<String>("Empty table entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kTableNotExist, std::move(err_msg))};
    }

    BaseEntry *header_base_entry = this->table_entry_list().front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        if (conflict_type == ConflictType::kIgnore) {
            LOG_TRACE(fmt::format("Ignore drop a not existed table entry {}", table_name));
            return {nullptr, Status::OK()};
        }
        UniquePtr<String> err_msg = MakeUnique<String>("No valid table entry.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status(ErrorCode::kTableNotExist, std::move(err_msg))};
    }

    auto *header_table_entry = (TableEntry *)header_base_entry;
    if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if (begin_ts > header_table_entry->commit_ts_) {
            // No conflict
            if (header_table_entry->deleted_) {
                if (conflict_type == ConflictType::kIgnore) {
                    LOG_TRACE(fmt::format("Ignore drop a not existed table entry {}", table_name));
                    return {nullptr, Status::OK()};
                }
                UniquePtr<String> err_msg = MakeUnique<String>("Table was dropped before.");
                LOG_ERROR(*err_msg);
                return {nullptr, Status(ErrorCode::kTableNotExist, std::move(err_msg))};
            }

            Vector<SharedPtr<ColumnDef>> dummy_columns;

            SharedPtr<TableEntry> table_entry =
                TableEntry::NewTableEntry(this->db_entry_dir_, this->table_name_, dummy_columns, TableEntryType::kTableEntry, this, txn_id, begin_ts);
            // physical wal log
            {
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<AddTableEntryOp>(table_entry);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
            }

            table_entry_ptr = table_entry.get();
            table_entry_ptr->deleted_ = true;
            this->table_entry_list().emplace_front(std::move(table_entry));

            return {table_entry_ptr, Status::OK()};
        } else {
            // Write-Write conflict
            UniquePtr<String> err_msg =
                MakeUnique<String>(fmt::format("Write-write conflict: There is a committed database which is later than current transaction."));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_table_entry->txn_id_) {
            // Same txn, remove the header table entry
            table_entry_ptr = header_table_entry;

            auto base_entry_ptr = this->table_entry_list().front();
            auto table_entry_ptr = std::static_pointer_cast<TableEntry>(base_entry_ptr);

            // Physical log
            {
                if (txn_mgr != nullptr) {
                    auto operation = MakeUnique<AddTableEntryOp>(table_entry_ptr);
                    txn_mgr->GetTxn(txn_id)->AddCatalogDeltaOperation(std::move(operation));
                }
            }
            this->table_entry_list().pop_front();
            return {table_entry_ptr.get(), Status::OK()};
        } else {
            // Not same txn, issue WW conflict
            UniquePtr<String> err_msg = MakeUnique<String>(fmt::format("Write-write conflict: There is another uncommitted table entry."));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
    }
}

void TableMeta::DeleteNewEntry(TransactionID txn_id, TxnManager *) {
    std::unique_lock<std::shared_mutex> rw_locker(this->rw_locker());
    if (this->table_entry_list().empty()) {
        LOG_TRACE("Empty table entry list.");
        return;
    }

    auto removed_iter = std::remove_if(this->table_entry_list().begin(), this->table_entry_list().end(), [&](SharedPtr<TableEntry> &entry) -> bool {
        return entry->txn_id_ == txn_id;
    });

    this->table_entry_list().erase(removed_iter, this->table_entry_list().end());
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
Tuple<TableEntry *, Status> TableMeta::GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts) {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker());
    if (this->table_entry_list().empty()) {
        UniquePtr<String> err_msg = MakeUnique<String>("Empty table entry list.");
        LOG_ERROR(*err_msg);
        return {nullptr, Status::EmptyEntryList()};
    }

    for (const auto &table_entry : this->table_entry_list()) {
        if (table_entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid table entry. dummy entry");
            LOG_ERROR(*err_msg);
            return {nullptr, Status::InvalidEntry()};
        }

        u64 table_entry_commit_ts = table_entry->commit_ts_;
        if (table_entry_commit_ts < UNCOMMIT_TS) {
            // committed
            if (begin_ts > table_entry_commit_ts) {
                if (table_entry->deleted_) {
                    UniquePtr<String> err_msg = MakeUnique<String>("Table was dropped.");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status::TableNotExist(this->table_name())};
                } else {
                    return {static_cast<TableEntry *>(table_entry.get()), Status::OK()};
                }
            }
        } else {
            // not committed
            if (txn_id == table_entry->txn_id_) {
                // same txn
                return {static_cast<TableEntry *>(table_entry.get()), Status::OK()};
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No table entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status::NotFoundEntry()};
}

Tuple<TableEntry *, Status> TableMeta::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {

    if (!this->table_entry_list().empty()) {
        const auto &entry = table_entry_list().front();
        if (entry->entry_type_ == EntryType::kDummy) {
            UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
            LOG_ERROR(*err_msg);
            return {nullptr, Status::InvalidEntry()};
        }

        TransactionID entry_txn_id = entry->txn_id_.load();
        // committed
        if (begin_ts > entry->commit_ts_) {
            if (entry->deleted_) {
                UniquePtr<String> err_msg = MakeUnique<String>("No valid entry");
                LOG_ERROR(*err_msg);
                return {nullptr, Status::InvalidEntry()};
            } else {
                auto table_entry = static_cast<TableEntry *>(entry.get());
                return {table_entry, Status::OK()};
            }
        } else {
            if (txn_id == entry_txn_id) {
                auto table_entry = static_cast<TableEntry *>(entry.get());
                return {table_entry, Status::OK()};
            }
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No table entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status::NotFoundEntry()};
}

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
