//
// Created by jinhai on 23-6-23.
//

module;

#include <algorithm>

import base_entry;
import stl;
import table_collection_entry;
import table_collection_type;
import parser;
import logger;
import default_values;
import third_party;
import txn_state;
import txn_manager;
import buffer_manager;
import db_entry;
import third_party;

import infinity_exception;

module table_collection_meta;

namespace infinity {

/**
 * @brief Create a new table entry.
 *        LIST: [👇(insert a new).... table_entry2 , table_entry1 , dummy_entry] insert new table entry from front.
 * @param table_meta
 * @param table_collection_type
 * @param table_collection_name_ptr
 * @param columns
 * @param txn_id
 * @param begin_ts
 * @param txn_mgr
 * @return EntryResult
 */
EntryResult TableCollectionMeta::CreateNewEntry(TableCollectionMeta *table_meta,
                                                TableCollectionType table_collection_type,
                                                const SharedPtr<String> &table_collection_name_ptr,
                                                const Vector<SharedPtr<ColumnDef>> &columns,
                                                u64 txn_id,
                                                TxnTimeStamp begin_ts,
                                                TxnManager *txn_mgr) {
    TableCollectionEntry *res = nullptr;
    UniqueLock<RWMutex> rw_locker(table_meta->rw_locker_);
    const String &table_collection_name = *table_collection_name_ptr;

    if (table_meta->entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        table_meta->entry_list_.emplace_back(Move(dummy_entry));

        // Insert the new table entry
        UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                       table_collection_name_ptr,
                                                                                       columns,
                                                                                       table_collection_type,
                                                                                       table_meta,
                                                                                       txn_id,
                                                                                       begin_ts);
        res = table_entry.get();
        table_meta->entry_list_.emplace_front(Move(table_entry));

        LOG_TRACE(Format("New table entry is added: {}.", table_collection_name));
        return {res, nullptr};
    } else {
        // Already have a table entry, check if the table entry is valid here.
        BaseEntry *header_base_entry = table_meta->entry_list_.front().get();
        if (header_base_entry->entry_type_ == EntryType::kDummy) {
            // Dummy entry in the header
            UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                           table_collection_name_ptr,
                                                                                           columns,
                                                                                           table_collection_type,
                                                                                           table_meta,
                                                                                           txn_id,
                                                                                           begin_ts);
            res = table_entry.get();
            table_meta->entry_list_.emplace_front(Move(table_entry));
            return {res, nullptr};
        }

        TableCollectionEntry *header_table_entry = (TableCollectionEntry *)header_base_entry;
        if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if (begin_ts > header_table_entry->commit_ts_) {
                if (header_table_entry->deleted_) {
                    // No conflict
                    UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                                   table_collection_name_ptr,
                                                                                                   columns,
                                                                                                   table_collection_type,
                                                                                                   table_meta,
                                                                                                   txn_id,
                                                                                                   begin_ts);
                    res = table_entry.get();
                    table_meta->entry_list_.emplace_front(Move(table_entry));
                    return {res, nullptr};
                } else {
                    // Duplicated table
                    LOG_TRACE(Format("Duplicated table: {}.", table_collection_name));
                    return {nullptr, MakeUnique<String>("Duplicated table.")};
                }
            } else {
                // Write-Write conflict
                LOG_TRACE(
                    Format("Write-write conflict: There is a committed table: {} which is later than current transaction.", table_collection_name));
                return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
            }
        } else {

            TxnState head_table_entry_state = txn_mgr->GetTxnState(header_table_entry->txn_id_);

            switch (head_table_entry_state) {
                case TxnState::kStarted: {
                    // Started
                    if (header_table_entry->txn_id_ == txn_id) {
                        // Same txn
                        if (header_table_entry->deleted_) {
                            UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                                           table_collection_name_ptr,
                                                                                                           columns,
                                                                                                           table_collection_type,
                                                                                                           table_meta,
                                                                                                           txn_id,
                                                                                                           begin_ts);
                            res = table_entry.get();
                            table_meta->entry_list_.emplace_front(Move(table_entry));
                            return {res, nullptr};
                        } else {
                            LOG_TRACE(Format("Create a duplicated table {}.", table_collection_name));
                            return {nullptr, MakeUnique<String>("Create a duplicated name table.")};
                        }
                    } else {
                        LOG_TRACE(Format("Write-write conflict: There is a uncommitted transaction."));
                        return {nullptr, MakeUnique<String>("Write-write conflict: There is a uncommitted transaction.")};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    LOG_TRACE(Format("Write-write conflict: There is a committing/committed table which is later than current transaction."));
                    return {
                        nullptr,
                        MakeUnique<String>("Write-write conflict: There is a committing/committed table which is later than current transaction.")};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    table_meta->entry_list_.erase(table_meta->entry_list_.begin());

                    // Append new one
                    UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                                   table_collection_name_ptr,
                                                                                                   columns,
                                                                                                   table_collection_type,
                                                                                                   table_meta,
                                                                                                   txn_id,
                                                                                                   begin_ts);
                    res = table_entry.get();
                    table_meta->entry_list_.emplace_front(Move(table_entry));
                    return {res, nullptr};
                }
                default: {
                    LOG_TRACE("Invalid table entry txn state");
                    return {nullptr, MakeUnique<String>("Invalid table entry txn state.")};
                }
            }
        }
    }
}

EntryResult TableCollectionMeta::DropNewEntry(TableCollectionMeta *table_meta,
                                              u64 txn_id,
                                              TxnTimeStamp begin_ts,
                                              TxnManager *txn_mgr,
                                              const String &table_name,
                                              ConflictType conflict_type) {
    TableCollectionEntry *res = nullptr;
    UniqueLock<RWMutex> rw_locker(table_meta->rw_locker_);
    if (table_meta->entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.");
        return {nullptr, MakeUnique<String>("Empty table entry list.")};
    }

    BaseEntry *header_base_entry = table_meta->entry_list_.front().get();
    if (header_base_entry->entry_type_ == EntryType::kDummy) {
        //            rw_locker_.unlock();
        LOG_TRACE("No valid table entry.");
        return {nullptr, MakeUnique<String>("No valid table entry.")};
    }

    auto *header_table_entry = (TableCollectionEntry *)header_base_entry;
    if (header_table_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if (begin_ts > header_table_entry->commit_ts_) {
            // No conflict
            if (header_table_entry->deleted_) {
                if (conflict_type == ConflictType::kIgnore) {
                    LOG_TRACE(Format("Ignore drop a not existed table entry {}", table_name));
                    return {nullptr, nullptr};
                }
                LOG_TRACE("Table was dropped before.");
                return {nullptr, MakeUnique<String>("Table was dropped before.")};
            }

            Vector<SharedPtr<ColumnDef>> dummy_columns;
            UniquePtr<TableCollectionEntry> table_entry = MakeUnique<TableCollectionEntry>(table_meta->db_entry_dir_,
                                                                                           table_meta->table_collection_name_,
                                                                                           dummy_columns,
                                                                                           TableCollectionType::kTableEntry,
                                                                                           table_meta,
                                                                                           txn_id,
                                                                                           begin_ts);
            res = table_entry.get();
            res->deleted_ = true;
            table_meta->entry_list_.emplace_front(Move(table_entry));

            return {res, nullptr};
        } else {
            // Write-Write conflict
            LOG_TRACE(Format("Write-write conflict: There is a committed database which is later than current transaction."));
            return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
        }
    } else {
        // Uncommitted, check if the same txn
        if (txn_id == header_table_entry->txn_id_) {
            // Same txn, remove the header table entry
            res = header_table_entry;
            table_meta->entry_list_.erase(table_meta->entry_list_.begin());

            return {res, nullptr};
        } else {
            // Not same txn, issue WW conflict
            LOG_TRACE(Format("Write-write conflict: There is another uncommitted table entry."));
            return {nullptr, MakeUnique<String>("Write-write conflict: There is another uncommitted table entry.")};
        }
    }
}

void TableCollectionMeta::DeleteNewEntry(TableCollectionMeta *table_meta, u64 txn_id, TxnManager *txn_mgr) {
    UniqueLock<RWMutex> rw_locker(table_meta->rw_locker_);
    if (table_meta->entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.");
        return;
    }

    auto removed_iter = std::remove_if(table_meta->entry_list_.begin(), table_meta->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) -> bool {
        return entry->txn_id_ == txn_id;
    });

    table_meta->entry_list_.erase(removed_iter, table_meta->entry_list_.end());
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
 * @return EntryResult
 */
EntryResult TableCollectionMeta::GetEntry(TableCollectionMeta *table_meta, u64 txn_id, TxnTimeStamp begin_ts) {
    SharedLock<RWMutex> r_locker(table_meta->rw_locker_);
    if (table_meta->entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.");
        return {nullptr, MakeUnique<String>("Empty table entry list.")};
    }

    for (const auto &table_entry : table_meta->entry_list_) {
        if (table_entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("No valid table entry. dummy entry");
            return {nullptr, MakeUnique<String>("No valid table entry.")};
        }

        if (table_entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if (begin_ts > table_entry->commit_ts_) {
                if (table_entry->deleted_) {
                    // LOG_TRACE("Table was dropped.")
                    return {nullptr, MakeUnique<String>("Table was dropped.")};
                } else {
                    return {table_entry.get(), nullptr};
                }
            }
        } else {
            // not committed
            if (txn_id == table_entry->txn_id_) {
                // same txn
                return {table_entry.get(), nullptr};
            }
        }
    }
    LOG_TRACE("No table entry found.");
    return {nullptr, MakeUnique<String>("No table entry found.")};
}

SharedPtr<String> TableCollectionMeta::ToString(TableCollectionMeta *table_meta) {
    SharedLock<RWMutex> r_locker(table_meta->rw_locker_);
    SharedPtr<String> res = MakeShared<String>(Format("TableMeta, db_entry_dir: {}, table name: {}, entry count: ",
                                                      *table_meta->db_entry_dir_,
                                                      *table_meta->table_collection_name_,
                                                      table_meta->entry_list_.size()));
    return res;
}

Json TableCollectionMeta::Serialize(TableCollectionMeta *table_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    Json json_res;
    Vector<TableCollectionEntry *> table_entries;
    {
        SharedLock<RWMutex> lck(table_meta->rw_locker_);
        json_res["db_entry_dir"] = *table_meta->db_entry_dir_;
        json_res["table_name"] = *table_meta->table_collection_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        for (auto &table_entry : table_meta->entry_list_) {
            if (table_entry->entry_type_ == EntryType::kTable && table_entry->Committed() && table_entry->commit_ts_ <= max_commit_ts)
                table_entries.push_back((TableCollectionEntry *)table_entry.get());
        }
    }
    for (TableCollectionEntry *table_entry : table_entries) {
        json_res["entries"].emplace_back(TableCollectionEntry::Serialize(table_entry, max_commit_ts, is_full_checkpoint));
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
 * @return UniquePtr<TableCollectionMeta>
 */
UniquePtr<TableCollectionMeta> TableCollectionMeta::Deserialize(const Json &table_meta_json, DBEntry *db_entry, BufferManager *buffer_mgr) {
    SharedPtr<String> db_entry_dir = MakeShared<String>(table_meta_json["db_entry_dir"]);
    SharedPtr<String> table_name = MakeShared<String>(table_meta_json["table_name"]);
    LOG_TRACE(Format("load table {}", *table_name));
    UniquePtr<TableCollectionMeta> res = MakeUnique<TableCollectionMeta>(db_entry_dir, table_name, db_entry);
    if (table_meta_json.contains("entries")) {
        for (const auto &table_entry_json : table_meta_json["entries"]) {
            UniquePtr<TableCollectionEntry> table_entry = TableCollectionEntry::Deserialize(table_entry_json, res.get(), buffer_mgr);
            res->entry_list_.emplace_back(Move(table_entry));
        }
    }
    res->entry_list_.sort([](const UniquePtr<BaseEntry> &ent1, const UniquePtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
    dummy_entry->deleted_ = true;
    res->entry_list_.emplace_back(Move(dummy_entry));

    return res;
}

void TableCollectionMeta::MergeFrom(TableCollectionMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    Assert<StorageException>(IsEqual(*this->table_collection_name_, *other.table_collection_name_),
                             "DBEntry::MergeFrom requires table_collection_name_ match");
    Assert<StorageException>(IsEqual(*this->db_entry_dir_, *other.db_entry_dir_), "DBEntry::MergeFrom requires db_entry_dir_ match");
    MergeLists(this->entry_list_, other.entry_list_);
}

} // namespace infinity
