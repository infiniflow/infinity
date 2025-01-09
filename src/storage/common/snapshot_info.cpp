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

nlohmann::json BlockSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["block_id"] = block_id_;
    json_res["block_dir"] = block_dir_;
    for (const auto &column_block_snapshot : column_block_snapshots_) {
        json_res["columns"].emplace_back(column_block_snapshot->Serialize());
    }
    return json_res;
}

nlohmann::json SegmentSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["segment_id"] = segment_id_;
    json_res["segment_dir"] = segment_dir_;
    for (const auto &block_snapshot : block_snapshots_) {
        json_res["blocks"].emplace_back(block_snapshot->Serialize());
    }
    return json_res;
}

nlohmann::json ChunkIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    return json_res;
}

nlohmann::json SegmentIndexSnapshotInfo::Serialize() {
    nlohmann::json json_res;
    json_res["segment_id"] = segment_id_;
    for (const auto &chunk_index_snapshot : chunk_index_snapshots_) {
        json_res["chunk_indexes"].emplace_back(chunk_index_snapshot->Serialize());
    }
    return json_res;
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

    json_res["next_column_id"] = next_column_id_;
    json_res["next_segment_id"] = next_segment_id_;

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
        }
    }

    for (const auto &table_index_snapshot_pair : table_index_snapshots_) {
        for (const auto &segment_index_snapshot_pair : table_index_snapshot_pair.second->index_by_segment_) {
            for (const auto &chunk_index_snapshot : segment_index_snapshot_pair.second->chunk_index_snapshots_) {
                if(chunk_index_snapshot->files_.empty()) {
                    files.emplace_back(VirtualStore::ConcatenatePath(*table_index_snapshot_pair.second->index_dir_, chunk_index_snapshot->base_name_));
                } else {
                    files.insert(files.end(), chunk_index_snapshot->files_.cbegin(), chunk_index_snapshot->files_.cend());
                }
            }
        }
    }
    return files;
}

} // namespace infinity
