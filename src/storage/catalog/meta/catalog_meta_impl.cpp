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

#include <vector>

module infinity_core:catalog_meta.impl;

import :catalog_meta;
import :kv_code;
import :kv_store;
import :infinity_exception;
import :third_party;
import :kv_utility;
import :default_values;
import :new_txn;
import :new_txn_manager;
import :storage;
import :meta_cache;

namespace infinity {

CatalogMeta::CatalogMeta(NewTxn *txn) : txn_(txn) {
    if (txn == nullptr) {
        UnrecoverableError("Null txn pointer");
    }
    read_ts_ = txn->BeginTS();
    kv_instance_ = txn_->kv_instance();
    meta_cache_ = txn_->txn_mgr()->storage()->meta_cache();
}

CatalogMeta::CatalogMeta(KVInstance *kv_instance, MetaCache *meta_cache)
    : read_ts_{MAX_TIMESTAMP}, kv_instance_{kv_instance}, meta_cache_(meta_cache) {}

Status CatalogMeta::GetDBID(const String &db_name, String &db_key, String &db_id_str, TxnTimeStamp &create_ts) {

//    SharedPtr<MetaDbCache> db_cache = meta_cache_->GetDb(db_name, this->read_ts_);
//    if (db_cache.get() != nullptr) {
//        if (db_cache->is_dropped_) {
//            return Status::DBNotExist(db_name);
//        }
//        db_id_str = std::to_string(db_cache->db_id_);
//        db_key = db_cache->db_key_;
//        create_ts = db_cache->commit_ts_;
//        LOG_TRACE(fmt::format("Get db meta from cache, db: {}, db_id: {}, commit_ts: {}", db_name, db_cache->db_id_, create_ts));
//        return Status::OK();
//    }

    String db_key_prefix = KeyEncode::CatalogDbPrefix(db_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(db_key_prefix);

    Vector<Pair<String, String>> db_kvs;
    while (iter2->Valid() && iter2->Key().starts_with(db_key_prefix)) {
        db_kvs.emplace_back(iter2->Key().ToString(), iter2->Value().ToString());
        iter2->Next();
    }

    if (db_kvs.size() == 0) {
        return Status::DBNotExist(db_name);
    }

    SizeT max_visible_db_index = std::numeric_limits<SizeT>::max();
    TxnTimeStamp max_commit_ts = 0;
    for (SizeT i = 0; i < db_kvs.size(); ++i) {
        String commit_ts_str = GetLastPartOfKey(db_kvs[i].first, '|');
        TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
        if (commit_ts <= read_ts_ && commit_ts > max_commit_ts) {
            max_commit_ts = commit_ts;
            max_visible_db_index = i;
        }
    }

    if (max_visible_db_index == std::numeric_limits<SizeT>::max()) {
        return Status::DBNotExist(db_name);
    }

    db_key = db_kvs[max_visible_db_index].first;
    db_id_str = db_kvs[max_visible_db_index].second;

    String drop_db_ts{};
    kv_instance_->Get(KeyEncode::DropDBKey(db_name, max_commit_ts, db_id_str), drop_db_ts);

    if (!drop_db_ts.empty() && std::stoull(drop_db_ts) <= read_ts_) {
//        db_cache = MakeShared<MetaDbCache>(db_name, std::stoull(db_id_str), max_commit_ts, db_key, true);
//        meta_cache_->Operate({db_cache}, nullptr);
        return Status::DBNotExist(db_name);
    }

//    db_cache = MakeShared<MetaDbCache>(db_name, std::stoull(db_id_str), max_commit_ts, db_key, false);
//    meta_cache_->Operate({db_cache}, nullptr);

//    LOG_TRACE(fmt::format("Save created db meta from cache, db_id: {}, db_key: {}, commit_ts: {}",
//                          db_cache->db_id_,
//                          db_name,
//                          db_key,
//                          db_cache->commit_ts_));

    create_ts = max_commit_ts;
    return Status::OK();
}

Status CatalogMeta::GetDBIDs(Vector<String> *&db_id_strs, Vector<String> **db_names) {
    Status status = LoadDBIDs();
    if (!status.ok()) {
        return status;
    }

    db_id_strs = &*db_id_strs_;
    if (db_names) {
        *db_names = &*db_names_;
    }
    return Status::OK();
}

Status CatalogMeta::LoadDBIDs() {
    Vector<String> db_id_strs;
    Vector<String> db_names;
    Map<String, Vector<Pair<String, String>>> db_kvs_map;

    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(KeyEncode::kCatalogDbHeader);
    while (iter2->Valid() && iter2->Key().starts_with(KeyEncode::kCatalogDbHeader)) {
        String key_str = iter2->Key().ToString();
        String db_id = iter2->Value().ToString();
        size_t start = KeyEncode::kCatalogDbHeader.size();
        size_t end = key_str.find('|', start);
        String db_name = key_str.substr(start, end - start);
        db_kvs_map[db_name].emplace_back(key_str, db_id);
        iter2->Next();
    }

    for (const auto &[db_name, db_kv] : db_kvs_map) {
        SizeT max_visible_db_index = std::numeric_limits<SizeT>::max();
        TxnTimeStamp max_commit_ts = 0;
        for (SizeT i = 0; i < db_kv.size(); ++i) {
            String commit_ts_str = GetLastPartOfKey(db_kv[i].first, '|');
            TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
            if (commit_ts <= read_ts_ && commit_ts > max_commit_ts) {
                max_commit_ts = commit_ts;
                max_visible_db_index = i;
            }
        }

        if (max_visible_db_index != std::numeric_limits<SizeT>::max()) {
            String drop_db_ts{};
            const String &db_id = db_kv[max_visible_db_index].second;
            kv_instance_->Get(KeyEncode::DropDBKey(db_name, max_commit_ts, db_id), drop_db_ts);

            if (drop_db_ts.empty() || std::stoull(drop_db_ts) > read_ts_) {
                db_id_strs.push_back(db_id);
                db_names.push_back(db_name);
            }
        }
    }

    db_id_strs_ = std::move(db_id_strs);
    db_names_ = std::move(db_names);

    return Status::OK();
}

} // namespace infinity