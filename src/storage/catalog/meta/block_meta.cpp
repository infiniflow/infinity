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

module block_meta;

import kv_code;
import third_party;
import kv_store;
import segment_meta;
import table_meeta;
import new_catalog;
import infinity_context;

namespace infinity {

BlockMeta::BlockMeta(BlockID block_id, SegmentMeta &segment_meta, KVInstance &kv_instance)
    : kv_instance_(kv_instance), segment_meta_(segment_meta), block_id_(block_id) {}

Status BlockMeta::GetBlockLock(SharedPtr<BlockLock> &block_lock) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    String block_lock_key = GetBlockTag("lock");
    Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status BlockMeta::SetRowCnt(SizeT row_cnt) {
    row_cnt_ = row_cnt;
    String block_dir_key = GetBlockTag("row_cnt");
    Status status = kv_instance_.Put(block_dir_key, fmt::format("{}", row_cnt));
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status BlockMeta::InitSet() {
    TableMeeta &table_meta = segment_meta_.table_meta();
    SharedPtr<String> table_dir_ptr{};
    {
        auto [table_dir, status] = table_meta.GetTableDir();
        if (!status.ok()) {
            return status;
        }
        table_dir_ptr = table_dir;
    }
    block_dir_ = MakeShared<String>(fmt::format("tbl_{}/seg_{}/block_{}", *table_dir_ptr, segment_meta_.segment_id(), block_id_));
    {
        String block_dir_key = GetBlockTag("dir");
        String block_dir_str;
        Status status = kv_instance_.Get(block_dir_key, block_dir_str);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }

            Status status = kv_instance_.Put(block_dir_key, *block_dir_);
            if (!status.ok()) {
                return status;
            }
        }
    }

    {
        String block_row_cnt_key = GetBlockTag("row_cnt");
        String block_row_cnt_str;
        Status status = kv_instance_.Get(block_row_cnt_key, block_row_cnt_str);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }

            Status status = kv_instance_.Put(block_row_cnt_key, "0");
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        segment_meta_.AddBlockID(block_id_);
    }
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        String block_lock_key = GetBlockTag("lock");
        Status status = new_catalog->AddBlockLock(std::move(block_lock_key));
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}

Status BlockMeta::LoadBlockDir() {
    String block_dir_key = GetBlockTag("dir");
    block_dir_ = MakeShared<String>();
    Status status = kv_instance_.Get(block_dir_key, *block_dir_);
    return status;
}

Status BlockMeta::LoadRowCnt() {
    String row_cnt_key = GetBlockTag("row_cnt");
    String row_cnt_str;
    Status status = kv_instance_.Get(row_cnt_key, row_cnt_str);
    if (!status.ok()) {
        return status;
    }
    row_cnt_ = std::stoull(row_cnt_str);
    return Status::OK();
}

String BlockMeta::GetBlockTag(const String &tag) const {
    TableMeeta &table_meta = segment_meta_.table_meta();
    return KeyEncode::CatalogTableSegmentBlockTagKey(table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_, tag);
}

} // namespace infinity
