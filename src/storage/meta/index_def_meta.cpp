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

IndexDefMeta::IndexDefMeta(SharedPtr<String> index_name, TableCollectionEntry *table_collection_entry)
    : index_name_(Move(index_name)), table_collection_entry_(table_collection_entry) {
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
            auto index_def_entry = MakeUnique<IndexDefEntry>(index_def, index_def_meta, txn_id, begin_ts);
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

SharedPtr<String> IndexDefMeta::ToString(IndexDefMeta *index_def_meta) { throw StorageException("Not implemented"); }

Json IndexDefMeta::Serialize(const IndexDefMeta *index_def_meta) {
    Json json;

    json["index_name"] = *index_def_meta->index_name_;
    for (const auto &entry : index_def_meta->entry_list_) {
        if (entry->entry_type_ == EntryType::kIndexDef) {
            json["entries"].emplace_back(IndexDefEntry::Serialize(static_cast<IndexDefEntry *>(entry.get())));
        } else if (entry->entry_type_ == EntryType::kDummy) {
            json["entries"].emplace_back(BaseEntry::Serialize(entry.get()));
        } else {
            Error<StorageException>("Unexpected entry type.");
        }
    }
    return json;
}

UniquePtr<IndexDefMeta> IndexDefMeta::Deserialize(const Json &index_def_meta_json, TableCollectionEntry *table_collection_entry) {
    auto index_name = MakeShared<String>(index_def_meta_json["index_name"]);
    LOG_TRACE(Format("load index {}", *index_name));

    auto res = MakeUnique<IndexDefMeta>(Move(index_name), table_collection_entry);
    if (index_def_meta_json.contains("entries")) {
        auto &entries = index_def_meta_json["entries"];
        for (auto iter = entries.rbegin(); iter != entries.rend(); iter++) {
            auto entry = IndexDefEntry::Deserialize(*iter, res.get());
            res->entry_list_.emplace_front(Move(entry));
        }
    }
    return res;
}
} // namespace infinity
