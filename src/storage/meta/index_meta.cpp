#include "index_meta.h"
#include "common/default_values.h"
#include "common/types/alias/concurrency.h"
#include "common/utility/infinity_assert.h"
#include "storage/meta/entry/base_entry.h"
#include "storage/meta/entry/index_entry.h"
#include "storage/meta/entry/table_collection_entry.h"
#include "storage/txn/txn_manager.h"
#include "storage/txn/txn_state.h"

namespace infinity {
EntryResult IndexMeta::CreateNewEntry(IndexMeta *index_meta, SharedPtr<IndexDef> index_def, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    auto index_entry = MakeUnique<IndexEntry>(std::move(index_def), index_meta, txn_id, begin_ts);
    IndexEntry *res = index_entry.get();

    // write lock guard
    std::unique_lock<RWMutex> lock(index_meta->rw_locker_);
    if (index_meta->entry_list_.empty()) {
        // Insert a dummy entry
        auto dummy_entry = MakeUnique<BaseEntry>(EntryType::kDummy);
        dummy_entry->deleted_ = true;
        index_meta->entry_list_.emplace_back(std::move(dummy_entry));
    }

    BaseEntry *header_base_entry = index_meta->entry_list_.front().get();
    // auto header_index_entry = static_cast<IndexEntry *>(header_base_entry);
    if (header_base_entry->commit_ts_ < UNCOMMIT_TS) {
        // Header has committed or is dummy.
        if (begin_ts > header_base_entry->commit_ts_) {
            if (header_base_entry->deleted_) {
                // No conflict. The dummy entry commit_ts is 0 and deleted so can always reach here.
                index_meta->entry_list_.emplace_back(std::move(index_entry));
                return {.entry_ = res, .err_ = nullptr};
            } else {
                LOG_TRACE("Duplicated index: {}.", index_def->index_name_);
                return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
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
        TxnState header_index_entry_state = txn_mgr->GetTxnState(header_base_entry->txn_id_);

        switch (header_index_entry_state) {
            case TxnState::kStarted: {
                if (header_base_entry->txn_id_ == txn_id) {
                    // Same txn
                    if (header_base_entry->deleted_) {
                        index_meta->entry_list_.emplace_back(std::move(index_entry));
                        return {.entry_ = res, .err_ = nullptr};
                    } else {
                        LOG_TRACE("Duplicated index: {}.", index_def->index_name_);
                        return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
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
                index_meta->entry_list_.pop_front();
                index_meta->entry_list_.emplace_back(std::move(index_entry));
                return {.entry_ = res, .err_ = nullptr};
            }
            default:
                LOG_TRACE("Invalid index entry txn state.");
                return {.entry_ = nullptr, .err_ = MakeUnique<String>("Invalid index entry txn state.")};
        }
    }
}

EntryResult IndexMeta::DropNewEntry(IndexMeta *index_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    NotImplementError("Not implemented");
}

void IndexMeta::DeleteNewEntry(IndexMeta *index_meta, u64 txn_id, TxnManager *txn_mgr) { NotImplementError("Not implemented"); }

EntryResult IndexMeta::GetEntry(IndexMeta *index_meta, u64 txn_id, TxnTimeStamp begin_ts) { NotImplementError("Not implemented"); }

SharedPtr<String> IndexMeta::ToString(IndexMeta *index_meta) { NotImplementError("Not implemented"); }

nlohmann::json IndexMeta::Serialize(const IndexMeta *index_meta) { NotImplementError("Not implemented"); }

UniquePtr<IndexMeta> IndexMeta::Deserialize(const nlohmann::json &index_meta_json, TableCollectionEntry *table_collection_entry) {
    NotImplementError("Not implemented");
}
} // namespace infinity
