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
import base_entry;
import third_party;
import infinity_exception;
import table_index_entry;
import buffer_manager;

module irs_index_entry;

namespace infinity {

IrsIndexEntry::IrsIndexEntry(TableIndexEntry *table_index_entry, SharedPtr<String> index_dir, u64 txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIRSIndex), index_dir_(Move(index_dir)) {
    txn_id_ = txn_id;
    begin_ts_ = begin_ts;
}

Json IrsIndexEntry::Serialize(const IrsIndexEntry *index_def_entry, TxnTimeStamp max_commit_ts) {
    Error<StorageException>("Not implemented");
    Json irs_index_entry_json;

    return irs_index_entry_json;
}

SharedPtr<IrsIndexEntry> IrsIndexEntry::Deserialize(const Json &index_def_entry_json, TableIndexEntry *table_index_entry, BufferManager *buffer_mgr) {
    Error<StorageException>("Not implemented");
    return nullptr;
}

SharedPtr<String> IrsIndexEntry::DetermineIndexDir(const String &parent_dir, const String &index_name) {
    Error<StorageException>("Not implemented");
    return nullptr;
}

} // namespace infinity
