//
// Created by jinhai on 23-6-23.
//

#include "table_meta.h"
#include "storage/meta/entry/table_entry.h"
#include "common/utility/defer_op.h"

namespace infinity {

EntryResult
TableMeta::CreateNewEntry(u64 txn_id,
                          TxnTimeStamp begin_ts,
                          TxnContext *txn_context,
                          UniquePtr<TableDesc> table_desc,
                          void* root_db) {
    TableEntry* res = nullptr;
    std::unique_lock<RWMutex> rw_locker(rw_locker_);
    const String& table_name = table_desc->table_name_;

    if(entry_list_.empty()) {
        // Insert a dummy entry.
        UniquePtr<BaseEntry> dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy, nullptr);
        dummy_entry->deleted_ = true;
        entry_list_.emplace_back(std::move(dummy_entry));

        // Insert the new table entry
        UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(std::move(table_desc),
                                                                   root_db,
                                                                   txn_id,
                                                                   begin_ts,
                                                                   txn_context);
        res = table_entry.get();
        entry_list_.emplace_front(std::move(table_entry));

        LOG_TRACE("New table entry is added: {}.", table_name);
        return {res, nullptr};
    } else {
        // Already have a table entry, check if the table entry is valid here.
        BaseEntry* header_base_entry = entry_list_.front().get();
        if(header_base_entry->entry_type_ == EntryType::kDummy) {
            // Dummy entry in the header
            UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(std::move(table_desc),
                                                                       root_db,
                                                                       txn_id,
                                                                       begin_ts,
                                                                       txn_context);
            res = table_entry.get();
            entry_list_.emplace_front(std::move(table_entry));
            return {res, nullptr};
        }

        TableEntry* header_table_entry = (TableEntry*)header_base_entry;
        if(header_table_entry->commit_ts_ < UNCOMMIT_TS) {
            // Committed
            if(begin_ts > header_table_entry->commit_ts_) {
                if(header_table_entry->deleted_) {
                    // No conflict
                    UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(std::move(table_desc),
                                                                               root_db,
                                                                               txn_id,
                                                                               begin_ts,
                                                                               txn_context);
                    res = table_entry.get();
                    entry_list_.emplace_front(std::move(table_entry));
                    return {res, nullptr};
                } else {
                    // Duplicated table
                    LOG_TRACE("Duplicated table name {}.", table_name)
                    return {nullptr, MakeUnique<String>("Duplicated table.")};
                }
            } else {
                // Write-Write conflict
                LOG_TRACE("Write-write conflict: There is a committed table: {} which is later than current transaction.",
                          table_name)
                return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
            }
        } else {
            header_table_entry->txn_context_->Lock();
            DeferFn defer_fn([&]() {
                header_table_entry->txn_context_->UnLock();
            });

            switch(header_table_entry->txn_context_->state_) {
                case TxnState::kStarted: {
                    // Started
                    if(header_table_entry->txn_id_ < txn_id) {
                        // header table entry txn is earlier than current txn
                        LOG_TRACE("Write-write conflict: There is a uncommitted database which is older than current transaction.")
                        return {nullptr, MakeUnique<String>("Write-write conflict: There is a uncommitted database which is older than current transaction.")};
                    } else if (header_table_entry->txn_id_ > txn_id){
                        // Current txn is older

                        // Rollback header table entry txn
                        header_table_entry->txn_context_->state_ = TxnState::kRollbacking;

                        // Erase header table entry
                        entry_list_.erase(entry_list_.begin());

                        // Append new one
                        UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(std::move(table_desc),
                                                                                   root_db,
                                                                                   txn_id,
                                                                                   begin_ts,
                                                                                   txn_context);
                        res = table_entry.get();
                        entry_list_.emplace_front(std::move(table_entry));
                        return {res, nullptr};
                    } else {
                        // Same txn
                        LOG_TRACE("Create a duplicated table {}.", table_name)
                        return {nullptr, MakeUnique<String>("Create a duplicated name table.")};
                    }
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    // Committing / Committed, report WW conflict and rollback current txn
                    LOG_TRACE("Write-write conflict: There is a committing/committed table which is later than current transaction.")
                    return {nullptr, MakeUnique<String>("Write-write conflict: There is a committing/committed table which is later than current transaction.")};
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    // Remove the header entry
                    entry_list_.erase(entry_list_.begin());

                    // Append new one
                    UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(std::move(table_desc),
                                                                               root_db,
                                                                               txn_id,
                                                                               begin_ts,
                                                                               txn_context);
                    res = table_entry.get();
                    entry_list_.emplace_front(std::move(table_entry));
                    return {res, nullptr};
                }
                default: {
                    LOG_TRACE("Invalid table entry txn state")
                    return {nullptr, MakeUnique<String>("Invalid table entry txn state.")};
                }
            }
        }
    }
}

EntryResult
TableMeta::DropNewEntry(u64 txn_id,
                        TxnTimeStamp begin_ts,
                        TxnContext *txn_context,
                        const String& table_name,
                        void* root_db) {

    TableEntry* res = nullptr;
    std::unique_lock<RWMutex> rw_locker(rw_locker_);
    if(entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.")
        return {nullptr, MakeUnique<String>("Empty table entry list.")};
    }

    BaseEntry* header_base_entry = entry_list_.front().get();
    if(header_base_entry->entry_type_ == EntryType::kDummy) {
//            rw_locker_.unlock();
        LOG_TRACE("No valid table entry.")
        return {nullptr, MakeUnique<String>("No valid table entry.")};
    }

    TableEntry* header_table_entry = (TableEntry*)header_base_entry;
    if(header_table_entry->commit_ts_ < UNCOMMIT_TS) {
        // Committed
        if(begin_ts > header_table_entry->commit_ts_) {
            // No conflict
            if(header_table_entry->deleted_) {
                LOG_TRACE("Table is dropped before.")
                return {nullptr, MakeUnique<String>("Table is dropped before.")};
            }

            UniquePtr<TableEntry> table_entry = MakeUnique<TableEntry>(nullptr,
                                                                       root_db,
                                                                       txn_id,
                                                                       begin_ts,
                                                                       txn_context);
            res = table_entry.get();
            res->deleted_ = true;
            entry_list_.emplace_front(std::move(table_entry));

            return {res, nullptr};
        } else {
            // Write-Write conflict
            LOG_TRACE("Write-write conflict: There is a committed database which is later than current transaction.");
            return {nullptr, MakeUnique<String>("Write-write conflict: There is a committed database which is later than current transaction.")};
        }
    } else {
        // Uncommitted, check if the same txn
        if(txn_id == header_table_entry->txn_id_) {
            // Same txn, remove the header table entry
            res = header_table_entry;
            entry_list_.erase(entry_list_.begin());

            return {res, nullptr};
        } else {
            // Not same txn, issue WW conflict
            LOG_TRACE("Write-write conflict: There is another uncommitted table entry.")
            return {nullptr, MakeUnique<String>("Write-write conflict: There is another uncommitted table entry.")};
        }
    }
}

void
TableMeta::DeleteNewEntry(u64 txn_id, TxnContext *txn_context) {
    std::unique_lock<RWMutex> rw_locker(rw_locker_);
    if(entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.")
        return ;
    }

    auto removed_iter = std::remove_if(entry_list_.begin(), entry_list_.end(), [&](UniquePtr<BaseEntry>& entry)->bool {
        return entry->txn_id_ == txn_id;
    });

    entry_list_.erase(removed_iter, entry_list_.end());
}

EntryResult TableMeta::GetEntry(u64 txn_id, TxnTimeStamp begin_ts) {
    std::shared_lock<RWMutex> r_locker(rw_locker_);
    if(entry_list_.empty()) {
        LOG_TRACE("Empty table entry list.")
        return {nullptr, MakeUnique<String>("Empty table entry list.")};
    }


    for(const auto& db_entry: entry_list_) {
        if(db_entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("No valid table entry.")
            return {nullptr, MakeUnique<String>("No valid table entry.")};
        }

        if(db_entry->commit_ts_ < UNCOMMIT_TS) {
            // committed
            if(begin_ts > db_entry->commit_ts_) {
                if(db_entry->deleted_) {
                    LOG_TRACE("Table is dropped.")
                    return {nullptr, MakeUnique<String>("Table is dropped.")};
                } else {
                    return {db_entry.get(), nullptr};
                }
            }
        } else {
            // not committed
            if(txn_id == db_entry->txn_id_) {
                // same txn
                return {db_entry.get(), nullptr};
            }
        }
    }
    LOG_TRACE("No table entry found.")
    return {nullptr, MakeUnique<String>("No table entry found.")};
}

}


