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

import buffer_manager;
import block_version;
import version_file_worker;

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
    {
        Status status = SetRowCnt(0);
        if (!status.ok()) {
            return status;
        }
    }
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        String block_lock_key = GetBlockTag("lock");
        Status status = new_catalog->AddBlockLock(std::move(block_lock_key));
        if (!status.ok()) {
            return status;
        }
    }
    SharedPtr<String> block_dir_ptr = this->GetBlockDir();
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    {
        auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                                 MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                                 block_dir_ptr,
                                                                 BlockVersion::FileName(),
                                                                 this->block_capacity(),
                                                                 buffer_mgr->persistence_manager());
        version_buffer_ = buffer_mgr->AllocateBufferObject(std::move(version_file_worker));
        if (!version_buffer_) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_file_worker->GetFilePath()));
        }
        version_buffer_->AddObjRc();
    }
    return Status::OK();
}

Status BlockMeta::LoadSet(TxnTimeStamp checkpoint_ts) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        String block_lock_key = GetBlockTag("lock");
        Status status = new_catalog->AddBlockLock(std::move(block_lock_key), checkpoint_ts);
        if (!status.ok()) {
            return status;
        }
    }
    auto *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    SharedPtr<String> block_dir_ptr = this->GetBlockDir();
    auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             BlockVersion::FileName(),
                                                             this->block_capacity(),
                                                             buffer_mgr->persistence_manager());
    version_buffer_ = buffer_mgr->GetBufferObject(std::move(version_file_worker));
    if (!version_buffer_) {
        return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_file_worker->GetFilePath()));
    }
    version_buffer_->AddObjRc();

    return Status::OK();
}

Status BlockMeta::UninitSet() {
    {
        String block_row_cnt_key = GetBlockTag("row_cnt");
        Status status = kv_instance_.Delete(block_row_cnt_key);
        if (!status.ok()) {
            return status;
        }
    }
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        String block_lock_key = GetBlockTag("lock");
        Status status = new_catalog->DropBlockLockByBlockKey(block_lock_key);
        if (!status.ok()) {
            return status;
        }
    }

    auto [version_buffer, status] = this->GetVersionBuffer();
    if (!status.ok()) {
        return status;
    }
    version_buffer->PickForCleanup();
    return Status::OK();
}

Tuple<BufferObj *, Status> BlockMeta::GetVersionBuffer() {
    if (!version_buffer_) {
        Status status = LoadVersionBuffer();
        if (!status.ok()) {
            return {nullptr, status};
        }
    }
    return {version_buffer_, Status::OK()};
}

Vector<String> BlockMeta::FilePaths() {
    Vector<String> res;
    SharedPtr<String> block_dir_ptr = this->GetBlockDir();
    String version_filepath = *block_dir_ptr + "/" + String(BlockVersion::PATH);
    res.push_back(version_filepath);
    return res;
}

SharedPtr<String> BlockMeta::GetBlockDir() {
    if (block_dir_ == nullptr) {
        TableMeeta &table_meta = segment_meta_.table_meta();
        block_dir_ = MakeShared<String>(fmt::format("tbl_{}/seg_{}/block_{}", table_meta.table_id_str(), segment_meta_.segment_id(), block_id_));
    }

    return block_dir_;
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

Status BlockMeta::LoadVersionBuffer() {
    BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

    SharedPtr<String> block_dir_ptr = this->GetBlockDir();
    String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
    version_buffer_ = buffer_mgr->GetBufferObject(version_filepath);
    if (version_buffer_ == nullptr) {
        return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath));
    }

    return Status::OK();
}

String BlockMeta::GetBlockTag(const String &tag) const {
    TableMeeta &table_meta = segment_meta_.table_meta();
    return KeyEncode::CatalogTableSegmentBlockTagKey(table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_, tag);
}

Tuple<SizeT, Status> BlockMeta::GetRowCnt() {
    if (!row_cnt_) {
        Status status = LoadRowCnt();
        if (!status.ok()) {
            return {0, status};
        }
    }
    return {*row_cnt_, Status::OK()};
}

} // namespace infinity
