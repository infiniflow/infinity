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

namespace infinity {

SegmentMeta::SegmentMeta(SegmentID segment_id, TableMeeta &table_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), table_meta_(table_meta), segment_id_(segment_id) {}

Status SegmentMeta::SetBlockIDs(const Vector<BlockID> &block_ids) {
    block_ids_ = MakeShared<Vector<BlockID>>(block_ids);
    String block_ids_key = GetSegmentTag("block_ids");
    String block_ids_str = nlohmann::json(block_ids).dump();
    Status status = kv_instance_.Put(block_ids_key, block_ids_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

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

Status SegmentMeta::SetRowCnt(SizeT row_cnt) {
    row_cnt_ = row_cnt;
    String row_cnt_key = GetSegmentTag("row_cnt");
    String row_cnt_str = fmt::format("{}", row_cnt);
    Status status = kv_instance_.Put(row_cnt_key, row_cnt_str);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status SegmentMeta::InitSet() {
    {
        Status status = SetBlockIDs(Vector<BlockID>());
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetNextBlockID(0);
        if (!status.ok()) {
            return status;
        }
    }
    {
        Status status = SetRowCnt(0);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentMeta::UninitSet() {
    {
        String block_ids_key = GetSegmentTag("block_ids");
        Status status = kv_instance_.Delete(block_ids_key);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String next_block_id_key = GetSegmentTag(String(LATEST_BLOCK_ID));
        Status status = kv_instance_.Delete(next_block_id_key);
        if (!status.ok()) {
            return status;
        }
    }
    {
        String row_cnt_key = GetSegmentTag("row_cnt");
        Status status = kv_instance_.Delete(row_cnt_key);
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status SegmentMeta::LoadBlockIDs() {
    String block_ids_key = GetSegmentTag("block_ids");
    String block_ids_str;
    Status status = kv_instance_.Get(block_ids_key, block_ids_str);
    if (!status.ok()) {
        return status;
    }
    block_ids_ = MakeShared<Vector<BlockID>>(nlohmann::json::parse(block_ids_str).get<Vector<BlockID>>());
    return Status::OK();
}

Status SegmentMeta::LoadBlockIDs1(TxnTimeStamp begin_ts) {
    Vector<BlockID> block_ids;

    String block_id_prefix = KeyEncode::CatalogTableSegmentBlockKeyPrefix(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_);
    auto iter = kv_instance_.GetIterator();
    iter->Seek(block_id_prefix);
    while (iter->Valid() && iter->Key().starts_with(block_id_prefix)) {
        TxnTimeStamp commit_ts = std::stoull(iter->Value().ToString());
        if (commit_ts < begin_ts) {
            iter->Next();
            continue;
        }
        BlockID block_id = std::stoull(iter->Key().ToString().substr(block_id_prefix.size()));
        block_ids.push_back(block_id);
        iter->Next();
    }

    std::sort(block_ids.begin(), block_ids.end());
    block_ids_ = MakeShared<Vector<BlockID>>(std::move(block_ids));

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

Status SegmentMeta::LoadRowCnt() {
    String row_cnt_key = GetSegmentTag("row_cnt");
    String row_cnt_str;
    Status status = kv_instance_.Get(row_cnt_key, row_cnt_str);
    if (!status.ok()) {
        return status;
    }
    row_cnt_ = std::stoull(row_cnt_str);
    return Status::OK();
}

String SegmentMeta::GetSegmentTag(const String &tag) const {
    return KeyEncode::CatalogTableSegmentTagKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, tag);
}

Status SegmentMeta::Init() {
    {
        table_meta_.AddSegmentID(segment_id_);
    }
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
    {
        String block_ids_key = GetSegmentTag("block_ids");
        String block_ids_str = nlohmann::json::array().dump();
        Status status = kv_instance_.Put(block_ids_key, block_ids_str);
        if (!status.ok()) {
            return status;
        }
    }
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

Status SegmentMeta::AddBlockID(BlockID block_id) {
    if (block_ids_ == nullptr) {
        Status status = LoadBlockIDs();
        if (!status.ok()) {
            return status;
        }
    }

    block_ids_->push_back(block_id);
    String block_ids_key = GetSegmentTag("block_ids");
    String block_ids_str = nlohmann::json(*block_ids_).dump();
    Status status = kv_instance_.Put(block_ids_key, block_ids_str);
    if (!status.ok()) {
        LOG_ERROR(fmt::format("Fail to add block_id: {} to key: {}", block_id, block_ids_key));
        return status;
    }
    return Status::OK();
}

Pair<BlockID, Status> SegmentMeta::AddBlockID1(TxnTimeStamp commit_ts) {
    Status status;

    BlockID block_id = 0;
    {
        SharedPtr<Vector<BlockID>> block_ids;
        std::tie(block_ids, status) = GetBlockIDs();
        if (!status.ok()) {
            return {INVALID_BLOCK_ID, status};
        }
        block_id = block_ids->empty() ? 0 : block_ids->back() + 1;
        block_ids_->push_back(block_id);
    }

    String block_id_key = KeyEncode::CatalogTableSegmentBlockKey(table_meta_.db_id_str(), table_meta_.table_id_str(), segment_id_, block_id);
    String commit_ts_str = fmt::format("{}", commit_ts);
    status = kv_instance_.Put(block_id_key, commit_ts_str);
    if (!status.ok()) {
        return {0, status};
    }
    return {block_id, Status::OK()};
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

Tuple<SharedPtr<Vector<BlockID>>, Status> SegmentMeta::GetBlockIDs() {
    if (block_ids_ == nullptr) {
        Status status = LoadBlockIDs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {MakeShared<Vector<BlockID>>(*block_ids_), Status::OK()};
}

Tuple<SharedPtr<Vector<BlockID>>, Status> SegmentMeta::GetBlockIDs1(TxnTimeStamp begin_ts) {
    if (block_ids_ == nullptr) {
        Status status = LoadBlockIDs1(begin_ts);
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {block_ids_, Status::OK()};
}

Tuple<SizeT, Status> SegmentMeta::GetRowCnt() {
    if (!row_cnt_) {
        Status status = LoadRowCnt();
        if (!status.ok()) {
            return {0, status};
        }
    }
    return {row_cnt_.value(), Status::OK()};
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

} // namespace infinity
