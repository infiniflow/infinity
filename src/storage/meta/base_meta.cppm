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

#include <concepts>

export module base_meta;

import stl;
import base_entry;
import infinity_exception;

namespace infinity {

export template <EntryConcept Entry>
class BaseMeta {
public:
    virtual ~BaseMeta() = default;

    // Cleanup the delete inner entry under this meta, return true if all inner entry are deleted
    Pair<Vector<SharedPtr<Entry>>, bool> PickCleanup(TxnTimeStamp oldest_txn_ts);

    // Have cleaned up all inner entry, clean the meta itself
    void CleanupMeta();

    // Cleanup the delete inner entry under this meta
    void CleanupDelete(TxnTimeStamp oldest_txn_ts);

    void Cleanup();

protected:
    void MergeWith(BaseMeta<Entry> &other);

protected:
    std::shared_mutex rw_locker_{};

    // Ordered by commit_ts from latest to oldest.
    List<SharedPtr<Entry>> entry_list_;
};

template <EntryConcept Entry>
void BaseMeta<Entry>::MergeWith(BaseMeta<Entry> &other) {
    auto &other_list = other.entry_list_;
    auto it1 = entry_list_.begin();
    auto it2 = other_list.begin();

    while (it1 != entry_list_.end() && it2 != other_list.end()) {
        if ((*it1)->entry_type_ == EntryType::kDummy) {
            ++it1;
        } else if ((*it2)->entry_type_ == EntryType::kDummy) {
            ++it2;
        } else {
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
    }

    while (it2 != other_list.end()) {
        if ((*it2)->entry_type_ != EntryType::kDummy) {
            entry_list_.insert(it1, std::move(*it2));
        }
        ++it2;
    }

    other_list.clear();
}

template <EntryConcept Entry>
Pair<Vector<SharedPtr<Entry>>, bool> BaseMeta<Entry>::PickCleanup(TxnTimeStamp oldest_txn_ts) {
    Vector<SharedPtr<Entry>> cleanup_entries;
    bool all_delete = true;
    {
        std::unique_lock wlock(rw_locker_);
        for (auto iter = entry_list_.begin(); iter != entry_list_.end();) {
            // TODO
        }
    }
    return {cleanup_entries, all_delete};
}

template <EntryConcept Entry>
void BaseMeta<Entry>::CleanupMeta() {
    //
}

template <EntryConcept Entry>
void BaseMeta<Entry>::CleanupDelete(TxnTimeStamp oldest_txn_ts) {
    //
}

template <EntryConcept Entry>
void BaseMeta<Entry>::Cleanup() {
    //
}

// old
export template <typename Meta>
concept MetaConcept = std::derived_from<Meta, BaseMeta<typename Meta::EntryT>> && std::derived_from<typename Meta::EntryT, BaseEntry>;

// new
export class MetaInterface {
public:
    virtual ~MetaInterface() = default;

    virtual void Cleanup() = 0;

    virtual void CleanupDelete(TxnTimeStamp oldest_txn_ts) = 0;

    virtual void CleanupMeta() = 0;
};

export template <typename Meta>
concept MetaConcept1 = std::derived_from<Meta, MetaInterface> && std::derived_from<typename Meta::EntryT, BaseEntry>;

} // namespace infinity