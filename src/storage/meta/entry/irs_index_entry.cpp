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
import wal;

namespace infinity {

IrsIndexEntry::IrsIndexEntry(TableIndexEntry *table_index_entry, SharedPtr<String> index_dir, TransactionID txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIRSIndex), index_dir_(std::move(index_dir)) {
    table_index_entry_ = table_index_entry;
    txn_id_ = txn_id;
    begin_ts_ = begin_ts;
}

SharedPtr<IrsIndexEntry> IrsIndexEntry::NewIrsIndexEntry(TableIndexEntry *table_index_entry,
                                                         Txn *txn,
                                                         TransactionID txn_id,
                                                         SharedPtr<String> index_dir,
                                                         TxnTimeStamp begin_ts) {
    auto irs_index_entry = MakeShared<IrsIndexEntry>(table_index_entry, index_dir, txn_id, begin_ts);
    irs_index_entry->irs_index_ = MakeUnique<IRSDataStore>(*(table_index_entry->table_index_meta()->GetTableEntry()->GetTableName()), *(index_dir));

    {
        if (txn != nullptr) {
            auto operation = MakeUnique<AddIrsIndexEntryOperation>(irs_index_entry);
            txn->AddCatalogDeltaOperation(std::move(operation));
        }
    }
    return irs_index_entry;
}

nlohmann::json IrsIndexEntry::Serialize(TxnTimeStamp) {
    nlohmann::json json;
    json["txn_id"] = this->txn_id_.load();
    json["begin_ts"] = this->begin_ts_;
    json["commit_ts"] = this->commit_ts_.load();
    json["index_dir"] = *this->index_dir_;

    return json;
}

SharedPtr<IrsIndexEntry> IrsIndexEntry::Deserialize(const nlohmann::json &index_def_entry_json, TableIndexEntry *table_index_entry, BufferManager *) {
    TransactionID txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];
    auto index_dir = MakeShared<String>(index_def_entry_json["index_dir"]);

    auto irs_index_entry = NewIrsIndexEntry(table_index_entry, nullptr, txn_id, index_dir, begin_ts);
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

} // namespace infinity
