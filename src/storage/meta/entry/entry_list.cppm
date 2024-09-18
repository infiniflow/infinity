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

#include <type_traits>

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

class DBEntry;
class TableEntry;
class TableIndexEntry;

enum class FindResult : u8 {
    kFound,
    kNotFound,
    kUncommitted,
    kUncommittedDelete,
    kConflict,
};

export template <EntryConcept Entry>
class EntryList {
public:
    EntryList() = default;

    EntryList(const EntryList &&other) {
        std::unique_lock lock(rw_locker_);
        entry_list_ = std::move(other.entry_list_);
    }

    EntryList &operator=(EntryList &&other) {
        if (this != &other) {
            std::unique_lock lock(rw_locker_);
            entry_list_ = std::move(other.entry_list_);
        }
        return *this;
    }

    template<typename T>
    EntryList Clone(T *parent) const {
        EntryList new_entry_list;
        std::shared_lock r_lock(rw_locker_);
        for (const auto &entry : entry_list_) {
            new_entry_list.entry_list_.push_back(entry->Clone(parent));
        }
        return new_entry_list;
    }

    // op
    Tuple<Entry *, Status> AddEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                    std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func,
                                    TransactionID txn_id,
                                    TxnTimeStamp begin_ts,
                                    TxnManager *txn_mgr,
                                    ConflictType conflict_type,
                                    bool add_if_found = false);

    Tuple<SharedPtr<Entry>, Status> DropEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                              std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func,
                                              TransactionID txn_id,
                                              TxnTimeStamp begin_ts,
                                              TxnManager *txn_mgr,
                                              ConflictType conflict_type);

    Tuple<Entry *, Status> GetEntry(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<Entry *, Status> GetEntryNolock(TransactionID txn_id, TxnTimeStamp begin_ts);

    // Replay op
    Tuple<Entry *, Status>
    AddEntryReplay(std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func, TransactionID txn_id, TxnTimeStamp begin_ts);

    Status
    UpdateEntryReplay(std::function<void(SharedPtr<Entry>, TransactionID, TxnTimeStamp)> &&update_func, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<Entry *, Status>
    DropEntryReplay(std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func, TransactionID txn_id, TxnTimeStamp begin_ts);

    Tuple<Entry *, Status> GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);

    // other
    List<SharedPtr<Entry>> DeleteEntry(TransactionID txn_id);

    bool PickCleanup(CleanupScanner *scanner);

    void Cleanup(CleanupInfoTracer *info_tracer = nullptr, bool dropped = true);

    void Iterate(std::function<void(Entry *)> func, TxnTimeStamp visible_ts);

    inline bool Empty() const {
        std::shared_lock lock(rw_locker_);
        return entry_list_.empty();
    }

    inline SizeT size() const {
        std::shared_lock lock(rw_locker_);
        return entry_list_.size();
    }

    inline void PushFrontEntry(const SharedPtr<Entry>& entry) {
        std::unique_lock lock(rw_locker_);
        entry_list_.push_front(entry);
    }

    inline void PushBackEntry(const SharedPtr<Entry>& entry) {
        std::unique_lock lock(rw_locker_);
        entry_list_.push_back(entry);
    }

    Vector<BaseEntry *> GetCandidateEntry(TxnTimeStamp max_commit_ts, EntryType entry_type) {
        std::shared_lock lock(rw_locker_);
        Vector<BaseEntry*> result;
        result.reserve(entry_list_.size());
        for (const auto &base_entry : entry_list_) {
            if (base_entry->entry_type_ != entry_type) {
                String error_message = fmt::format("Unexpected entry type {}", ToString(entry_type));
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
            if (base_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                result.push_back(base_entry.get());
            }
        }
        return result;
    }

    void SortEntryListByTS() {
        // New->Old
        entry_list_.sort([](const SharedPtr<BaseEntry> &entry_left, const SharedPtr<BaseEntry> &entry_right) {
            return entry_left->commit_ts_ > entry_right->commit_ts_;
        });
    }

    List<SharedPtr<Entry>> GetAllEntries() const {
        return entry_list_;
    }
private:
    // helper
    FindResult FindEntryNoLock(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    FindResult FindEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts);

    Pair<Entry *, FindResult> GetEntryInner1NoLock(TransactionID txn_id, TxnTimeStamp begin_ts);

    Pair<Entry *, Status> GetEntryInner2NoLock(Entry *entry_ptr, FindResult find_res);

    mutable std::shared_mutex rw_locker_{};

    List<SharedPtr<Entry>> entry_list_;
};

template <EntryConcept Entry>
FindResult EntryList<Entry>::FindEntryNoLock(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    FindResult find_res = FindResult::kNotFound;
    bool continue_loop = true;
    for (auto iter = entry_list_.begin(); iter != entry_list_.end() && continue_loop; ++iter) {
        continue_loop = false;
        Entry *entry = iter->get();
        if (entry->Committed()) {
            if (begin_ts >= entry->commit_ts_) {
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
                String error_message = "Txn manager is null";
                UnrecoverableError(error_message);
            }
            TxnState txn_state = txn_mgr->GetTxnState(entry->txn_id_);
            switch (txn_state) {
                case TxnState::kStarted: {
                    if (entry->txn_id_ == txn_id) {
                        if (entry->Deleted()) {
                            find_res = FindResult::kUncommittedDelete;
                        } else {
                            find_res = FindResult::kUncommitted;
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
                    continue_loop = true;
                    break;
                }
                default: {
                    String error_message = "Invalid entry txn state";
                    UnrecoverableError(error_message);
                }
            }
        }
    }
    return find_res;
}

template <EntryConcept Entry>
FindResult EntryList<Entry>::FindEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
    FindResult find_res = FindResult::kNotFound;
    if (!entry_list_.empty()) {
        auto *entry = entry_list_.front().get();
        // FIXME
        if (true || begin_ts >= entry->commit_ts_ || txn_id == entry->txn_id_) {
            if (!entry->deleted_) {
                find_res = FindResult::kFound;
            }
        } else {
            find_res = FindResult::kConflict;
        }
    }
    return find_res;
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::AddEntry(std::shared_lock<std::shared_mutex> &&parent_r_lock,
                                                  std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func,
                                                  TransactionID txn_id,
                                                  TxnTimeStamp begin_ts,
                                                  TxnManager *txn_mgr,
                                                  ConflictType conflict_type,
                                                  bool add_if_found) {
    std::unique_lock lock(rw_locker_);
    parent_r_lock.unlock();
    FindResult find_res = this->FindEntryNoLock(txn_id, begin_ts, txn_mgr);
    if (add_if_found && find_res == FindResult::kFound) {
        find_res = FindResult::kNotFound;
    }
    switch (find_res) {
        case FindResult::kUncommittedDelete:
        case FindResult::kNotFound: {
            SharedPtr<Entry> entry = init_func(txn_id, begin_ts);
            auto *entry_ptr = entry.get();
            entry_list_.push_front(std::move(entry));
            return {entry_ptr, Status::OK()};
        }
        case FindResult::kUncommitted:
        case FindResult::kFound: {
            switch (conflict_type) {
                case ConflictType::kIgnore: {
                    LOG_TRACE(fmt::format("Ignore Add an existed entry."));
                    return {nullptr, Status::Ignore()};
                }
                default: {
                    if constexpr (std::is_same_v<Entry, DBEntry>) {
                        UniquePtr<String> err_msg = MakeUnique<String>("Duplicated db entry");
                        LOG_ERROR(*err_msg);
                        return {nullptr, Status(ErrorCode::kDuplicateDatabaseName, std::move(err_msg))};
                    } else if constexpr (std::is_same_v<Entry, TableEntry>) {
                        UniquePtr<String> err_msg = MakeUnique<String>("Duplicated table entry");
                        LOG_ERROR(*err_msg);
                        return {nullptr, Status(ErrorCode::kDuplicateTableName, std::move(err_msg))};
                    } else if constexpr (std::is_same_v<Entry, TableIndexEntry>) {
                        UniquePtr<String> err_msg = MakeUnique<String>("Duplicated table index entry");
                        LOG_ERROR(*err_msg);
                        return {nullptr, Status(ErrorCode::kDuplicateIndexName, std::move(err_msg))};
                    } else {
                        String error_message = "Unimplemented";
                        UnrecoverableError(error_message);
                    }
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
            String error_message = "Invalid find result";
            UnrecoverableError(error_message);
        }
    }
}

template <EntryConcept Entry>
Tuple<SharedPtr<Entry>, Status> EntryList<Entry>::DropEntry(std::shared_lock<std::shared_mutex> &&parent_r_lock,
                                                            std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func,
                                                            TransactionID txn_id,
                                                            TxnTimeStamp begin_ts,
                                                            TxnManager *txn_mgr,
                                                            ConflictType conflict_type) {
    std::unique_lock lock(rw_locker_);
    parent_r_lock.unlock();
    FindResult find_res = this->FindEntryNoLock(txn_id, begin_ts, txn_mgr);
    switch (find_res) {
        case FindResult::kUncommittedDelete:
        case FindResult::kNotFound: {
            switch (conflict_type) {
                case ConflictType::kIgnore: {
                    LOG_TRACE(fmt::format("Ignore drop a not existed entry."));
                    return {nullptr, Status::Ignore()};
                }
                default: {
                    auto err_msg = MakeUnique<String>("Not existed entry.");
                    // LOG_ERROR(*err_msg);
                    if constexpr (std::is_same_v<Entry, DBEntry>) {
                        return {nullptr, Status(ErrorCode::kDBNotExist, std::move(err_msg))};
                    } else if constexpr (std::is_same_v<Entry, TableEntry>) {
                        return {nullptr, Status(ErrorCode::kTableNotExist, std::move(err_msg))};
                    } else if constexpr (std::is_same_v<Entry, TableIndexEntry>) {
                        return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg))};
                    } else {
                        String error_message = "Unimplemented";
                        UnrecoverableError(error_message);
                    }
                }
            }
        }
        case FindResult::kUncommitted: {
            SharedPtr<Entry> drop_entry = init_func(txn_id, begin_ts);
            SharedPtr<Entry> uncommitted_entry = std::move(entry_list_.front());
            entry_list_.pop_front();
            return {uncommitted_entry, Status::OK()};
        }
        case FindResult::kFound: {
            SharedPtr<Entry> drop_entry = init_func(txn_id, begin_ts);
            if constexpr (std::is_same_v<Entry, TableIndexEntry>) {
                drop_entry->table_index_def().get()->index_type_ = entry_list_.front()->index_base()->index_type_;
            }
            entry_list_.push_front(drop_entry);
            return {drop_entry, Status::OK()};
        }
        case FindResult::kConflict: {
            auto err_msg = MakeUnique<String>(
                fmt::format("Write-write conflict: There is a committing/committed entry which is later than current transaction."));
            LOG_ERROR(*err_msg);
            return {nullptr, Status(ErrorCode::kTxnConflict, std::move(err_msg))};
        }
        default: {
            String error_message = "Invalid find result";
            UnrecoverableError(error_message);
        }
    }
}

template <EntryConcept Entry>
Pair<Entry *, FindResult> EntryList<Entry>::GetEntryInner1NoLock(TransactionID txn_id, TxnTimeStamp begin_ts) {
    Entry *entry_ptr = nullptr;
    FindResult find_res = FindResult::kNotFound;
    for (const auto &entry : entry_list_) {
        if (entry->Committed()) {
            if (begin_ts >= entry->commit_ts_) {
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
    return {entry_ptr, find_res};
}

template <EntryConcept Entry>
Pair<Entry *, Status> EntryList<Entry>::GetEntryInner2NoLock(Entry *entry_ptr, FindResult find_res) {
    switch (find_res) {
        case FindResult::kNotFound: {
            auto err_msg = MakeUnique<String>("Not existed entry.");
            // LOG_ERROR(*err_msg);
            if constexpr (std::is_same_v<Entry, DBEntry>) {
                return {nullptr, Status(ErrorCode::kDBNotExist, std::move(err_msg))};
            } else if constexpr (std::is_same_v<Entry, TableEntry>) {
                return {nullptr, Status(ErrorCode::kTableNotExist, std::move(err_msg))};
            } else if constexpr (std::is_same_v<Entry, TableIndexEntry>) {
                return {nullptr, Status(ErrorCode::kIndexNotExist, std::move(err_msg))};
            } else {
                String error_message = "Unimplemented";
                UnrecoverableError(error_message);
            }
        }
        case FindResult::kFound: {
            return {entry_ptr, Status::OK()};
        }
        default: {
            String error_message = "Invalid find result";
            UnrecoverableError(error_message);
        }
    }
    return {nullptr, Status::UnexpectedError("Unreachable")};
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::GetEntry(std::shared_lock<std::shared_mutex> &&parent_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
    std::shared_lock r_lock(rw_locker_);
    parent_lock.unlock();
    auto [entry_ptr, find_res] = this->GetEntryInner1NoLock(txn_id, begin_ts);
    r_lock.unlock();

    return this->GetEntryInner2NoLock(entry_ptr, find_res);
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::GetEntryNolock(TransactionID txn_id, TxnTimeStamp begin_ts) {
    std::shared_lock r_lock(rw_locker_);
    auto [entry_ptr, find_res] = this->GetEntryInner1NoLock(txn_id, begin_ts);
    r_lock.unlock();
    return this->GetEntryInner2NoLock(entry_ptr, find_res);
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::AddEntryReplay(std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_func,
                                                        TransactionID txn_id,
                                                        TxnTimeStamp begin_ts) {
    FindResult find_res = FindEntryReplay(txn_id, begin_ts);
    switch (find_res) {
        case FindResult::kNotFound: {
            SharedPtr<Entry> entry = init_func(txn_id, begin_ts);
            auto *entry_ptr = entry.get();
            entry_list_.push_front(std::move(entry));
            return {entry_ptr, Status::OK()};
        }
        case FindResult::kFound: {
            SharedPtr<Entry> entry = init_func(txn_id, begin_ts);
            auto *entry_ptr = entry.get();
            entry_list_.push_front(std::move(entry));
            return {entry_ptr, Status::OK()};
        }
        default: {
            return {nullptr, Status::InvalidEntry()};
        }
    }
}

template <EntryConcept Entry>
Status EntryList<Entry>::UpdateEntryReplay(std::function<void(SharedPtr<Entry>, TransactionID, TxnTimeStamp)> &&update_func,
                                           TransactionID txn_id,
                                           TxnTimeStamp begin_ts) {
    FindResult find_res = FindEntryReplay(txn_id, begin_ts);
    switch (find_res) {
        case FindResult::kNotFound: {
            return Status::NotFoundEntry();
        }
        case FindResult::kFound: {
            SharedPtr<Entry> src = entry_list_.front();
            update_func(src, txn_id, begin_ts);
            return Status::OK();
        }
        default: {
            return Status::InvalidEntry();
        }
    }
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::DropEntryReplay(std::function<SharedPtr<Entry>(TransactionID, TxnTimeStamp)> &&init_entry,
                                                         TransactionID txn_id,
                                                         TxnTimeStamp begin_ts) {
    FindResult find_res = FindEntryReplay(txn_id, begin_ts);
    switch (find_res) {
        case FindResult::kNotFound: {
            return {nullptr, Status::NotFoundEntry()};
        }
        case FindResult::kFound: {
            auto dropped_entry = init_entry(txn_id, begin_ts);
            auto *dropped_entry_ptr = dropped_entry.get();
            entry_list_.push_front(std::move(dropped_entry));
            return {dropped_entry_ptr, Status::OK()};
        }
        default: {
            return {nullptr, Status::InvalidEntry()};
        }
    }
}

template <EntryConcept Entry>
Tuple<Entry *, Status> EntryList<Entry>::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
    if (!entry_list_.empty()) {
        auto *entry = entry_list_.front().get();
        // FIXME
        if ((true || (begin_ts >= entry->commit_ts_ || txn_id == entry->txn_id_)) && !entry->deleted_) {
            return {entry, Status::OK()};
        }
    }
    UniquePtr<String> err_msg = MakeUnique<String>("No entry found.");
    LOG_ERROR(*err_msg);
    return {nullptr, Status::NotFoundEntry()};
}

template <EntryConcept Entry>
List<SharedPtr<Entry>> EntryList<Entry>::DeleteEntry(TransactionID txn_id) {
    std::unique_lock lock(rw_locker_);
    auto removed_iter = std::remove_if(entry_list_.begin(), entry_list_.end(), [&](auto &entry) { return entry->txn_id_ == txn_id; });
    List<SharedPtr<Entry>> erase_list;
    erase_list.splice(erase_list.begin(), entry_list_, removed_iter, entry_list_.end());
    return erase_list;
}

template <EntryConcept Entry>
bool EntryList<Entry>::PickCleanup(CleanupScanner *scanner) {
    std::unique_lock lock(rw_locker_);

    TxnTimeStamp visible_ts = scanner->visible_ts();
    auto iter = entry_list_.begin();
    bool dropped = true;
    while (iter != entry_list_.end()) {
        SharedPtr<Entry> &entry = *iter;
        if (entry->commit_ts_ < visible_ts) {
            if (entry->deleted_) {
                LOG_DEBUG(fmt::format("PickCleanup entry: {}, commit_ts: {}", entry->encode(), entry->commit_ts_));
                iter = entry_list_.erase(iter);
            } else {
                lock.unlock();
                entry->PickCleanup(scanner);
                lock.lock();
                ++iter;
                dropped = false;
            }
            break;
        }
        ++iter;
    }
    while (iter != entry_list_.end()) {
        SharedPtr<Entry> &entry = *iter;
        if (entry->Committed()) {
            if (!entry->deleted_) {
                scanner->AddEntry(std::move(entry), dropped);
                dropped = false;
            } else {
                dropped = true;
            }
            iter = entry_list_.erase(iter);
        } else {
            ++iter;
        }
    }
    return entry_list_.empty();
}

// TODO: check if this need to lock
template <EntryConcept Entry>
void EntryList<Entry>::Cleanup(CleanupInfoTracer *info_tracer, bool dropped) {
    for (auto iter = entry_list_.begin(); iter != entry_list_.end(); ++iter) {
        SharedPtr<Entry> &entry = *iter;
        entry->Cleanup(info_tracer, dropped);
    }
}

template <EntryConcept Entry>
void EntryList<Entry>::Iterate(std::function<void(Entry *)> func, TxnTimeStamp visible_ts) {
    std::shared_lock lock(rw_locker_);
    for (auto &entry : entry_list_) {
        if (entry->commit_ts_ < visible_ts && !entry->Deleted()) {
            func(entry.get());
        }
    }
}

} // namespace infinity