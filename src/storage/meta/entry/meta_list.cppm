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

export module meta_list;

import stl;
import base_entry;
import infinity_exception;

namespace infinity {

export template <EntryConcept1 Entry>
class MetaList {
public:
    // Cleanup the delete inner entry under this meta, return true if all inner entry are deleted
    Pair<Vector<SharedPtr<Entry>>, bool> PickCleanup(TxnTimeStamp oldest_txn_ts);

    // Have cleaned up all inner entry, clean the meta itself
    void CleanupMeta();

    // Cleanup the delete inner entry under this meta
    void CleanupDelete(TxnTimeStamp oldest_txn_ts);

    // Cleanup All(no lock is needed)
    void Cleanup();

    void MergeWith(MetaList<Entry> &other);

public: // TODO: make both private
    std::shared_mutex rw_locker_{}; 

    List<SharedPtr<Entry>> entry_list_;
};

template <EntryConcept1 Entry>
Pair<Vector<SharedPtr<Entry>>, bool> MetaList<Entry>::PickCleanup(TxnTimeStamp oldest_txn_ts) {
    //
}

template <EntryConcept1 Entry>
void MetaList<Entry>::CleanupMeta() {
    //
}

template <EntryConcept1 Entry>
void MetaList<Entry>::CleanupDelete(TxnTimeStamp oldest_txn_ts) {
    //
}

template <EntryConcept1 Entry>
void MetaList<Entry>::Cleanup() {
    //
}

template <EntryConcept1 Entry>
void MetaList<Entry>::MergeWith(MetaList<Entry> &other) {
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

} // namespace infinity