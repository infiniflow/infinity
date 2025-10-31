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

module infinity_core:snapshot_info.impl;

import :snapshot_info;
import :status;
import :logger;
import :virtual_store;
import :local_file_handle;
import :infinity_exception;
import :infinity_context;
import :config;
import :persistence_manager;
import :persist_result_handler;
import :defer_op;
import :utility;
import :block_version;
import :fst.fst;

import std.compat;
import third_party;

import column_def;
import data_type;
import parsed_expr;
import command_statement;
import constant_expr;

namespace infinity {

// Simple FST validation function
bool ValidateFstFile(const std::string &file_path) {
    try {
        LOG_DEBUG("FST validation: Starting validation for " + file_path);

        // Try to memory map the file
        u8 *data_ptr = nullptr;
        size_t data_len = 0;
        int rc = VirtualStore::MmapFile(file_path, data_ptr, data_len);
        LOG_DEBUG("FST validation: MmapFile result=" + std::to_string(rc) + ", data_len=" + std::to_string(data_len));

        if (rc < 0 || data_ptr == nullptr || data_len < 36) {
            LOG_WARN("FST validation: Failed to mmap file or file too small. rc=" + std::to_string(rc) +
                     ", data_ptr=" + (data_ptr ? "valid" : "null") + ", data_len=" + std::to_string(data_len));
            return false;
        }

        // For dictionary files, the FST is embedded at the end
        // Read the FST root address from the end of the file
        size_t fst_root_addr = ReadU64LE(data_ptr + data_len - 4 - 8);
        size_t fst_len;
        if (fst_root_addr == 0UL) {
            fst_len = 36;
        } else {
            fst_len = fst_root_addr + 21;
        }

        // Calculate the start of FST data
        u8 *fst_data = data_ptr + (data_len - fst_len);

        // Now read the version from the FST data (not the beginning of the file)
        u64 version = ReadU64LE(fst_data);
        LOG_DEBUG("FST validation: Version=" + std::to_string(version) + ", expected=3");

        if (version != 3) { // FST VERSION should be 3
            LOG_WARN("FST validation: Version mismatch. Got=" + std::to_string(version) + ", expected=3");
            VirtualStore::MunmapFile(file_path);
            return false;
        }

        // Check if we can read the root address from FST data
        size_t end = fst_len - 4;
        size_t root_addr = ReadU64LE(fst_data + end - 8);
        size_t len = ReadU64LE(fst_data + end - 16);

        LOG_DEBUG("FST validation: root_addr=" + std::to_string(root_addr) + ", len=" + std::to_string(len) + ", fst_len=" + std::to_string(fst_len));

        // Basic sanity checks
        if (root_addr >= fst_len || len > fst_len) {
            LOG_WARN("FST validation: Invalid addresses. root_addr=" + std::to_string(root_addr) + " >= fst_len=" + std::to_string(fst_len) +
                     " OR len=" + std::to_string(len) + " > fst_len=" + std::to_string(fst_len));
            VirtualStore::MunmapFile(file_path);
            return false;
        }

        LOG_DEBUG("FST validation: Validation passed for " + file_path);
        VirtualStore::MunmapFile(file_path);
        return true;
    } catch (const std::exception &e) {
        LOG_ERROR("FST validation: Exception during validation: " + std::string(e.what()));
        return false;
    } catch (...) {
        LOG_ERROR("FST validation: Unknown exception during validation");
        return false;
    }
}

nlohmann::json BlockColumnSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["column_id"] = column_id_;
    json_res["filepath"] = filepath_;
    for (const auto &outline_snapshot : outline_snapshots_) {
        json_res["outlines"].emplace_back(outline_snapshot->filepath_);
    }
    return json_res;
}

std::shared_ptr<BlockColumnSnapshotInfo> BlockColumnSnapshotInfo::Deserialize(const nlohmann::json &column_block_json) {
    auto column_block_snapshot = std::make_shared<BlockColumnSnapshotInfo>();
    column_block_snapshot->column_id_ = column_block_json["column_id"];
    column_block_snapshot->filepath_ = column_block_json["filepath"];
    if (column_block_json.contains("outlines")) {
        for (const auto &outline_snapshot : column_block_json["outlines"]) {
            auto outline_snapshot_info = std::make_shared<OutlineSnapshotInfo>();
            outline_snapshot_info->filepath_ = outline_snapshot;
            column_block_snapshot->outline_snapshots_.emplace_back(outline_snapshot_info);
        }
    }
    return column_block_snapshot;
}

nlohmann::json BlockSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["block_id"] = block_id_;
    json_res["block_dir"] = block_dir_;
    for (const auto &column_block_snapshot : column_block_snapshots_) {
        json_res["columns"].emplace_back(column_block_snapshot->Serialize());
    }
    return json_res;
}

std::shared_ptr<BlockSnapshotInfo> BlockSnapshotInfo::Deserialize(const nlohmann::json &block_json) {
    auto block_snapshot = std::make_shared<BlockSnapshotInfo>();
    block_snapshot->block_id_ = block_json["block_id"];
    block_snapshot->block_dir_ = block_json["block_dir"];
    for (const auto &column_block_json : block_json["columns"]) {
        auto column_block_snapshot = BlockColumnSnapshotInfo::Deserialize(column_block_json);
        block_snapshot->column_block_snapshots_.emplace_back(column_block_snapshot);
    }
    return block_snapshot;
}

nlohmann::json SegmentSnapshotInfo::Serialize() {
    nlohmann::json json_res;

    json_res["segment_id"] = segment_id_;
    json_res["segment_dir"] = segment_dir_;
    json_res["first_delete_ts"] = first_delete_ts_;
    json_res["deprecate_ts"] = deprecate_ts_;
    json_res["row_count"] = row_count_;
    json_res["actual_row_count"] = actual_row_count_;
    json_res["segment_status"] = status_;

    for (const auto &block_snapshot : block_snapshots_) {
        json_res["blocks"].emplace_back(block_snapshot->Serialize());
    }
    return json_res;
}

std::shared_ptr<SegmentSnapshotInfo> SegmentSnapshotInfo::Deserialize(const nlohmann::json &segment_json) {
    auto segment_snapshot = std::make_shared<SegmentSnapshotInfo>();
    segment_snapshot->segment_id_ = segment_json["segment_id"];
    segment_snapshot->segment_dir_ = segment_json["segment_dir"];

    segment_snapshot->first_delete_ts_ = segment_json["first_delete_ts"];
    segment_snapshot->deprecate_ts_ = segment_json["deprecate_ts"];
    segment_snapshot->row_count_ = segment_json["row_count"];
    segment_snapshot->actual_row_count_ = segment_json["actual_row_count"];
    segment_snapshot->status_ = static_cast<SegmentStatus>(segment_json["segment_status"]);

    for (const auto &block_json : segment_json["blocks"]) {
        auto block_snapshot = BlockSnapshotInfo::Deserialize(block_json);
        segment_snapshot->block_snapshots_.emplace_back(block_snapshot);
    }
    return segment_snapshot;
}

nlohmann::json ChunkIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["chunk_id"] = chunk_id_;
    json_res["base_name"] = base_name_;
    json_res["base_row_id"] = base_row_id_.ToString();
    json_res["row_cnt"] = row_cnt_;
    json_res["index_size"] = index_size_;
    json_res["index_filename"] = index_filename_;
    for (const auto &file : full_text_files_) {
        json_res["full_text_files"].emplace_back(file);
    }
    return json_res;
}

std::shared_ptr<ChunkIndexSnapshotInfo> ChunkIndexSnapshotInfo::Deserialize(const nlohmann::json &chunk_index_json) {
    auto chunk_index_snapshot = std::make_shared<ChunkIndexSnapshotInfo>();
    chunk_index_snapshot->chunk_id_ = chunk_index_json["chunk_id"];
    chunk_index_snapshot->base_name_ = chunk_index_json["base_name"];
    chunk_index_snapshot->base_row_id_ = RowID::FromUint64(std::stoull(chunk_index_json["base_row_id"].get<std::string>()));
    chunk_index_snapshot->row_cnt_ = chunk_index_json["row_cnt"];
    chunk_index_snapshot->index_size_ = chunk_index_json["index_size"];
    chunk_index_snapshot->index_filename_ = chunk_index_json["index_filename"];
    if (chunk_index_json.contains("full_text_files")) {
        for (const auto &file : chunk_index_json["full_text_files"]) {
            chunk_index_snapshot->full_text_files_.emplace_back(file);
        }
    }
    return chunk_index_snapshot;
}

nlohmann::json SegmentIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["segment_id"] = segment_id_;
    for (const auto &chunk_index_snapshot : chunk_index_snapshots_) {
        json_res["chunk_indexes"].emplace_back(chunk_index_snapshot->Serialize());
    }
    return json_res;
}

std::shared_ptr<SegmentIndexSnapshotInfo> SegmentIndexSnapshotInfo::Deserialize(const nlohmann::json &segment_index_json) {
    auto segment_index_snapshot = std::make_shared<SegmentIndexSnapshotInfo>();
    segment_index_snapshot->segment_id_ = segment_index_json["segment_id"];
    if (segment_index_json.contains("chunk_indexes")) {
        for (const auto &chunk_index_json : segment_index_json["chunk_indexes"]) {
            auto chunk_index_snapshot = ChunkIndexSnapshotInfo::Deserialize(chunk_index_json);
            segment_index_snapshot->chunk_index_snapshots_.emplace_back(chunk_index_snapshot);
        }
    }
    return segment_index_snapshot;
}

nlohmann::json TableIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["index_dir"] = *index_dir_;
    json_res["index_id_str"] = *index_id_str_;
    json_res["index_base"] = index_base_->Serialize();
    for (const auto &segment_index_snapshot : segment_index_snapshots_) {
        json_res["segment_indexes"].emplace_back(segment_index_snapshot->Serialize());
    }
    return json_res;
}

std::shared_ptr<TableIndexSnapshotInfo> TableIndexSnapshotInfo::Deserialize(const nlohmann::json &table_index_json) {
    auto table_index_snapshot = std::make_shared<TableIndexSnapshotInfo>();
    table_index_snapshot->index_dir_ = std::make_shared<std::string>(table_index_json["index_dir"]);
    table_index_snapshot->index_id_str_ = std::make_shared<std::string>(table_index_json["index_id_str"]);
    table_index_snapshot->index_base_ = IndexBase::Deserialize(table_index_json["index_base"].dump());
    if (table_index_json.contains("segment_indexes")) {
        for (const auto &segment_index_json : table_index_json["segment_indexes"]) {
            auto segment_index_snapshot = SegmentIndexSnapshotInfo::Deserialize(segment_index_json);
            table_index_snapshot->segment_index_snapshots_.emplace_back(segment_index_snapshot);
        }
    }
    return table_index_snapshot;
}

Status TableSnapshotInfo::Serialize(const std::string &save_dir, TransactionID txn_id) {

    Config *config = InfinityContext::instance().config();
    PersistenceManager *persistence_manager = InfinityContext::instance().persistence_manager();

    LOG_INFO(fmt::format("Serialize snapshot at {} with txn_id {}", snapshot_name_, txn_id));

    // Start timing for data copying
    auto data_copy_start = std::chrono::high_resolution_clock::now();
    // Create temporary directory for atomic operation
    std::string temp_snapshot_dir = fmt::format("{}/temp_{}_{}", save_dir, snapshot_name_, txn_id);

    // Create temporary directory
    Status create_temp_status = VirtualStore::MakeDirectory(temp_snapshot_dir);
    if (!create_temp_status.ok()) {
        return create_temp_status;
    }

    // Get files
    std::vector<std::string> original_files = GetFiles();

    // Copy files to temporary location
    if (persistence_manager != nullptr) {
        PersistResultHandler pm_handler(persistence_manager);
        for (const auto &file : original_files) {
            PersistReadResult result = persistence_manager->GetObjCache(file);
            const ObjAddr &obj_addr = pm_handler.HandleReadResult(result);
            if (!obj_addr.Valid()) {
                continue;
            }

            DeferFn defer_fn([&]() {
                auto res = persistence_manager->PutObjCache(file);
                pm_handler.HandleWriteResult(res);
            });

            std::string read_path = persistence_manager->GetObjPath(obj_addr.obj_key_);
            LOG_INFO(fmt::format("READ: {} from {}", file, read_path));

            auto [reader_handle, reader_open_status] = VirtualStore::Open(read_path, FileAccessMode::kRead);
            if (!reader_open_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return reader_open_status;
            }

            auto seek_status = reader_handle->Seek(obj_addr.part_offset_);
            if (!seek_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return seek_status;
            }

            auto file_size = obj_addr.part_size_;
            auto buffer = std::make_unique<char[]>(file_size);
            auto [nread, read_status] = reader_handle->Read(buffer.get(), file_size);

            std::string dst_file_path = fmt::format("{}/{}", temp_snapshot_dir, file);
            std::string dst_dir = VirtualStore::GetParentPath(dst_file_path);

            if (!VirtualStore::Exists(dst_dir)) {
                Status mkdir_status = VirtualStore::MakeDirectory(dst_dir);
                if (!mkdir_status.ok()) {
                    VirtualStore::RemoveDirectory(temp_snapshot_dir);
                    return mkdir_status;
                }
            }

            auto [write_file_handle, writer_open_status] = VirtualStore::Open(dst_file_path, FileAccessMode::kWrite);
            if (!writer_open_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return writer_open_status;
            }

            Status write_status = write_file_handle->Append(buffer.get(), file_size);
            if (!write_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return write_status;
            }
            write_file_handle->Sync();
            // Validate FST files after copying to catch corruption early
            if (dst_file_path.find(".dic") != std::string::npos) {
                bool is_valid = ValidateFstFile(dst_file_path);
                if (!is_valid) {
                    LOG_WARN("FST validation failed during snapshot creation for file: " + dst_file_path);
                    // Continue with snapshot creation but log the warning
                } else {
                    LOG_DEBUG("FST validation passed during snapshot creation for file: " + dst_file_path);
                }
            }
        }
    } else {
        std::string data_dir = config->DataDir();
        for (const auto &file : original_files) {
            std::string src_file_path = fmt::format("{}/{}", data_dir, file);
            std::string dst_file_path = fmt::format("{}/{}", temp_snapshot_dir, file);

            Status copy_status = VirtualStore::Copy(dst_file_path, src_file_path);
            if (!copy_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return copy_status;
            }
        }
    }

    // End timing for data copying
    auto data_copy_end = std::chrono::high_resolution_clock::now();
    auto data_copy_duration = std::chrono::duration_cast<std::chrono::milliseconds>(data_copy_end - data_copy_start);
    LOG_INFO(fmt::format("Data copying took {} ms", data_copy_duration.count()));

    // Start timing for JSON serialization
    auto json_start = std::chrono::high_resolution_clock::now();

    // Create metadata JSON
    nlohmann::json json_res = CreateSnapshotMetadataJSON();
    std::string json_string = json_res.dump();

    // End timing for JSON serialization
    auto json_end = std::chrono::high_resolution_clock::now();
    auto json_duration = std::chrono::duration_cast<std::chrono::milliseconds>(json_end - json_start);
    LOG_INFO(fmt::format("JSON serialization took {} ms", json_duration.count()));

    std::string snapshot_dir = fmt::format("{}/{}", save_dir, snapshot_name_);
    if (!VirtualStore::Exists(snapshot_dir)) {
        Status mkdir_status = VirtualStore::MakeDirectory(snapshot_dir);
        if (!mkdir_status.ok()) {
            VirtualStore::RemoveDirectory(temp_snapshot_dir);
            return mkdir_status;
        }
    }

    std::string meta_path = fmt::format("{}/{}.json", snapshot_dir, snapshot_name_);
    auto [snapshot_file_handle, status] = VirtualStore::Open(meta_path, FileAccessMode::kWrite);
    if (!status.ok()) {
        VirtualStore::RemoveDirectory(temp_snapshot_dir);
        VirtualStore::RemoveDirectory(snapshot_dir);
        return status;
    }

    status = snapshot_file_handle->Append(json_string.data(), json_string.size());
    if (!status.ok()) {
        VirtualStore::RemoveDirectory(temp_snapshot_dir);
        VirtualStore::RemoveDirectory(snapshot_dir);
        return status;
    }
    snapshot_file_handle->Sync();
    VirtualStore::RemoveDirectory(temp_snapshot_dir);

    //    // Write metadata to temporary location
    //    std::string temp_meta_path = fmt::format("{}/{}.json", temp_snapshot_dir, snapshot_name_);
    //    auto [snapshot_file_handle, status] = VirtualStore::Open(temp_meta_path, FileAccessMode::kWrite);
    //    if (!status.ok()) {
    //        VirtualStore::RemoveDirectory(temp_snapshot_dir);
    //        return status;
    //    }
    //
    //    status = snapshot_file_handle->Append(json_string.data(), json_string.size());
    //    if (!status.ok()) {
    //        VirtualStore::RemoveDirectory(temp_snapshot_dir);
    //        return status;
    //    }
    //    snapshot_file_handle->Sync();
    //
    //    // Atomically move temporary directory to final location
    //    std::string final_snapshot_dir = fmt::format("{}/{}", save_dir, snapshot_name_);
    //
    //    // Ensure save directory exists
    //    if (!VirtualStore::Exists(save_dir)) {
    //        Status mkdir_status = VirtualStore::MakeDirectory(save_dir);
    //        if (!mkdir_status.ok()) {
    //            VirtualStore::RemoveDirectory(temp_snapshot_dir);
    //            return mkdir_status;
    //        }
    //    }
    //    if (VirtualStore::Exists(final_snapshot_dir)) {
    //        VirtualStore::RemoveDirectory(temp_snapshot_dir);
    //        return Status::SnapshotAlreadyExists(snapshot_name_);
    //    }
    //
    //    // Atomic rename operation
    //    try {
    //        Status rename_status = VirtualStore::Rename(temp_snapshot_dir, final_snapshot_dir);
    //        if (!rename_status.ok()) {
    //            VirtualStore::RemoveDirectory(temp_snapshot_dir);
    //            return rename_status;
    //        }
    //    } catch (const std::exception &e) {
    //        VirtualStore::RemoveDirectory(temp_snapshot_dir);
    //        return Status::SnapshotAlreadyExists(snapshot_name_);
    //    }

    LOG_INFO(fmt::format("Atomic snapshot creation completed: {}", json_res.dump()));
    return Status::OK();
}

std::vector<std::string> TableSnapshotInfo::GetFiles() const {
    std::vector<std::string> files;
    for (const auto &segment_snapshot_pair : segment_snapshots_) {
        for (const auto &block_snapshot : segment_snapshot_pair.second->block_snapshots_) {
            for (const auto &column_block_snapshot : block_snapshot->column_block_snapshots_) {
                files.emplace_back(column_block_snapshot->filepath_);
                for (const auto &outline_snapshot : column_block_snapshot->outline_snapshots_) {
                    files.emplace_back(outline_snapshot->filepath_);
                }
            }

            files.emplace_back(VirtualStore::ConcatenatePath(block_snapshot->block_dir_, *BlockVersion::FileName()));
        }
    }

    for (const auto &table_index_snapshot_pair : table_index_snapshots_) {
        for (const auto &segment_index_snapshot : table_index_snapshot_pair.second->segment_index_snapshots_) {
            for (const auto &chunk_index_snapshot : segment_index_snapshot->chunk_index_snapshots_) {
                if (chunk_index_snapshot->full_text_files_.empty()) {
                    files.emplace_back(
                        VirtualStore::ConcatenatePath(*table_index_snapshot_pair.second->index_dir_,
                                                      VirtualStore::ConcatenatePath("seg_" + std::to_string(segment_index_snapshot->segment_id_),
                                                                                    chunk_index_snapshot->index_filename_)));
                } else {
                    files.insert(files.end(), chunk_index_snapshot->full_text_files_.cbegin(), chunk_index_snapshot->full_text_files_.cend());
                }
            }
        }
    }
    return files;
}

std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> TableSnapshotInfo::Deserialize(const std::string &snapshot_dir,
                                                                                      const std::string &snapshot_name) {

    LOG_INFO(fmt::format("Deserialize snapshot: {}/{}", snapshot_dir, snapshot_name));

    // Start timing for JSON deserialization
    auto json_deserialize_start = std::chrono::high_resolution_clock::now();

    std::string meta_path = fmt::format("{}/{}/{}.json", snapshot_dir, snapshot_name, snapshot_name);

    if (!VirtualStore::Exists(meta_path)) {
        return {nullptr, Status::FileNotFound(meta_path)};
    }
    auto [meta_file_handle, status] = VirtualStore::Open(meta_path, FileAccessMode::kRead);
    if (!status.ok()) {
        return {nullptr, status};
    }

    i64 file_size = meta_file_handle->FileSize();
    std::string json_str(file_size, 0);
    auto [n_bytes, status_read] = meta_file_handle->Read(json_str.data(), file_size);
    if (!status.ok()) {
        RecoverableError(status_read);
    }
    if ((size_t)file_size != n_bytes) {
        Status status = Status::FileCorrupted(meta_path);
        RecoverableError(status);
    }

    nlohmann::json snapshot_meta_json = nlohmann::json::parse(json_str);

    // End timing for JSON deserialization
    auto json_deserialize_end = std::chrono::high_resolution_clock::now();
    auto json_deserialize_duration = std::chrono::duration_cast<std::chrono::milliseconds>(json_deserialize_end - json_deserialize_start);
    LOG_INFO(fmt::format("JSON deserialization took {} ms", json_deserialize_duration.count()));

    LOG_INFO(fmt::format("Deserialize snapshot: {}", snapshot_meta_json.dump()));
    return Deserialize(snapshot_meta_json);
}

std::tuple<std::shared_ptr<TableSnapshotInfo>, Status> TableSnapshotInfo::Deserialize(const nlohmann::json &snapshot_meta_json) {
    std::shared_ptr<TableSnapshotInfo> table_snapshot = std::make_shared<TableSnapshotInfo>();

    table_snapshot->snapshot_name_ = snapshot_meta_json["snapshot_name"];
    SnapshotScope scope = static_cast<SnapshotScope>(snapshot_meta_json["snapshot_scope"]);
    if (scope != SnapshotScope::kTable) {
        return {nullptr, Status::Unknown("Invalid snapshot scope")};
    }

    table_snapshot->scope_ = SnapshotScope::kTable;
    table_snapshot->version_ = snapshot_meta_json["version"];
    table_snapshot->db_name_ = snapshot_meta_json["database_name"];
    table_snapshot->table_name_ = snapshot_meta_json["table_name"];
    table_snapshot->db_id_str_ = snapshot_meta_json["db_id_str"];
    table_snapshot->table_id_str_ = snapshot_meta_json["table_id_str"];
    table_snapshot->table_comment_ = snapshot_meta_json["table_comment"];

    table_snapshot->txn_id_ = snapshot_meta_json["txn_id"];
    // table_snapshot->begin_ts_ = snapshot_meta_json["begin_ts"];
    // table_snapshot->commit_ts_ = snapshot_meta_json["commit_ts"];
    table_snapshot->create_ts_ = snapshot_meta_json["create_ts"];

    table_snapshot->max_commit_ts_ = snapshot_meta_json["max_commit_ts"];
    table_snapshot->next_column_id_ = snapshot_meta_json["next_column_id"];
    table_snapshot->unsealed_id_ = snapshot_meta_json["unsealed_id"];
    table_snapshot->next_segment_id_ = snapshot_meta_json["next_segment_id"];
    table_snapshot->row_count_ = snapshot_meta_json["row_count"];

    for (const auto &column_def_json : snapshot_meta_json["column_definition"]) {
        std::shared_ptr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"].dump());
        i64 column_id = column_def_json["column_id"];
        std::string column_name = column_def_json["column_name"];

        std::set<ConstraintType> constraints;
        if (column_def_json.contains("constraints")) {
            for (const auto &column_constraint : column_def_json["constraints"]) {
                ConstraintType constraint = column_constraint;
                constraints.emplace(constraint);
            }
        }

        std::string comment;
        if (column_def_json.contains("column_comment")) {
            comment = column_def_json["column_comment"];
        }

        std::shared_ptr<ParsedExpr> default_expr = nullptr;
        if (column_def_json.contains("default")) {
            default_expr = ConstantExpr::Deserialize(column_def_json["default"].dump());
        }

        std::shared_ptr<ColumnDef> column_def = std::make_shared<ColumnDef>(column_id, data_type, column_name, constraints, comment, default_expr);
        table_snapshot->columns_.emplace_back(column_def);
    }

    if (snapshot_meta_json.contains("segments")) {
        for (const auto &segment_meta_json : snapshot_meta_json["segments"]) {
            std::shared_ptr<SegmentSnapshotInfo> segment_snapshot = SegmentSnapshotInfo::Deserialize(segment_meta_json);
            table_snapshot->segment_snapshots_.emplace(segment_snapshot->segment_id_, segment_snapshot);
        }
    }

    if (snapshot_meta_json.contains("table_indexes")) {
        for (const auto &table_index_meta_json : snapshot_meta_json["table_indexes"]) {
            std::shared_ptr<TableIndexSnapshotInfo> table_index_snapshot = TableIndexSnapshotInfo::Deserialize(table_index_meta_json);
            table_snapshot->table_index_snapshots_.emplace(*table_index_snapshot->index_base_->index_name_, table_index_snapshot);
        }
    }

    //    LOG_INFO(table_snapshot->ToString());
    // Status restore_status = RestoreSnapshotFiles(snapshot_dir, snapshot_name, table_snapshot->GetFiles());
    // if (!restore_status.ok()) {
    //     return {nullptr, restore_status};
    // }

    return {table_snapshot, Status::OK()};
}

std::string TableSnapshotInfo::ToString() const {
    return fmt::format("TableSnapshotInfo: db_name: {}, table_name: {}, snapshot_name: {}, version: {}",
                       db_name_,
                       table_name_,
                       snapshot_name_,
                       version_);
}

nlohmann::json TableSnapshotInfo::CreateSnapshotMetadataJSON() const {
    nlohmann::json json_res;
    json_res["version"] = version_;
    json_res["snapshot_name"] = snapshot_name_;
    json_res["snapshot_scope"] = SnapshotScope::kTable;
    json_res["database_name"] = db_name_;
    json_res["table_name"] = table_name_;
    json_res["db_id_str"] = db_id_str_;
    json_res["table_id_str"] = table_id_str_;

    json_res["table_comment"] = table_comment_;

    json_res["txn_id"] = txn_id_;
    json_res["max_commit_ts"] = max_commit_ts_;
    json_res["create_ts"] = create_ts_;

    json_res["next_column_id"] = next_column_id_;
    json_res["unsealed_id"] = unsealed_id_;
    json_res["next_segment_id"] = next_segment_id_;
    json_res["row_count"] = row_count_;

    for (const auto &column_def : this->columns_) {
        nlohmann::json column_def_json;
        column_def_json["column_type"] = column_def->type()->Serialize();
        column_def_json["column_id"] = column_def->id();
        column_def_json["column_name"] = column_def->name();

        for (const auto &column_constraint : column_def->constraints_) {
            column_def_json["constraints"].emplace_back(column_constraint);
        }

        if (!(column_def->comment().empty())) {
            column_def_json["column_comment"] = column_def->comment();
        }

        if (column_def->has_default_value()) {
            auto default_expr = dynamic_pointer_cast<ConstantExpr>(column_def->default_expr_);
            column_def_json["default"] = default_expr->Serialize();
        }

        json_res["column_definition"].emplace_back(column_def_json);
    }

    for (const auto &segment_snapshot_pair : segment_snapshots_) {
        json_res["segments"].emplace_back(segment_snapshot_pair.second->Serialize());
    }

    for (const auto &table_index_snapshot_pair : table_index_snapshots_) {
        json_res["table_indexes"].emplace_back(table_index_snapshot_pair.second->Serialize());
    }

    return json_res;
}

// copy files from snapshot back to data
Status SnapshotInfo::RestoreSnapshotFiles(const std::string &snapshot_dir,
                                          const std::string &snapshot_name,
                                          const std::vector<std::string> &files_to_restore,
                                          const std::string &new_table_id_str,
                                          const std::string &new_db_id_str,
                                          std::vector<std::string> &restored_file_paths,
                                          bool ignore_table_id) {
    Config *config = InfinityContext::instance().config();
    PersistenceManager *persistence_manager = InfinityContext::instance().persistence_manager();

    // Start timing for file restoration
    auto file_restore_start = std::chrono::high_resolution_clock::now();

    for (const auto &file : files_to_restore) {
        std::string src_file_path = fmt::format("{}/{}/{}", snapshot_dir, snapshot_name, file);

        // Replace the old table_id_str with the new one in the file path
        std::string modified_file = file;

        // Find and replace the tbl_* pattern in the file path
        // Look for pattern: /db_*/tbl_* where * can be any number of digits
        size_t db_pos = modified_file.find("db_");
        if (db_pos != std::string::npos) {
            // Replace the old database ID with the new one
            size_t db_end = modified_file.find('/', db_pos + 3);
            if (db_end == std::string::npos) {
                db_end = modified_file.length();
            }
            std::string old_db_id_in_path = modified_file.substr(db_pos + 3, db_end - (db_pos + 3));
            modified_file.replace(db_pos + 3, old_db_id_in_path.length(), new_db_id_str);

            // Replace the old table ID with the new one
            size_t tbl_pos = modified_file.find("/tbl_", db_pos);
            if (tbl_pos != std::string::npos && !ignore_table_id) {
                // Find the end of the table ID (next '/' or end of string)
                size_t tbl_end = modified_file.find('/', tbl_pos + 5);
                if (tbl_end == std::string::npos) {
                    tbl_end = modified_file.length();
                }

                // Extract the old table ID from the path
                std::string old_tbl_id_in_path = modified_file.substr(tbl_pos + 5, tbl_end - (tbl_pos + 5));

                // Replace the old table ID with the new one
                modified_file.replace(tbl_pos + 5, old_tbl_id_in_path.length(), new_table_id_str);
            }
        }

        std::string dst_file_path = fmt::format("{}/{}", config->DataDir(), modified_file);

        if (persistence_manager != nullptr) {
            // Use persistence manager to restore files
            // Create a temporary file path for the source file
            std::string tmp_file_path = fmt::format("{}/{}", config->TempDir(), StringTransform(src_file_path, "/", "_"));

            // Copy source file to temporary location first
            Status copy_status = VirtualStore::Copy(tmp_file_path, src_file_path);
            if (!copy_status.ok()) {
                LOG_WARN(fmt::format("Failed to copy file to temp: {}", copy_status.message()));
                continue;
            }

            // Use persistence manager to persist the file
            PersistResultHandler handler(persistence_manager);

            // Check if this is an index file (in idx_* subdirectory) and disable composition
            bool try_compose = true;
            if (dst_file_path.find("/idx_") != std::string::npos) {
                try_compose = false;
                LOG_DEBUG(fmt::format("Index file detected, disabling composition: {}", dst_file_path));
            }

            PersistWriteResult persist_result = persistence_manager->Persist(dst_file_path, tmp_file_path, try_compose);
            handler.HandleWriteResult(persist_result);

            // Add the destination file path to the output vector
            restored_file_paths.push_back(modified_file);

            LOG_TRACE(fmt::format("Restored file via persistence manager: {} -> {}", src_file_path, dst_file_path));
        } else {
            // Fallback to direct file copying when no persistence manager
            // Create destination directory
            // no race as unique table/db_id_str
            std::string dst_dir = VirtualStore::GetParentPath(dst_file_path);
            if (!VirtualStore::Exists(dst_dir)) {
                VirtualStore::MakeDirectory(dst_dir);
            }
            // there exists empty files getting deleted, so we need to ignore them
            Status copy_status = VirtualStore::Copy(dst_file_path, src_file_path);
            if (!copy_status.ok()) {
                LOG_WARN(fmt::format("Failed to copy file: {}", copy_status.message()));
            } else {
                // Add the destination file path to the output vector
                restored_file_paths.push_back(modified_file);
            }
        }
    }

    // End timing for file restoration
    auto file_restore_end = std::chrono::high_resolution_clock::now();
    auto file_restore_duration = std::chrono::duration_cast<std::chrono::milliseconds>(file_restore_end - file_restore_start);
    LOG_INFO(fmt::format("File restoration took {} ms", file_restore_duration.count()));

    return Status::OK();
}

std::vector<std::string> DatabaseSnapshotInfo::GetFiles() const {
    std::vector<std::string> files;
    for (const auto &table_snapshot : table_snapshots_) {
        std::vector<std::string> table_files = table_snapshot->GetFiles();
        files.insert(files.end(), table_files.begin(), table_files.end());
    }
    return files;
}

Status DatabaseSnapshotInfo::Serialize(const std::string &save_dir, TxnTimeStamp commit_ts) {
    Config *config = InfinityContext::instance().config();
    PersistenceManager *persistence_manager = InfinityContext::instance().persistence_manager();

    // Create temporary directory for atomic operation
    std::string temp_snapshot_dir = fmt::format("{}/temp_{}_{}", save_dir, snapshot_name_, commit_ts);

    // Create temporary directory
    Status create_temp_status = VirtualStore::MakeDirectory(temp_snapshot_dir);
    if (!create_temp_status.ok()) {
        return create_temp_status;
    }

    // Get files from all table snapshots
    std::vector<std::string> original_files = GetFiles();

    // Copy files to temporary location
    if (persistence_manager != nullptr) {
        PersistResultHandler pm_handler(persistence_manager);
        for (const auto &file : original_files) {
            PersistReadResult result = persistence_manager->GetObjCache(file);
            DeferFn defer_fn([&]() {
                auto res = persistence_manager->PutObjCache(file);
                pm_handler.HandleWriteResult(res);
            });

            const ObjAddr &obj_addr = pm_handler.HandleReadResult(result);
            if (!obj_addr.Valid()) {
                // Clean up temp directory on failure
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                std::string error_msg = fmt::format("Failed to find object for local path {}", file);
                return Status::FileNotFound(error_msg);
            }
            std::string read_path = persistence_manager->GetObjPath(obj_addr.obj_key_);
            LOG_INFO(fmt::format("READ: {} from {}", file, read_path));

            auto [reader_handle, reader_open_status] = VirtualStore::Open(read_path, FileAccessMode::kRead);
            if (!reader_open_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return reader_open_status;
            }

            auto seek_status = reader_handle->Seek(obj_addr.part_offset_);
            if (!seek_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return seek_status;
            }

            auto file_size = obj_addr.part_size_;
            auto buffer = std::make_unique<char[]>(file_size);
            auto [nread, read_status] = reader_handle->Read(buffer.get(), file_size);

            std::string dst_file_path = fmt::format("{}/{}", temp_snapshot_dir, file);
            std::string dst_dir = VirtualStore::GetParentPath(dst_file_path);

            if (!VirtualStore::Exists(dst_dir)) {
                Status mkdir_status = VirtualStore::MakeDirectory(dst_dir);
                if (!mkdir_status.ok()) {
                    VirtualStore::RemoveDirectory(temp_snapshot_dir);
                    return mkdir_status;
                }
            }

            auto [write_file_handle, writer_open_status] = VirtualStore::Open(dst_file_path, FileAccessMode::kWrite);
            if (!writer_open_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return writer_open_status;
            }

            Status write_status = write_file_handle->Append(buffer.get(), file_size);
            if (!write_status.ok()) {
                VirtualStore::RemoveDirectory(temp_snapshot_dir);
                return write_status;
            }
            write_file_handle->Sync();
        }
    } else {
        std::string data_dir = config->DataDir();
        for (const auto &file : original_files) {
            std::string src_file_path = fmt::format("{}/{}", data_dir, file);
            std::string dst_file_path = fmt::format("{}/{}", temp_snapshot_dir, file);

            try {
                Status copy_status = VirtualStore::Copy(dst_file_path, src_file_path);
            } catch (const std::exception &e) {
                LOG_WARN(fmt::format("Failed to copy file: {}", e.what()));
            }
        }
    }

    // Create metadata JSON
    nlohmann::json json_res = CreateSnapshotMetadataJSON();
    std::string json_string = json_res.dump();

    // Write metadata to temporary location
    std::string temp_meta_path = fmt::format("{}/{}.json", temp_snapshot_dir, snapshot_name_);
    auto [snapshot_file_handle, status] = VirtualStore::Open(temp_meta_path, FileAccessMode::kWrite);
    if (!status.ok()) {
        VirtualStore::RemoveDirectory(temp_snapshot_dir);
        return status;
    }

    status = snapshot_file_handle->Append(json_string.data(), json_string.size());
    if (!status.ok()) {
        VirtualStore::RemoveDirectory(temp_snapshot_dir);
        return status;
    }
    snapshot_file_handle->Sync();

    // Atomically move temporary directory to final location
    std::string final_snapshot_dir = fmt::format("{}/{}", save_dir, snapshot_name_);

    // Ensure save directory exists
    if (!VirtualStore::Exists(save_dir)) {
        Status mkdir_status = VirtualStore::MakeDirectory(save_dir);
        if (!mkdir_status.ok()) {
            VirtualStore::RemoveDirectory(temp_snapshot_dir);
            return mkdir_status;
        }
    }
    if (VirtualStore::Exists(final_snapshot_dir)) {
        VirtualStore::RemoveDirectory(temp_snapshot_dir);
        return Status::SnapshotAlreadyExists(snapshot_name_);
    }

    // Atomic rename operation
    try {
        Status rename_status = VirtualStore::Rename(temp_snapshot_dir, final_snapshot_dir);
        if (!rename_status.ok()) {
            VirtualStore::RemoveDirectory(temp_snapshot_dir);
            return rename_status;
        }
    } catch (const std::exception &e) {
        VirtualStore::RemoveDirectory(temp_snapshot_dir);
        return Status::SnapshotAlreadyExists(snapshot_name_);
    }

    LOG_INFO(fmt::format("Atomic database snapshot creation completed: {}", json_res.dump()));
    return Status::OK();
}

std::string DatabaseSnapshotInfo::ToString() const {
    return fmt::format("DatabaseSnapshotInfo: db_name: {}, snapshot_name: {}, version: {}", db_name_, snapshot_name_, version_);
}

nlohmann::json DatabaseSnapshotInfo::CreateSnapshotMetadataJSON() const {
    nlohmann::json json_res;
    json_res["version"] = version_;
    json_res["snapshot_name"] = snapshot_name_;
    json_res["snapshot_scope"] = SnapshotScope::kDatabase;
    json_res["database_name"] = db_name_;
    json_res["db_id_str"] = db_id_str_;
    json_res["db_comment"] = db_comment_;
    json_res["db_next_table_id_str"] = db_next_table_id_str_;

    // Serialize all table snapshots
    for (const auto &table_snapshot : table_snapshots_) {
        json_res["table_snapshots"].emplace_back(table_snapshot->CreateSnapshotMetadataJSON());
    }

    return json_res;
}

std::tuple<std::shared_ptr<DatabaseSnapshotInfo>, Status> DatabaseSnapshotInfo::Deserialize(const std::string &snapshot_dir,
                                                                                            const std::string &snapshot_name) {
    LOG_INFO(fmt::format("Deserialize snapshot: {}/{}", snapshot_dir, snapshot_name));

    std::string meta_path = fmt::format("{}/{}/{}.json", snapshot_dir, snapshot_name, snapshot_name);

    if (!VirtualStore::Exists(meta_path)) {
        return {nullptr, Status::FileNotFound(meta_path)};
    }
    auto [meta_file_handle, status] = VirtualStore::Open(meta_path, FileAccessMode::kRead);
    if (!status.ok()) {
        return {nullptr, status};
    }

    i64 file_size = meta_file_handle->FileSize();
    std::string json_str(file_size, 0);
    auto [n_bytes, status_read] = meta_file_handle->Read(json_str.data(), file_size);
    if (!status.ok()) {
        RecoverableError(status_read);
    }
    if ((size_t)file_size != n_bytes) {
        Status status = Status::FileCorrupted(meta_path);
        RecoverableError(status);
    }

    nlohmann::json snapshot_meta_json = nlohmann::json::parse(json_str);

    //    LOG_INFO(snapshot_meta_json.dump());
    // Validate snapshot scope
    if (!snapshot_meta_json.contains("snapshot_scope") || snapshot_meta_json["snapshot_scope"] != SnapshotScope::kDatabase) {
        return {nullptr, Status::Unknown("Invalid snapshot scope")};
    }

    // Create DatabaseSnapshotInfo object
    auto database_snapshot = std::make_shared<DatabaseSnapshotInfo>();

    // Deserialize basic fields
    database_snapshot->version_ = snapshot_meta_json["version"];
    database_snapshot->snapshot_name_ = snapshot_meta_json["snapshot_name"];
    database_snapshot->scope_ = static_cast<SnapshotScope>(snapshot_meta_json["snapshot_scope"]);
    database_snapshot->db_name_ = snapshot_meta_json["database_name"];
    database_snapshot->db_id_str_ = snapshot_meta_json["db_id_str"];
    database_snapshot->db_comment_ = snapshot_meta_json["db_comment"];
    database_snapshot->db_next_table_id_str_ = snapshot_meta_json["db_next_table_id_str"];

    // Deserialize table snapshots
    if (snapshot_meta_json.contains("table_snapshots")) {
        for (const auto &table_snapshot_json : snapshot_meta_json["table_snapshots"]) {
            auto [table_snapshot, table_status] = TableSnapshotInfo::Deserialize(table_snapshot_json);
            if (!table_status.ok()) {
                return {nullptr, table_status};
            }
            database_snapshot->table_snapshots_.emplace_back(table_snapshot);
        }
    }

    return {database_snapshot, Status::OK()};
}

} // namespace infinity