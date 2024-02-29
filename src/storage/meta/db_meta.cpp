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

module db_meta;

import stl;
import txn_manager;
import logger;
import third_party;
import buffer_manager;
import txn_state;
import status;
import catalog_delta_entry;
import infinity_exception;
import base_entry;
import local_file_system;

namespace infinity {

// Use by default
UniquePtr<DBMeta> DBMeta::NewDBMeta(const SharedPtr<String> &data_dir, const SharedPtr<String> &db_name) {
    auto db_meta = MakeUnique<DBMeta>(data_dir, db_name);
    return db_meta;
}

// TODO: Use Txn* txn as parma instead of TransactionID txn_id and TxnManager *txn_mgr
Tuple<DBEntry *, Status> DBMeta::CreateNewEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(false, this->data_dir_, this->db_name_, txn_id, begin_ts);
    auto [db_entry_ptr, status] = db_entry_list_.AddEntry(db_entry, txn_id, begin_ts, txn_mgr, conflict_type);
    if (status.code() == ErrorCode::kDuplicateDatabaseName) {
        return {db_entry_ptr, Status::DuplicateDatabase(*this->db_name_)};
    }
    return {db_entry_ptr, status};
}

Tuple<DBEntry *, Status> DBMeta::DropNewEntry(TransactionID txn_id, TxnTimeStamp begin_ts, TxnManager *txn_mgr, ConflictType conflict_type) {
    SharedPtr<DBEntry> db_entry = DBEntry::NewDBEntry(true, this->data_dir_, this->db_name_, txn_id, begin_ts);
    auto [db_entry_ptr, status] = db_entry_list_.DropEntry(db_entry, txn_id, begin_ts, txn_mgr, conflict_type);
    if (status.code() == ErrorCode::kDBNotExist) {
        return {db_entry_ptr, Status::DBNotExist(*this->db_name_)};
    }
    return {db_entry_ptr, status};
}

Tuple<DBEntry *, Status> DBMeta::GetEntry(TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [db_entry_ptr, status] = db_entry_list_.GetEntry(txn_id, begin_ts);
    if (status.code() == ErrorCode::kDBNotExist) {
        return {db_entry_ptr, Status::DBNotExist(*this->db_name_)};
    }
    return {db_entry_ptr, status};
}

void DBMeta::DeleteNewEntry(TransactionID txn_id) { db_entry_list_.DeleteEntry(txn_id); }

SharedPtr<String> DBMeta::ToString() {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker());
    SharedPtr<String> res = MakeShared<String>(
        fmt::format("DBMeta, data_dir: {}, db name: {}, entry count: ", *this->data_dir_, *this->db_name_, this->db_entry_list().size()));
    return res;
}

nlohmann::json DBMeta::Serialize(TxnTimeStamp max_commit_ts, bool is_full_checkpoint) {
    nlohmann::json json_res;
    Vector<DBEntry *> db_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["data_dir"] = *this->data_dir_;
        json_res["db_name"] = *this->db_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        db_candidates.reserve(this->db_entry_list().size());
        for (auto &db_entry : this->db_entry_list()) {
            if (db_entry->entry_type_ == EntryType::kDatabase) {
                // Put it to candidate list
                db_candidates.push_back((DBEntry *)db_entry.get());
            }
        }
    }
    for (DBEntry *db_entry : db_candidates) {
        json_res["db_entries"].emplace_back(db_entry->Serialize(max_commit_ts, is_full_checkpoint));
    }
    return json_res;
}

UniquePtr<DBMeta> DBMeta::Deserialize(const nlohmann::json &db_meta_json, BufferManager *buffer_mgr) {
    SharedPtr<String> data_dir = MakeShared<String>(db_meta_json["data_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_meta_json["db_name"]);
    UniquePtr<DBMeta> res = MakeUnique<DBMeta>(data_dir, db_name);

    if (db_meta_json.contains("db_entries")) {
        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            res->db_entry_list().emplace_back(DBEntry::Deserialize(db_entry_json, buffer_mgr));
        }
    }
    res->db_entry_list().sort([](const SharedPtr<BaseEntry> &ent1, const SharedPtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    return res;
}

void DBMeta::MergeFrom(DBMeta &other) {
    // No locking here since only the load stage needs MergeFrom.
    if (!IsEqual(*this->db_name_, *other.db_name_)) {
        UnrecoverableError("DBMeta::MergeFrom requires db_name_ match");
    }
    if (!IsEqual(*this->data_dir_, *other.data_dir_)) {
        UnrecoverableError("DBMeta::MergeFrom requires db_dir_ match");
    }
    this->db_entry_list_.MergeWith(db_entry_list_);
}

void DBMeta::Cleanup() && { std::move(db_entry_list_).Cleanup(); }

bool DBMeta::PickCleanup(CleanupScanner *scanner) { return db_entry_list_.PickCleanup(scanner); }

} // namespace infinity
