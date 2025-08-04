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

#include <string>

module infinity_core:segment_meta.impl;

import :segment_meta;
import :kv_store;
import :kv_code;
import :third_party;
import :default_values;
import :table_meeta;
import :logger;
import :infinity_exception;

import :block_meta;
import :meta_info;
import :new_catalog;
import :fast_rough_filter;
import column_def;
import :kv_utility;
import :meta_type;

namespace infinity {

SegmentMeta::SegmentMeta(SegmentID segment_id, TableMeeta &table_meta)
    : BaseMeta(MetaType::kSegment), table_meta_(table_meta), segment_id_(segment_id) {}

Status SegmentMeta::SetFirstDeleteTS(KVInstance *kv_instance, TxnTimeStamp first_delete_ts) {
    String first_delete_ts_key = GetSegmentTag("first_delete_ts");
    String first_delete_ts_str = fmt::format("{}", first_delete_ts);
    Status status = kv_instance->Put(first_delete_ts_key, first_delete_ts_str);
    if (!status.ok()) {
        return status;
    }
    first_delete_ts_ = first_delete_ts;
    return Status::OK();
}

Status SegmentMeta::InitSet(KVInstance *kv_instance) {
    {
        Status status = SetFirstDeleteTS(kv_instance, UNCOMMIT_TS);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentMeta::UninitSet(UsageFlag usage_flag, KVInstance *kv_instance, TxnTimeStamp begin_ts) {
    {
        String block_id_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_);
        auto iter = kv_instance->GetIterator();
        iter->Seek(block_id_prefix);
        Vector<String> delete_keys;
        while (iter->Valid() && iter->Key().starts_with(block_id_prefix)) {
            TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
            if (commit_ts > begin_ts and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                BlockID block_id = std::stoull(iter->Key().ToString().substr(block_id_prefix.size()));
                UnrecoverableError(fmt::format("Block id: {}.{} is not allowed to be removed. commit_ts: {}, begin_ts: {}",
                                               segment_id_,
                                               block_id,
                                               commit_ts,
                                               begin_ts));
            }
            // the key is committed before the txn or the key isn't committed
            delete_keys.push_back(iter->Key().ToString());
            iter->Next();
        }
        for (const String &key : delete_keys) {
            Status status = kv_instance->Delete(key);
            if (!status.ok()) {
                return status;
            }
        }
        block_ids1_.reset();
    }
    {
        String first_delete_ts_key = GetSegmentTag("first_delete_ts");
        Status status = kv_instance->Delete(first_delete_ts_key);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String filter_key = GetSegmentTag("fast_rough_filter");
        Status status = kv_instance->Delete(filter_key);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
        }
    }
    return Status::OK();
}

Status SegmentMeta::LoadFirstDeleteTS(KVInstance *kv_instance) {
    String first_delete_ts_key = GetSegmentTag("first_delete_ts");
    String first_delete_ts_str;
    Status status = kv_instance->Get(first_delete_ts_key, first_delete_ts_str);
    if (!status.ok()) {
        return status;
    }
    first_delete_ts_ = std::stoull(first_delete_ts_str);
    return Status::OK();
}

String SegmentMeta::GetSegmentTag(const String &tag) const {
    return KeyEncode::CatalogTableSegmentTagKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, tag);
}

Status SegmentMeta::AddBlockWithID(KVInstance *kv_instance, TxnTimeStamp commit_ts, BlockID block_id) {
    Status status;
    String block_id_key = KeyEncode::CatalogTableSegmentBlockKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, block_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance->Put(block_id_key, commit_ts_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Pair<BlockID, Status> SegmentMeta::AddBlockID1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    Status status;

    BlockID block_id = 0;
    {
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = GetBlockIDs1(kv_instance, begin_ts, commit_ts);
        if (!status.ok()) {
            return {INVALID_BLOCK_ID, status};
        }
        block_id = block_ids_ptr->empty() ? 0 : block_ids_ptr->back() + 1;
        block_ids1_->push_back(block_id);
    }

    String block_id_key = KeyEncode::CatalogTableSegmentBlockKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, block_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance->Put(block_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {block_id, Status::OK()};
}

Status SegmentMeta::CommitBlock(KVInstance *kv_instance, BlockID block_id, TxnTimeStamp commit_ts) {
    String block_id_key = KeyEncode::CatalogTableSegmentBlockKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, block_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    Status status = kv_instance->Put(block_id_key, commit_ts_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Tuple<SharedPtr<String>, Status> SegmentMeta::GetSegmentDir(KVInstance *kv_instance) {
    String seg_dir_key = GetSegmentTag("dir");
    String seg_dir;
    Status status = kv_instance->Get(seg_dir_key, seg_dir);
    if (!status.ok()) {
        return {nullptr, status};
    }
    segment_dir_ = seg_dir;
    return {MakeShared<String>(seg_dir), Status::OK()};
}

Tuple<Vector<BlockID> *, Status> SegmentMeta::GetBlockIDs1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    if (!block_ids1_) {
        block_ids1_ =
            infinity::GetTableSegmentBlocks(kv_instance, table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, begin_ts, commit_ts);
    }
    return {&*block_ids1_, Status::OK()};
}

Tuple<SizeT, Status> SegmentMeta::GetRowCnt1(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (row_cnt_) {
        return {*row_cnt_, Status::OK()};
    }
    Status status;
#if 1
    row_cnt_ = infinity::GetSegmentRowCount(kv_instance, table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, begin_ts, commit_ts);
    return {*row_cnt_, Status::OK()};
#else
    SizeT row_cnt = 0;
    {
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = GetBlockIDs1();
        if (block_ids_ptr->size() > 0) {
            row_cnt += DEFAULT_BLOCK_CAPACITY * (block_ids_ptr->size() - 1);

            BlockMeta block_meta(block_ids_ptr->back(), *this);
            SizeT block_row_cnt;
            std::tie(block_row_cnt, status) = block_meta.GetRowCnt1();
            if (!status.ok()) {
                return {0, status};
            }
            row_cnt += block_row_cnt;
        }
    }
    row_cnt_ = row_cnt;
    return {row_cnt, Status::OK()};
#endif
}

Status SegmentMeta::GetFirstDeleteTS(KVInstance *kv_instance, TxnTimeStamp &first_delete_ts, TxnTimeStamp begin_ts) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!first_delete_ts_) {
        Status status = LoadFirstDeleteTS(kv_instance);
        if (!status.ok()) {
            return status;
        }
    }
    if (*first_delete_ts_ > begin_ts) {
        first_delete_ts = UNCOMMIT_TS;
    }
    first_delete_ts = *first_delete_ts_;
    return Status::OK();
}

Tuple<SharedPtr<SegmentInfo>, Status> SegmentMeta::GetSegmentInfo(KVInstance *kv_instance, TxnTimeStamp begin_ts, TxnTimeStamp commit_ts) {
    auto segment_info = MakeShared<SegmentInfo>();
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs = nullptr;
    i64 column_count = 0;
    SegmentID unsealed_segment_id = 0;
    Status status;
    std::tie(column_defs, status) = table_meta_.GetColumnDefs(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }
    column_count = column_defs->size();
    status = table_meta_.GetUnsealedSegmentID(kv_instance, unsealed_segment_id);
    if (!status.ok()) {
        unsealed_segment_id = INVALID_SEGMENT_ID;
    }

    segment_info->segment_id_ = segment_id_;
    segment_info->status_ =
        (segment_id_ == unsealed_segment_id) ? SegmentStatus::kUnsealed : SegmentStatus::kSealed; // TODO: How to determine other status?
    segment_info->column_count_ = column_count;
    segment_info->row_capacity_ = segment_capacity();
    segment_info->storage_size_ = 0; // TODO: How to determine storage size?
    Vector<BlockID> *block_ids_ptr = nullptr;
    std::tie(block_ids_ptr, status) = GetBlockIDs1(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }
    segment_info->block_count_ = block_ids_ptr->size();
    std::tie(segment_info->row_count_, status) = GetRowCnt1(kv_instance, begin_ts, commit_ts);
    if (!status.ok()) {
        return {nullptr, status};
    }
    segment_info->actual_row_count_ = segment_info->row_count_;
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, *this);
        Vector<String> file_paths;
        status = NewCatalog::GetBlockFilePaths(kv_instance, begin_ts, commit_ts, block_meta, file_paths, nullptr);
        if (!status.ok()) {
            return {nullptr, status};
        }
        segment_info->files_.insert(segment_info->files_.end(), file_paths.begin(), file_paths.end());
    }
    return {std::move(segment_info), Status::OK()};
}

Status SegmentMeta::GetFastRoughFilter(KVInstance *kv_instance, SharedPtr<FastRoughFilter> &fast_rough_filter) {
    fast_rough_filter.reset();

    std::unique_lock lock(mtx_);

    if (fast_rough_filter_) {
        fast_rough_filter = fast_rough_filter_;
        return Status::OK();
    }

    String filter_key = GetSegmentTag("fast_rough_filter");
    String filter_str;
    Status status = kv_instance->Get(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = MakeShared<FastRoughFilter>();
    fast_rough_filter_->DeserializeFromString(filter_str);
    fast_rough_filter = fast_rough_filter_;

    return Status::OK();
}

Status SegmentMeta::SetFastRoughFilter(KVInstance *kv_instance, SharedPtr<FastRoughFilter> fast_rough_filter) {
    String filter_key = GetSegmentTag("fast_rough_filter");
    String filter_str = fast_rough_filter->SerializeToString();
    std::lock_guard<std::mutex> lock(mtx_);
    Status status = kv_instance->Put(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = fast_rough_filter;
    return Status::OK();
}

} // namespace infinity
