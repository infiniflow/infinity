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

//#include "faiss/index_io.h"
#include <ctime>
#include <string>

import stl;
import base_entry;
import base_index;
import third_party;
import segment_column_index_entry;
import table_index_entry;
import local_file_system;
import default_values;
import random;
import buffer_manager;

module column_index_entry;

namespace infinity {

ColumnIndexEntry::ColumnIndexEntry(SharedPtr<BaseIndex> base_index,
                                   TableIndexEntry *table_index_entry,
                                   SharedPtr<String> index_dir,
                                   u64 txn_id,
                                   TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kColumnIndex), table_index_entry_(table_index_entry), index_dir_(index_dir), base_index_(base_index) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

SharedPtr<ColumnIndexEntry> ColumnIndexEntry::NewColumnIndexEntry(SharedPtr<BaseIndex> base_index,
                                                                  u64 column_id,
                                                                  TableIndexEntry *table_index_entry,
                                                                  u64 txn_id,
                                                                  SharedPtr<String> index_dir,
                                                                  TxnTimeStamp begin_ts) {
    //    SharedPtr<String> index_dir =
    //        DetermineIndexDir(*TableIndexMeta::GetTableCollectionEntry(table_index_meta)->table_entry_dir_, base_index->file_name_);
    return MakeShared<ColumnIndexEntry>(base_index, table_index_entry, index_dir, txn_id, begin_ts);
}

void ColumnIndexEntry::CommitCreatedIndex(ColumnIndexEntry *column_index_entry, u32 segment_id, UniquePtr<SegmentColumnIndexEntry> index_entry) {
    UniqueLock<RWMutex> w_locker(column_index_entry->rw_locker_);
    column_index_entry->index_by_segment.emplace(segment_id, Move(index_entry));
}

Json ColumnIndexEntry::Serialize(const ColumnIndexEntry *column_index_entry, TxnTimeStamp max_commit_ts) {
    Json json;
    json["txn_id"] = column_index_entry->txn_id_.load();
    json["begin_ts"] = column_index_entry->begin_ts_;
    json["commit_ts"] = column_index_entry->commit_ts_.load();
    json["deleted"] = column_index_entry->deleted_;
    if (!column_index_entry->deleted_) {
        json["index_dir"] = *column_index_entry->index_dir_;
        json["base_index"] = column_index_entry->base_index_->Serialize();
        for (const auto &[segment_id, index_entry] : column_index_entry->index_by_segment) {
            SegmentColumnIndexEntry::Flush(index_entry.get(), max_commit_ts);
            json["indexes"].push_back(SegmentColumnIndexEntry::Serialize(index_entry.get()));
        }
    }
    return json;
}

UniquePtr<ColumnIndexEntry> ColumnIndexEntry::Deserialize(const Json &column_index_entry_json,
                                                          TableIndexEntry *table_index_entry,
                                                          BufferManager *buffer_mgr,
                                                          TableCollectionEntry *table_entry) {
    u64 txn_id = column_index_entry_json["txn_id"];
    TxnTimeStamp begin_ts = column_index_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = column_index_entry_json["commit_ts"];
    bool deleted = column_index_entry_json["deleted"];

    if (deleted) {
        auto column_index_entry = MakeUnique<ColumnIndexEntry>(nullptr, table_index_entry, nullptr, txn_id, begin_ts);
        column_index_entry->deleted_ = true;
        column_index_entry->commit_ts_.store(commit_ts);
        return column_index_entry;
    }

    auto index_dir = MakeShared<String>(column_index_entry_json["index_dir"]);
    SharedPtr<BaseIndex> base_index = BaseIndex::Deserialize(column_index_entry_json["base_index"]);

    auto column_index_entry = MakeUnique<ColumnIndexEntry>(base_index, table_index_entry, index_dir, txn_id, begin_ts);
    column_index_entry->commit_ts_.store(commit_ts);
    column_index_entry->deleted_ = deleted;

    for (const auto &index_by_segment_json : column_index_entry_json["index_by_segment"]) {
        UniquePtr<SegmentColumnIndexEntry> segment_column_index_entry =
            SegmentColumnIndexEntry::Deserialize(index_by_segment_json, column_index_entry.get(), buffer_mgr);
        column_index_entry->index_by_segment.emplace(segment_column_index_entry->segment_id_, Move(segment_column_index_entry));
    }

    return column_index_entry;
}

SharedPtr<String> ColumnIndexEntry::DetermineIndexDir(const String &parent_dir, const String &index_name) {
    LocalFileSystem fs;
    SharedPtr<String> index_dir;
    do {
        u32 seed = time(nullptr);
        index_dir = MakeShared<String>(parent_dir + "/" + RandomString(DEFAULT_RANDOM_NAME_LEN, seed) + "_index_" + index_name);
    } while (!fs.CreateDirectoryNoExp(*index_dir));
    return index_dir;
}

} // namespace infinity
