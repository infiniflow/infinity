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
import txn_manager;
import buffer_manager;
import third_party;

export module db_meta;

namespace infinity {

export struct DBMeta {
public:
    explicit DBMeta(const SharedPtr<String> &data_dir, SharedPtr<String> name) : db_name_(Move(name)), data_dir_(data_dir) {}

public:
    // Reserved
    static EntryResult CreateNewEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static EntryResult DropNewEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr);

    static void DeleteNewEntry(DBMeta *db_meta, u64 txn_id, TxnManager *txn_mgr);

    static EntryResult GetEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts);

    static SharedPtr<String> ToString(DBMeta *db_meta);

    static Json Serialize(DBMeta *db_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<DBMeta> Deserialize(const Json &db_meta_json, BufferManager *buffer_mgr);

    void MergeFrom(DBMeta &other);

public:
    RWMutex rw_locker_{};
    SharedPtr<String> db_name_{};
    SharedPtr<String> data_dir_{};

    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

} // namespace infinity
