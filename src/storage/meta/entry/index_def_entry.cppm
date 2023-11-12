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
import index_entry;

export module index_def_entry;

namespace infinity {

class IndexDefMeta;
class BufferManager;
class TxnTableStore;
class CreateIndexPara;
class TableCollectionEntry;

export struct IndexDefEntry : public BaseEntry {
public:
    explicit IndexDefEntry(SharedPtr<IndexDef> index_def,
                           IndexDefMeta *index_def_meta,
                           SharedPtr<String> index_dir,
                           u64 txn_id,
                           TxnTimeStamp begin_ts);

    static UniquePtr<IndexDefEntry> NewIndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static void CommitCreatedIndex(IndexDefEntry *index_def_entry, u32 segment_id, SharedPtr<IndexEntry> index_entry);

    static Json Serialize(const IndexDefEntry *index_def_entry, TxnTimeStamp max_commit_ts);

    static UniquePtr<IndexDefEntry>
    Deserialize(const Json &index_def_entry_json, IndexDefMeta *index_def_meta, BufferManager *buffer_mgr, TableCollectionEntry *table_entry);

private:
    static SharedPtr<String> DetermineIndexDir(const String &parent_dir, const String &index_name);

public:
    RWMutex rw_locker_{};

    IndexDefMeta *const index_def_meta_;

    SharedPtr<String> index_dir_;
    const SharedPtr<IndexDef> index_def_;
    HashMap<u32, SharedPtr<IndexEntry>> indexes_{};
};
} // namespace infinity
