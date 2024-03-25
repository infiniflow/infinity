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

module fulltext_index_entry;

import stl;
import third_party;
import infinity_exception;
import buffer_manager;
import index_base;
import index_full_text;
import logger;
import catalog_delta_entry;
import status;

namespace infinity {

FulltextIndexEntry::FulltextIndexEntry(TableIndexEntry *table_index_entry, TransactionID txn_id, TxnTimeStamp begin_ts)
    : BaseEntry(EntryType::kIRSIndex) {
    table_index_entry_ = table_index_entry;
    txn_id_ = txn_id;
    begin_ts_ = begin_ts;
}

SharedPtr<FulltextIndexEntry>
FulltextIndexEntry::NewFulltextIndexEntry(TableIndexEntry *table_index_entry, TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto fulltext_index_entry = MakeShared<FulltextIndexEntry>(table_index_entry, txn_id, begin_ts);
    bool homebrewed = table_index_entry->IsFulltextIndexHomebrewed();
    if (!homebrewed) {
    }

    return fulltext_index_entry;
}

SharedPtr<FulltextIndexEntry> FulltextIndexEntry::NewReplayFulltextIndexEntry(TableIndexEntry *table_index_entry,
                                                                              TransactionID txn_id,
                                                                              TxnTimeStamp begin_ts,
                                                                              TxnTimeStamp commit_ts,
                                                                              bool is_delete) {
    auto fulltext_index_entry = MakeShared<FulltextIndexEntry>(table_index_entry, txn_id, begin_ts);
    bool homebrewed = table_index_entry->IsFulltextIndexHomebrewed();
    if (!homebrewed) {
    }
    fulltext_index_entry->commit_ts_.store(commit_ts);
    fulltext_index_entry->deleted_ = is_delete;
    return fulltext_index_entry;
}

nlohmann::json FulltextIndexEntry::Serialize(TxnTimeStamp) {
    nlohmann::json json;
    json["txn_id"] = this->txn_id_.load();
    json["begin_ts"] = this->begin_ts_;
    json["commit_ts"] = this->commit_ts_.load();

    return json;
}

SharedPtr<FulltextIndexEntry>
FulltextIndexEntry::Deserialize(const nlohmann::json &index_def_entry_json, TableIndexEntry *table_index_entry, BufferManager *) {
    TransactionID txn_id = index_def_entry_json["txn_id"];
    TxnTimeStamp begin_ts = index_def_entry_json["begin_ts"];
    TxnTimeStamp commit_ts = index_def_entry_json["commit_ts"];

    auto fulltext_index_entry = NewFulltextIndexEntry(table_index_entry, txn_id, begin_ts);
    fulltext_index_entry->commit_ts_.store(commit_ts);
    fulltext_index_entry->txn_id_.store(txn_id);
    fulltext_index_entry->begin_ts_ = begin_ts;

    return fulltext_index_entry;
}

void FulltextIndexEntry::Cleanup() {
    // FIXME: remove the index
}

SharedPtr<String> FulltextIndexEntry::DetermineIndexDir(const String &, const String &) {
    RecoverableError(Status::NotSupport("Not implemented"));
    return nullptr;
}

const SharedPtr<String> &FulltextIndexEntry::index_dir() const { return table_index_entry_->index_dir(); }

} // namespace infinity
