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

#include <ctime>
#include <string>

import stl;
import base_entry;
import index_base;
import third_party;
import segment_column_index_entry;
import table_index_entry;
import local_file_system;
import default_values;
import random;
import buffer_manager;
import infinity_exception;
import table_collection_entry;

module column_index_entry;

namespace infinity {

ColumnIndexEntry::ColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                   TableIndexEntry *table_index_entry,
                                   u64 column_id,
                                   SharedPtr<String> index_dir,
                                   u64 txn_id,
                                   TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kColumnIndex), table_index_entry_(table_index_entry), column_id_(column_id), index_dir_(index_dir),
      index_base_(index_base) {
    begin_ts_ = begin_ts; // TODO:: begin_ts and txn_id should be const and set in BaseEntry
    txn_id_ = txn_id;
}

SharedPtr<ColumnIndexEntry> ColumnIndexEntry::NewColumnIndexEntry(SharedPtr<IndexBase> index_base,
                                                                  u64 column_id,
                                                                  TableIndexEntry *table_index_entry,
                                                                  u64 txn_id,
                                                                  SharedPtr<String> index_dir,
                                                                  TxnTimeStamp begin_ts) {
    //    SharedPtr<String> index_dir =
    //        DetermineIndexDir(*TableIndexMeta::GetTableCollectionEntry(table_index_meta)->table_entry_dir_, index_base->file_name_);
    return MakeShared<ColumnIndexEntry>(index_base, table_index_entry, column_id, index_dir, txn_id, begin_ts);
}

void ColumnIndexEntry::CommitCreatedIndex(ColumnIndexEntry *column_index_entry, u32 segment_id, UniquePtr<SegmentColumnIndexEntry> index_entry) {
    UniqueLock<RWMutex> w_locker(column_index_entry->rw_locker_);
    column_index_entry->index_by_segment.emplace(segment_id, Move(index_entry));
}

Json ColumnIndexEntry::Serialize(ColumnIndexEntry *column_index_entry, TxnTimeStamp max_commit_ts) {
    if (column_index_entry->deleted_) {
        Error<StorageException>("Column index entry can't be deleted.");
    }

    Json json;
    Vector<SegmentColumnIndexEntry *> segment_column_index_entry_candidates;
    {
        SharedLock<RWMutex> lck(column_index_entry->rw_locker_);

        json["txn_id"] = column_index_entry->txn_id_.load();
        json["begin_ts"] = column_index_entry->begin_ts_;
        json["commit_ts"] = column_index_entry->commit_ts_.load();
        json["deleted"] = column_index_entry->deleted_;
        json["column_id"] = column_index_entry->column_id_;
        json["index_dir"] = *column_index_entry->index_dir_;
        json["index_base"] = column_index_entry->index_base_->Serialize();

        for (const auto &[segment_id, index_entry] : column_index_entry->index_by_segment) {
            segment_column_index_entry_candidates.emplace_back((SegmentColumnIndexEntry*)index_entry.get());
        }
    }

    for(const auto& segment_column_index_entry: segment_column_index_entry_candidates) {
        SegmentColumnIndexEntry::Flush(segment_column_index_entry, max_commit_ts);
        json["index_by_segment"].push_back(SegmentColumnIndexEntry::Serialize(segment_column_index_entry));
    }

    return json;
}

UniquePtr<ColumnIndexEntry> ColumnIndexEntry::Deserialize(const Json &column_index_entry_json,
                                                          TableIndexEntry *table_index_entry,
                                                          BufferManager *buffer_mgr,
                                                          TableCollectionEntry *table_collection_entry) {
    bool deleted = column_index_entry_json["deleted"];
    if (deleted) {
        Error<StorageException>("Column index entry can't be deleted.");
    }

    u64 txn_id = column_index_entry_json["txn_id"];
    TxnTimeStamp begin_ts = column_index_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = column_index_entry_json["commit_ts"];
    u64 column_id = column_index_entry_json["column_id"];
    auto index_dir = MakeShared<String>(column_index_entry_json["index_dir"]);
    SharedPtr<IndexBase> index_base = IndexBase::Deserialize(column_index_entry_json["index_base"]);

    auto column_index_entry = MakeUnique<ColumnIndexEntry>(index_base, table_index_entry, column_id, index_dir, txn_id, begin_ts);
    column_index_entry->commit_ts_.store(commit_ts);
    column_index_entry->deleted_ = deleted;

    if (column_index_entry_json.contains("index_by_segment")) {
        for (const auto &index_by_segment_json : column_index_entry_json["index_by_segment"]) {
            UniquePtr<SegmentColumnIndexEntry> segment_column_index_entry =
                SegmentColumnIndexEntry::Deserialize(index_by_segment_json, column_index_entry.get(), buffer_mgr, table_collection_entry);
            column_index_entry->index_by_segment.emplace(segment_column_index_entry->segment_id_, Move(segment_column_index_entry));
        }
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
