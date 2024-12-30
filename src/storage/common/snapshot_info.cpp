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

void TableSnapshotInfo::Serialize(const String &save_dir) {

    Config *config = InfinityContext::instance().config();
    String data_dir = config->DataDir();
    // Get files
    Vector<String> original_files = GetFiles();

    // Copy files
    for (const auto &file : original_files) {
        String src_file_path = fmt::format("{}/{}", data_dir, file);
        String dst_file_path = fmt::format("{}/{}/{}", save_dir, snapshot_name_, file);
        LOG_INFO(fmt::format("Copy from: {} to {}", src_file_path, dst_file_path));
    }

    nlohmann::json json_res;
    json_res["version"] = version_;
    json_res["snapshot_name"] = snapshot_name_;
    json_res["snapshot_scope"] = SnapshotScope::kTable;
    json_res["database_name"] = db_name_;
    json_res["table_name"] = table_name_;
    json_res["table_comment"] = table_comment_;

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
    return files;
}

} // namespace infinity
