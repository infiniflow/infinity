// Copyri1ht(C) 2024 InfiniFlow, Inc. All rights reserved.
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

import buffer_handle;
import meta_info;
import column_def;
import column_meta;
import fast_rough_filter;
import kv_code;
import kv_utility;

namespace infinity {

BlockMeta::BlockMeta(BlockID block_id, SegmentMeta &segment_meta)
    : begin_ts_(segment_meta.begin_ts()), kv_instance_(segment_meta.kv_instance()), segment_meta_(segment_meta), block_id_(block_id) {}

Status BlockMeta::GetBlockLock(SharedPtr<BlockLock> &block_lock) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    String block_lock_key = GetBlockTag("lock");
    Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

Status BlockMeta::InitSet() {
    // {
    //     Status status = SetRowCnt(0);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
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

Status BlockMeta::UninitSet(UsageFlag usage_flag) {
    // {
    //     String block_row_cnt_key = GetBlockTag("row_cnt");
    //     Status status = kv_instance_.Delete(block_row_cnt_key);
    //     if (!status.ok()) {
    //         return status;
    //     }
    // }
    if (usage_flag == UsageFlag::kOther) {
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        {
            String block_lock_key = GetBlockTag("lock");
            Status status = new_catalog->DropBlockLockByBlockKey(block_lock_key);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        String filter_key = GetBlockTag("fast_rough_filter");
        Status status = kv_instance_.Delete(filter_key);
        if (!status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
        }
    }

    if (usage_flag == UsageFlag::kOther) {
        auto [version_buffer, status] = this->GetVersionBuffer();
        if (!status.ok()) {
            return status;
        }
        version_buffer->PickForCleanup();
    }
    return Status::OK();
}

Tuple<BufferObj *, Status> BlockMeta::GetVersionBuffer() {
    if (!version_buffer_) {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

        SharedPtr<String> block_dir_ptr = this->GetBlockDir();
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ptr + "/" + String(BlockVersion::PATH);
        version_buffer_ = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer_ == nullptr) {
            return {nullptr, Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath))};
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
        block_dir_ = MakeShared<String>(
            fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_));
    }

    return block_dir_;
}

Tuple<Vector<ColumnID> *, Status> BlockMeta::GetBlockColumnIDs1() {
    if (!column_ids1_) {
        column_ids1_ = infinity::GetTableSegmentBlockColumns(&kv_instance_,
                                                             segment_meta_.table_meta().db_id_str(),
                                                             segment_meta_.table_meta().table_id_str(),
                                                             segment_meta_.segment_id(),
                                                             block_id_,
                                                             begin_ts_);
    }
    return {&*column_ids1_, Status::OK()};
}

String BlockMeta::GetBlockTag(const String &tag) const {
    TableMeeta &table_meta = segment_meta_.table_meta();
    return KeyEncode::CatalogTableSegmentBlockTagKey(table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_, tag);
}

Tuple<SizeT, Status> BlockMeta::GetRowCnt1() {
    if (row_cnt_) {
        return {*row_cnt_, Status::OK()};
    }
    Status status;

    SharedPtr<BlockLock> block_lock;
    status = this->GetBlockLock(block_lock);
    if (!status.ok()) {
        return {0, status};
    }
    BufferObj *version_buffer;
    std::tie(version_buffer, status) = this->GetVersionBuffer();
    if (!status.ok()) {
        return {0, status};
    }

    BufferHandle buffer_handle = version_buffer->Load();
    const auto *block_version = reinterpret_cast<const BlockVersion *>(buffer_handle.GetData());

    SizeT row_cnt = 0;
    {
        std::shared_lock lock(block_lock->mtx_);
        row_cnt = block_version->GetRowCount(begin_ts_);
    }
    row_cnt_ = row_cnt;
    return {row_cnt, Status::OK()};
}

Tuple<SharedPtr<BlockInfo>, Status> BlockMeta::GetBlockInfo() {
    SharedPtr<BlockInfo> block_info = MakeShared<BlockInfo>();
    auto [row_count, status] = this->GetRowCnt1();
    if (!status.ok()) {
        return {nullptr, status};
    }

    block_info->block_id_ = this->block_id();
    block_info->block_dir_ = this->GetBlockDir();
    block_info->row_count_ = row_count;
    block_info->row_capacity_ = this->block_capacity();
    block_info->checkpoint_row_count_ = 0; // TODO
    block_info->column_count_ = 0;         // TODO
    block_info->checkpoint_ts_ = 0;        // TODO
    block_info->storage_size_ = 0;         // TODO
    block_info->files_ = this->FilePaths();
    return {block_info, Status::OK()};
}

Tuple<SharedPtr<BlockColumnInfo>, Status> BlockMeta::GetBlockColumnInfo(ColumnID column_id) {
    SharedPtr<BlockColumnInfo> block_column_info = MakeShared<BlockColumnInfo>();
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs = nullptr;
    SharedPtr<ColumnDef> column_def = nullptr;
    Status status;
    SizeT select_column_idx = 0;
    std::tie(column_defs, status) = segment_meta_.table_meta().GetColumnDefs();
    if (!status.ok()) {
        return {nullptr, status};
    }
    for (auto &column_def_ptr : *column_defs) {
        if (ColumnID(column_def_ptr->id()) == column_id) {
            column_def = column_def_ptr;
            break;
        }
        select_column_idx++;
    }
    if (column_def == nullptr) {
        return {nullptr, Status::CatalogError("Column not found")};
    }
    ColumnMeta column_meta(select_column_idx, *this);
    Vector<String> file_paths;
    status = column_meta.FilePaths(file_paths);
    if (!status.ok()) {
        return {nullptr, status};
    }

    block_column_info->column_id_ = column_id;
    block_column_info->data_type_ = column_def->type();
    block_column_info->filename_ = nullptr; // TODO
    block_column_info->storage_size_ = 0;   // TODO
    block_column_info->extra_file_count_ = file_paths.size();
    for (auto &file_path : file_paths) {
        block_column_info->extra_file_names_.push_back(MakeShared<String>(file_path));
    }
    return {block_column_info, Status::OK()};
}

Status BlockMeta::GetFastRoughFilter(SharedPtr<FastRoughFilter> &fast_rough_filter) {
    fast_rough_filter.reset();
    if (fast_rough_filter_) {
        fast_rough_filter = fast_rough_filter_;
        return Status::OK();
    }

    String filter_key = GetBlockTag("fast_rough_filter");
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

Status BlockMeta::SetFastRoughFilter(SharedPtr<FastRoughFilter> fast_rough_filter) {
    String filter_key = GetBlockTag("fast_rough_filter");
    String filter_str = fast_rough_filter->SerializeToString();
    Status status = kv_instance_.Put(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = fast_rough_filter;
    return Status::OK();
}

} // namespace infinity
