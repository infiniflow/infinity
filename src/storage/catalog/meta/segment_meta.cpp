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

module segment_meta;

import kv_store;
import kv_code;
import third_party;
import default_values;
import table_meeta;
import logger;
import infinity_exception;

import block_meta;
import meta_info;
import segment_entry;
import new_catalog;
import fast_rough_filter;

namespace infinity {

SegmentMeta::SegmentMeta(SegmentID segment_id, TableMeeta &table_meta)
    : begin_ts_(table_meta.begin_ts()), kv_instance_(table_meta.kv_instance()), table_meta_(table_meta), segment_id_(segment_id) {}

// Status SegmentMeta::SetBlockIDs(const Vector<BlockID> &block_ids) {
//     block_ids_ = MakeShared<Vector<BlockID>>(block_ids);
//     String block_ids_key = GetSegmentTag("block_ids");
//     String block_ids_str = nlohmann::json(block_ids).dump();
//     Status status = kv_instance_.Put(block_ids_key, block_ids_str);
//     if (!status.ok()) {
//         return status;
//     }
//     return Status::OK();
// }

// Status SegmentMeta::AddBlockID(BlockID block_id) {
//     if (!block_ids_) {
//         Status status = LoadBlockIDs();
//         if (!status.ok()) {
//             return status;
//         }
//     }
//     block_ids_->push_back(block_id);
//     Status status = SetBlockIDs(*block_ids_);
//     if (!status.ok()) {
//         return status;
//     }
//     return Status::OK();
// }

Status SegmentMeta::SetNextBlockID(BlockID next_block_id) {
    next_block_id_ = next_block_id;
    String next_block_id_key = GetSegmentTag(String(LATEST_BLOCK_ID));
    String next_block_id_str = fmt::format("{}", next_block_id);
    Status status = kv_instance_.Put(next_block_id_key, next_block_id_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

// Status SegmentMeta::SetRowCnt(SizeT row_cnt) {
//     row_cnt_ = row_cnt;
//     String row_cnt_key = GetSegmentTag("row_cnt");
//     String row_cnt_str = fmt::format("{}", row_cnt);
//     Status status = kv_instance_.Put(row_cnt_key, row_cnt_str);
//     if (!status.ok()) {
//         return status;
//     }
//     return Status::OK();
// }

Status SegmentMeta::SetFirstDeleteTS(TxnTimeStamp first_delete_ts) {
    String first_delete_ts_key = GetSegmentTag("first_delete_ts");
    String first_delete_ts_str = fmt::format("{}", first_delete_ts);
    Status status = kv_instance_.Put(first_delete_ts_key, first_delete_ts_str);
    if (!status.ok()) {
        return status;
    }
    first_delete_ts_ = first_delete_ts;
    return Status::OK();
}

Status SegmentMeta::InitSet() {
    // {
    //     Status status = SetBlockIDs(Vector<BlockID>());
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    {
        Status status = SetNextBlockID(0);
        if (!status.ok()) {
            return status;
        }
    }
    // {
    //     Status status = SetRowCnt(0);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    {
        Status status = SetFirstDeleteTS(UNCOMMIT_TS);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentMeta::UninitSet() {
    // {
    //     String block_ids_key = GetSegmentTag("block_ids");
    //     Status status = kv_instance_.Delete(block_ids_key);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    {
        String block_id_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_);
        auto iter = kv_instance_.GetIterator();
        iter->Seek(block_id_prefix);
        Vector<String> delete_keys;
        while (iter->Valid() && iter->Key().starts_with(block_id_prefix)) {
            TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
            if (commit_ts > begin_ts_ and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
                BlockID block_id = std::stoull(iter->Key().ToString().substr(block_id_prefix.size()));
                UnrecoverableError(fmt::format("Block id: {}.{} is not allowed to be removed. commit_ts: {}, begin_ts: {}",
                                               segment_id_,
                                               block_id,
                                               commit_ts,
                                               begin_ts_));
            }
            // the key is committed before the txn or the key isn't committed
            delete_keys.push_back(iter->Key().ToString());
            iter->Next();
        }
        for (const String &key : delete_keys) {
            Status status = kv_instance_.Delete(key);
            if (!status.ok()) {
                return status;
            }
        }
        block_ids1_.reset();
    }
    {
        String next_block_id_key = GetSegmentTag(String(LATEST_BLOCK_ID));
        Status status = kv_instance_.Delete(next_block_id_key);
        if (!status.ok()) {
            return status;
        }
    }
    // {
    //     String row_cnt_key = GetSegmentTag("row_cnt");
    //     Status status = kv_instance_.Delete(row_cnt_key);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    {
        String first_delete_ts_key = GetSegmentTag("first_delete_ts");
        Status status = kv_instance_.Delete(first_delete_ts_key);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String filter_key = GetSegmentTag("fast_rough_filter");
        Status status = kv_instance_.Delete(filter_key);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
        }
    }
    return Status::OK();
}

// Status SegmentMeta::LoadBlockIDs() {
//     String block_ids_key = GetSegmentTag("block_ids");
//     String block_ids_str;
//     Status status = kv_instance_.Get(block_ids_key, block_ids_str);
//     if (!status.ok()) {
//         return status;
//     }
//     block_ids_ = MakeShared<Vector<BlockID>>(nlohmann::json::parse(block_ids_str).get<Vector<BlockID>>());
//     return Status::OK();
// }

Status SegmentMeta::LoadBlockIDs1() {
    block_ids1_ = Vector<BlockID>();
    Vector<BlockID> &block_ids = *block_ids1_;

    String block_id_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(block_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(block_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts > begin_ts_ and commit_ts != std::numeric_limits<TxnTimeStamp>::max()) {
            iter->Next();
            continue;
        }
        // the key is committed before the txn or the key isn't committed
        BlockID block_id = std::stoull(iter->Key().ToString().substr(block_id_prefix.size()));
        block_ids.push_back(block_id);
        iter->Next();
    }

    std::sort(block_ids.begin(), block_ids.end());
    return Status::OK();
}

Status SegmentMeta::LoadNextBlockID() {
    String next_block_id_key = GetSegmentTag(String(LATEST_BLOCK_ID));
    String next_block_id_str;
    Status status = kv_instance_.Get(next_block_id_key, next_block_id_str);
    if (!status.ok()) {
        return status;
    }
    next_block_id_ = std::stoull(next_block_id_str);
    return Status::OK();
}

// Status SegmentMeta::LoadRowCnt() {
//     String row_cnt_key = GetSegmentTag("row_cnt");
//     String row_cnt_str;
//     Status status = kv_instance_.Get(row_cnt_key, row_cnt_str);
//     if (!status.ok()) {
//         return status;
//     }
//     row_cnt_ = std::stoull(row_cnt_str);
//     return Status::OK();
// }

Status SegmentMeta::LoadFirstDeleteTS() {
    String first_delete_ts_key = GetSegmentTag("first_delete_ts");
    String first_delete_ts_str;
    Status status = kv_instance_.Get(first_delete_ts_key, first_delete_ts_str);
    if (!status.ok()) {
        return status;
    }
    first_delete_ts_ = std::stoull(first_delete_ts_str);
    return Status::OK();
}

String SegmentMeta::GetSegmentTag(const String &tag) const {
    return KeyEncode::CatalogTableSegmentTagKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, tag);
}

Status SegmentMeta::Init() {
    // {
    //     table_meta_.AddSegmentID(segment_id_);
    // }
    {
        String latest_block_id_key = GetSegmentTag(String(LATEST_BLOCK_ID));
        String latest_block_id_str;
        Status status = kv_instance_.Get(latest_block_id_key, latest_block_id_str);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
            status = kv_instance_.Put(latest_block_id_key, "0");
            if (!status.ok()) {
                return status;
            }
        }
    }
    // {
    //     String block_ids_key = GetSegmentTag("block_ids");
    //     String block_ids_str = nlohmann::json::array().dump();
    //     Status status = kv_instance_.Put(block_ids_key, block_ids_str);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    {

        String row_cnt_key = GetSegmentTag("row_cnt");
        String row_cnt_str;
        Status status = kv_instance_.Get(row_cnt_key, row_cnt_str);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
            status = kv_instance_.Put(row_cnt_key, "0");
            if (!status.ok()) {
                return status;
            }
        }
    }
    return Status::OK();
}

// Status SegmentMeta::AddBlockID(BlockID block_id) {
//     if (block_ids_ == nullptr) {
//         Status status = LoadBlockIDs();
//         if (!status.ok()) {
//             return status;
//         }
//     }

//     block_ids_->push_back(block_id);
//     String block_ids_key = GetSegmentTag("block_ids");
//     String block_ids_str = nlohmann::json(*block_ids_).dump();
//     Status status = kv_instance_.Put(block_ids_key, block_ids_str);
//     if (!status.ok()) {
//         LOG_ERROR(fmt::format("Fail to add block_id: {} to key: {}", block_id, block_ids_key));
//         return status;
//     }
//     return Status::OK();
// }

Pair<BlockID, Status> SegmentMeta::AddBlockID1(TxnTimeStamp commit_ts) {
    Status status;

    BlockID block_id = 0;
    {
        Vector<BlockID> *block_ids_ptr = nullptr;
        std::tie(block_ids_ptr, status) = GetBlockIDs1();
        if (!status.ok()) {
            return {INVALID_BLOCK_ID, status};
        }
        block_id = block_ids_ptr->empty() ? 0 : block_ids_ptr->back() + 1;
        block_ids1_->push_back(block_id);
    }

    String block_id_key = KeyEncode::CatalogTableSegmentBlockKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, block_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance_.Put(block_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {block_id, Status::OK()};
}

Status SegmentMeta::CommitBlock(BlockID block_id, TxnTimeStamp commit_ts) {
    String block_id_key = KeyEncode::CatalogTableSegmentBlockKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, block_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    Status status = kv_instance_.Put(block_id_key, commit_ts_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Tuple<SharedPtr<String>, Status> SegmentMeta::GetSegmentDir() {
    String seg_dir_key = GetSegmentTag("dir");
    String seg_dir;
    Status status = kv_instance_.Get(seg_dir_key, seg_dir);
    if (!status.ok()) {
        return {nullptr, status};
    }
    segment_dir_ = seg_dir;
    return {MakeShared<String>(seg_dir), Status::OK()};
}

// Tuple<SharedPtr<Vector<BlockID>>, Status> SegmentMeta::GetBlockIDs() {
//     if (block_ids_ == nullptr) {
//         Status status = LoadBlockIDs();
//         if (!status.ok()) {
//             return {nullptr, status};
//         }
//     }
//     return {MakeShared<Vector<BlockID>>(*block_ids_), Status::OK()};
// }

Tuple<Vector<BlockID> *, Status> SegmentMeta::GetBlockIDs1() {
    if (!block_ids1_) {
        Status status = LoadBlockIDs1();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {&*block_ids1_, Status::OK()};
}

// Tuple<SizeT, Status> SegmentMeta::GetRowCnt() {
//     if (!row_cnt_) {
//         Status status = LoadRowCnt();
//         if (!status.ok()) {
//             return {0, status};
//         }
//     }
//     return {row_cnt_.value(), Status::OK()};
// }

Tuple<SizeT, Status> SegmentMeta::GetRowCnt1() {
    if (row_cnt_) {
        return {*row_cnt_, Status::OK()};
    }
    Status status;

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
}

Tuple<BlockID, Status> SegmentMeta::GetNextBlockID() {
    if (!next_block_id_) {
        Status status = LoadNextBlockID();
        if (!status.ok()) {
            return {INVALID_BLOCK_ID, status};
        }
    }
    return {*next_block_id_, Status::OK()};
}

Status SegmentMeta::GetFirstDeleteTS(TxnTimeStamp &first_delete_ts) {
    if (!first_delete_ts_) {
        Status status = LoadFirstDeleteTS();
        if (!status.ok()) {
            return status;
        }
    }
    if (*first_delete_ts_ > begin_ts_) {
        first_delete_ts = UNCOMMIT_TS;
    }
    first_delete_ts = *first_delete_ts_;
    return Status::OK();
}

Tuple<SharedPtr<SegmentInfo>, Status> SegmentMeta::GetSegmentInfo() {
    auto segment_info = MakeShared<SegmentInfo>();
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs = nullptr;
    i64 column_count = 0;
    SegmentID unsealed_segment_id = 0;
    Status status;
    std::tie(column_defs, status) = table_meta_.GetColumnDefs();
    if (!status.ok()) {
        return {nullptr, status};
    }
    column_count = column_defs->size();
    status = table_meta_.GetUnsealedSegmentID(unsealed_segment_id);
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
    std::tie(block_ids_ptr, status) = GetBlockIDs1();
    if (!status.ok()) {
        return {nullptr, status};
    }
    segment_info->block_count_ = block_ids_ptr->size();
    std::tie(segment_info->row_count_, status) = GetRowCnt1();
    if (!status.ok()) {
        return {nullptr, status};
    }
    segment_info->actual_row_count_ = segment_info->row_count_;
    for (BlockID block_id : *block_ids_ptr) {
        BlockMeta block_meta(block_id, *this);
        Vector<String> file_paths;
        status = NewCatalog::GetBlockFilePaths(block_meta, file_paths, nullptr);
        if (!status.ok()) {
            return {nullptr, status};
        }
        segment_info->files_.insert(segment_info->files_.end(), file_paths.begin(), file_paths.end());
    }
    return {std::move(segment_info), Status::OK()};
}

Status SegmentMeta::GetFastRoughFilter(SharedPtr<FastRoughFilter> &fast_rough_filter) {
    fast_rough_filter.reset();

    std::unique_lock lock(mtx_);

    if (fast_rough_filter_) {
        fast_rough_filter = fast_rough_filter_;
        return Status::OK();
    }

    String filter_key = GetSegmentTag("fast_rough_filter");
    String filter_str;
    Status status = kv_instance_.Get(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = MakeShared<FastRoughFilter>();
    fast_rough_filter_->DeserializeFromString(filter_str);
    fast_rough_filter = fast_rough_filter_;

    return Status::OK();
}

Status SegmentMeta::SetFastRoughFilter(SharedPtr<FastRoughFilter> fast_rough_filter) {
    String filter_key = GetSegmentTag("fast_rough_filter");
    String filter_str = fast_rough_filter->SerializeToString();
    Status status = kv_instance_.Put(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = fast_rough_filter;
    return Status::OK();
}

} // namespace infinity
