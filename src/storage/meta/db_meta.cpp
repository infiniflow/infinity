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
Tuple<DBEntry *, Status> DBMeta::CreateNewEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                TransactionID txn_id,
                                                TxnTimeStamp begin_ts,
                                                TxnManager *txn_mgr,
                                                ConflictType conflict_type) {
    auto init_db_entry = [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
        return DBEntry::NewDBEntry(this, false, this->data_dir_, this->db_name_, txn_id, begin_ts);
    };
    return db_entry_list_.AddEntry(std::move(r_lock), std::move(init_db_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

Tuple<SharedPtr<DBEntry>, Status> DBMeta::DropNewEntry(std::shared_lock<std::shared_mutex> &&r_lock,
                                                       TransactionID txn_id,
                                                       TxnTimeStamp begin_ts,
                                                       TxnManager *txn_mgr,
                                                       ConflictType conflict_type) {
    auto init_drop_entry = [&](TransactionID txn_id, TxnTimeStamp begin_ts) {
        return DBEntry::NewDBEntry(this, true, this->data_dir_, this->db_name_, txn_id, begin_ts);
    };
    return db_entry_list_.DropEntry(std::move(r_lock), std::move(init_drop_entry), txn_id, begin_ts, txn_mgr, conflict_type);
}

void DBMeta::DeleteNewEntry(TransactionID txn_id) { auto erase_list = db_entry_list_.DeleteEntry(txn_id); }

void DBMeta::CreateEntryReplay(std::function<SharedPtr<DBEntry>(TransactionID, TxnTimeStamp)> &&init_entry,
                               TransactionID txn_id,
                               TxnTimeStamp begin_ts) {
    auto [entry, status] = db_entry_list_.AddEntryReplay(std::move(init_entry), txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

void DBMeta::DropEntryReplay(std::function<SharedPtr<DBEntry>(TransactionID, TxnTimeStamp)> &&init_entry,
                             TransactionID txn_id,
                             TxnTimeStamp begin_ts) {
    auto [dropped_entry, status] = db_entry_list_.DropEntryReplay(std::move(init_entry), txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

DBEntry *DBMeta::GetEntryReplay(TransactionID txn_id, TxnTimeStamp begin_ts) {
    auto [entry, status] = db_entry_list_.GetEntryReplay(txn_id, begin_ts);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
    return entry;
}

Tuple<SharedPtr<DatabaseInfo>, Status>
DBMeta::GetDatabaseInfo(std::shared_lock<std::shared_mutex> &&r_lock, TransactionID txn_id, TxnTimeStamp begin_ts) {
    SharedPtr<DatabaseInfo> db_info = MakeShared<DatabaseInfo>();
    auto [db_entry, status] = db_entry_list_.GetEntry(std::move(r_lock), txn_id, begin_ts);
    if (!status.ok()) {
        // Error
        LOG_ERROR(fmt::format("Database: {} is invalid.", *db_name_));
        return {db_info, status};
    }
    db_info->db_name_ = db_entry->db_name_ptr();
    db_info->db_entry_dir_ = db_entry->db_entry_dir();

    return {db_info, Status::OK()};
}

SharedPtr<String> DBMeta::ToString() {
    std::shared_lock<std::shared_mutex> r_locker(this->rw_locker());
    SharedPtr<String> res = MakeShared<String>(
        fmt::format("DBMeta, data_dir: {}, db name: {}, entry count: ", *this->data_dir_, *this->db_name_, this->db_entry_list().size()));
    return res;
}

nlohmann::json DBMeta::Serialize(TxnTimeStamp max_commit_ts) {
    nlohmann::json json_res;
    Vector<DBEntry *> db_candidates;
    {
        std::shared_lock<std::shared_mutex> lck(this->rw_locker());
        json_res["data_dir"] = *this->data_dir_;
        json_res["db_name"] = *this->db_name_;
        // Need to find the full history of the entry till given timestamp. Note that GetEntry returns at most one valid entry at given timestamp.
        db_candidates.reserve(this->db_entry_list().size());
        for (auto &db_entry : this->db_entry_list()) {
            if (db_entry->entry_type_ == EntryType::kDatabase && db_entry->commit_ts_ <= max_commit_ts) {
                // Put it to candidate list
                db_candidates.push_back((DBEntry *)db_entry.get());
            }
        }
    }
    for (DBEntry *db_entry : db_candidates) {
        json_res["db_entries"].emplace_back(db_entry->Serialize(max_commit_ts));
    }
    return json_res;
}

UniquePtr<DBMeta> DBMeta::Deserialize(const nlohmann::json &db_meta_json, BufferManager *buffer_mgr) {
    SharedPtr<String> data_dir = MakeShared<String>(db_meta_json["data_dir"]);
    SharedPtr<String> db_name = MakeShared<String>(db_meta_json["db_name"]);
    UniquePtr<DBMeta> res = MakeUnique<DBMeta>(data_dir, db_name);

    if (db_meta_json.contains("db_entries")) {
        for (const auto &db_entry_json : db_meta_json["db_entries"]) {
            res->db_entry_list().emplace_back(DBEntry::Deserialize(db_entry_json, res.get(), buffer_mgr));
        }
    }
    res->db_entry_list().sort([](const SharedPtr<BaseEntry> &ent1, const SharedPtr<BaseEntry> &ent2) { return ent1->commit_ts_ > ent2->commit_ts_; });
    return res;
}

void DBMeta::Cleanup() { db_entry_list_.Cleanup(); }

bool DBMeta::PickCleanup(CleanupScanner *scanner) { return db_entry_list_.PickCleanup(scanner); }

} // namespace infinity
