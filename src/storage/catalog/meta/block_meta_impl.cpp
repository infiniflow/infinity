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

module infinity_core:block_meta.impl;

import :block_meta;
import :kv_code;
import :kv_store;
import :segment_meta;
import :table_meta;
import :new_catalog;
import :infinity_context;
import :fileworker_manager;
import :block_version;
import :version_file_worker;
import :meta_info;
import :column_meta;
import :fast_rough_filter;
import :kv_utility;
import :logger;
import :snapshot_info;
import :new_txn_manager;

import std;
import third_party;

import column_def;

namespace infinity {

BlockMeta::BlockMeta(BlockID block_id, SegmentMeta &segment_meta)
    : begin_ts_(segment_meta.begin_ts()), commit_ts_(segment_meta.commit_ts()), kv_instance_(segment_meta.kv_instance()), segment_meta_(segment_meta),
      block_id_(block_id) {}

Status BlockMeta::GetBlockLock(std::shared_ptr<BlockLock> &block_lock) {
    auto *new_catalog = InfinityContext::instance().storage()->new_catalog();
    auto block_lock_key = GetBlockTag("lock");
    auto status = new_catalog->GetBlockLock(block_lock_key, block_lock);
    if (!status.ok()) {
        return status;
    }
    return Status::OK();
}

TxnTimeStamp BlockMeta::GetCreateTimestampFromKV() const {
    std::string block_key = KeyEncode::CatalogTableSegmentBlockKey(segment_meta_.table_meta().db_id_str(),
                                                                   segment_meta_.table_meta().table_id_str(),
                                                                   segment_meta_.segment_id(),
                                                                   block_id_);
    std::string create_ts_str;
    Status status = kv_instance_.Get(block_key, create_ts_str);
    if (!status.ok()) {
        return 0;
    }
    return std::stoull(create_ts_str);
}

Status BlockMeta::InitOrLoadSet(TxnTimeStamp checkpoint_ts) {
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        std::string block_lock_key = GetBlockTag("lock");
        Status status;
        if (checkpoint_ts == 0) {
            status = new_catalog->AddBlockLock(std::move(block_lock_key));
        } else {
            status = new_catalog->AddBlockLock(std::move(block_lock_key), checkpoint_ts);
        }
        if (!status.ok()) {
            return status;
        }
    }
    auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    std::shared_ptr<std::string> block_dir_ptr = GetBlockDir();
    auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir_ptr, BlockVersion::PATH));
    auto version_file_worker = std::make_unique<VersionFileWorker>(rel_file_path, block_capacity());
    version_file_worker_ = fileworker_mgr->EmplaceFileWorker(std::move(version_file_worker));
    return Status::OK();
}

Status BlockMeta::RestoreSetFromSnapshot() {
    // TODO: need to fix this
    NewCatalog *new_catalog = InfinityContext::instance().storage()->new_catalog();
    {
        std::string block_lock_key = GetBlockTag("lock");

        Status status = new_catalog->AddBlockLock(std::move(block_lock_key));
    }
    // auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();
    std::shared_ptr<std::string> block_dir_ptr = GetBlockDir();
    auto rel_file_path = std::make_shared<std::string>(fmt::format("{}/{}", *block_dir_ptr, BlockVersion::PATH));
    auto version_file_worker = std::make_unique<VersionFileWorker>(rel_file_path, block_capacity());

    version_file_worker_ = version_file_worker.get();
    if (!version_file_worker_) {
        return Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_file_worker->GetFilePath()));
    }

    BlockVersion *block_version{};
    version_file_worker_->Read(block_version);
    block_version->RestoreFromSnapshot(commit_ts_);

    return Status::OK();
}

Status BlockMeta::UninitSet(UsageFlag usage_flag) {
    if (usage_flag == UsageFlag::kOther) {
        auto *new_catalog = InfinityContext::instance().storage()->new_catalog();
        {
            std::string block_lock_key = GetBlockTag("lock");
            LOG_TRACE(fmt::format("UninitSet: dropping block lock for block key: {}", block_lock_key));
            if (auto status = new_catalog->DropBlockLockByBlockKey(block_lock_key); !status.ok()) {
                return status;
            }
        }
    }
    {
        std::string filter_key = GetBlockTag("fast_rough_filter");
        LOG_TRACE(fmt::format("UninitSet: fast rough filter key: {}", filter_key));
        if (auto status = kv_instance_.Delete(filter_key); !status.ok()) {
            if (status.code() != ErrorCode::kNotFound) {
                return status;
            }
        }
    }

    if (usage_flag == UsageFlag::kOther) {
        auto [version_buffer, status] = GetVersionBuffer();
        if (!status.ok()) {
            return status;
        }
        version_buffer->PickForCleanup();
    }
    return Status::OK();
}

std::tuple<FileWorker *, Status> BlockMeta::GetVersionBuffer() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (!version_file_worker_) {
        auto *fileworker_mgr = InfinityContext::instance().storage()->fileworker_manager();

        // Get block directory without acquiring lock again (avoid recursive lock)
        if (block_dir_ == nullptr) {
            auto &table_meta = segment_meta_.table_meta();
            block_dir_ = std::make_shared<std::string>(
                fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_));
        }
        auto version_filepath = fmt::format("{}/{}", *block_dir_, BlockVersion::PATH);
        version_file_worker_ = fileworker_mgr->GetFileWorker(version_filepath);
        if (version_file_worker_ == nullptr) {
            return {nullptr, Status::BufferManagerError(fmt::format("Get version buffer failed: {}", version_filepath))};
        }
    }
    return {version_file_worker_, Status::OK()};
}

std::vector<std::string> BlockMeta::FilePaths() {
    std::vector<std::string> res;
    std::shared_ptr<std::string> block_dir_ptr = this->GetBlockDir();
    std::string version_filepath = *block_dir_ptr + "/" + std::string(BlockVersion::PATH);
    res.push_back(version_filepath);
    return res;
}

std::shared_ptr<std::string> BlockMeta::GetBlockDir() {
    std::lock_guard<std::mutex> lock(mtx_);
    if (block_dir_ == nullptr) {
        TableMeta &table_meta = segment_meta_.table_meta();
        block_dir_ = std::make_shared<std::string>(
            fmt::format("db_{}/tbl_{}/seg_{}/blk_{}", table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_));
    }

    return block_dir_;
}

std::string BlockMeta::GetBlockTag(const std::string &tag) const {
    TableMeta &table_meta = segment_meta_.table_meta();
    return KeyEncode::CatalogTableSegmentBlockTagKey(table_meta.db_id_str(), table_meta.table_id_str(), segment_meta_.segment_id(), block_id_, tag);
}

std::tuple<size_t, Status> BlockMeta::GetRowCnt1() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        if (row_cnt_) {
            return {*row_cnt_, Status::OK()};
        }
    }
#if 1
    TableMeta &table_meta = segment_meta_.table_meta();
    auto row_cnt = infinity::GetBlockRowCount(&kv_instance_,
                                              table_meta.db_id_str(),
                                              table_meta.table_id_str(),
                                              segment_meta_.segment_id(),
                                              block_id_,
                                              begin_ts_,
                                              commit_ts_);

    std::lock_guard<std::mutex> lock(mtx_);
    row_cnt_ = row_cnt;
    return {*row_cnt_, Status::OK()};
#else
    Status status;

    std::shared_ptr<BlockLock> block_lock;
    status = this->GetBlockLock(block_lock);
    if (!status.ok()) {
        return {0, status};
    }
    FileWorker *version_buffer;
    std::tie(version_buffer, status) = this->GetVersionBuffer();
    if (!status.ok()) {
        return {0, status};
    }

    const auto *block_version = reinterpret_cast<const BlockVersion *>(version_buffer->GetData());

    size_t row_cnt = 0;
    {
        std::shared_lock lock(block_lock->mtx_);
        row_cnt = block_version->GetRowCount(begin_ts_);
    }
    row_cnt_ = row_cnt;
    return {row_cnt, Status::OK()};
#endif
}

std::tuple<std::shared_ptr<BlockInfo>, Status> BlockMeta::GetBlockInfo() {
    std::shared_ptr<BlockInfo> block_info = std::make_shared<BlockInfo>();
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

    std::vector<std::string> file_paths;
    status = NewCatalog::GetBlockFilePaths(*this, file_paths, nullptr);
    block_info->files_ = file_paths;

    return {block_info, Status::OK()};
}

std::tuple<std::shared_ptr<BlockColumnInfo>, Status> BlockMeta::GetBlockColumnInfo(ColumnID column_id) {
    std::shared_ptr<BlockColumnInfo> block_column_info = std::make_shared<BlockColumnInfo>();
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs = nullptr;
    std::shared_ptr<ColumnDef> column_def = nullptr;
    Status status;
    size_t select_column_idx = 0;
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
    std::vector<std::string> file_paths;
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
        block_column_info->extra_file_names_.push_back(std::make_shared<std::string>(file_path));
    }
    return {block_column_info, Status::OK()};
}

Status BlockMeta::GetFastRoughFilter(std::shared_ptr<FastRoughFilter> &fast_rough_filter) {
    std::lock_guard<std::mutex> lock(mtx_);
    fast_rough_filter.reset();
    if (fast_rough_filter_) {
        fast_rough_filter = fast_rough_filter_;
        return Status::OK();
    }

    std::string filter_key = GetBlockTag("fast_rough_filter");
    std::string filter_str;
    Status status = kv_instance_.Get(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = std::make_shared<FastRoughFilter>();
    fast_rough_filter_->DeserializeFromString(filter_str);
    fast_rough_filter = fast_rough_filter_;

    return Status::OK();
}

Status BlockMeta::SetFastRoughFilter(std::shared_ptr<FastRoughFilter> fast_rough_filter) {
    std::lock_guard<std::mutex> lock(mtx_);
    std::string filter_key = GetBlockTag("fast_rough_filter");
    std::string filter_str = fast_rough_filter->SerializeToString();
    Status status = kv_instance_.Put(filter_key, filter_str);
    if (!status.ok()) {
        return status;
    }
    fast_rough_filter_ = fast_rough_filter;
    return Status::OK();
}

std::tuple<std::shared_ptr<BlockSnapshotInfo>, Status> BlockMeta::MapMetaToSnapShotInfo() {
    std::shared_ptr<BlockSnapshotInfo> block_snapshot_info = std::make_shared<BlockSnapshotInfo>();
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
    std::shared_ptr<std::string> block_dir_ptr;
    block_dir_ptr = this->GetBlockDir();
    block_snapshot_info->block_dir_ = *block_dir_ptr;

    // Get fast rough filter if exists
    std::shared_ptr<FastRoughFilter> fast_rough_filter;
    status = GetFastRoughFilter(fast_rough_filter);
    // serialize fast rough filter if exists
    if (status.ok()) {
        block_snapshot_info->fast_rough_filter_ = fast_rough_filter->SerializeToString();
    }

    // Get column ids
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
    std::tie(column_defs, status) = segment_meta_.table_meta().GetColumnDefs();
    if (!status.ok()) {
        return {nullptr, status};
    }
    for (auto &column_def : *column_defs) {
        ColumnMeta column_meta(column_def->id(), *this);
        auto [column_snapshot_info, status] = column_meta.MapMetaToSnapShotInfo();
        if (!status.ok()) {
            return {nullptr, status};
        }
        block_snapshot_info->column_block_snapshots_.push_back(column_snapshot_info);
    }

    return {block_snapshot_info, Status::OK()};
}

Status BlockMeta::RestoreFromSnapshot() {
    Status status = RestoreSetFromSnapshot();
    if (!status.ok()) {
        return status;
    }
    std::shared_ptr<std::vector<std::shared_ptr<ColumnDef>>> column_defs;
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
