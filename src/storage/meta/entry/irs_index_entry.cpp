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
import third_party;
import infinity_exception;
import buffer_manager;
import iresearch_datastore;
import index_base;
import index_full_text;
import logger;

namespace infinity {

IrsIndexEntry::IrsIndexEntry(TableIndexEntry *, SharedPtr<String> index_dir, u64 txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIRSIndex), index_dir_(Move(index_dir)) {
    txn_id_ = txn_id;
    begin_ts_ = begin_ts;
}

Json IrsIndexEntry::Serialize(const IrsIndexEntry *index_def_entry, TxnTimeStamp) {
    Json json;
    json["txn_id"] = index_def_entry->txn_id_.load();
    json["begin_ts"] = index_def_entry->begin_ts_;
    json["commit_ts"] = index_def_entry->commit_ts_.load();
    json["index_dir"] = *index_def_entry->index_dir_;

    return json;
}

SharedPtr<IrsIndexEntry> IrsIndexEntry::Deserialize(const Json &index_def_entry_json, TableIndexEntry *table_index_entry, BufferManager *) {
    u64 txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    auto index_dir = MakeShared<String>(index_def_entry_json["index_dir"]);

    auto irs_index_entry = NewIrsIndexEntry(table_index_entry, txn_id, index_dir, begin_ts);
    irs_index_entry->commit_ts_.store(commit_ts);
    irs_index_entry->txn_id_.store(txn_id);
    irs_index_entry->begin_ts_ = begin_ts;
    irs_index_entry->index_dir_ = index_dir;

    return irs_index_entry;
}

SharedPtr<String> IrsIndexEntry::DetermineIndexDir(const String &, const String &) {
    Error<StorageException>("Not implemented");
    return nullptr;
}

SharedPtr<IrsIndexEntry>
IrsIndexEntry::NewIrsIndexEntry(TableIndexEntry *table_index_entry, u64 txn_id, SharedPtr<String> index_dir, TxnTimeStamp begin_ts) {
    auto irs_index_entry = MakeShared<IrsIndexEntry>(table_index_entry, index_dir, txn_id, begin_ts);
    irs_index_entry->irs_index_ = MakeShared<IRSDataStore>(*(table_index_entry->table_index_meta()->GetTableEntry()->GetTableName()), *(index_dir));
    return irs_index_entry;
}

} // namespace infinity
