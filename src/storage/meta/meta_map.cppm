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

import stl;
import base_meta;

export module meta_map;

namespace infinity {

export template <MetaConcept1 Meta>
class MetaMap {
public:
    using InnerEntry = typename Meta::EntryT;

public:
    void Cleanup();

    void CleanupDelete(TxnTimeStamp oldest_txn_ts);

public: // TODO: make both private
    mutable std::shared_mutex rw_locker_{};

    HashMap<String, UniquePtr<Meta>> meta_map_;
};

template <MetaConcept1 Meta>
void MetaMap<Meta>::Cleanup() {
    //
}

template <MetaConcept1 Meta>
void MetaMap<Meta>::CleanupDelete(TxnTimeStamp oldest_txn_ts) {
    //
}

} // namespace infinity