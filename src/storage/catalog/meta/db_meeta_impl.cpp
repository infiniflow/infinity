// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

#include <string>

module infinity_core:db_meeta.impl;

import :db_meeta;
import :stl;
import :kv_code;
import :kv_store;
import :third_party;
import :infinity_exception;
import :meta_info;
import :default_values;
import :new_txn;
import :utility;
import :kv_utility;
import :new_txn_manager;
import :meta_cache;

namespace infinity {

DBMeeta::DBMeeta(String db_id_str, NewTxn *txn) : db_id_str_(std::move(db_id_str)), txn_(txn) {
    if (txn == nullptr) {
        UnrecoverableError("Null txn pointer");
    }
    txn_begin_ts_ = txn->BeginTS();
    kv_instance_ = txn_->kv_instance();
    meta_cache_ = txn_->txn_mgr()->storage()->meta_cache();
}

DBMeeta::DBMeeta(String db_id_str, KVInstance *kv_instance, MetaCache *meta_cache)
    : db_id_str_(std::move(db_id_str)), txn_begin_ts_{MAX_TIMESTAMP}, kv_instance_{kv_instance}, meta_cache_(meta_cache) {}

const String &DBMeeta::db_id_str() const { return db_id_str_; }

Status DBMeeta::InitSet(const String *comment) {
    if (comment) {
        String db_comment_key = GetDBTag("comment");
        Status status = kv_instance_->Put(db_comment_key, *comment);
        if (!status.ok()) {
            return status;
        }
    }

    // Create next table id;
    String next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    Status status = kv_instance_->Put(next_table_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status DBMeeta::UninitSet(UsageFlag usage_flag) {
    // called by cleanup to clean all data of the database.

    Status status;

    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str_);

    auto iter = kv_instance_->GetIterator();
    iter->Seek(db_table_prefix);
    while (iter->Valid() && iter->Key().starts_with(db_table_prefix)) {
        String table_key = iter->Key().ToString();
        status = kv_instance_->Delete(table_key);
        if (!status.ok()) {
            return status;
        }
        iter->Next();
    }

    String db_comment_key = GetDBTag("comment");
    status = kv_instance_->Delete(db_comment_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    String db_next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    status = kv_instance_->Delete(db_next_table_id_key);
    if (!status.ok()) {
        return status;
    }

    return Status::OK();
}

Status DBMeeta::GetComment(String *&comment) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!comment_) {
        SharedPtr<MetaDbCache> db_cache = meta_cache_->GetDb(db_name_, txn_begin_ts_);
        if (db_cache.get() != nullptr) {
            if (db_cache->get_comment_) {
                LOG_TRACE(fmt::format("Get db comment from db: {}", db_name_));
                comment_ = *db_cache->comment_;
                comment = &*comment_;
                return Status::OK();
            }
        }

        String comment_str;
        String db_comment_key = GetDBTag("comment");
        Status status = kv_instance_->Get(db_comment_key, comment_str);
        if (!status.ok() && status.code() != ErrorCode::kNotFound) {
            // "comment" not found is ok
            return status;
        }
        comment_ = std::move(comment_str);

        if (db_cache.get() != nullptr) {
            db_cache->get_comment_ = true;
            db_cache->comment_ = MakeShared<String>(*comment_);
        }
    }
    comment = &*comment_;
    return Status::OK();
}

Status DBMeeta::GetTableIDs(Vector<String> *&table_id_strs, Vector<String> **table_names) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!table_id_strs_ || !table_names_) {
        Status status = LoadTableIDs();
        if (!status.ok()) {
            return status;
        }
    }
    table_id_strs = &*table_id_strs_;
    if (table_names) {
        *table_names = &*table_names_;
    }
    return Status::OK();
}

Status DBMeeta::GetTableID(const String &table_name, String &table_key, String &table_id_str, TxnTimeStamp &create_table_ts) {

    u64 db_id = std::stoull(db_id_str_);
    SharedPtr<MetaTableCache> table_cache = meta_cache_->GetTable(db_id, table_name, txn_begin_ts_);
    if (table_cache.get() != nullptr) {
        if (table_cache->is_dropped_) {
            return Status::TableNotExist(table_name);
        }
        table_id_str = std::to_string(table_cache->table_id_);
        table_key = table_cache->table_key_;
        create_table_ts = table_cache->commit_ts_;
        LOG_TRACE(fmt::format("Get table meta from cache, db_id: {}, table_name: {}, table_key: {}, table_id: {}, commit_ts: {}",
                              table_cache->db_id_,
                              table_name,
                              table_cache->table_key_,
                              table_cache->table_id_,
                              table_cache->commit_ts_));
        return Status::OK();
    }

    String table_key_prefix = KeyEncode::CatalogTablePrefix(db_id_str_, table_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_key_prefix);

    Vector<Pair<String, String>> table_kvs;
    while (iter2->Valid() && iter2->Key().starts_with(table_key_prefix)) {
        table_kvs.emplace_back(iter2->Key().ToString(), iter2->Value().ToString());
        iter2->Next();
    }

    if (table_kvs.empty()) {
        return Status::TableNotExist(table_name);
    }

    SizeT max_visible_table_index = std::numeric_limits<SizeT>::max();
    TxnTimeStamp max_commit_ts = 0;
    for (SizeT i = 0; i < table_kvs.size(); ++i) {
        TxnTimeStamp commit_ts = infinity::GetTimestampFromKey(table_kvs[i].first);
        if (commit_ts <= txn_begin_ts_ && commit_ts > max_commit_ts) {
            max_commit_ts = commit_ts;
            max_visible_table_index = i;
        }
    }

    if (max_visible_table_index == std::numeric_limits<SizeT>::max()) {
        return Status::TableNotExist(table_name);
    }

    table_id_str = table_kvs[max_visible_table_index].second;
    table_key = table_kvs[max_visible_table_index].first;

    String drop_table_ts{};
    kv_instance_->Get(KeyEncode::DropTableKey(db_id_str_, table_name, table_id_str, max_commit_ts), drop_table_ts);

    String rename_table_ts{};
    kv_instance_->Get(KeyEncode::RenameTableKey(db_id_str_, table_name, table_id_str, max_commit_ts), rename_table_ts);

    if ((!drop_table_ts.empty() && std::stoull(drop_table_ts) <= txn_begin_ts_) ||
        (!rename_table_ts.empty() && std::stoull(rename_table_ts) <= txn_begin_ts_)) {

        table_cache = MakeShared<MetaTableCache>(db_id, table_name, std::stoull(table_id_str), max_commit_ts, table_key, true);
        meta_cache_->Put({table_cache});

        LOG_TRACE(fmt::format("Save dropped table meta from cache, db_id: {}, table_name: {}, table_key: {}, table_id: {}, commit_ts: {}",
                              table_cache->db_id_,
                              table_name,
                              table_key,
                              table_cache->table_id_,
                              table_cache->commit_ts_));

        return Status::TableNotExist(table_name);
    }

    table_cache = MakeShared<MetaTableCache>(db_id, table_name, std::stoull(table_id_str), max_commit_ts, table_key, false);
    meta_cache_->Put({table_cache});

    LOG_TRACE(fmt::format("Save created table meta from cache, db_id: {}, table_name: {}, table_key: {}, table_id: {}, commit_ts: {}",
                          table_cache->db_id_,
                          table_name,
                          table_key,
                          table_cache->table_id_,
                          table_cache->commit_ts_));

    create_table_ts = max_commit_ts;
    return Status::OK();
}

Tuple<SharedPtr<DatabaseInfo>, Status> DBMeeta::GetDatabaseInfo() {
    Status status;

    String *db_comment = nullptr;
    status = this->GetComment(db_comment);
    if (!status.ok()) {
        return {nullptr, status};
    }

    SharedPtr<DatabaseInfo> db_info = MakeShared<DatabaseInfo>();

    db_info->db_comment_ = MakeShared<String>(*db_comment);
    db_info->db_entry_dir_ = MakeShared<String>(fmt::format("db_{}", db_id_str_));

    return {db_info, Status::OK()};
}

Tuple<String, Status> DBMeeta::GetNextTableID() {
    String next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    String next_table_id_str;
    Status status = kv_instance_->Get(next_table_id_key, next_table_id_str);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("Fail to get next table id from kv store, key: {}, cause: {}", next_table_id_key, status.message()));
    }
    u64 next_table_id = std::stoull(next_table_id_str);
    ++next_table_id;
    String new_next_table_id_str = std::to_string(next_table_id);
    status = kv_instance_->Put(next_table_id_key, new_next_table_id_str);
    if (!status.ok()) {
        return {"", status};
    }
    return {next_table_id_str, Status::OK()};
}

Status DBMeeta::LoadTableIDs() {
    table_id_strs_ = Vector<String>();
    table_names_ = Vector<String>();

    Map<String, Vector<Pair<String, String>>> table_kvs_map;
    String db_table_prefix = KeyEncode::CatalogDbTablePrefix(db_id_str_);

    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(db_table_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(db_table_prefix)) {
        String key_str = iter2->Key().ToString();
        size_t start = db_table_prefix.size();
        size_t end = key_str.find('|', start);
        String table_id_str = iter2->Value().ToString();
        String table_name = key_str.substr(start, end - start);
        table_kvs_map[table_name].emplace_back(key_str, table_id_str);
        iter2->Next();
    }

    for (const auto &[table_name, table_kv] : table_kvs_map) {
        SizeT max_visible_table_index = std::numeric_limits<SizeT>::max();
        TxnTimeStamp max_commit_ts = 0;
        for (SizeT i = 0; i < table_kv.size(); ++i) {
            String commit_ts_str = GetLastPartOfKey(table_kv[i].first, '|');
            TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
            if (commit_ts <= txn_begin_ts_ && commit_ts > max_commit_ts) {
                max_commit_ts = commit_ts;
                max_visible_table_index = i;
            }
        }

        if (max_visible_table_index != std::numeric_limits<SizeT>::max()) {
            const String &table_id_ref = table_kv[max_visible_table_index].second;
            String drop_table_ts{};
            kv_instance_->Get(KeyEncode::DropTableKey(db_id_str_, table_name, table_id_ref, max_commit_ts), drop_table_ts);

            String rename_table_ts{};
            kv_instance_->Get(KeyEncode::RenameTableKey(db_id_str_, table_name, table_id_ref, max_commit_ts), rename_table_ts);

            if ((drop_table_ts.empty() || std::stoull(drop_table_ts) > txn_begin_ts_) &&
                (rename_table_ts.empty() || std::stoull(rename_table_ts) > txn_begin_ts_)) {
                table_id_strs_->emplace_back(table_id_ref);
                table_names_->emplace_back(table_name);
            }
        }
    }

    return Status::OK();
}

String DBMeeta::GetDBTag(const String &tag) const { return KeyEncode::CatalogDbTagKey(db_id_str_, tag); }

Status DBMeeta::SetNextTableID(const String &table_id_str) {
    String next_table_id_key = GetDBTag(NEXT_TABLE_ID.data());
    Status status = kv_instance_->Put(next_table_id_key, table_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}
} // namespace infinity