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

namespace infinity {

nlohmann::json BlockColumnSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["column_id"] = column_id_;
    json_res["filename"] = filename_;
    for (const auto &outline_snapshot : outline_snapshots_) {
        json_res["outlines"].emplace_back(outline_snapshot->filename_);
    }
    return json_res;
}

SharedPtr<BlockColumnSnapshotInfo> BlockColumnSnapshotInfo::Deserialize(std::string_view column_block_str) {
    simdjson::padded_string column_block_json(column_block_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(column_block_json);
    auto column_block_snapshot = MakeShared<BlockColumnSnapshotInfo>();
    column_block_snapshot->column_id_ = doc["column_id"].get<ColumnID>();
    column_block_snapshot->filename_ = doc["filename"].get<String>();
    if (simdjson::array array; doc["outlines"].get(array) == simdjson::SUCCESS) {
        for (auto outline_snapshot : array) {
            auto outline_snapshot_info = MakeShared<OutlineSnapshotInfo>();
            outline_snapshot_info->filename_ = outline_snapshot.get<String>();
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

SharedPtr<BlockSnapshotInfo> BlockSnapshotInfo::Deserialize(std::string_view block_str) {
    simdjson::padded_string block_json(block_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(block_json);
    auto block_snapshot = MakeShared<BlockSnapshotInfo>();
    block_snapshot->block_id_ = doc["block_id"].get<BlockID>();
    block_snapshot->block_dir_ = doc["block_dir"].get<String>();
    for (auto column_block_json : doc["columns"]) {
        auto column_block_snapshot = BlockColumnSnapshotInfo::Deserialize(column_block_json.raw_json());
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

SharedPtr<SegmentSnapshotInfo> SegmentSnapshotInfo::Deserialize(std::string_view segment_str) {
    simdjson::padded_string segment_json(segment_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(segment_json);
    auto segment_snapshot = MakeShared<SegmentSnapshotInfo>();
    segment_snapshot->segment_id_ = doc["segment_id"].get<SegmentID>();
    segment_snapshot->segment_dir_ = doc["segment_dir"].get<String>();

    segment_snapshot->first_delete_ts_ = doc["first_delete_ts"].get<TxnTimeStamp>();
    segment_snapshot->deprecate_ts_ = doc["deprecate_ts"].get<TxnTimeStamp>();
    segment_snapshot->row_count_ = doc["row_count"].get<TxnTimeStamp>();
    segment_snapshot->actual_row_count_ = doc["actual_row_count"].get<TxnTimeStamp>();
    segment_snapshot->status_ = (SegmentStatus)(u8)doc["segment_status"].get<u8>();

    for (auto block_json : doc["blocks"]) {
        auto block_snapshot = BlockSnapshotInfo::Deserialize(block_json.raw_json());
        segment_snapshot->block_snapshots_.emplace_back(block_snapshot);
    }
    return segment_snapshot;
}

nlohmann::json ChunkIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    return json_res;
}

SharedPtr<ChunkIndexSnapshotInfo> ChunkIndexSnapshotInfo::Deserialize(std::string_view chunk_index_str) {
    auto chunk_index_snapshot = MakeShared<ChunkIndexSnapshotInfo>();
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

SharedPtr<SegmentIndexSnapshotInfo> SegmentIndexSnapshotInfo::Deserialize(std::string_view segment_index_str) {
    simdjson::padded_string segment_index_json(segment_index_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(segment_index_json);
    auto segment_index_snapshot = MakeShared<SegmentIndexSnapshotInfo>();
    segment_index_snapshot->segment_id_ = doc["segment_id"].get<SegmentID>();
    for (auto chunk_index_json : doc["chunk_indexes"]) {
        auto chunk_index_snapshot = ChunkIndexSnapshotInfo::Deserialize(chunk_index_json.raw_json());
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

SharedPtr<TableIndexSnapshotInfo> TableIndexSnapshotInfo::Deserialize(std::string_view table_index_str) {
    simdjson::padded_string table_index_json(table_index_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(table_index_json);
    auto table_index_snapshot = MakeShared<TableIndexSnapshotInfo>();
    table_index_snapshot->index_dir_ = MakeShared<String>(doc["index_dir"].get<String>());
    table_index_snapshot->index_base_ = IndexBase::Deserialize(doc["index_base"].raw_json());
    for (auto segment_index_json : doc["segment_indexes"]) {
        auto segment_index_snapshot = SegmentIndexSnapshotInfo::Deserialize(segment_index_json.raw_json());
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
            for (const auto &chunk_index_snapshot : segment_index_snapshot_pair.second->chunk_index_snapshots_) {
                if (chunk_index_snapshot->files_.empty()) {
                    files.emplace_back(
                        VirtualStore::ConcatenatePath(*table_index_snapshot_pair.second->index_dir_, chunk_index_snapshot->base_name_));
                } else {
                    files.insert(files.end(), chunk_index_snapshot->files_.cbegin(), chunk_index_snapshot->files_.cend());
                }
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

    simdjson::padded_string snapshot_meta_json(json_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(snapshot_meta_json);

    //    LOG_INFO(snapshot_meta_json.dump());

    SharedPtr<TableSnapshotInfo> table_snapshot = MakeShared<TableSnapshotInfo>();

    table_snapshot->snapshot_name_ = doc["snapshot_name"].get<String>();
    SnapshotScope scope = (SnapshotScope)(uint8_t)doc["snapshot_scope"].get<uint8_t>();
    if (scope != SnapshotScope::kTable) {
        return {nullptr, Status::Unknown("Invalid snapshot scope")};
    }

    table_snapshot->scope_ = SnapshotScope::kTable;
    table_snapshot->version_ = doc["version"].get<SizeT>();
    table_snapshot->db_name_ = doc["database_name"].get<String>();
    table_snapshot->table_name_ = doc["table_name"].get<String>();
    table_snapshot->table_comment_ = doc["table_comment"].get<String>();

    table_snapshot->txn_id_ = doc["txn_id"].get<TxnTimeStamp>();
    table_snapshot->begin_ts_ = doc["begin_ts"].get<TxnTimeStamp>();
    table_snapshot->commit_ts_ = doc["commit_ts"].get<TxnTimeStamp>();
    table_snapshot->max_commit_ts_ = doc["max_commit_ts"].get<TxnTimeStamp>();
    table_snapshot->table_entry_dir_ = doc["table_entry_dir"].get<String>();
    table_snapshot->next_column_id_ = doc["next_column_id"].get<ColumnID>();
    table_snapshot->unsealed_id_ = doc["unsealed_id"].get<SegmentID>();
    table_snapshot->next_segment_id_ = doc["next_segment_id"].get<SegmentID>();
    table_snapshot->row_count_ = doc["row_count"].get<SizeT>();

    for (simdjson::array array = doc["column_definition"]; simdjson::simdjson_result<simdjson::value> column_def_json : array) {
        SharedPtr<DataType> data_type = DataType::Deserialize(column_def_json.raw_json());
        i64 column_id = column_def_json["column_id"].get<i64>();
        String column_name = column_def_json["column_name"].get<String>();

        std::set<ConstraintType> constraints;
        if (simdjson::array constraints_json; doc["constraints"].get(constraints_json) == simdjson::SUCCESS) {
            for (auto column_constraint : constraints_json) {
                ConstraintType constraint = (ConstraintType)(char)column_constraint.get<char>();
                constraints.emplace(constraint);
            }
        }

        String comment;
        if (String comment_json; doc["column_comment"].get(comment_json) == simdjson::SUCCESS) {
            comment = comment_json;
        }

        SharedPtr<ParsedExpr> default_expr = nullptr;
        if (auto default_expr_json = doc["default"]; default_expr_json.error() == simdjson::SUCCESS) {
            default_expr = ConstantExpr::Deserialize(default_expr_json);
        }

        SharedPtr<ColumnDef> column_def = MakeShared<ColumnDef>(column_id, data_type, column_name, constraints, comment, default_expr);
        table_snapshot->columns_.emplace_back(column_def);
    }

    for (simdjson::array array = doc["segments"]; auto segment_meta_json : array) {
        SharedPtr<SegmentSnapshotInfo> segment_snapshot = SegmentSnapshotInfo::Deserialize(segment_meta_json.raw_json());
        table_snapshot->segment_snapshots_.emplace(segment_snapshot->segment_id_, segment_snapshot);
    }

    for (simdjson::array array = doc["table_indexes"]; auto table_index_meta_json : array) {
        SharedPtr<TableIndexSnapshotInfo> table_index_snapshot = TableIndexSnapshotInfo::Deserialize(table_index_meta_json.raw_json());
        table_snapshot->table_index_snapshots_.emplace(*table_index_snapshot->index_base_->index_name_, table_index_snapshot);
    }

    //    LOG_INFO(table_snapshot->ToString());

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
