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

#include "faiss/index_io.h"
#include <vector>

import stl;
import base_entry;
import index_def;
import third_party;
import index_def_meta;
import buffer_manager;
import txn_store;
import index_entry;
import parser;
import infinity_exception;
import buffer_handle;
import txn_store;

import ivfflat_index_def;
import hnsw_index_def;
import index_file_worker;
import faiss_index_file_worker;
import hnsw_file_worker;
import knn_hnsw;
import dist_func;

module index_def_entry;

namespace infinity {
IndexDefEntry::IndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIndexDef), index_def_meta_(index_def_meta), index_def_(index_def) {
    begin_ts_ = begin_ts;
    txn_id_ = txn_id;
}

void IndexDefEntry::CommitCreatedIndex(IndexDefEntry *index_def_entry, u32 segment_id, SharedPtr<IndexEntry> index_entry) {
    UniqueLock<RWMutex> w_locker(index_def_entry->rw_locker_);
    index_def_entry->indexes_.emplace(segment_id, index_entry);
}

Json IndexDefEntry::Serialize(const IndexDefEntry *index_def_entry) {
    Json json = BaseEntry::Serialize(index_def_entry);
    index_def_entry->index_def_->Serialize();
    for (const auto &[segment_id, index_entry] : index_def_entry->indexes_) {
        json["index_entry_list"].push_back(IndexEntry::Serialize(index_entry.get()));
    }
    return json;
}

UniquePtr<IndexDefEntry> IndexDefEntry::Deserialize(const Json &index_def_entry_json, IndexDefMeta *index_def_meta) {
    u64 txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    bool deleted = index_def_entry_json["delete"];
    if (!deleted) {
        auto index_def = IndexDef::Deserialize(index_def_entry_json["index_def"]);
        auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, txn_id, begin_ts);
        index_def_entry->commit_ts_.store(commit_ts);
        index_def_entry->deleted_ = deleted;
        index_def_entry->index_def_meta_ = index_def_meta;
        index_def_entry->index_def_ = index_def;
        return index_def_entry;
    }
    auto index_def_entry = MakeUnique<IndexDefEntry>(nullptr, index_def_meta, txn_id, begin_ts);
    index_def_entry->commit_ts_.store(commit_ts);
    index_def_entry->deleted_ = deleted;
    index_def_entry->index_def_meta_ = index_def_meta;
    return index_def_entry;
}
} // namespace infinity
