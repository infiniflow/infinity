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

module catalog;

import stl;

import infinity_exception;

namespace infinity {

// Merge two reverse-ordered list inplace.
void MergeLists(List<UniquePtr<BaseEntry>> &list1, List<UniquePtr<BaseEntry>> &list2) {
    auto it1 = list1.begin();
    auto it2 = list2.begin();

    while (it1 != list1.end() && it2 != list2.end()) {
        if ((*it1)->entry_type_ == EntryType::kDummy) {
            ++it1;
        } else if ((*it2)->entry_type_ == EntryType::kDummy) {
            ++it2;
        } else {
            if (!(*it1)->Committed() || !(*it2)->Committed()) {
                Error<StorageException>("MergeLists requires entries be committed");
            }
            if ((*it1)->commit_ts_ > (*it2)->commit_ts_) {
                ++it1;
            } else if ((*it1)->commit_ts_ < (*it2)->commit_ts_) {
                list1.insert(it1, Move(*it2));
                ++it2;
            } else {
                (*it1)->MergeFrom(**it2);
                ++it1;
                ++it2;
            }
        }
    }

    while(it2 != list2.end()) {
        if ((*it2)->entry_type_ != EntryType::kDummy) {
            list1.insert(it1, Move(*it2));
        }
        ++it2;
    }

    list2.clear();
}

} // namespace infinity
