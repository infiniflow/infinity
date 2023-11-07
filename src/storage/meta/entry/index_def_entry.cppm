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
import parser;
import index_def;
import base_entry;
import third_party;

export module index_def_entry;

namespace infinity {

class IndexDefMeta;

export struct IndexDefEntry : public BaseEntry {
public:
    explicit IndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts);

public:
    static Json Serialize(const IndexDefEntry *index_def_entry);

    static UniquePtr<IndexDefEntry> Deserialize(const Json &index_def_entry_json, IndexDefMeta *index_def_meta);

public:
    RWMutex rw_locker_{};

    IndexDefMeta *index_def_meta_{};

    SharedPtr<IndexDef> index_def_{};
};
} // namespace infinity
