// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:table_meta.impl;

import :table_meta;
import :status;
import :kv_code;
import :kv_store;
import :default_values;
import :logger;
import :table_def;
import :infinity_exception;
import :meta_info;
import :infinity_context;
import :storage;
import :new_catalog;
import :table_index_meta;
import :segment_meta;
import :block_meta;
import :kv_utility;
import :column_index_reader;
import :new_txn;
import :meta_cache;
import :new_txn_manager;

import std.compat;
import third_party;
import row_id;
import column_def;
import create_index_info;

namespace infinity {

TableMeta::TableMeta(const std::string &db_id_str,
                     const std::string &table_id_str,
                     const std::string &table_name,
                     KVInstance *kv_instance,
                     TxnTimeStamp begin_ts,
                     TxnTimeStamp commit_ts,
                     MetaCache *meta_cache)
    : begin_ts_(begin_ts), commit_ts_(commit_ts), kv_instance_(kv_instance), meta_cache_(meta_cache), db_id_str_(db_id_str),
      table_id_str_(table_id_str), table_name_(table_name) {
    db_id_ = std::stoull(db_id_str);
    table_id_ = std::stoull(table_id_str);
}

TableMeta::TableMeta(const std::string &db_id_str, const std::string &table_id_str, const std::string &table_name, NewTxn *txn)
    : txn_(txn), db_id_str_(db_id_str), table_id_str_(table_id_str), table_name_(table_name) {
    if (txn == nullptr) {
        UnrecoverableError("Null txn pointer");
    }
    begin_ts_ = txn->BeginTS();
    commit_ts_ = txn->CommitTS();
    kv_instance_ = txn->kv_instance();
    meta_cache_ = txn->txn_mgr()->storage()->meta_cache();
    db_id_ = std::stoull(db_id_str);
    table_id_ = std::stoull(table_id_str);
}

Status TableMeta::GetComment(TableInfo &table_info) {
    if (!comment_) {
        std::string table_comment_key = GetTableTag("comment");
        std::string table_comment;
        Status status = kv_instance_->Get(table_comment_key, table_comment);
        if (!status.ok() && status.code() != ErrorCode::kNotFound) { // not found is ok
            LOG_ERROR(fmt::format("Fail to get table comment from kv store, key: {}, cause: {}", table_comment_key, status.message()));
            return status;
        }
        comment_ = std::move(table_comment);
    }
    table_info.table_comment_ = std::make_shared<std::string>(*comment_);
    return Status::OK();
}

Status TableMeta::GetIndexIDs(std::vector<std::string> *&index_id_strs, std::vector<std::string> **index_names) {
    if (!index_id_strs_ || !index_name_strs_) {
        Status status = LoadIndexIDs();
        if (!status.ok()) {
            return status;
        }
    }
    index_id_strs = &index_id_strs_.value();
    if (index_names) {
        *index_names = &index_name_strs_.value();
    }
    return Status::OK();
}

Status TableMeta::GetIndexID(const std::string &index_name, std::string &index_key, std::string &index_id_str, TxnTimeStamp &create_index_ts) {

    if (txn_ != nullptr and txn_->readonly()) {
        std::shared_ptr<MetaIndexCache> index_cache = meta_cache_->GetIndex(db_id_, table_id_, index_name, begin_ts_);
        if (index_cache.get() != nullptr) {
            if (index_cache->is_dropped()) {
                return Status::IndexNotExist(index_name);
            }
            index_key = index_cache->index_key();
            index_id_str = std::to_string(index_cache->index_id());
            create_index_ts = index_cache->commit_ts();
            LOG_TRACE(
                fmt::format("Get table index meta from cache, db_id: {}, table_id: {}, index_name: {}, index_id{}, index_key: {}, commit_ts: {}",
                            db_id_,
                            table_id_,
                            index_name,
                            index_id_str,
                            index_key,
                            create_index_ts));
            return Status::OK();
        }
    }

    std::string index_key_prefix = KeyEncode::CatalogIndexPrefix(db_id_str_, table_id_str_, index_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(index_key_prefix);

    std::vector<std::pair<std::string, std::string>> index_kvs;
    while (iter2->Valid() && iter2->Key().starts_with(index_key_prefix)) {
        index_kvs.emplace_back(iter2->Key().ToString(), iter2->Value().ToString());
        iter2->Next();
    }

    if (index_kvs.empty()) {
        return Status::IndexNotExist(index_name);
    }

    size_t max_visible_index_index = std::numeric_limits<size_t>::max();
    TxnTimeStamp max_commit_ts = 0;
    for (size_t i = 0; i < index_kvs.size(); ++i) {
        TxnTimeStamp commit_ts = infinity::GetTimestampFromKey(index_kvs[i].first);
        if ((commit_ts <= begin_ts_ || (txn_ != nullptr && txn_->IsReplay() && commit_ts == commit_ts_)) && commit_ts > max_commit_ts) {
            max_commit_ts = commit_ts;
            max_visible_index_index = i;
        }
    }

    if (max_visible_index_index == std::numeric_limits<size_t>::max()) {
        return Status::IndexNotExist(index_name);
    }

    index_id_str = index_kvs[max_visible_index_index].second;
    index_key = index_kvs[max_visible_index_index].first;

    std::string drop_index_ts{};
    kv_instance_->Get(KeyEncode::DropTableIndexKey(db_id_str_, table_id_str_, index_name, max_commit_ts, index_id_str), drop_index_ts);

    if (!drop_index_ts.empty() && std::stoull(drop_index_ts) <= begin_ts_) {
        return Status::IndexNotExist(index_name);
    }

    if (txn_ != nullptr and txn_->readonly()) {
        txn_->AddMetaCache(std::make_shared<
                           MetaIndexCache>(db_id_, table_id_, index_name, std::stoull(index_id_str), max_commit_ts, index_key, false, txn_->TxnID()));
    }

    create_index_ts = max_commit_ts;
    return Status::OK();
}

std::tuple<std::shared_ptr<ColumnDef>, Status> TableMeta::GetColumnDefByColumnName(const std::string &column_name, size_t *column_idx_ptr) {
    std::unique_lock<std::mutex> lock(mtx_);
    if (column_defs_ == nullptr) {
        Status status = LoadColumnDefs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    for (size_t column_idx = 0; column_idx < column_defs_->size(); ++column_idx) {
        if ((*column_defs_)[column_idx]->name() == column_name) {
            if (column_idx_ptr) {
                *column_idx_ptr = column_idx;
            }
            return {(*column_defs_)[column_idx], Status::OK()};
        }
    }
    return {nullptr, Status::ColumnNotExist(column_name)};
}

Status TableMeta::RemoveSegmentIDs1(const std::vector<SegmentID> &segment_ids) {
    std::unordered_set<SegmentID> segment_ids_set(segment_ids.begin(), segment_ids.end());

    std::string segment_id_prefix = KeyEncode::CatalogTableSegmentKeyPrefix(db_id_str_, table_id_str_);
    auto kv_iter = kv_instance_->GetIterator();
    kv_iter->Seek(segment_id_prefix);
    std::vector<std::string> delete_keys;
    while (kv_iter->Valid() && kv_iter->Key().starts_with(segment_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(kv_iter->Value().ToString());
        SegmentID segment_id = std::stoull(kv_iter->Key().ToString().substr(segment_id_prefix.size()));
        if (segment_ids_set.contains(segment_id)) {
            if (commit_ts > begin_ts_ and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                UnrecoverableError(
                    fmt::format("Segment id: {} is not allowed to be removed. commit_ts: {}, begin_ts: {}", segment_id, commit_ts, begin_ts_));
            }
            // the key is committed before the txn or the key isn't committed
            delete_keys.push_back(kv_iter->Key().ToString());
        }
        kv_iter->Next();
    }
    for (const std::string &key : delete_keys) {
        Status status = kv_instance_->Delete(key);
        if (!status.ok()) {
            return status;
        }
    }

    if (segment_ids1_ != nullptr) {
        for (auto iter = segment_ids1_->begin(); iter != segment_ids1_->end();) {
            if (segment_ids_set.contains(*iter)) {
                iter = segment_ids1_->erase(iter);
            } else {
                ++iter;
            }
        }
    }

    return Status::OK();
}

std::pair<SegmentID, Status> TableMeta::AddSegmentID1(TxnTimeStamp commit_ts) {
    Status status;

    SegmentID segment_id = 0;
    {
        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
        if (!status.ok()) {
            return {0, status};
        }
        segment_id = segment_ids_ptr->empty() ? 0 : segment_ids_ptr->back() + 1;
        segment_ids1_->push_back(segment_id);
    }

    std::string segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    std::string commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance_->Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {segment_id, Status::OK()};
}

Status TableMeta::AddSegmentWithID(TxnTimeStamp commit_ts, SegmentID segment_id) {
    const std::string segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    const std::string commit_ts_str = fmt::format("{}", commit_ts);
    return kv_instance_->Put(segment_id_key, commit_ts_str);
}

Status TableMeta::CommitSegment(SegmentID segment_id, TxnTimeStamp commit_ts) {
    const std::string segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    const std::string commit_ts_str = fmt::format("{}", commit_ts);
    Status status = kv_instance_->Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeta::InitSet(std::shared_ptr<TableDef> table_def) {
    Status status;

    // Create table comment;
    if (table_def->table_comment() != nullptr and !table_def->table_comment()->empty()) {
        const std::string &table_comment = *table_def->table_comment();
        const std::string table_comment_key = GetTableTag("comment");
        status = kv_instance_->Put(table_comment_key, table_comment);
        if (!status.ok()) {
            return status;
        }
    }

    // Create table column id;
    size_t column_size = table_def->column_count();
    status = SetNextColumnID(column_size);
    if (!status.ok()) {
        return status;
    }

    // Create the next segment id;
    const std::string table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_->Put(table_latest_segment_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // Create next index id;
    const std::string next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    status = kv_instance_->Put(next_index_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // {
    //     // Create segment ids
    //     std::string table_segment_ids_key = GetTableTag("segment_ids");
    //     std::string table_segment_ids_str = nlohmann::json::array().dump();
    //     Status status = kv_instance_.Put(table_segment_ids_key, table_segment_ids_str);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }

    for (const auto &column : table_def->columns()) {
        const std::string column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column->name(), commit_ts_);
        status = kv_instance_->Put(column_key, column->ToJson().dump());
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status TableMeta::LoadSet() {
    std::vector<std::string> *index_id_strs_ptr = nullptr;
    std::vector<std::string> *index_name_strs_ptr = nullptr;
    Status status = this->GetIndexIDs(index_id_strs_ptr, &index_name_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    LOG_DEBUG(fmt::format("LoadSet for table: {} with number of indexes: {}", table_id_str_, index_id_strs_ptr->size()));

    size_t index_count = index_id_strs_ptr->size();
    for (size_t idx = 0; idx < index_count; ++idx) {
        const std::string &index_id_str = index_id_strs_ptr->at(idx);
        const std::string &index_name_str = index_name_strs_ptr->at(idx);
        TableIndexMeta table_index_meta(index_id_str, index_name_str, *this);
        auto [index_def, status] = table_index_meta.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
        if (index_def->index_type_ == IndexType::kFullText) {
            LOG_DEBUG(fmt::format("Add ft index cache for table: {}, commit ts: {}", table_id_str_, commit_ts_));
            auto ft_index_cache = std::make_shared<TableIndexReaderCache>(db_id_str_, table_id_str_, table_name_);
            status = AddFtIndexCache(std::move(ft_index_cache));
            if (!status.ok()) {
                return status;
            }
            break;
        }
    }
    return Status::OK();
}

Status TableMeta::UninitSet(UsageFlag usage_flag) {

    // Create next index id;
    const std::string next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    Status status = kv_instance_->Delete(next_index_id_key);
    if (!status.ok()) {
        return status;
    }

    // delete table segment id;
    const std::string table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_->Delete(table_latest_segment_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table column id;
    const std::string table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    status = kv_instance_->Delete(table_latest_column_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    const std::string table_comment_key = GetTableTag("comment");
    status = kv_instance_->Delete(table_comment_key);
    if (!status.ok()) {
        return status;
    }

    // std::string segment_ids_key = GetTableTag("segment_ids");
    // status = kv_instance_.Delete(segment_ids_key);
    // if (!status.ok()) {
    //     return status;
    // }
    {
        std::vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
        status = RemoveSegmentIDs1(*segment_ids_ptr);
        if (!status.ok()) {
            return status;
        }
    }

    const std::string unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    status = kv_instance_->Delete(unsealed_seg_id_key);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kNotFound) {
            return status;
        }
    }

    const std::string table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);

    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        const std::string table_column_key = iter2->Key().ToString();
        status = kv_instance_->Delete(table_column_key);
        if (!status.ok()) {
            return status;
        }
        iter2->Next();
    }

    const std::string index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(index_prefix);

    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        const std::string index_key = iter->Key().ToString();
        status = kv_instance_->Delete(index_key);
        if (!status.ok()) {
            return status;
        }
        iter->Next();
    }

    if (usage_flag == UsageFlag::kOther) {
        status = RemoveFtIndexCache();
        if (!status.ok()) {
            if (status.code() != ErrorCode::kCatalogError) {
                return status;
            }
        }
    }

    return Status::OK();
}

Status TableMeta::GetTableInfo(TableInfo &table_info) {
    Status status;

    table_info.table_full_dir_ =
        std::make_shared<std::string>(fmt::format("{}/db_{}/tbl_{}", InfinityContext::instance().config()->DataDir(), db_id_str_, table_id_str_));

    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = this->GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    table_info.column_defs_ = *column_defs;
    std::sort(table_info.column_defs_.begin(),
              table_info.column_defs_.end(),
              [](const std::shared_ptr<ColumnDef> &a, const std::shared_ptr<ColumnDef> &b) { return a->id_ < b->id_; });
    table_info.column_count_ = table_info.column_defs_.size();

    table_info.db_id_ = db_id_str_;
    table_info.table_id_ = table_id_str_;

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    table_info.segment_count_ = segment_ids_ptr->size();
    return Status::OK();
}

Status TableMeta::GetTableDetail(TableDetail &table_detail) {
    TableInfo table_info;
    Status status = GetTableInfo(table_info);
    if (!status.ok()) {
        return status;
    }
    status = GetComment(table_info);
    if (!status.ok()) {
        return status;
    }
    table_detail.db_name_ = std::make_shared<std::string>(db_name_);
    table_detail.table_name_ = std::make_shared<std::string>(table_name_);
    table_detail.table_id_ = std::make_shared<std::string>(table_id_str_);
    table_detail.table_comment_ = table_info.table_comment_;
    table_detail.column_count_ = table_info.column_count_;

    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    size_t block_count = 0;
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, *this);
        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return status;
        }
        block_count += block_ids_ptr->size();
    }
    table_detail.block_count_ = block_count;
    table_detail.segment_count_ = segment_ids_ptr->size();

    return Status::OK();
}

Status TableMeta::AddColumn(const ColumnDef &column_def) {
    std::string column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_def.name(), commit_ts_);
    std::string column_name_value;
    Status status = kv_instance_->Put(column_key, column_def.ToJson().dump());
    return status;
}

Status TableMeta::AddFtIndexCache(std::shared_ptr<TableIndexReaderCache> ft_index_cache) {
    std::string ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->AddFtIndexCache(std::move(ft_index_cache_key), std::move(ft_index_cache));
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeta::GetFtIndexCache(std::shared_ptr<TableIndexReaderCache> &ft_index_cache) {
    std::string ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();

    ft_index_cache = std::make_shared<TableIndexReaderCache>(db_id_str_, table_id_str_, table_name_);
    Status status = new_catalog->GetFtIndexCache(ft_index_cache_key, ft_index_cache);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeta::RemoveFtIndexCache() {
    std::string ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->DropFtIndexCacheByFtIndexCacheKey(ft_index_cache_key);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeta::InvalidateFtIndexCache() {
    std::string ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    std::shared_ptr<TableIndexReaderCache> ft_index_cache = std::make_shared<TableIndexReaderCache>(db_id_str_, table_id_str_, table_name_);
    Status status = new_catalog->GetFtIndexCache(ft_index_cache_key, ft_index_cache);
    if (!status.ok()) {
        if (status.code() == ErrorCode::kCatalogError) {
            return Status::OK();
        }
        return status;
    }
    ft_index_cache->Invalidate();
    return Status::OK();
}

Status TableMeta::GetNextColumnID(ColumnID &next_column_id) {
    if (!next_column_id_) {
        Status status = LoadNextColumnID();
        if (!status.ok()) {
            return status;
        }
    }
    next_column_id = *next_column_id_;
    return Status::OK();
}

Status TableMeta::SetNextColumnID(ColumnID next_column_id) {
    std::string table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    std::string next_column_id_str = fmt::format("{}", next_column_id);
    Status status = kv_instance_->Put(table_latest_column_id_key, next_column_id_str);
    if (!status.ok()) {
        return status;
    }
    next_column_id_ = next_column_id;
    return Status::OK();
}

Status TableMeta::LoadColumnDefs() {

    std::shared_ptr<MetaTableCache> table_cache{};
    if (column_defs_ == nullptr) {
        if (txn_ != nullptr and txn_->readonly()) {
            table_cache = meta_cache_->GetTable(db_id_, table_name_, begin_ts_);
            if (table_cache.get() != nullptr) {
                column_defs_ = table_cache->get_columns();
                if (column_defs_ != nullptr) {
                    return Status::OK();
                }
            }
        }
    }

    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs = std::make_shared<std::vector<std::shared_ptr<ColumnDef>>>();
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> column_kvs_map;
    std::string column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);

    auto iter = kv_instance_->GetIterator();
    iter->Seek(column_prefix);
    while (iter->Valid() && iter->Key().starts_with(column_prefix)) {
        std::string column_key = iter->Key().ToString();
        std::string column_value = iter->Value().ToString();
        size_t start = column_prefix.size();
        size_t end = column_key.find('|', start);
        std::string column_name = column_key.substr(start, end - start);
        column_kvs_map[column_name].emplace_back(column_key, column_value);
        iter->Next();
    }

    for (const auto &[column_name, column_kv] : column_kvs_map) {
        size_t max_visible_column_index = std::numeric_limits<size_t>::max();
        TxnTimeStamp max_commit_ts = 0;
        for (size_t i = 0; i < column_kv.size(); ++i) {
            std::string commit_ts_str = GetLastPartOfKey(column_kv[i].first, '|');
            TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
            if ((commit_ts <= begin_ts_ || commit_ts == commit_ts_) && commit_ts > max_commit_ts) {
                {
                    max_commit_ts = commit_ts;
                    max_visible_column_index = i;
                }
            }
        }

        if (max_visible_column_index != std::numeric_limits<size_t>::max()) {
            std::string drop_column_ts{};
            const std::string &column_value = column_kv[max_visible_column_index].second;
            kv_instance_->Get(KeyEncode::DropTableColumnKey(db_id_str_, table_id_str_, column_name, max_commit_ts), drop_column_ts);

            if (drop_column_ts.empty() || std::stoull(drop_column_ts) > begin_ts_) {
                auto column_def = ColumnDef::FromJson(column_value);
                column_defs->push_back(column_def);
            }
        }
    }
    std::sort(column_defs->begin(), column_defs->end(), [](const std::shared_ptr<ColumnDef> &a, const std::shared_ptr<ColumnDef> &b) {
        return a->id_ < b->id_;
    });
    column_defs_ = std::move(column_defs);
    if (table_cache.get() != nullptr && txn_ != nullptr && txn_->readonly()) {
        txn_->AddCacheInfo(std::make_shared<TableCacheColumnInfo>(db_id_, table_name_, begin_ts_, column_defs_));
    }
    return Status::OK();
}

Status TableMeta::LoadIndexIDs() {

    std::shared_ptr<MetaTableCache> table_cache{};
    if (index_id_strs_ == std::nullopt or index_name_strs_ == std::nullopt) {
        if (txn_ != nullptr and txn_->readonly()) {
            table_cache = meta_cache_->GetTable(db_id_, table_name_, begin_ts_);
            if (table_cache.get() != nullptr) {
                auto [index_ids_ptr, index_names_ptr] = table_cache->get_index_ids();
                if (index_ids_ptr != nullptr and index_names_ptr != nullptr) {
                    index_id_strs_ = *index_ids_ptr;
                    index_name_strs_ = *index_names_ptr;
                    return Status::OK();
                }
            }
        }
    }

    std::shared_ptr<std::vector<std::string>> index_ids_ptr = std::make_shared<std::vector<std::string>>();
    std::shared_ptr<std::vector<std::string>> index_names_ptr = std::make_shared<std::vector<std::string>>();

    std::vector<std::string> &index_id_strs = *index_ids_ptr;
    std::vector<std::string> &index_names = *index_names_ptr;
    std::map<std::string, std::vector<std::pair<std::string, std::string>>> index_kvs_map;
    std::string index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);

    auto iter = kv_instance_->GetIterator();
    iter->Seek(index_prefix);
    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        std::string index_key = iter->Key().ToString();
        std::string index_id = iter->Value().ToString();
        size_t start = index_prefix.size();
        size_t end = index_key.find('|', start);
        std::string index_name = index_key.substr(start, end - start);
        index_kvs_map[index_name].emplace_back(index_key, index_id);
        iter->Next();
    }

    for (const auto &[index_name, index_kv] : index_kvs_map) {
        size_t max_visible_index_index = std::numeric_limits<size_t>::max();
        TxnTimeStamp max_commit_ts = 0;
        for (size_t i = 0; i < index_kv.size(); ++i) {
            std::string commit_ts_str = GetLastPartOfKey(index_kv[i].first, '|');
            TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
            if (commit_ts <= begin_ts_ && commit_ts > max_commit_ts) {
                max_commit_ts = commit_ts;
                max_visible_index_index = i;
            }
        }

        if (max_visible_index_index != std::numeric_limits<size_t>::max()) {
            std::string drop_index_ts{};
            const std::string &index_key = index_kv[max_visible_index_index].first;
            const std::string &index_id = index_kv[max_visible_index_index].second;
            kv_instance_->Get(KeyEncode::DropTableIndexKey(db_id_str_, table_id_str_, index_name, max_commit_ts, index_id), drop_index_ts);

            if (drop_index_ts.empty() || std::stoull(drop_index_ts) > begin_ts_) {
                index_id_strs.push_back(index_id);
                index_names.push_back(index_name);
                if (txn_ != nullptr and txn_->readonly()) {
                    std::shared_ptr<MetaIndexCache> index_cache = meta_cache_->GetIndex(db_id_, table_id_, index_name, begin_ts_);
                    if (index_cache.get() == nullptr) {
                        index_cache = std::make_shared<MetaIndexCache>(db_id_,
                                                                       table_id_,
                                                                       index_name,
                                                                       std::stoull(index_id),
                                                                       max_commit_ts,
                                                                       index_key,
                                                                       false,
                                                                       txn_->TxnID());
                        txn_->AddMetaCache(index_cache);
                    }
                }
            }
        }
    }

    if (table_cache.get() != nullptr && txn_ != nullptr && txn_->readonly()) {
        txn_->AddCacheInfo(std::make_shared<TableCacheIndexInfo>(db_id_, table_name_, begin_ts_, index_ids_ptr, index_names_ptr));
    }

    index_id_strs_ = index_id_strs;
    index_name_strs_ = index_names;
    return Status::OK();
}

// Status TableMeta::LoadNextSegmentID() {
//     std::string next_seg_id_key = GetTableTag("next_segment_id");
//     std::string next_seg_id_str;
//     Status status = kv_instance_.Get(next_seg_id_key, next_seg_id_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to get next segment id from kv store, key: {}, cause: {}", next_seg_id_key, status.message()));
//         return status;
//     }
//     next_segment_id_ = std::stoull(next_seg_id_str);
//     return Status::OK();
// }

Status TableMeta::LoadUnsealedSegmentID() {
    std::string unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    std::string unsealed_seg_id_str;
    Status status = kv_instance_->Get(unsealed_seg_id_key, unsealed_seg_id_str);
    if (!status.ok()) {
        LOG_DEBUG(fmt::format("Fail to get unsealed segment id from kv store, key: {}, cause: {}", unsealed_seg_id_key, status.message()));
        return status;
    }
    unsealed_segment_id_ = std::stoull(unsealed_seg_id_str);
    return Status::OK();
}

Status TableMeta::LoadNextColumnID() {
    std::string table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    std::string next_column_id_str;
    Status status = kv_instance_->Get(table_latest_column_id_key, next_column_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get next column id from kv store, key: {}, cause: {}", table_latest_column_id_key, status.message()));
        return status;
    }
    next_column_id_ = std::stoull(next_column_id_str);
    return Status::OK();
}

std::string TableMeta::GetTableTag(const std::string &tag) const { return KeyEncode::CatalogTableTagKey(db_id_str_, table_id_str_, tag); }

// Status TableMeta::SetNextSegmentID(SegmentID next_segment_id) {
//     next_segment_id_ = next_segment_id;
//     std::string next_id_key = GetTableTag("next_segment_id");
//     std::string next_id_str = fmt::format("{}", next_segment_id);
//     Status status = kv_instance_.Put(next_id_key, next_id_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to set next segment id from kv store, key: {}:{}, cause: {}", next_id_key, next_id_str, status.message()));
//         return status;
//     }
//     return Status::OK();
// }

Status TableMeta::GetUnsealedSegmentID(SegmentID &unsealed_segment_id) {
    if (!unsealed_segment_id_) {
        Status status = LoadUnsealedSegmentID();
        if (!status.ok()) {
            return status;
        }
    }
    unsealed_segment_id = *unsealed_segment_id_;
    return Status::OK();
}

Status TableMeta::SetUnsealedSegmentID(SegmentID unsealed_segment_id) {
    unsealed_segment_id_ = unsealed_segment_id;
    std::string unsealed_id_key = GetTableTag("unsealed_segment_id");
    std::string unsealed_id_str = fmt::format("{}", unsealed_segment_id);
    Status status = kv_instance_->Put(unsealed_id_key, unsealed_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to set unsealed segment id from kv store, {}:{}, cause: {}", unsealed_id_key, unsealed_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

Status TableMeta::DelUnsealedSegmentID() {
    std::string unsealed_id_key = GetTableTag("unsealed_segment_id");
    Status status = kv_instance_->Delete(unsealed_id_key);
    return status;
}

std::tuple<ColumnID, Status> TableMeta::GetColumnIDByColumnName(const std::string &column_name) {
    Status status;
    std::shared_ptr<ColumnDef> column_def = nullptr;
    std::tie(column_def, status) = GetColumnDefByColumnName(column_name);
    if (!status.ok()) {
        return {INVALID_COLUMN_ID, status};
    }

    return {column_def->id_, Status::OK()};
}

std::tuple<std::string, Status> TableMeta::GetColumnKeyByColumnName(const std::string &column_name) const {
    std::vector<std::shared_ptr<ColumnDef>> column_defs;
    std::string column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_, column_name);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(column_prefix);

    std::vector<std::pair<std::string, std::string>> column_kvs;
    while (iter->Valid() && iter->Key().starts_with(column_prefix)) {
        column_kvs.emplace_back(iter->Key().ToString(), iter->Value().ToString());
        iter->Next();
    }

    if (column_kvs.empty()) {
        return {"", Status::ColumnNotExist(column_name)};
    }

    size_t max_visible_column_index = std::numeric_limits<size_t>::max();
    TxnTimeStamp max_commit_ts = 0;
    for (size_t i = 0; i < column_kvs.size(); ++i) {
        TxnTimeStamp commit_ts = infinity::GetTimestampFromKey(column_kvs[i].first);
        if ((commit_ts <= begin_ts_ || commit_ts == commit_ts_) && commit_ts > max_commit_ts) {
            max_commit_ts = commit_ts;
            max_visible_column_index = i;
        }
    }

    if (max_visible_column_index == std::numeric_limits<size_t>::max()) {
        return {"", Status::ColumnNotExist(column_name)};
    }

    std::string drop_column_ts{};
    kv_instance_->Get(KeyEncode::DropTableColumnKey(db_id_str_, table_id_str_, column_name, max_commit_ts), drop_column_ts);
    if (!drop_column_ts.empty() && std::stoull(drop_column_ts) <= begin_ts_) {
        return {"", Status::ColumnNotExist(column_name)};
    }

    return {column_kvs[max_visible_column_index].first, Status::OK()};
}

std::shared_ptr<std::string> TableMeta::GetTableDir() { return {std::make_shared<std::string>(table_id_str_)}; }

std::tuple<std::vector<SegmentID> *, Status> TableMeta::GetSegmentIDs1() {
    std::lock_guard<std::mutex> lock(mtx_);
    std::shared_ptr<MetaTableCache> table_cache{};
    if (segment_ids1_.get() == nullptr) {
        if (txn_ != nullptr and txn_->readonly()) {
            table_cache = meta_cache_->GetTable(db_id_, table_name_, begin_ts_);
            if (table_cache.get() != nullptr) {
                segment_ids1_ = table_cache->get_segments();
                if (segment_ids1_ != nullptr) {
                    return {&*segment_ids1_, Status::OK()};
                }
            }
        }

        segment_ids1_ = infinity::GetTableSegments(kv_instance_, db_id_str_, table_id_str_, begin_ts_, commit_ts_);
    }

    if (table_cache.get() != nullptr && txn_ != nullptr && txn_->readonly()) {
        txn_->AddCacheInfo(std::make_shared<TableCacheSegmentInfo>(db_id_, table_name_, begin_ts_, segment_ids1_));
    }

    return {&*segment_ids1_, Status::OK()};
}

Status TableMeta::CheckSegments(const std::vector<SegmentID> &segment_ids) {
    for (SegmentID segment_id : segment_ids) {
        std::string segment_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
        std::string commit_ts_str;
        Status status = kv_instance_->Get(segment_key, commit_ts_str);
        if (!status.ok()) {
            return status;
        }

        std::string drop_segment_ts{};
        kv_instance_->Get(KeyEncode::DropSegmentKey(db_id_str_, table_id_str_, segment_id), drop_segment_ts);
        if (!drop_segment_ts.empty() && std::stoull(drop_segment_ts) <= begin_ts_) {
            return Status::SegmentNotExist(segment_id);
        }
    }
    return Status::OK();
}

std::tuple<std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>>, Status> TableMeta::GetColumnDefs() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (!column_defs_) {
        auto status = LoadColumnDefs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }

    return {column_defs_, Status::OK()};
}

std::tuple<std::string, Status> TableMeta::GetNextIndexID() {
    std::string next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    std::string next_index_id_str;
    Status status = kv_instance_->Get(next_index_id_key, next_index_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get next index id from kv store, key: {}, cause: {}", next_index_id_key, status.message()));
        return {"", status};
    }
    std::string next_index_id = fmt::format("{}", std::stoull(next_index_id_str) + 1);
    status = kv_instance_->Put(next_index_id_key, next_index_id);
    if (!status.ok()) {
        LOG_ERROR(
            fmt::format("Fail to set next index id to kv store, key: {}, value: {}, cause: {}", next_index_id_key, next_index_id, status.message()));
        return {"", status};
    }
    return {next_index_id, Status::OK()};
}

Status TableMeta::SetNextIndexID(const std::string &index_id_str) {
    std::string next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    Status status = kv_instance_->Put(next_index_id_key, index_id_str);
    if (!status.ok()) {
        LOG_ERROR(
            fmt::format("Fail to set next index id to kv store, key: {}, value: {}, cause: {}", next_index_id_key, index_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> TableMeta::MapMetaToSnapShotInfo(const std::string &db_name, const std::string &table_name) {
    // TxnTimeStamp txn_id_{};

    // TxnTimeStamp max_commit_ts_{};
    // std::string table_entry_dir_{};
    // ColumnID next_column_id_{};
    // SegmentID unsealed_id_{};
    // SegmentID next_segment_id_{};
    // size_t row_count_{};
    // std::vector<std::shared_ptr<ColumnDef>> columns_{};
    // std::map<SegmentID, std::shared_ptr<SegmentSnapshotInfo>> segment_snapshots_{};
    // std::map<std::string, std::shared_ptr<TableIndexSnapshotInfo>> table_index_snapshots_{};
    std::shared_ptr<TableSnapshotInfo> table_snapshot_info = std::make_shared<TableSnapshotInfo>();
    // Get comment
    // TableInfo table_info;
    // Status status = GetComm(table_info);
    // if (!status.ok()) {
    //     return {nullptr, status};
    // }
    table_snapshot_info->table_name_ = table_name;
    table_snapshot_info->db_name_ = db_name;
    table_snapshot_info->db_id_str_ = db_id_str_;
    table_snapshot_info->table_id_str_ = table_id_str_;
    // table_snapshot_info->table_comment_ = table_info.comment_;

    // TODO: remove these two fields and figure out why they are here in the first place
    // table_snapshot_info->begin_ts_ = begin_ts_;
    // table_snapshot_info->commit_ts_ = commit_ts_;

    // table_snapshot_info->create_ts_ = table_info.create_ts_;

    // Get unsealed segment id
    SegmentID unsealed_segment_id = 0;
    Status status = GetUnsealedSegmentID(unsealed_segment_id);
    // if (!status.ok()) {
    //     return {nullptr, status};
    // }
    table_snapshot_info->unsealed_id_ = unsealed_segment_id;

    // Get next column id
    ColumnID next_column_id = 0;
    status = GetNextColumnID(next_column_id);
    // if (!status.ok()) {
    //     return {nullptr, status};
    // }
    table_snapshot_info->next_column_id_ = next_column_id;

    // Get column defs
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = this->GetColumnDefs();
    if (!status.ok()) {
        return {nullptr, status};
    }
    table_snapshot_info->columns_ = *column_defs;
    std::sort(table_snapshot_info->columns_.begin(),
              table_snapshot_info->columns_.end(),
              [](const std::shared_ptr<ColumnDef> &a, const std::shared_ptr<ColumnDef> &b) { return a->id_ < b->id_; });

    // Get segment ids
    std::vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
    // if (!status.ok()) {
    //     return {nullptr, status};
    // }
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, *this);
        auto [segment_snapshot, segment_status] = segment_meta.MapMetaToSnapShotInfo();
        if (!segment_status.ok()) {
            return {nullptr, segment_status};
        }
        table_snapshot_info->segment_snapshots_.emplace(segment_id, segment_snapshot);
    }

    // Get index ids
    if (!index_id_strs_) {
        status = LoadIndexIDs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    size_t index_count = index_id_strs_->size();
    for (size_t idx = 0; idx < index_count; ++idx) {
        const std::string &index_id = index_id_strs_->at(idx);
        const std::string &index_name = index_name_strs_->at(idx);
        TableIndexMeta table_index_meta(index_id, index_name, *this);
        auto [table_index_snapshot, table_index_status] = table_index_meta.MapMetaToSnapShotInfo();
        if (!table_index_status.ok()) {
            return {nullptr, table_index_status};
        }
        table_snapshot_info->table_index_snapshots_.emplace(index_id, table_index_snapshot);
    }

    return {table_snapshot_info, Status::OK()};
}

Status TableMeta::RestoreFromSnapshot(WalCmdRestoreTableSnapshot *restore_table_snapshot_cmd, bool is_link_files) {
    for (const WalSegmentInfoV2 &segment_info : restore_table_snapshot_cmd->segment_infos_) {
        if (!is_link_files) {
            Status status = AddSegmentWithID(commit_ts(), segment_info.segment_id_);
            if (!status.ok()) {
                return status;
            }
        }
        SegmentMeta segment_meta(segment_info.segment_id_, *this);
        Status status = segment_meta.RestoreFromSnapshot(segment_info, is_link_files);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status TableMeta::SetBeginTS(TxnTimeStamp begin_ts) {
    begin_ts_ = begin_ts;
    return Status::OK();
}

std::tuple<size_t, Status> TableMeta::GetTableRowCount() {
    Status status{};
    size_t row_count{};
    auto [segment_ids, seg_status] = GetSegmentIDs1();
    for (auto &segment_id : *segment_ids) {
        SegmentMeta segment_meta(segment_id, *this);
        std::vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
        if (!status.ok()) {
            return {row_count, status};
        }

        for (auto &block_id : *block_ids_ptr) {
            BlockMeta block_meta(block_id, segment_meta);
            NewTxnGetVisibleRangeState state;
            status = NewCatalog::GetBlockVisibleRange(block_meta, begin_ts_, commit_ts_, state);
            if (!status.ok()) {
                return {row_count, status};
            }

            std::pair<BlockOffset, BlockOffset> range;
            BlockOffset offset = 0;
            while (state.Next(offset, range)) {
                row_count += (range.second - range.first);
                offset = range.second;
            }
        }
    }
    return {row_count, Status::OK()};
}

MetaCache *TableMeta::meta_cache() const { return meta_cache_; }

const std::string &TableMeta::table_name() const { return table_name_; }

u64 TableMeta::db_id() const { return db_id_; }

u64 TableMeta::table_id() const { return table_id_; }

NewTxn *TableMeta::txn() const { return txn_; }

} // namespace infinity
