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

namespace infinity {

TableMeeta::TableMeeta(const String &db_id_str, const String &table_id_str, KVInstance &kv_instance, TxnTimeStamp begin_ts)
    : begin_ts_(begin_ts), kv_instance_(kv_instance), db_id_str_(db_id_str), table_id_str_(table_id_str) {}

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

Status TableMeeta::GetIndexID(const String &index_name, String &index_key, String &index_id_str) {
    String index_key_prefix = KeyEncode::CatalogIndexPrefix(db_id_str_, table_id_str_, index_name);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(index_key_prefix);
    SizeT found_count = 0;

    Vector<String> error_index_keys;
    while (iter2->Valid() && iter2->Key().starts_with(index_key_prefix)) {
        if (found_count > 0) {
            // Error branch
            error_index_keys.push_back(index_key);
        }
        index_key = iter2->Key().ToString();
        index_id_str = iter2->Value().ToString();
        iter2->Next();
        ++found_count;
    }

    if (found_count == 0) {
        return Status::IndexNotExist(index_name);
    }

    if (!error_index_keys.empty()) {
        error_index_keys.push_back(index_key);
        // join error_index_keys
        String error_index_keys_str =
            std::accumulate(std::next(error_index_keys.begin()), error_index_keys.end(), error_index_keys.front(), [](String a, String b) {
                return std::move(a) + ", " + std::move(b);
            });
        UnrecoverableError(fmt::format("Found multiple index keys: {}", error_index_keys_str));
    }

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
    auto iter = kv_instance_.GetIterator();
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
        Status status = kv_instance_.Delete(key);
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

// Status TableMeeta::AddSegmentID(SegmentID segment_id) {
//     if (!segment_ids_) {
//         Status status = LoadSegmentIDs();
//         //        if (!status.ok() && status.code() != ErrorCode::kNotFound) {
//         //            return status;
//         //        }
//         //        segment_ids_ = Vector<SegmentID>();
//         if (!status.ok()) {
//             return status;
//         }
//     }

//     segment_ids_->emplace_back(segment_id);
//     Status status = SetSegmentIDs(*segment_ids_);
//     if (!status.ok()) {
//         return status;
//     }
//     return Status::OK();
// }

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
    status = kv_instance_.Put(segment_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {segment_id, Status::OK()};
}

Status TableMeeta::CommitSegment(SegmentID segment_id, TxnTimeStamp commit_ts) {
    String segment_id_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    Status status = kv_instance_.Put(segment_id_key, commit_ts_str);
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
        status = kv_instance_.Put(table_comment_key, table_comment);
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

    // Create table segment id;
    String table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_.Put(table_latest_segment_id_key, "0");
    if (!status.ok()) {
        return status;
    }

    // {
    //     // Create segment ids
    //     String table_segment_ids_key = GetTableTag("segment_ids");
    //     String table_segment_ids_str = nlohmann::json::array().dump();
    //     Status status = kv_instance_.Put(table_segment_ids_key, table_segment_ids_str);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }

    for (const auto &column : table_def->columns()) {
        String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column->name());
        Status status = kv_instance_.Put(column_key, column->ToJson().dump());
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

    // delete table segment id;
    String table_latest_segment_id_key = GetTableTag("next_segment_id");
    status = kv_instance_.Delete(table_latest_segment_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table column id;
    String table_latest_column_id_key = GetTableTag(NEXT_COLUMN_ID.data());
    status = kv_instance_.Delete(table_latest_column_id_key);
    if (!status.ok()) {
        return status;
    }

    // Delete table comment
    String table_comment_key = GetTableTag("comment");
    status = kv_instance_.Delete(table_comment_key);
    if (!status.ok()) {
        return status;
    }

    // String segment_ids_key = GetTableTag("segment_ids");
    // status = kv_instance_.Delete(segment_ids_key);
    // if (!status.ok()) {
    //     return status;
    // }
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
    status = kv_instance_.Delete(unsealed_seg_id_key);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kNotFound) {
            return status;
        }
    }

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(table_column_prefix);

    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String table_column_key = iter2->Key().ToString();
        Status status = kv_instance_.Delete(table_column_key);
        if (!status.ok()) {
            return status;
        }
        iter2->Next();
    }

    String index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(index_prefix);

    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        String index_key = iter->Key().ToString();
        Status status = kv_instance_.Delete(index_key);
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

    String *table_comment = nullptr;
    status = GetComment(table_comment);
    if (!status.ok()) {
        return status;
    }
    table_info.table_comment_ = MakeShared<String>(*table_comment);

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

Status TableMeeta::GetTableDetail(TableDetail &table_detail, const String &db_name, const String &table_name) {
    TableInfo table_info;
    Status status = GetTableInfo(table_info);
    if (!status.ok()) {
        return status;
    }
    table_detail.db_name_ = MakeShared<String>(db_name);
    table_detail.table_name_ = MakeShared<String>(table_name);
    table_detail.table_comment_ = table_info.table_comment_;
    table_detail.table_entry_type_ = table_info.table_entry_type_;
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
        std::tie(block_ids_ptr, status) = segment_meta.GetBlockIDs1();
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

Status TableMeeta::AddColumn(const ColumnDef &column_def) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_def.name());
    String column_name_value;
    Status status = kv_instance_.Get(column_key, column_name_value);
    if (status.code() == ErrorCode::kNotFound) {
        Status status = kv_instance_.Put(column_key, column_def.ToJson().dump());
        if (!status.ok()) {
            return status;
        }
    } else {
        return Status::DuplicateColumnName(column_def.name());
    }
    return Status::OK();
}

Status TableMeeta::DropColumn(const String &column_name) {
    String column_name_value;
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    Status status = kv_instance_.Get(column_key, column_name_value);
    if (status.ok()) {
        status = kv_instance_.Delete(column_key);
        if (!status.ok()) {
            return status;
        }
    } else {
        return Status::ColumnNotExist(column_name);
    }
    return Status::OK();
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
    Status status = kv_instance_.Put(table_latest_column_id_key, next_column_id_str);
    if (!status.ok()) {
        return status;
    }
    next_column_id_ = next_column_id;
    return Status::OK();
}

Status TableMeeta::UpdateFulltextSegmentTS(TxnTimeStamp ts, SegmentUpdateTS &segment_update_ts) {
    SharedPtr<TableIndexReaderCache> ft_index_cache;
    Status status = GetFtIndexCache(ft_index_cache);
    if (!status.ok()) {
        return status;
    }
    ft_index_cache->UpdateKnownUpdateTs(ts, segment_update_ts.mtx_, segment_update_ts.ts_);

    return Status::OK();
}

Status TableMeeta::LoadComment() {
    String table_comment_key = GetTableTag("comment");
    String table_comment;
    Status status = kv_instance_.Get(table_comment_key, table_comment);
    if (!status.ok() && status.code() != ErrorCode::kNotFound) { // not found is ok
        LOG_ERROR(fmt::format("Fail to get table comment from kv store, key: {}, cause: {}", table_comment_key, status.message()));
        return status;
    }
    comment_ = std::move(table_comment);
    return Status::OK();
}

Status TableMeeta::LoadColumnDefs() {
    Vector<SharedPtr<ColumnDef>> column_defs;

    String table_column_prefix = KeyEncode::TableColumnPrefix(db_id_str_, table_id_str_);
    auto iter2 = kv_instance_.GetIterator();
    iter2->Seek(table_column_prefix);
    while (iter2->Valid() && iter2->Key().starts_with(table_column_prefix)) {
        String column_key = iter2->Key().ToString();
        String column_value = iter2->Value().ToString();
        [[maybe_unused]] String column_name = column_key.substr(column_key.find_last_of('|') + 1);
        auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value));
        column_defs.emplace_back(column_def);
        iter2->Next();
    }
    std::sort(column_defs.begin(), column_defs.end(), [](const SharedPtr<ColumnDef> &a, const SharedPtr<ColumnDef> &b) { return a->id_ < b->id_; });
    column_defs_ = std::move(column_defs);

    return Status::OK();
}

// Status TableMeeta::LoadSegmentIDs() {
//     String segment_ids_key = GetTableTag("segment_ids");
//     String segment_ids_str;
//     Status status = kv_instance_.Get(segment_ids_key, segment_ids_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to get segment ids from kv store, key: {}, cause: {}", segment_ids_key, status.message()));
//         return status;
//     }
//     segment_ids_ = nlohmann::json::parse(segment_ids_str).get<Vector<SegmentID>>();
//     return Status::OK();
// }

Status TableMeeta::LoadSegmentIDs1() {
    segment_ids1_ = infinity::GetTableSegments(&kv_instance_, db_id_str_, table_id_str_, begin_ts_);
    return Status::OK();
}

Status TableMeeta::LoadIndexIDs() {
    Vector<String> index_id_strs;
    Vector<String> index_names;
    String index_prefix = KeyEncode::CatalogTableIndexPrefix(db_id_str_, table_id_str_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(index_prefix);
    while (iter->Valid() && iter->Key().starts_with(index_prefix)) {
        String column_key = iter->Key().ToString();
        size_t start = index_prefix.size();
        size_t end = column_key.find('|', start);
        index_names.emplace_back(column_key.substr(start, end - start));
        index_id_strs.emplace_back(iter->Value().ToString());
        iter->Next();
    }
    index_id_strs_ = std::move(index_id_strs);
    index_names_ = std::move(index_names);
    return Status::OK();
}

// Status TableMeeta::LoadNextSegmentID() {
//     String next_seg_id_key = GetTableTag("next_segment_id");
//     String next_seg_id_str;
//     Status status = kv_instance_.Get(next_seg_id_key, next_seg_id_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to get next segment id from kv store, key: {}, cause: {}", next_seg_id_key, status.message()));
//         return status;
//     }
//     next_segment_id_ = std::stoull(next_seg_id_str);
//     return Status::OK();
// }

Status TableMeeta::LoadUnsealedSegmentID() {
    String unsealed_seg_id_key = GetTableTag("unsealed_segment_id");
    String unsealed_seg_id_str;
    Status status = kv_instance_.Get(unsealed_seg_id_key, unsealed_seg_id_str);
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
    Status status = kv_instance_.Get(table_latest_column_id_key, next_column_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get next column id from kv store, key: {}, cause: {}", table_latest_column_id_key, status.message()));
        return status;
    }
    next_column_id_ = std::stoull(next_column_id_str);
    return Status::OK();
}

String TableMeeta::GetTableTag(const String &tag) const { return KeyEncode::CatalogTableTagKey(db_id_str_, table_id_str_, tag); }

// Status TableMeeta::SetNextSegmentID(SegmentID next_segment_id) {
//     next_segment_id_ = next_segment_id;
//     String next_id_key = GetTableTag("next_segment_id");
//     String next_id_str = fmt::format("{}", next_segment_id);
//     Status status = kv_instance_.Put(next_id_key, next_id_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to set next segment id from kv store, key: {}:{}, cause: {}", next_id_key, next_id_str, status.message()));
//         return status;
//     }
//     return Status::OK();
// }

Status TableMeeta::SetUnsealedSegmentID(SegmentID unsealed_segment_id) {
    unsealed_segment_id_ = unsealed_segment_id;
    String unsealed_id_key = GetTableTag("unsealed_segment_id");
    String unsealed_id_str = fmt::format("{}", unsealed_segment_id);
    Status status = kv_instance_.Put(unsealed_id_key, unsealed_id_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to set unsealed segment id from kv store, {}:{}, cause: {}", unsealed_id_key, unsealed_id_str, status.message()));
        return status;
    }
    return Status::OK();
}

Tuple<ColumnID, Status> TableMeeta::GetColumnIDByColumnName(const String &column_name) {
    String column_key = KeyEncode::TableColumnKey(db_id_str_, table_id_str_, column_name);
    String column_value_str;
    Status status = kv_instance_.Get(column_key, column_value_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to get column id by column name: key: {}, cause: {}", column_key, status.message()));
        return {INVALID_COLUMN_ID, status};
    }
    auto column_def = ColumnDef::FromJson(nlohmann::json::parse(column_value_str));
    return {column_def->id_, Status::OK()};
}

SharedPtr<String> TableMeeta::GetTableDir() { return {MakeShared<String>(table_id_str_)}; }

// Tuple<SharedPtr<Vector<SegmentID>>, Status> TableMeeta::GetSegmentIndexIDs1() {
//     if (!segment_ids_) {
//         auto status = LoadSegmentIDs();
//         if (!status.ok()) {
//             //            if (status.code() == ErrorCode::kNotFound) {
//             //                return {MakeShared<Vector<SegmentID>>(), Status::OK()};
//             //            } else {
//             return {nullptr, status};
//             //            }
//         }
//     }
//     return {MakeShared<Vector<SegmentID>>(segment_ids_.value()), Status::OK()};
// }

Tuple<Vector<SegmentID> *, Status> TableMeeta::GetSegmentIDs1() {
    if (!segment_ids1_) {
        segment_ids1_ = infinity::GetTableSegments(&kv_instance_, db_id_str_, table_id_str_, begin_ts_);
    }
    return {&*segment_ids1_, Status::OK()};
}

Status TableMeeta::CheckSegments(const Vector<SegmentID> &segment_ids) {
    for (SegmentID segment_id : segment_ids) {
        String segment_key = KeyEncode::CatalogTableSegmentKey(db_id_str_, table_id_str_, segment_id);
        String commit_ts_str;
        Status status = kv_instance_.Get(segment_key, commit_ts_str);
        if (!status.ok()) {
            return status;
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

Status TableMeeta::GetNextRowID(RowID &next_row_id) {
    SegmentID unsealed_segment_id = 0;
    Status status = GetUnsealedSegmentID(unsealed_segment_id);
    if (!status.ok()) {
        if (status.code() != ErrorCode::kNotFound) {
            return status;
        }
        Vector<SegmentID> *segment_ids_ptr = nullptr;
        std::tie(segment_ids_ptr, status) = GetSegmentIDs1();
        if (!status.ok()) {
            return status;
        }
        SegmentID segment_id = segment_ids_ptr->empty() ? 0 : segment_ids_ptr->back() + 1;
        next_row_id = RowID(segment_id, 0);
        return Status::OK();
    }

    SegmentMeta segment_meta(unsealed_segment_id, *this);
    SizeT seg_row_cnt = 0;
    std::tie(seg_row_cnt, status) = segment_meta.GetRowCnt1();
    if (!status.ok()) {
        return status;
    }
    next_row_id = RowID(unsealed_segment_id, seg_row_cnt);
    return Status::OK();
}

} // namespace infinity
