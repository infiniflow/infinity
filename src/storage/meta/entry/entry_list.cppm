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

namespace infinity {

export template <EntryConcept Entry>
class EntryList {
public:
    bool PickCleanup(CleanupScanner *scanner);

    void Cleanup() &&;

    void MergeWith(EntryList<Entry> &other);

public: // TODO: make both private
    std::shared_mutex rw_locker_{};

    List<SharedPtr<Entry>> entry_list_;
};

template <EntryConcept Entry>
bool EntryList<Entry>::PickCleanup(CleanupScanner *scanner) {
    std::unique_lock lock(rw_locker_);

    if (entry_list_.empty()) {
        return true;
    }

    TxnTimeStamp visible_ts = scanner->visible_ts();
    auto iter = entry_list_.begin();
    while ((*iter)->entry_type_ != EntryType::kDummy) {
        SharedPtr<Entry> &entry = *iter;
        if (entry->commit_ts_ < visible_ts) {
            if (entry->deleted_) {
                scanner->AddEntry(std::move(entry));
                iter = entry_list_.erase(iter);
            } else {
                entry->PickCleanup(scanner);
                ++iter;
            }
            break;
        }
        ++iter;
    }
    while ((*iter)->entry_type_ != EntryType::kDummy) {
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