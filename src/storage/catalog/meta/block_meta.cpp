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
import kv_utility;
import logger;
import snapshot_info;
import new_txn_manager;

namespace infinity {

BlockMeta::BlockMeta(BlockID block_id, SegmentMeta &segment_meta)
    : begin_ts_(segment_meta.begin_ts()), commit_ts_(segment_meta.commit_ts()), kv_instance_(segment_meta.kv_instance()), segment_meta_(segment_meta),
      block_id_(block_id) {}

Status BlockMeta::GetBlockLock(SharedPtr<BlockLock> &block_lock) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    String block_lock_key = GetBlockTag("lock");
    Status status = new_catalog->GetBlockLock(block_lock_key, block_lock);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}


TxnTimeStamp BlockMeta::GetCreateTimestampFromKV() const {
    String block_key = KeyEncode::CatalogTableSegmentBlockKey(segment_meta_.table_meta().db_id_str(), 
                                                              segment_meta_.table_meta().table_id_str(), 
                                                              segment_meta_.segment_id(), 
                                                              block_id_);
    String create_ts_str;
    Status status = kv_instance_.Get(block_key, create_ts_str);
    if (!status.ok()) {
        return 0;
    }
    return std::stoull(create_ts_str);
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

Status BlockMeta::RestoreSet() {
    auto *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();
    SharedPtr<String> block_dir_ptr = this->GetBlockDir();
    auto version_file_worker = MakeUnique<VersionFileWorker>(MakeShared<String>(InfinityContext::instance().config()->DataDir()),
                                                             MakeShared<String>(InfinityContext::instance().config()->TempDir()),
                                                             block_dir_ptr,
                                                             BlockVersion::FileName(),
                                                             this->block_capacity(),
                                                             buffer_mgr->persistence_manager());
    auto *buffer_obj = buffer_mgr->GetBufferObject(version_file_worker->GetFilePath());
    if (buffer_obj == nullptr) {
        version_buffer_ = buffer_mgr->GetBufferObject(std::move(version_file_worker));
        if (!version_buffer_) {
            return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_file_worker->GetFilePath()));
        }
        version_buffer_->AddObjRc();
    }
    return Status::OK();
}

Status BlockMeta::RestoreSetFromSnapshot() {
    // TODO: need to fix this
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        String block_lock_key = GetBlockTag("lock");

        Status status = new_catalog->AddBlockLock(std::move(block_lock_key));
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

    BufferHandle buffer_handle = version_buffer_->Load();
    auto *block_version = reinterpret_cast<BlockVersion *>(buffer_handle.GetDataMut());
    block_version -> RestoreFromSnapshot(commit_ts_);

    return Status::OK();
}

Status BlockMeta::UninitSet(UsageFlag usage_flag) {
    if (usage_flag == UsageFlag::kOther) {
        NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
        {
            String block_lock_key = GetBlockTag("lock");
            LOG_TRACE(fmt::format("UninitSet: dropping block lock for block key: {}", block_lock_key));
            Status status = new_catalog->DropBlockLockByBlockKey(block_lock_key);
            if (!status.ok()) {
                return status;
            }
        }
    }
    {
        String filter_key = GetBlockTag("fast_rough_filter");
        LOG_TRACE(fmt::format("UninitSet: fast rough filter key: {}", filter_key));
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
    std::lock_guard<std::mutex> lock(mtx_);
    if (!version_buffer_) {
        BufferManager *buffer_mgr = InfinityContext::instance().storage()->buffer_manager();

        // Get block directory without acquiring lock again (avoid recursive lock)
        if (block_dir_ == nullptr) {
            TableMeeta &table_meta = segment_meta_.table_meta();
            block_dir_ = MakeShared<String>(
                fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_));
        }
        String version_filepath = InfinityContext::instance().config()->DataDir() + "/" + *block_dir_ + "/" + String(BlockVersion::PATH);
        version_buffer_ = buffer_mgr->GetBufferObject(version_filepath);
        if (version_buffer_ == nullptr) {
            auto new_txn_mgr = InfinityContext::instance().storage()-> new_txn_manager();
            new_txn_mgr->PrintAllDroppedKeys();
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
    std::lock_guard<std::mutex> lock(mtx_);
    if (block_dir_ == nullptr) {
        TableMeeta &table_meta = segment_meta_.table_meta();
        block_dir_ = MakeShared<String>(
            fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_));
    }

    return block_dir_;
}

// TODO: Need to fix this function: no data stored in kv_instance_ this way
// usually use column_def to get column_id
Tuple<Vector<ColumnID> *, Status> BlockMeta::GetBlockColumnIDs1() {
    if (!column_ids1_) {
        column_ids1_ = infinity::GetTableSegmentBlockColumns(&kv_instance_,
                                                             segment_meta_.table_meta().db_id_str(),
                                                             segment_meta_.table_meta().table_id_str(),
                                                             segment_meta_.segment_id(),
                                                             block_id_,
                                                             begin_ts_);
    }
    // auto new_txn_mgr = InfinityContext::instance().storage()-> new_txn_manager();

    // new_txn_mgr->PrintAllKeyValue();

    return {&*column_ids1_, Status::OK()};
}

String BlockMeta::GetBlockTag(const String &tag) const {
    TableMeeta &table_meta = segment_meta_.table_meta();
    return KeyEncode::CatalogTableSegmentBlockTagKey(table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_, tag);
}

Tuple<SizeT, Status> BlockMeta::GetRowCnt1() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (row_cnt_) {
        return {*row_cnt_, Status::OK()};
    }

#if 1
    TableMeeta &table_meta = segment_meta_.table_meta();
    row_cnt_ = infinity::GetBlockRowCount(&kv_instance_,
                                          table_meta.db_id_str(),
                                          table_meta.table_id_str(),
                                          segment_meta_.segment_id(),
                                          block_id_,
                                          begin_ts_,
                                          commit_ts_);
    return {*row_cnt_, Status::OK()};
#else
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
#endif
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
    std::lock_guard<std::mutex> lock(mtx_);
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
    std::lock_guard<std::mutex> lock(mtx_);
    String filter_key = GetBlockTag("fast_rough_filter");
    String filter_str = fast_rough_filter->SerializeToString();
    Status status = kv_instance_.Put(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = fast_rough_filter;
    return Status::OK();
}

Tuple<SharedPtr<BlockSnapshotInfo>, Status> BlockMeta::MapMetaToSnapShotInfo(){
    SharedPtr<BlockSnapshotInfo> block_snapshot_info = MakeShared<BlockSnapshotInfo>();
    Status status;
    block_snapshot_info->block_id_ = block_id_;

    block_snapshot_info->create_ts_ = GetCreateTimestampFromKV();
    // Get row count
    std::tie(block_snapshot_info->row_count_, status) = GetRowCnt1();
    if (!status.ok()) {
        return {nullptr, status};
    }
    block_snapshot_info->row_capacity_ = this->block_capacity();

    // Get block dir
    // TODO: FIGURE OUT WHAT BlockDir do
    SharedPtr<String> block_dir_ptr;
    block_dir_ptr = this->GetBlockDir();
    block_snapshot_info->block_dir_ = *block_dir_ptr;

    // Get fast rough filter if exists
    SharedPtr<FastRoughFilter> fast_rough_filter;
    status = GetFastRoughFilter(fast_rough_filter);
    // serialize fast rough filter if exists
    if (status.ok()) {
        block_snapshot_info->fast_rough_filter_ = fast_rough_filter->SerializeToString();
    }
    

    // Get column ids
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = segment_meta_.table_meta().GetColumnDefs();
    if (!status.ok()) {
        return {nullptr, status};
    }
    for (auto &column_def: *column_defs) {
        ColumnMeta column_meta(column_def->id(), *this);
        auto [column_snapshot_info, status] = column_meta.MapMetaToSnapShotInfo();
        if (!status.ok()) {
            return {nullptr, status};
        }
        block_snapshot_info->column_block_snapshots_.push_back(column_snapshot_info);
    }

    return {block_snapshot_info, Status::OK()};
}

Status BlockMeta::RestoreFromSnapshot(){
    Status status = RestoreSetFromSnapshot();
    if (!status.ok()) {
        return status;
    }
    SharedPtr<Vector<SharedPtr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = segment_meta_.table_meta().GetColumnDefs();
    if (!status.ok()) {
        return status;
    }
    for (const auto &column_def : *column_defs) {
        ColumnMeta column_meta(column_def->id(), *this);
        status = column_meta.RestoreFromSnapshot(column_def->id());
        if (!status.ok()) {
            return status;
        }
    }
    return Status::OK();
}




} // namespace infinity
