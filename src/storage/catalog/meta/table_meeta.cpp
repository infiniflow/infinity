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

#include "type/complex/row_id.h"
#include <string>

module table_meeta;

import status;
import kv_code;
import kv_store;
import column_def;
import third_party;
import default_values;
import logger;
import table_def;
import infinity_exception;
import meta_info;
import infinity_context;
import storage;
import new_catalog;
import table_index_meeta;
import create_index_info;
import segment_meta;
import kv_utility;
import column_index_reader;
import new_txn;
import meta_type;

namespace infinity {

TableMeeta::TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts)
    : BaseMeta(MetaType::kTable), begin_ts_(begin_ts), commit_ts_(commit_ts), kv_instance_(kv_instance), db_id_str_(db_id_str),
      table_id_str_(table_id_str) {}

TableMeeta::TableMeeta(const String &db_id_str, const String &table_id_str, NewTxn *txn)
    : BaseMeta(MetaType::kTable), txn_(txn), db_id_str_(db_id_str), table_id_str_(table_id_str) {
    if (txn == nullptr) {
        UnrecoverableError("Null txn pointer");
    }
    begin_ts_ = txn->BeginTS();
    commit_ts_ = txn->CommitTS();
    kv_instance_ = txn->kv_instance();
}

Status TableMeeta::GetComment(TableInfo &table_info) {
    if (!comment_) {
        Status status = LoadComment();
        if (!status.ok()) {
            return status;
        }
    }
    table_info.table_comment_ = MakeShared<String>(*comment_);
    return Status::OK();
}

Status TableMeeta::GetIndexIDs(Vector<String> *&index_id_strs, Vector<String> **index_names) {
    if (!index_id_strs_ || !index_names_) {
        Status status = LoadIndexIDs();
        if (!status.ok()) {
            return status;
        }
    }
    index_id_strs = &index_id_strs_.value();
    if (index_names) {
        *index_names = &index_names_.value();
    }
    return Status::OK();
}

Status TableMeeta::GetIndexID(const String &index_name, String &index_key, String &index_id_str, TxnTimeStamp &create_index_ts) {
    String index_key_prefix = KeyEncode::CatalogIndexPrefix(db_id_str_, table_id_str_, index_name);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(index_key_prefix);

    Vector<Pair<String, String>> index_kvs;
    while (iter2->Valid() && iter2->Key().starts_with(index_key_prefix)) {
        index_kvs.emplace_back(iter2->Key().ToString(), iter2->Value().ToString());
        iter2->Next();
    }

    if (index_kvs.empty()) {
        return Status::IndexNotExist(index_name);
    }

    SizeT max_visible_index_index = std::numeric_limits<SizeT>::max();
    TxnTimeStamp max_commit_ts = 0;
    for (SizeT i = 0; i < index_kvs.size(); ++i) {
        TxnTimeStamp commit_ts = infinity::GetTimestampFromKey(index_kvs[i].first);
        if ((commit_ts <= begin_ts_ || (txn_ != nullptr && txn_->IsReplay() && commit_ts == commit_ts_)) && commit_ts > max_commit_ts) {
            max_commit_ts = commit_ts;
            max_visible_index_index = i;
        }
    }

    if (max_visible_index_index == std::numeric_limits<SizeT>::max()) {
        return Status::IndexNotExist(index_name);
    }

    index_id_str = index_kvs[max_visible_index_index].second;
    index_key = index_kvs[max_visible_index_index].first;

    String drop_index_ts{};
    kv_instance_->Get(KeyEncode::DropTableIndexKey(db_id_str_, table_id_str_, index_name, max_commit_ts, index_id_str), drop_index_ts);

    if (!drop_index_ts.empty() && std::stoull(drop_index_ts) <= begin_ts_) {
        return Status::IndexNotExist(index_name);
    }

    create_index_ts = max_commit_ts;
    return Status::OK();
}

Tuple<SharedPtr<ColumnDef>, Status> TableMeeta::GetColumnDefByColumnName(const String &column_name, SizeT *column_idx_ptr) {
    if (!column_defs_) {
        Status status = LoadColumnDefs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    for (SizeT column_idx = 0; column_idx < column_defs_->size(); ++column_idx) {
        if ((*column_defs_)[column_idx]->name() == column_name) {
            if (column_idx_ptr) {
                *column_idx_ptr = column_idx;
            }
            return {(*column_defs_)[column_idx], Status::OK()};
        }
    }
    return {nullptr, Status::ColumnNotExist(column_name)};
}

Tuple<SharedPtr<ColumnDef>, Status> TableMeeta::GetColumnDefByColumnID(const SizeT &column_idx) {
    if (!column_defs_) {
        Status status = LoadColumnDefs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    if (column_idx >= column_defs_->size()) {
        return {nullptr, Status::ColumnNotExist(column_idx)};
    }
    return {(*column_defs_)[column_idx], Status::OK()};
}

// Status TableMeeta::SetSegmentIDs(const Vector<SegmentID> &segment_ids) {
//     segment_ids_ = segment_ids;
//     String segment_ids_key = GetTableTag("segment_ids");
//     String segment_ids_str = nlohmann::json(segment_ids).dump();
//     Status status = kv_instance_.Put(segment_ids_key, segment_ids_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to set segment ids to kv store, key: {}, cause: {}", segment_ids_key, status.message()));
//         return status;
//     }
//     return Status::OK();
// }

Status TableMeeta::RemoveSegmentIDs1(const Vector<SegmentID> &segment_ids) {
    HashSet<SegmentID> segment_ids_set(segment_ids.begin(), segment_ids.end());

    String segment_id_prefix = KeyEncode::CatalogTableSegmentKeyPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(segment_id_prefix);
    Vector<String> delete_keys;
    while (iter->Valid() && iter->Key().starts_with(segment_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        SegmentID segment_id = std::stoull(iter->Key().ToString().substr(segment_id_prefix.size()));
        if (segment_ids_set.contains(segment_id)) {
            if (commit_ts > begin_ts_ and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                UnrecoverableError(
                    fmt::format("Segment id: {} is not allowed to be removed. commit_ts: {}, begin_ts: {}", segment_id, commit_ts, begin_ts_));
            }
            // the key is committed before the txn or the key isn't committed
            delete_keys.push_back(iter->Key().ToString());
        }
        iter->Next();
    }
    for (const String &key : delete_keys) {
        Status status = kv_instance_->Delete(key);
        if (!status.ok()) {
            return status;
        }
    }

    if (segment_ids1_) {
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

Pair<SegmentID, Status> TableMeeta::AddSegmentID1(TxnTimeStamp commit_ts) {
    Status status;

    SegmentID segment_id = 0;
    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
        if (!status.ok()) {
            return {0, status};
        }
        segment_id = segment_ids_ptr->empty() ? 0 : segment_ids_ptr->back() + 1;
        segment_ids1_->push_back(segment_id);
    }

    String segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance_->Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {segment_id, Status::OK()};
}

Status TableMeeta::AddSegmentWithID(TxnTimeStamp commit_ts, SegmentID segment_id) {
    String segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    return kv_instance_->Put(segment_id_key, commit_ts_str);
}

Status TableMeeta::CommitSegment(SegmentID segment_id, TxnTimeStamp commit_ts) {
    String segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    Status status = kv_instance_->Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::InitSet(SharedPtr<TableDef> table_def) {
    Status status;

    // Create table comment;
    if (table_def->table_comment() != nullptr and !table_def->table_comment()->empty()) {
        String &table_comment = *table_def->table_comment();
        String table_comment_key = GetTableTag("comment");
        status = kv_instance_->Put(table_comment_key, table_comment);
        if (!status.ok()) {
            return status;
        }
    }

    // Create table column id;
    SizeT column_size = table_def->column_count();
    status = SetNextColumnID(column_size);
    if (!status.ok()) {
        return status;
    }

    // Create the next segment id;
    String table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_->Put(table_latest_segment_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // Create next index id;
    String next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    status = kv_instance_->Put(next_index_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    for (const auto &column : table_def->columns()) {
        String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column->name(), commit_ts_);
        status = kv_instance_->Put(column_key, column->ToJson().dump());
        if (!status.ok()) {
            return status;
        }
    }

    return Status::OK();
}

Status TableMeeta::LoadSet() {
    Vector<String> *index_id_strs_ptr = nullptr;
    Status status = GetIndexIDs(index_id_strs_ptr);
    if (!status.ok()) {
        return status;
    }
    for (const String &index_id_str : *index_id_strs_ptr) {
        TableIndexMeeta table_index_meta(index_id_str, *this);
        auto [index_def, status] = table_index_meta.GetIndexBase();
        if (!status.ok()) {
            return status;
        }
        if (index_def->index_type_ == IndexType::kFullText) {
            NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
            String ft_index_cache_key = GetTableTag("ft_index_cache");
            auto ft_index_cache = MakeShared<TableIndexReaderCache>(db_id_str_, table_id_str_);
            status = new_catalog->AddFtIndexCache(std::move(ft_index_cache_key), std::move(ft_index_cache));
            if (!status.ok()) {
                return status;
            }
            break;
        }
    }
    return Status::OK();
}

Status TableMeeta::UninitSet(UsageFlag usage_flag) {
    Status status;

    // Create next index id;
    String next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    status = kv_instance_->Delete(next_index_id_key);
    if (!status.ok()) {
        return status;
    }

    // delete table segment id;
    String table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_->Delete(table_latest_segment_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table column id;
    String table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    status = kv_instance_->Delete(table_latest_column_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    String table_comment_key = GetTableTag("comment");
    status = kv_instance_->Delete(table_comment_key);
    if (!status.ok()) {
        return status;
    }

    {
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
        status = RemoveSegmentIDs1(*segment_ids_ptr);
        if (!status.ok()) {
            return status;
        }
    }

    String unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    status = kv_instance_->Delete(unsealed_seg_id_key);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kNotFound) {
            return status;
        }
    }

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);
    auto iter2 = kv_instance_->GetIterator();
    iter2->Seek(table_column_prefix);

    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String table_column_key = iter2->Key().ToString();
        status = kv_instance_->Delete(table_column_key);
        if (!status.ok()) {
            return status;
        }
        iter2->Next();
    }

    String index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(index_prefix);

    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        String index_key = iter->Key().ToString();
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

Status TableMeeta::GetTableInfo(TableInfo &table_info) {
    Status status;

    table_info.table_full_dir_ =
        MakeShared<String>(fmt::format("{}/db_{}/tbl_{}", InfinityContext::instance().config()->DataDir(), db_id_str_, table_id_str_));

    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = this->GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    table_info.column_defs_ = *column_defs;
    std::sort(table_info.column_defs_.begin(), table_info.column_defs_.end(), [](const SharedPtr<ColumnDef> &a, const SharedPtr<ColumnDef> &b) {
        return a->id_ < b->id_;
    });
    table_info.column_count_ = table_info.column_defs_.size();

    table_info.db_id_ = db_id_str_;
    table_info.table_id_ = table_id_str_;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    table_info.segment_count_ = segment_ids_ptr->size();
    return Status::OK();
}

Status TableMeeta::GetTableDetail(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts, TableDetail &table_detail) {
    TableInfo table_info;
    Status status = GetTableInfo(table_info);
    if (!status.ok()) {
        return status;
    }
    status = GetComment(table_info);
    if (!status.ok()) {
        return status;
    }
    table_detail.db_name_ = MakeShared<String>(db_name_);
    table_detail.table_name_ = MakeShared<String>(table_name_);
    table_detail.table_comment_ = table_info.table_comment_;
    table_detail.column_count_ = table_info.column_count_;
    table_detail.row_count_ = table_info.row_count_;

    Vector<SegmentID> *segment_ids_ptr = nullptr;
    std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
    if (!status.ok()) {
        return status;
    }
    SizeT block_count = 0;
    for (SegmentID segment_id : *segment_ids_ptr) {
        SegmentMeta segment_meta(segment_id, *this);
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1(kv_instance, begin_ts, commit_ts);
        if (!status.ok()) {
            return status;
        }
        block_count += block_ids_ptr->size();
    }
    table_detail.block_count_ = block_count;
    table_detail.block_capacity_ = DEFAULT_BLOCK_CAPACITY;
    table_detail.segment_count_ = segment_ids_ptr->size();
    table_detail.segment_capacity_ = DEFAULT_SEGMENT_CAPACITY;

    return Status::OK();
}

Pair<String, String> TableMeeta::GetDBTableName() const { return MakePair(db_name_, table_name_); }

void TableMeeta::SetDBTableName(const String &db_name, const String &table_name) {
    db_name_ = db_name;
    table_name_ = table_name;
}

Status TableMeeta::AddColumn(const ColumnDef &column_def) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_def.name(), commit_ts_);
    String column_name_value;
    Status status = kv_instance_->Put(column_key, column_def.ToJson().dump());
    return status;
}

Status TableMeeta::AddFtIndexCache(SharedPtr<TableIndexReaderCache> ft_index_cache) {
    String ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->AddFtIndexCache(std::move(ft_index_cache_key), std::move(ft_index_cache));
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::GetFtIndexCache(SharedPtr<TableIndexReaderCache> &ft_index_cache) {
    String ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();

    ft_index_cache = MakeShared<TableIndexReaderCache>(db_id_str_, table_id_str_);
    Status status = new_catalog->GetFtIndexCache(ft_index_cache_key, ft_index_cache);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::RemoveFtIndexCache() {
    String ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    Status status = new_catalog->DropFtIndexCacheByFtIndexCacheKey(ft_index_cache_key);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status TableMeeta::InvalidateFtIndexCache() {
    String ft_index_cache_key = GetTableTag("ft_index_cache");
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    SharedPtr<TableIndexReaderCache> ft_index_cache = MakeShared<TableIndexReaderCache>(db_id_str_, table_id_str_);
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

Status TableMeeta::GetNextColumnID(ColumnID &next_column_id) {
    if (!next_column_id_) {
        Status status = LoadNextColumnID();
        if (!status.ok()) {
            return status;
        }
    }
    next_column_id = *next_column_id_;
    return Status::OK();
}

Status TableMeeta::SetNextColumnID(ColumnID next_column_id) {
    String table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    String next_column_id_str = fmt::format("{}", next_column_id);
    Status status = kv_instance_->Put(table_latest_column_id_key, next_column_id_str);
    if (!status.ok()) {
        return status;
    }
    next_column_id_ = next_column_id;
    return Status::OK();
}

Status TableMeeta::LoadComment() {
    String table_comment_key = GetTableTag("comment");
    String table_comment;
    Status status = kv_instance_->Get(table_comment_key, table_comment);
    if (!status.ok() && status.code() != ErrorCode::kNotFound) { // not found is ok
        LOG_ERROR(fmt::format("Fail to get table comment from kv store, key: {}, cause: {}", table_comment_key, status.message()));
        return status;
    }
    comment_ = std::move(table_comment);
    return Status::OK();
}

Status TableMeeta::LoadColumnDefs() {
    Vector<SharedPtr<ColumnDef>> column_defs;
    Map<String, Vector<Pair<String, String>>> column_kvs_map;
    String column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(column_prefix);
    while (iter->Valid() && iter->Key().starts_with(column_prefix)) {
        String column_key = iter->Key().ToString();
        String column_value = iter->Value().ToString();
        size_t start = column_prefix.size();
        size_t end = column_key.find('|', start);
        String column_name = column_key.substr(start, end - start);
        column_kvs_map[column_name].emplace_back(column_key, column_value);
        iter->Next();
    }

    for (const auto &[column_name, column_kv] : column_kvs_map) {
        SizeT max_visible_column_index = std::numeric_limits<SizeT>::max();
        TxnTimeStamp max_commit_ts = 0;
        for (SizeT i = 0; i < column_kv.size(); ++i) {
            String commit_ts_str = GetLastPartOfKey(column_kv[i].first, '|');
            TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
            if ((commit_ts <= begin_ts_ || commit_ts == commit_ts_) && commit_ts > max_commit_ts) {
                {
                    max_commit_ts = commit_ts;
                    max_visible_column_index = i;
                }
            }
        }

        if (max_visible_column_index != std::numeric_limits<SizeT>::max()) {
            String drop_column_ts{};
            const String &column_value = column_kv[max_visible_column_index].second;
            kv_instance_->Get(KeyEncode::DropTableColumnKey(db_id_str_, table_id_str_, column_name, max_commit_ts), drop_column_ts);

            if (drop_column_ts.empty() || std::stoull(drop_column_ts) > begin_ts_) {
                auto column_def = ColumnDef::FromJson(column_value);
                column_defs.push_back(column_def);
            }
        }
    }
    std::sort(column_defs.begin(), column_defs.end(), [](const SharedPtr<ColumnDef> &a, const SharedPtr<ColumnDef> &b) { return a->id_ < b->id_; });
    column_defs_ = std::move(column_defs);

    return Status::OK();
}

Status TableMeeta::LoadSegmentIDs1() {
    segment_ids1_ = infinity::GetTableSegments(kv_instance_, db_id_str_, table_id_str_, begin_ts_, commit_ts_);
    return Status::OK();
}

Status TableMeeta::LoadIndexIDs() {
    Vector<String> index_id_strs;
    Vector<String> index_names;
    Map<String, Vector<Pair<String, String>>> index_kvs_map;
    String index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(index_prefix);
    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        String index_key = iter->Key().ToString();
        String index_id = iter->Value().ToString();
        size_t start = index_prefix.size();
        size_t end = index_key.find('|', start);
        String index_name = index_key.substr(start, end - start);
        index_kvs_map[index_name].emplace_back(index_key, index_id);
        iter->Next();
    }

    for (const auto &[index_name, index_kv] : index_kvs_map) {
        SizeT max_visible_index_index = std::numeric_limits<SizeT>::max();
        TxnTimeStamp max_commit_ts = 0;
        for (SizeT i = 0; i < index_kv.size(); ++i) {
            String commit_ts_str = GetLastPartOfKey(index_kv[i].first, '|');
            TxnTimeStamp commit_ts = std::stoull(commit_ts_str);
            if (commit_ts <= begin_ts_ && commit_ts > max_commit_ts) {
                max_commit_ts = commit_ts;
                max_visible_index_index = i;
            }
        }

        if (max_visible_index_index != std::numeric_limits<SizeT>::max()) {
            String drop_index_ts{};
            const String &index_id = index_kv[max_visible_index_index].second;
            kv_instance_->Get(KeyEncode::DropTableIndexKey(db_id_str_, table_id_str_, index_name, max_commit_ts, index_id), drop_index_ts);

            if (drop_index_ts.empty() || std::stoull(drop_index_ts) > begin_ts_) {
                index_id_strs.push_back(index_id);
                index_names.push_back(index_name);
            }
        }
    }

    index_id_strs_ = std::move(index_id_strs);
    index_names_ = std::move(index_names);
    return Status::OK();
}

Status TableMeeta::LoadUnsealedSegmentID() {
    String unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    String unsealed_seg_id_str;
    Status status = kv_instance_->Get(unsealed_seg_id_key, unsealed_seg_id_str);
    if (!status.ok()) {
        LOG_DEBUG(fmt::format("Fail to get unsealed segment id from kv store, key: {}, cause: {}", unsealed_seg_id_key, status.message()));
        return status;
    }
    unsealed_segment_id_ = std::stoull(unsealed_seg_id_str);
    return Status::OK();
}

Status TableMeeta::LoadNextColumnID() {
    String table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    String next_column_id_str;
    Status status = kv_instance_->Get(table_latest_column_id_key, next_column_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get next column id from kv store, key: {}, cause: {}", table_latest_column_id_key, status.message()));
        return status;
    }
    next_column_id_ = std::stoull(next_column_id_str);
    return Status::OK();
}

String TableMeeta::GetTableTag(const String &tag) const { return KeyEncode::CatalogTableTagKey(db_id_str_, table_id_str_, tag); }

Status TableMeeta::GetUnsealedSegmentID(SegmentID &unsealed_segment_id) {
    if (!unsealed_segment_id_) {
        Status status = LoadUnsealedSegmentID();
        if (!status.ok()) {
            return status;
        }
    }
    unsealed_segment_id = *unsealed_segment_id_;
    return Status::OK();
}

Status TableMeeta::SetUnsealedSegmentID(SegmentID unsealed_segment_id) {
    unsealed_segment_id_ = unsealed_segment_id;
    String unsealed_id_key = GetTableTag("unsealed_segment_id");
    String unsealed_id_str = fmt::format("{}", unsealed_segment_id);
    Status status = kv_instance_->Put(unsealed_id_key, unsealed_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to set unsealed segment id from kv store, {}:{}, cause: {}", unsealed_id_key, unsealed_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

Status TableMeeta::DelUnsealedSegmentID() {
    String unsealed_id_key = GetTableTag("unsealed_segment_id");
    Status status = kv_instance_->Delete(unsealed_id_key);
    return status;
}

Tuple<ColumnID, Status> TableMeeta::GetColumnIDByColumnName(const String &column_name) {
    Status status;
    SharedPtr<ColumnDef> column_def = nullptr;
    std::tie(column_def, status) = GetColumnDefByColumnName(column_name);
    if (!status.ok()) {
        return {INVALID_COLUMN_ID, status};
    }

    return {column_def->id_, Status::OK()};
}

Tuple<String, Status> TableMeeta::GetColumnKeyByColumnName(const String &column_name) const {
    Vector<SharedPtr<ColumnDef>> column_defs;
    String column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_, column_name);
    auto iter = kv_instance_->GetIterator();
    iter->Seek(column_prefix);

    Vector<Pair<String, String>> column_kvs;
    while (iter->Valid() && iter->Key().starts_with(column_prefix)) {
        column_kvs.emplace_back(iter->Key().ToString(), iter->Value().ToString());
        iter->Next();
    }

    if (column_kvs.empty()) {
        return {"", Status::ColumnNotExist(column_name)};
    }

    SizeT max_visible_column_index = std::numeric_limits<SizeT>::max();
    TxnTimeStamp max_commit_ts = 0;
    for (SizeT i = 0; i < column_kvs.size(); ++i) {
        TxnTimeStamp commit_ts = infinity::GetTimestampFromKey(column_kvs[i].first);
        if ((commit_ts <= begin_ts_ || commit_ts == commit_ts_) && commit_ts > max_commit_ts) {
            max_commit_ts = commit_ts;
            max_visible_column_index = i;
        }
    }

    if (max_visible_column_index == std::numeric_limits<SizeT>::max()) {
        return {"", Status::ColumnNotExist(column_name)};
    }

    String drop_column_ts{};
    kv_instance_->Get(KeyEncode::DropTableColumnKey(db_id_str_, table_id_str_, column_name, max_commit_ts), drop_column_ts);
    if (!drop_column_ts.empty() && std::stoull(drop_column_ts) <= begin_ts_) {
        return {"", Status::ColumnNotExist(column_name)};
    }

    return {column_kvs[max_visible_column_index].first, Status::OK()};
}

SharedPtr<String> TableMeeta::GetTableDir() { return {MakeShared<String>(table_id_str_)}; }

Tuple<Vector<SegmentID> *, Status> TableMeeta::GetSegmentIDs1() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!segment_ids1_) {
        segment_ids1_ = infinity::GetTableSegments(kv_instance_, db_id_str_, table_id_str_, begin_ts_, commit_ts_);
    }
    return {&*segment_ids1_, Status::OK()};
}

Status TableMeeta::CheckSegments(const Vector<SegmentID> &segment_ids) {
    for (SegmentID segment_id : segment_ids) {
        String segment_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
        String commit_ts_str;
        Status status = kv_instance_->Get(segment_key, commit_ts_str);
        if (!status.ok()) {
            return status;
        }

        String drop_segment_ts{};
        kv_instance_->Get(KeyEncode::DropSegmentKey(db_id_str_, table_id_str_, segment_id), drop_segment_ts);
        if (!drop_segment_ts.empty() && std::stoull(drop_segment_ts) <= begin_ts_) {
            return Status::SegmentNotExist(segment_id);
        }
    }
    return Status::OK();
}

Tuple<SharedPtr<Vector<SharedPtr<ColumnDef>>>, Status> TableMeeta::GetColumnDefs() {
    std::unique_lock<std::mutex> lock(mtx_);

    if (!column_defs_) {
        auto status = LoadColumnDefs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {MakeShared<Vector<SharedPtr<ColumnDef>>>(column_defs_.value()), Status::OK()};
}

Tuple<String, Status> TableMeeta::GetNextIndexID() {
    String next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    String next_index_id_str;
    Status status = kv_instance_->Get(next_index_id_key, next_index_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get next index id from kv store, key: {}, cause: {}", next_index_id_key, status.message()));
        return {"", status};
    }
    String next_index_id = fmt::format("{}", std::stoull(next_index_id_str) + 1);
    status = kv_instance_->Put(next_index_id_key, next_index_id);
    if (!status.ok()) {
        LOG_ERROR(
            fmt::format("Fail to set next index id to kv store, key: {}, value: {}, cause: {}", next_index_id_key, next_index_id, status.message()));
        return {"", status};
    }
    return {next_index_id, Status::OK()};
}

Status TableMeeta::SetNextIndexID(const String &index_id_str) {
    String next_index_id_key = GetTableTag(NEXT_INDEX_ID.data());
    Status status = kv_instance_->Put(next_index_id_key, index_id_str);
    if (!status.ok()) {
        LOG_ERROR(
            fmt::format("Fail to set next index id to kv store, key: {}, value: {}, cause: {}", next_index_id_key, index_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

} // namespace infinity
