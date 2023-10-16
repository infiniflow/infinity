#include "index_def_meta.h"
#include "common/default_values.h"
#include "common/types/alias/concurrency.h"
#include "common/utility/infinity_assert.h"
#include "json.hpp"
#include "main/logger.h"
#include "parser/statement/extra/extra_ddl_info.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/meta/entry/index_def_entry.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "storage/txn/txn_manager.h"
#include "storage/txn/txn_state.h"

namespace infinity {
EntryResult IndexDefMeta::CreateNewEntry(IndexDefMeta *index_def_meta,
                                         SharedPtr<IndexDef> index_def,
                                         ConflictType conflict_type,
                                         u64 txn_id,
                                         TxnTimeStamp begin_ts,
                                         TxnManager *txn_mgr) {
    auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, txn_id, begin_ts);
    IndexDefEntry *res = index_def_entry.get();

    // write lock guard
    std::unique_lock<RWMutex> lock(index_def_meta->rw_locker_);
    if (index_def_meta->entry_list_.empty()) {
        // Insert a dummy entry
        auto dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        index_def_meta->entry_list_.emplace_front(std::move(dummy_entry));
    }

    BaseEntry *header_base_entry = index_def_meta->entry_list_.front().get();
    // auto header_index_def_entry = static_cast<IndexDefEntry *>(header_base_entry);
    if (header_base_entry->commit_ts_ < UNCOMMIT_TS) {
        // Header has committed or is dummy.
        if (begin_ts > header_base_entry->commit_ts_) {
            if (header_base_entry->deleted_) {
                // No conflict. The dummy entry commit_ts is 0 and deleted so can always reach here.
                index_def_meta->entry_list_.emplace_front(std::move(index_def_entry));
                return {.entry_ = res, .err_ = nullptr};
            } else {
                LOG_TRACE("Duplicated index: {}.", *index_def->index_name_);
                switch (conflict_type) {
                    case ConflictType::kIgnore: {
                        return {.entry_ = nullptr, .err_ = nullptr};
                    }
                    case ConflictType::kError: {
                        return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
                    }
                    default: {
                        StorageError("Invalid conflict type");
                    }
                }
                StorageError("Cannot reach here");
            }
        } else {
            // Write-Write conflict
            LOG_TRACE("Write-Write conflict: There is a committed index: which is later than current transaction.");
            return {.entry_ = nullptr,
                    .err_ = MakeUnique<String>("Write-Write conflict. There is a committed index which is later than current transaction.")};
        }
    } else {
        StorageAssert(header_base_entry->entry_type_ != EntryType::kDummy, "Dummy entry should always be committed.");
        // Uncommitted
        TxnState header_index_def_entry_state = txn_mgr->GetTxnState(header_base_entry->txn_id_);

        switch (header_index_def_entry_state) {
            case TxnState::kStarted: {
                if (header_base_entry->txn_id_ == txn_id) {
                    // Same txn
                    if (header_base_entry->deleted_) {
                        index_def_meta->entry_list_.emplace_front(std::move(index_def_entry));
                        return {.entry_ = res, .err_ = nullptr};
                    } else {
                        LOG_TRACE("Duplicated index: {}.", *index_def->index_name_);
                        switch (conflict_type) {
                            case ConflictType::kIgnore: {
                                return {.entry_ = nullptr, .err_ = nullptr};
                            }
                            case ConflictType::kError: {
                                return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
                            }
                            default: {
                                StorageError("Invalid conflict type");
                            }
                        }
                        StorageError("Cannot reach here");
                    }
                } else {
                    LOG_TRACE("Write-Write conflict: There is an uncommitted index.");
                    return {.entry_ = nullptr, .err_ = MakeUnique<String>("Write-Write conflict. There is an committing index.")};
                }
            }
            case TxnState::kCommitting: {
                LOG_TRACE("Write-Write conflict: There is an uncommitted index.");
            }
            case TxnState::kRollbacking: {
                // Remove the header entry
                index_def_meta->entry_list_.pop_front();
                index_def_meta->entry_list_.emplace_front(std::move(index_def_entry));
                return {.entry_ = res, .err_ = nullptr};
            }
            default:
                LOG_TRACE("Invalid index entry txn state.");
                return {.entry_ = nullptr, .err_ = MakeUnique<String>("Invalid index entry txn state.")};
        }
    }
}

EntryResult IndexDefMeta::DropNewEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    NotImplementError("Not implemented");
}

void IndexDefMeta::DeleteNewEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnManager *txn_mgr) {
    std::unique_lock<RWMutex> rw_locker(index_def_meta->rw_locker_);
    if (index_def_meta->entry_list_.empty()) {
        LOG_TRACE("Attempt to delete not existed index entry.");
        return;
    }

    // `std::remove_if` if move all elements that satisfy the predicate to the first part of the container and return the iterator to the end of first
    // part
    auto removed_iter = std::remove_if(index_def_meta->entry_list_.begin(), index_def_meta->entry_list_.end(), [&](UniquePtr<BaseEntry> &entry) {
        return entry->txn_id_ == txn_id;
    });
    // erase the all "moved" elements of the second part
    index_def_meta->entry_list_.erase(removed_iter, index_def_meta->entry_list_.end());
}

EntryResult IndexDefMeta::GetEntry(IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts) { NotImplementError("Not implemented"); }

SharedPtr<String> IndexDefMeta::ToString(IndexDefMeta *index_def_meta) { NotImplementError("Not implemented"); }

nlohmann::json IndexDefMeta::Serialize(const IndexDefMeta *index_def_meta) {
    nlohmann::json json;

    for (const auto &entry : index_def_meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kIndexDef) {
            json["entries"].emplace_back(IndexDefEntry::Serialize(static_cast<IndexDefEntry *>(entry.get())));
        } else if (entry->entry_type_ == EntryType::kDummy) {
            LOG_TRACE("Skip dummy entry during serialize index {} meta", *index_def_meta->index_name_);
        } else {
            StorageError("Unexpected entry type.");
        }
    }
    return json;
}

UniquePtr<IndexDefMeta> IndexDefMeta::Deserialize(const nlohmann::json &index_def_meta_json, TableCollectionEntry *table_collection_entry) {
    NotImplementError("Not implemented");
    auto table_entry_dir = MakeShared<String>(index_def_meta_json["table_entry_dir"]);
    auto index_name = MakeShared<String>(index_def_meta_json["index_name"]);
    LOG_TRACE("load index {}", *index_name);

    auto res = MakeUnique<IndexDefMeta>(std::move(index_name), table_collection_entry);
    if (index_def_meta_json.contains("entries")) {
        for (const auto &entry_json : index_def_meta_json["entries"]) {
            auto entry = IndexDefEntry::Deserialize(entry_json, res.get());
            res->entry_list_.emplace_back(std::move(entry));
        }
    }
    return res;
}
} // namespace infinity
