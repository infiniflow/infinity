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

#include <vector>

module snapshot_info;

import stl;
import status;
import command_statement;
// import index_base;
import third_party;
import constant_expr;
import logger;
import virtual_store;
import local_file_handle;
import infinity_exception;
import infinity_context;
import config;
import persistence_manager;
import persist_result_handler;
import defer_op;
import utility;
import block_version;
import data_type;
import parsed_expr;
import segment_entry;
import create_index_info;

namespace infinity {

nlohmann::json BlockColumnSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["column_id"] = column_id_;
    json_res["filename"] = filename_;
    json_res["last_chunk_offset"] = last_chunk_offset_;
    for (const auto &outline_snapshot : outline_snapshots_) {
        json_res["outlines"].emplace_back(outline_snapshot->filename_);
    }
    return json_res;
}

SharedPtr<BlockColumnSnapshotInfo> BlockColumnSnapshotInfo::Deserialize(const nlohmann::json &column_block_json) {
    auto column_block_snapshot = MakeShared<BlockColumnSnapshotInfo>();
    column_block_snapshot->column_id_ = column_block_json["column_id"];
    column_block_snapshot->filename_ = column_block_json["filename"];
    column_block_snapshot->last_chunk_offset_ = column_block_json["last_chunk_offset"];
    if (column_block_json.contains("outlines")) {
        for (const auto &outline_snapshot : column_block_json["outlines"]) {
            auto outline_snapshot_info = MakeShared<OutlineSnapshotInfo>();
            outline_snapshot_info->filename_ = outline_snapshot;
            column_block_snapshot->outline_snapshots_.emplace_back(outline_snapshot_info);
        }
    }
    return column_block_snapshot;
}

nlohmann::json BlockSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["block_id"] = block_id_;
    json_res["block_dir"] = block_dir_;
    json_res["row_count"] = row_count_;
    json_res["min_row_ts"] = min_row_ts_;
    json_res["max_row_ts"] = max_row_ts_;
    for (const auto &column_block_snapshot : column_block_snapshots_) {
        json_res["columns"].emplace_back(column_block_snapshot->Serialize());
    }
    return json_res;
}

SharedPtr<BlockSnapshotInfo> BlockSnapshotInfo::Deserialize(const nlohmann::json &block_json) {
    auto block_snapshot = MakeShared<BlockSnapshotInfo>();
    block_snapshot->block_id_ = block_json["block_id"];
    block_snapshot->block_dir_ = block_json["block_dir"];
    block_snapshot->min_row_ts_ = block_json["min_row_ts"];
    block_snapshot->max_row_ts_ = block_json["max_row_ts"];
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

SharedPtr<SegmentSnapshotInfo> SegmentSnapshotInfo::Deserialize(const nlohmann::json &segment_json) {
    auto segment_snapshot = MakeShared<SegmentSnapshotInfo>();
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
    for (const auto &file : files_) {
        json_res["files"].emplace_back(file);
    }
    return json_res;
}

SharedPtr<ChunkIndexSnapshotInfo> ChunkIndexSnapshotInfo::Deserialize(const nlohmann::json &chunk_index_json) {
    auto chunk_index_snapshot = MakeShared<ChunkIndexSnapshotInfo>();
    chunk_index_snapshot->chunk_id_ = chunk_index_json["chunk_id"];
    chunk_index_snapshot->base_name_ = chunk_index_json["base_name"];
    for (auto &file : chunk_index_snapshot->files_) {
        chunk_index_snapshot->files_.emplace_back(file);
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

SharedPtr<SegmentIndexSnapshotInfo> SegmentIndexSnapshotInfo::Deserialize(const nlohmann::json &segment_index_json) {
    auto segment_index_snapshot = MakeShared<SegmentIndexSnapshotInfo>();
    segment_index_snapshot->segment_id_ = segment_index_json["segment_id"];
    for (const auto &chunk_index_json : segment_index_json["chunk_indexes"]) {
        auto chunk_index_snapshot = ChunkIndexSnapshotInfo::Deserialize(chunk_index_json);
        segment_index_snapshot->chunk_index_snapshots_.emplace_back(chunk_index_snapshot);
    }
    return segment_index_snapshot;
}

nlohmann::json TableIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["index_dir"] = *index_dir_;
    json_res["index_base"] = index_base_->Serialize();
    for (const auto &segment_index_entry : index_by_segment_) {
        json_res["segment_indexes"].emplace_back(segment_index_entry.second->Serialize());
    }
    return json_res;
}

SharedPtr<TableIndexSnapshotInfo> TableIndexSnapshotInfo::Deserialize(const nlohmann::json &table_index_json) {
    auto table_index_snapshot = MakeShared<TableIndexSnapshotInfo>();
    table_index_snapshot->index_dir_ = MakeShared<String>(table_index_json["index_dir"]);
    table_index_snapshot->index_base_ = IndexBase::Deserialize(table_index_json["index_base"]);
    for (const auto &segment_index_json : table_index_json["segment_indexes"]) {
        auto segment_index_snapshot = SegmentIndexSnapshotInfo::Deserialize(segment_index_json);
        table_index_snapshot->index_by_segment_.emplace(segment_index_snapshot->segment_id_, segment_index_snapshot);
    }
    return table_index_snapshot;
}

void TableSnapshotInfo::Serialize(const String &save_dir) {

    Config *config = InfinityContext::instance().config();
    PersistenceManager *persistence_manager = InfinityContext::instance().persistence_manager();

    // Create compressed file
    //    String compressed_filename = fmt::format("{}/{}.lz4", save_dir, snapshot_name_);
    //    std::ofstream output_stream = VirtualStore::BeginCompress(compressed_filename);

    // Get files
    Vector<String> original_files = GetFiles();

    // Copy files
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
                String error_message = fmt::format("Failed to find object for local path {}", file);
                UnrecoverableError(error_message);
            }
            String read_path = persistence_manager->GetObjPath(obj_addr.obj_key_);
            LOG_INFO(fmt::format("READ: {} from {}", file, read_path));

            auto [reader_handle, reader_open_status] = VirtualStore::Open(read_path, FileAccessMode::kRead);
            if (!reader_open_status.ok()) {
                UnrecoverableError(reader_open_status.message());
            }

            auto seek_status = reader_handle->Seek(obj_addr.part_offset_);
            if (!seek_status.ok()) {
                UnrecoverableError(seek_status.message());
            }

            auto file_size = obj_addr.part_size_;
            auto buffer = std::make_unique<char[]>(file_size);
            auto [nread, read_status] = reader_handle->Read(buffer.get(), file_size);

            String dst_file_path = fmt::format("{}/{}/{}", save_dir, snapshot_name_, file);
            String dst_dir = VirtualStore::GetParentPath(dst_file_path);
            if (!VirtualStore::Exists(dst_dir)) {
                VirtualStore::MakeDirectory(dst_dir);
            }

            auto [write_file_handle, writer_open_status] = VirtualStore::Open(dst_file_path, FileAccessMode::kWrite);
            if (!writer_open_status.ok()) {
                UnrecoverableError(writer_open_status.message());
            }

            Status write_status = write_file_handle->Append(buffer.get(), file_size);
            if (!write_status.ok()) {
                UnrecoverableError(write_status.message());
            }
            write_file_handle->Sync();

            //            Status compress_status = VirtualStore::AddFileCompress(output_stream, dst_file_path);
            //            if (!compress_status.ok()) {
            //                RecoverableError(compress_status);
            //            }
        }
    } else {
        String data_dir = config->DataDir();
        for (const auto &file : original_files) {
            String src_file_path = fmt::format("{}/{}", data_dir, file);
            String dst_file_path = fmt::format("{}/{}/{}", save_dir, snapshot_name_, file);
            //        LOG_INFO(fmt::format("Copy from: {} to {}", src_file_path, dst_file_path));
            Status copy_status = VirtualStore::Copy(dst_file_path, src_file_path);
            if (!copy_status.ok()) {
                RecoverableError(copy_status);
            }

            //            Status compress_status = VirtualStore::AddFileCompress(output_stream, dst_file_path);
            //            if (!compress_status.ok()) {
            //                RecoverableError(compress_status);
            //            }
        }
    }

    //    VirtualStore::EndCompress(output_stream);

    //    String md5 = CalcMD5(compressed_filename);

    // Remove the directory
    //    String directory = fmt::format("{}/{}", save_dir, snapshot_name_);
    //    VirtualStore::RemoveDirectory(directory);

    nlohmann::json json_res;
    json_res["version"] = version_;
    json_res["snapshot_name"] = snapshot_name_;
    json_res["snapshot_scope"] = SnapshotScope::kTable;
    json_res["database_name"] = db_name_;
    json_res["table_name"] = table_name_;
    json_res["table_comment"] = table_comment_;
    //    json_res["md5"] = md5;

    json_res["txn_id"] = txn_id_;
    json_res["begin_ts"] = begin_ts_;
    json_res["commit_ts"] = commit_ts_;
    json_res["max_commit_ts"] = max_commit_ts_;
    json_res["table_entry_dir"] = table_entry_dir_;

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

    String json_string = json_res.dump();

    Path save_path = Path(save_dir) / fmt::format("{}.json", snapshot_name_);

    if (!VirtualStore::Exists(save_dir)) {
        VirtualStore::MakeDirectory(save_dir);
    }
    auto [snapshot_file_handle, status] = VirtualStore::Open(save_path.string(), FileAccessMode::kWrite);
    if (!status.ok()) {
        UnrecoverableError(fmt::format("{}: {}", save_path.string(), status.message()));
    }

    status = snapshot_file_handle->Append(json_string.data(), json_string.size());
    if (!status.ok()) {
        RecoverableError(status);
    }
    snapshot_file_handle->Sync();

    LOG_INFO(fmt::format("{}", json_res.dump()));

    // return json_res;
}

Vector<String> TableSnapshotInfo::GetFiles() const {
    Vector<String> files;
    for (const auto &segment_snapshot_pair : segment_snapshots_) {
        for (const auto &block_snapshot : segment_snapshot_pair.second->block_snapshots_) {
            for (const auto &column_block_snapshot : block_snapshot->column_block_snapshots_) {
                files.emplace_back(VirtualStore::ConcatenatePath(block_snapshot->block_dir_, column_block_snapshot->filename_));
                for (const auto &outline_snapshot : column_block_snapshot->outline_snapshots_) {
                    files.emplace_back(VirtualStore::ConcatenatePath(block_snapshot->block_dir_, outline_snapshot->filename_));
                }
            }

            files.emplace_back(VirtualStore::ConcatenatePath(block_snapshot->block_dir_, *BlockVersion::FileName()));
        }
    }

    for (const auto &table_index_snapshot_pair : table_index_snapshots_) {
        for (const auto &segment_index_snapshot_pair : table_index_snapshot_pair.second->index_by_segment_) {
            SegmentID segment_id=segment_index_snapshot_pair.second->segment_id_;
            for (const auto &chunk_index_snapshot : segment_index_snapshot_pair.second->chunk_index_snapshots_) {
                IndexBase *index_base = table_index_snapshot_pair.second->index_base_.get();

                switch (index_base->index_type_) {
                    case IndexType::kIVF: {
                        break;
                    }
                    case IndexType::kHnsw: {
                        break;
                    }
                    case IndexType::kBMP: {
                        break;
                    }
                    case IndexType::kFullText: {
                        break;
                    }
                    case IndexType::kSecondary: {
                        files.emplace_back(
                VirtualStore::ConcatenatePath(*table_index_snapshot_pair.second->index_dir_,fmt::format("seg{}_chunk{}.idx", segment_id, chunk_index_snapshot->chunk_id_) ));
                        break;
                    }
                    case IndexType::kEMVB: {
                        break;
                    }
                    case IndexType::kDiskAnn: {
                        break;
                    }
                    case IndexType::kInvalid: {
                        UnrecoverableError("Invalid index type");
                        break;
                    }
                }

                // if (chunk_index_snapshot->files_.empty()) {
                //     files.emplace_back(
                //         VirtualStore::ConcatenatePath(*table_index_snapshot_pair.second->index_dir_, chunk_index_snapshot->index_filename_));
                // } else {
                //     files.insert(files.end(), chunk_index_snapshot->files_.cbegin(), chunk_index_snapshot->files_.cend());
                // }
            }
        }
    }
    return files;
}

Tuple<SharedPtr<TableSnapshotInfo>, Status> TableSnapshotInfo::Deserialize(const String &snapshot_dir, const String &snapshot_name) {
    //    LOG_INFO(fmt::format("Deserialize snapshot: {}/{}", snapshot_dir, snapshot_name));

    String meta_path = fmt::format("{}/{}.json", snapshot_dir, snapshot_name);

    if (!VirtualStore::Exists(meta_path)) {
        return {nullptr, Status::FileNotFound(meta_path)};
    }
    auto [meta_file_handle, status] = VirtualStore::Open(meta_path, FileAccessMode::kRead);
    if (!status.ok()) {
        return {nullptr, status};
    }

    i64 file_size = meta_file_handle->FileSize();
    String json_str(file_size, 0);
    auto [n_bytes, status_read] = meta_file_handle->Read(json_str.data(), file_size);
    if (!status.ok()) {
        RecoverableError(status_read);
    }
    if ((SizeT)file_size != n_bytes) {
        Status status = Status::FileCorrupted(meta_path);
        RecoverableError(status);
    }

    nlohmann::json snapshot_meta_json = nlohmann::json::parse(json_str);

    //    LOG_INFO(snapshot_meta_json.dump());

    SharedPtr<TableSnapshotInfo> table_snapshot = MakeShared<TableSnapshotInfo>();

    table_snapshot->snapshot_name_ = snapshot_meta_json["snapshot_name"];
    SnapshotScope scope = static_cast<SnapshotScope>(snapshot_meta_json["snapshot_scope"]);
    if (scope != SnapshotScope::kTable) {
        return {nullptr, Status::Unknown("Invalid snapshot scope")};
    }

    table_snapshot->scope_ = SnapshotScope::kTable;
    table_snapshot->version_ = snapshot_meta_json["version"];
    table_snapshot->db_name_ = snapshot_meta_json["database_name"];
    table_snapshot->table_name_ = snapshot_meta_json["table_name"];
    table_snapshot->table_comment_ = snapshot_meta_json["table_comment"];

    table_snapshot->txn_id_ = snapshot_meta_json["txn_id"];
    table_snapshot->begin_ts_ = snapshot_meta_json["begin_ts"];
    table_snapshot->commit_ts_ = snapshot_meta_json["commit_ts"];
    table_snapshot->max_commit_ts_ = snapshot_meta_json["max_commit_ts"];
    table_snapshot->table_entry_dir_ = snapshot_meta_json["table_entry_dir"];
    table_snapshot->next_column_id_ = snapshot_meta_json["next_column_id"];
    table_snapshot->unsealed_id_ = snapshot_meta_json["unsealed_id"];
    table_snapshot->next_segment_id_ = snapshot_meta_json["next_segment_id"];
    table_snapshot->row_count_ = snapshot_meta_json["row_count"];

    for (const auto &column_def_json : snapshot_meta_json["column_definition"]) {
        SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json["column_type"]);
        i64 column_id = column_def_json["column_id"];
        String column_name = column_def_json["column_name"];

        std::set<ConstraintType> constraints;
        if (column_def_json.contains("constraints")) {
            for (const auto &column_constraint : column_def_json["constraints"]) {
                ConstraintType constraint = column_constraint;
                constraints.emplace(constraint);
            }
        }

        String comment;
        if (column_def_json.contains("column_comment")) {
            comment = column_def_json["column_comment"];
        }

        SharedPtr<ParsedExpr> default_expr = nullptr;
        if (column_def_json.contains("default")) {
            default_expr = ConstantExpr::Deserialize(column_def_json["default"]);
        }

        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints, comment, default_expr);
        table_snapshot->columns_.emplace_back(column_def);
    }

    for (const auto &segment_meta_json : snapshot_meta_json["segments"]) {
        SharedPtr<SegmentSnapshotInfo> segment_snapshot = SegmentSnapshotInfo::Deserialize(segment_meta_json);
        table_snapshot->segment_snapshots_.emplace(segment_snapshot->segment_id_, segment_snapshot);
    }

    for (const auto &table_index_meta_json : snapshot_meta_json["table_indexes"]) {
        SharedPtr<TableIndexSnapshotInfo> table_index_snapshot = TableIndexSnapshotInfo::Deserialize(table_index_meta_json);
        table_snapshot->table_index_snapshots_.emplace(*table_index_snapshot->index_base_->index_name_, table_index_snapshot);
    }

    //    LOG_INFO(table_snapshot->ToString());
    //    LOG_INFO(fmt::format("Deserialize src data: {}/{}", snapshot_dir, snapshot_name));
    Vector<String> original_files = table_snapshot->GetFiles();
    Config *config = InfinityContext::instance().config();
    String data_dir = config->DataDir();
    for (const auto &file : original_files) {
        String src_file_path = fmt::format("{}/{}/{}", snapshot_dir, snapshot_name, file);
        String dst_file_path = fmt::format("{}/{}", data_dir, file);
        //        LOG_INFO(fmt::format("Copy from: {} to {}", src_file_path, dst_file_path));
        Status copy_status = VirtualStore::Copy(dst_file_path, src_file_path);
        if (!copy_status.ok()) {
            RecoverableError(copy_status);
        }
    }

    return {table_snapshot, Status::OK()};
}

String TableSnapshotInfo::ToString() const {
    return fmt::format("TableSnapshotInfo: db_name: {}, table_name: {}, snapshot_name: {}, version: {}",
                       db_name_,
                       table_name_,
                       snapshot_name_,
                       version_);
}

} // namespace infinity
