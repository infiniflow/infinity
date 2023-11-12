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

#include <vector>

import base_entry;
import stl;
import parser;
import index_def;
import txn_manager;
import default_values;
import txn_state;
import index_def_entry;
import logger;
import third_party;

import infinity_exception;
import table_collection_entry;

module index_def_meta;

namespace infinity {

class SegmentEntry;

IndexDefMeta::IndexDefMeta(TableCollectionEntry *table_collection_entry) : table_collection_entry_(table_collection_entry) {
    // Insert a dummy entry
    entry_list_.emplace_front(MakeUnique<BaseEntry>(EntryType::kDummy));
}

EntryResult IndexDefMeta::CreateNewEntry(IndexDefMeta *index_def_meta,
                                         SharedPtr<IndexDef> index_def,
                                         ConflictType conflict_type,
                                         u64 txn_id,
                                         TxnTimeStamp begin_ts,
                                         TxnManager *txn_mgr) {
    UniqueLock<RWMutex> w_locker(index_def_meta->rw_locker_);

    EntryStatus status = index_def_meta->AddEntryInternal(txn_id, begin_ts, txn_mgr);
    switch (status) {
        case BaseMeta::kExisted: {
            LOG_TRACE(Format("Duplicated index."));
            switch (conflict_type) {
                case ConflictType::kIgnore: {
                    return {.entry_ = nullptr, .err_ = nullptr};
                }
                case ConflictType::kError: {
                    return {.entry_ = nullptr, .err_ = MakeUnique<String>("Duplicated index.")};
                }
                default: {
                    throw StorageException("Invalid conflict type.");
                }
            }
        }
        case BaseMeta::kNotExisted: {
            String &table_dir = *index_def_meta->table_collection_entry_->table_entry_dir_;
            SharedPtr<String> index_dir = index_def_meta->IndexDirName(table_dir, *index_def->index_name_);
            auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, txn_id, begin_ts, index_dir);
            IndexDefEntry *res = index_def_entry.get();
            index_def_meta->entry_list_.emplace_front(Move(index_def_entry));
            return {.entry_ = res, .err_ = nullptr};
        }
        case BaseMeta::kConflict: {
            // Write-Write conflict
            LOG_TRACE("Write-Write conflict: There is a committed index: which is later than current transaction.");
            return {.entry_ = nullptr,
                    .err_ = MakeUnique<String>("Write-Write conflict. There is a committed index which is later than current transaction.")};
        }
    }
}

EntryResult
IndexDefMeta::DropNewEntry(IndexDefMeta *index_def_meta, ConflictType conflict_type, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr) {
    UniqueLock<RWMutex> w_locker(index_def_meta->rw_locker_);

    EntryStatus status = index_def_meta->AddEntryInternal(txn_id, begin_ts, txn_mgr);
    switch (status) {
        case kExisted: {
            auto delete_entry = MakeUnique<IndexDefEntry>(nullptr, index_def_meta, txn_id, begin_ts, nullptr);
            delete_entry->deleted_ = true;
            index_def_meta->entry_list_.emplace_front(Move(delete_entry));
            return {.entry_ = index_def_meta->entry_list_.front().get(), .err_ = nullptr};
        }
        case kNotExisted: {
            LOG_TRACE(Format("Attempt to drop not existed entry."));
            switch (conflict_type) {
                case ConflictType::kIgnore: {
                    return {.entry_ = nullptr, .err_ = nullptr};
                }
                case ConflictType::kError: {
                    return {.entry_ = nullptr, .err_ = MakeUnique<String>("Not existed entry.")};
                }
                default: {
                    throw StorageException("Invalid conflict type");
                }
            }
        }
        case kConflict: {
            // Write-Write conflict
            LOG_TRACE("Write-Write conflict: There is a committed entry which is later than current transaction.");
            return {.entry_ = nullptr,
                    .err_ = MakeUnique<String>("Write-Write conflict. There is a committed entry which is later than current transaction.")};
        }
    }
}

SharedPtr<String> IndexDefMeta::ToString(IndexDefMeta *index_def_meta) { throw StorageException("Not implemented"); }

Json IndexDefMeta::Serialize(const IndexDefMeta *index_def_meta, TxnTimeStamp max_commit_ts) {
    Json json;

    for (const auto &entry : index_def_meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kIndexDef) {
            json["entries"].emplace_back(IndexDefEntry::Serialize(static_cast<IndexDefEntry *>(entry.get()), max_commit_ts));
        } else if (entry->entry_type_ == EntryType::kDummy) {
            continue;
        } else {
            Error<StorageException>("Unexpected entry type.");
        }
    }
    return json;
}

UniquePtr<IndexDefMeta> IndexDefMeta::Deserialize(const Json &index_def_meta_json, TableCollectionEntry *table_entry, BufferManager *buffer_mgr) {
    LOG_TRACE(Format("load index"));

    auto res = MakeUnique<IndexDefMeta>(table_entry);
    if (index_def_meta_json.contains("entries")) {
        auto &entries = index_def_meta_json["entries"];
        // traverse reversely because a dummy head has been inserted
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = IndexDefEntry::Deserialize(*iter, res.get(), buffer_mgr, table_entry);
            res->entry_list_.emplace_front(Move(entry));
        }
    }
    return res;
}

SharedPtr<String> IndexDefMeta::IndexDirName(const String &table_dir, const String &index_name) const {
    return MakeShared<String>(Format("{}/{}", table_dir, index_name));
}
} // namespace infinity
