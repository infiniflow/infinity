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

export module entry_list;

import stl;
import base_entry;
import meta_entry_interface;
import infinity_exception;
import cleanup_scanner;
import status;
import txn;
import extra_ddl_info;
import third_party;
import logger;
import txn_manager;
import txn_state;

namespace infinity {

export template <EntryConcept Entry>
class EntryList {
    enum class FindResult : u8 {
        kFound,
        kNotFound,
        kConflict,
    };

public:
    Tuple<Entry *, Status>
    AddEntry(SharedPtr<Entry> entry, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type);

    Tuple<Entry *, Status>
    DropEntry(SharedPtr<Entry> drop_entry, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type);

    Tuple<Entry *, Status> GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts);

    void DeleteEntry(TransactionID txn_id);

    bool PickCleanup(CleanupScanner *scanner);

    void Cleanup() &&;

    void MergeWith(EntryList<Entry> &other);

private:
    FindResult FindEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

public: // TODO: make both private
    std::shared_mutex rw_locker_{};

    List<SharedPtr<Entry>> entry_list_;
};

template <EntryConcept Entry>
EntryList<Entry>::FindResult EntryList<Entry>::FindEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    FindResult find_res = FindResult::kNotFound;
    bool continue_loop = true;
    std::unique_lock lock(rw_locker_);
    while (!entry_list_.empty() && continue_loop) {
        continue_loop = false;
        Entry *entry = entry_list_.front().get();
        if (entry->Committed()) {
            if (begin_ts > entry->commit_ts_) {
                if (entry->Deleted()) {
                    find_res = FindResult::kNotFound;
                } else {
                    find_res = FindResult::kFound;
                }
            } else {
                find_res = FindResult::kConflict;
            }
        } else {
            if (txn_mgr == nullptr) {
                UnrecoverableError("Txn manager is null");
            }
            TxnState txn_state = txn_mgr->GetTxnState(entry->txn_id_);
            switch (txn_state) {
                case TxnState::kStarted: {
                    if (entry->txn_id_ == txn_id) {
                        if (entry->Deleted()) {
                            find_res = FindResult::kNotFound;
                        } else {
                            find_res = FindResult::kFound;
                        }
                    } else {
                        find_res = FindResult::kConflict;
                    }
                    break;
                }
                case TxnState::kCommitting:
                case TxnState::kCommitted: {
                    find_res = FindResult::kConflict;
                    break;
                }
                case TxnState::kRollbacking:
                case TxnState::kRollbacked: {
                    entry_list_.pop_front();
                    continue_loop = true;
                    break;
                }
                default: {
                    UnrecoverableError("Invalid entry txn state");
                }
            }
        }
    }
    return find_res;
}

template <EntryConcept Entry>
Tuple<Entry *, Status>
EntryList<Entry>::AddEntry(SharedPtr<Entry> entry, TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    FindResult find_res = FindEntry(txn_id, begin_ts, txn_mgr);
    switch (find_res) {
        case FindResult::kNotFound: {
            entry_list_.push_front(entry);
            if (txn_mgr != nullptr) {
                auto op = MakeUnique<typename Entry::EntryOp>(entry);
                Txn *txn = txn_mgr->GetTxn(txn_id);
                txn->AddCatalogDeltaOperation(std::move(op));
            }
            return {entry.get(), Status::OK()};
        }
        case FindResult::kFound: {
            switch (conflict_type) {
                case ConflictType::kIgnore: {
                    LOG_TRACE(fmt::format("Ignore Add an existed entry."));
                    return {nullptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Duplicated entry");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status(ErrorCode::kDuplicateDatabaseName, std::move(err_msg))};
                }
            }
        }
        case FindResult::kConflict: {
            UniquePtr<String> err_msg = MakeUnique<String>(
                fmt::format("Write-write conflict: There is a committing/committed entry which is later than current transaction."));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
        default: {
            UnrecoverableError("Invalid find result");
        }
    }
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::DropEntry(SharedPtr<Entry> drop_entry,
                                                   TransactionID txn_id,
                                                   TxnTimeStamp begin_ts,
                                                   TxnManager *txn_mgr,
                                                   ConflictType conflict_type) {
    FindResult find_res = FindEntry(txn_id, begin_ts, txn_mgr);
    switch (find_res) {
        case FindResult::kNotFound: {
            switch (conflict_type) {
                case ConflictType::kIgnore: {
                    LOG_TRACE(fmt::format("Ignore drop a not existed entry."));
                    return {nullptr, Status::OK()};
                }
                default: {
                    UniquePtr<String> err_msg = MakeUnique<String>("Not exisited entry.");
                    LOG_ERROR(*err_msg);
                    return {nullptr, Status::DBNotExist("")};
                }
            }
        }
        case FindResult::kFound: {
            entry_list_.push_front(drop_entry);
            if (txn_mgr != nullptr) {
                auto op = MakeUnique<typename Entry::EntryOp>(drop_entry);
                Txn *txn = txn_mgr->GetTxn(txn_id);
                txn->AddCatalogDeltaOperation(std::move(op));
            }
            return {drop_entry.get(), Status::OK()};
        }
        case FindResult::kConflict: {
            UniquePtr<String> err_msg = MakeUnique<String>(
                fmt::format("Write-write conflict: There is a committing/committed entry which is later than current transaction."));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
        default: {
            UnrecoverableError("Invalid find result");
        }
    }
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts) {
    FindResult find_res = FindResult::kNotFound;
    Entry *entry_ptr = nullptr;
    {
        std::shared_lock lock(rw_locker_);
        for (const auto &entry : entry_list_) {
            if (entry->Committed()) {
                if (entry->commit_ts_ < begin_ts) {
                    if (!entry->Deleted()) {
                        entry_ptr = entry.get();
                        find_res = FindResult::kFound;
                    } else {
                        find_res = FindResult::kNotFound;
                    }
                    break;
                }
            } else if (txn_id == entry->txn_id_) {
                if (!entry->Deleted()) {
                    entry_ptr = entry.get();
                    find_res = FindResult::kFound;
                } else {
                    find_res = FindResult::kNotFound;
                }
                break;
            }
        }
    }
    switch (find_res) {
        case FindResult::kNotFound: {
            return {nullptr, Status::DBNotExist("")};
        }
        case FindResult::kFound: {
            return {entry_ptr, Status::OK()};
        }
        default: {
            UnrecoverableError("Invalid find result");
        }
    }
}

template <EntryConcept Entry>
void EntryList<Entry>::DeleteEntry(TransactionID txn_id) {
    std::unique_lock lock(rw_locker_);
    if (entry_list_.empty()) {
        LOG_TRACE("Empty entry list.");
        return;
    }
    auto removed_iter = std::remove_if(entry_list_.begin(), entry_list_.end(), [&](auto &entry) { return entry->txn_id_ == txn_id; });
    entry_list_.erase(removed_iter, entry_list_.end());
}

template <EntryConcept Entry>
bool EntryList<Entry>::PickCleanup(CleanupScanner *scanner) {
    std::unique_lock lock(rw_locker_);

    if (entry_list_.empty()) {
        return true;
    }

    TxnTimeStamp visible_ts = scanner->visible_ts();
    auto iter = entry_list_.begin();
    while (iter != entry_list_.end()) {
        SharedPtr<Entry> &entry = *iter;
        if (entry->commit_ts_ < visible_ts) {
            if (entry->deleted_) {
                scanner->AddEntry(std::move(entry));
                iter = entry_list_.erase(iter);
            } else {
                lock.unlock();
                entry->PickCleanup(scanner);
                lock.lock();
                ++iter;
            }
            break;
        }
        ++iter;
    }
    while (iter != entry_list_.end()) {
        SharedPtr<Entry> &entry = *iter;
        if (entry->Committed()) {
            scanner->AddEntry(std::move(entry));
            iter = entry_list_.erase(iter);
        } else {
            ++iter;
        }
    }
    return entry_list_.size() == 1;
}

template <EntryConcept Entry>
void EntryList<Entry>::Cleanup() && {
    if (entry_list_.empty()) {
        return;
    }
    for (auto iter = entry_list_.begin(); iter != Prev(entry_list_.end()); ++iter) {
        SharedPtr<Entry> &entry = *iter;
        std::move(*entry).Cleanup();
    }
}

template <EntryConcept Entry>
void EntryList<Entry>::MergeWith(EntryList<Entry> &other) {
    auto &other_list = other.entry_list_;
    auto it1 = entry_list_.begin();
    auto it2 = other_list.begin();

    while (it1 != entry_list_.end() && it2 != other_list.end()) {
        if (!(*it1)->Committed() || !(*it2)->Committed()) {
            UnrecoverableError("MergeLists requires entries be committed");
        }
        if ((*it1)->commit_ts_ > (*it2)->commit_ts_) {
            ++it1;
        } else if ((*it1)->commit_ts_ < (*it2)->commit_ts_) {
            entry_list_.insert(it1, std::move(*it2));
            ++it2;
        } else {
            (*it1)->MergeFrom(**it2);
            ++it1;
            ++it2;
        }
    }

    while (it2 != other_list.end()) {
        entry_list_.insert(it1, std::move(*it2));
        ++it2;
    }

    other_list.clear();
}

} // namespace infinity