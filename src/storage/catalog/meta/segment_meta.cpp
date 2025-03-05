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

namespace infinity {

SegmentMeta::SegmentMeta(SegmentID segment_id, TableMeeta &table_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), table_meta_(table_meta), segment_id_(segment_id) {}

Status SegmentMeta::SetBlockIDs(const Vector<BlockID> &block_ids) {
    block_ids_ = block_ids;
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

Status SegmentMeta::LoadBlockIDs() {
    String block_ids_key = GetSegmentTag("block_ids");
    String block_ids_str;
    Status status = kv_instance_.Get(block_ids_key, block_ids_str);
    if (!status.ok()) {
        return status;
    }
    block_ids_ = nlohmann::json::parse(block_ids_str).get<Vector<BlockID>>();
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
    if (!block_ids_) {
        Status status = LoadBlockIDs();
        if (!status.ok()) {
                return status;
            }
        }

    block_ids_->push_back(block_id);
    String block_ids_key = GetSegmentTag("block_ids");
    String block_ids_str = nlohmann::json(block_ids_.value()).dump();
    Status status = kv_instance_.Put(block_ids_key, block_ids_str);
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

Tuple<SharedPtr<Vector<BlockID>>, Status> SegmentMeta::GetBlockIDs() {
    if (!block_ids_) {
        Status status = LoadBlockIDs();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {MakeShared<Vector<BlockID>>(block_ids_.value()), Status::OK()};
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

} // namespace infinity
