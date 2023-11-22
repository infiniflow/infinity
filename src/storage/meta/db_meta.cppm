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
import status;

export module db_meta;

namespace infinity {

export struct DBMeta {
public:
    explicit DBMeta(const SharedPtr<String> &data_dir, SharedPtr<String> name) : db_name_(Move(name)), data_dir_(data_dir) {}

public:
    // Reserved
    static Status CreateNewEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, BaseEntry *&db_entry);

    static Status DropNewEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, BaseEntry *&db_entry);

    static void DeleteNewEntry(DBMeta *db_meta, u64 txn_id, TxnManager *txn_mgr);

    static Status GetEntry(DBMeta *db_meta, u64 txn_id, TxnTimeStamp begin_ts, BaseEntry *&new_db_entry);

    static SharedPtr<String> ToString(DBMeta *db_meta);

    static Json Serialize(DBMeta *db_meta, TxnTimeStamp max_commit_ts, bool is_full_checkpoint);

    static UniquePtr<DBMeta> Deserialize(const Json &db_meta_json, BufferManager *buffer_mgr);

    // Used in initialization phase
    static void AddEntry(DBMeta *db_meta, UniquePtr<BaseEntry> db_entry);

    void MergeFrom(DBMeta &other);

    SharedPtr<String> db_name() const {
        return db_name_;
    }

    SharedPtr<String> data_dir() const {
        return data_dir_;
    }

    // Thread-unsafe
    List<UniquePtr<BaseEntry>>& entry_list() {
        return entry_list_;
    }

private:

    SharedPtr<String> db_name_{};
    SharedPtr<String> data_dir_{};

    RWMutex rw_locker_{};
    // Ordered by commit_ts from latest to oldest.
    List<UniquePtr<BaseEntry>> entry_list_{};
};

} // namespace infinity
