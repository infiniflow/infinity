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
#include <time.h>

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
import table_collection_entry;
import segment_entry;
import local_file_system;
import default_values;
import random;

module index_def_entry;

namespace infinity {
IndexDefEntry::IndexDefEntry(SharedPtr<IndexDef> index_def,
                             IndexDefMeta *index_def_meta,
                             SharedPtr<String> index_dir,
                             u64 txn_id,
                             TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIndexDef), index_def_(index_def), index_def_meta_(index_def_meta), index_dir_(index_dir) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

UniquePtr<IndexDefEntry>
IndexDefEntry::NewIndexDefEntry(SharedPtr<IndexDef> index_def, IndexDefMeta *index_def_meta, u64 txn_id, TxnTimeStamp begin_ts) {
    SharedPtr<String> index_dir = DetermineIndexDir(*index_def_meta->table_collection_entry_->table_entry_dir_, *index_def->index_name_);
    return MakeUnique<IndexDefEntry>(index_def, index_def_meta, index_dir, txn_id, begin_ts);
}

void IndexDefEntry::CommitCreatedIndex(IndexDefEntry *index_def_entry, u32 segment_id, SharedPtr<IndexEntry> index_entry) {
    UniqueLock<RWMutex> w_locker(index_def_entry->rw_locker_);
    index_def_entry->indexes_.emplace(segment_id, index_entry);
}

Json IndexDefEntry::Serialize(const IndexDefEntry *index_def_entry, TxnTimeStamp max_commit_ts) {
    Json json;
    json["txn_id"] = index_def_entry->txn_id_.load();
    json["begin_ts"] = index_def_entry->begin_ts_;
    json["commit_ts"] = index_def_entry->commit_ts_.load();
    json["deleted"] = index_def_entry->deleted_;
    if (!index_def_entry->deleted_) {
        json["index_dir"] = *index_def_entry->index_dir_;
        json["index_def"] = index_def_entry->index_def_->Serialize();
        for (const auto &[segment_id, index_entry] : index_def_entry->indexes_) {
            IndexEntry::Flush(index_entry.get(), max_commit_ts);
            json["indexes"].push_back(IndexEntry::Serialize(index_entry.get()));
        }
    }
    return json;
}

UniquePtr<IndexDefEntry> IndexDefEntry::Deserialize(const Json &index_def_entry_json,
                                                    IndexDefMeta *index_def_meta,
                                                    BufferManager *buffer_mgr,
                                                    TableCollectionEntry *table_entry) {
    u64 txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    bool deleted = index_def_entry_json["deleted"];

    if (deleted) {
        return MakeUnique<IndexDefEntry>(nullptr, index_def_meta, nullptr, txn_id, begin_ts);
    }

    auto index_dir = MakeShared<String>(index_def_entry_json["index_dir"]);
    SharedPtr<IndexDef> index_def = IndexDef::Deserialize(index_def_entry_json["index_def"]);

    auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, index_dir, txn_id, begin_ts);
    index_def_entry->commit_ts_.store(commit_ts);
    index_def_entry->deleted_ = deleted;

    if (index_def_entry_json.contains("indexes")) {
        for (const auto &index_entry_json : index_def_entry_json["indexes"]) {
            u32 segment_id = index_entry_json["segment_id"];
            SegmentEntry *segment_entry = table_entry->segments_.at(segment_id).get();

            if (index_def->column_names_.size() > 1) {
                StorageException("Not implemented.");
            }
            u64 column_id = table_entry->GetColumnIdByName(index_def->column_names_[0]);
            SharedPtr<ColumnDef> column_def = table_entry->columns_[column_id];

            UniquePtr<CreateIndexPara> create_index_para = SegmentEntry::GetCreateIndexPara(segment_entry, index_def, column_def);

            auto index_entry = IndexEntry::Deserialize(index_entry_json, index_def_entry.get(), segment_entry, buffer_mgr, Move(create_index_para));
            index_def_entry->indexes_.emplace(index_entry->segment_entry_->segment_id_, index_entry);
        }
    }

    return index_def_entry;
}

SharedPtr<String> IndexDefEntry::DetermineIndexDir(const String &parent_dir, const String &index_name) {
    LocalFileSystem fs;
    SharedPtr<String> index_dir;
    do {
        u32 seed = time(nullptr);
        index_dir = MakeShared<String>(parent_dir + '/' + RandomString(DEFAULT_RANDOM_NAME_LEN, seed) + "_index_" + index_name);
    } while (!fs.CreateDirectoryNoExp(*index_dir));
    return index_dir;
}

} // namespace infinity
