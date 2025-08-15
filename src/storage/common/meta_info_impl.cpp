// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:meta_info.impl;

import :meta_info;
import :stl;
import :status;
import :infinity_exception;
import :wal_entry;

import std;
import third_party;

import internal_types;
import column_def;

namespace infinity {

u64 TableInfo::GetColumnIdByName(const std::string &column_name) const {
    auto iter = std::find_if(column_defs_.begin(), column_defs_.end(), [column_name](const std::shared_ptr<ColumnDef> &column_def) {
        return column_def->name() == column_name;
    });
    if (iter == column_defs_.end()) {
        Status status = Status::ColumnNotExist(column_name);
        RecoverableError(status);
    }
    return (*iter)->id();
}

const ColumnDef *TableInfo::GetColumnDefByID(ColumnID column_id) const {
    auto iter = std::find_if(column_defs_.begin(), column_defs_.end(), [column_id](const std::shared_ptr<ColumnDef> &column_def) {
        return static_cast<ColumnID>(column_def->id()) == column_id;
    });
    if (iter == column_defs_.end()) {
        return nullptr;
    }
    return iter->get();
}

const ColumnDef *TableInfo::GetColumnDefByIdx(size_t idx) const {
    if (idx >= column_defs_.size()) {
        return nullptr;
    }
    return column_defs_[idx].get();
}

const ColumnDef *TableInfo::GetColumnDefByName(const std::string &column_name) const {
    auto iter = std::find_if(column_defs_.begin(), column_defs_.end(), [column_name](const std::shared_ptr<ColumnDef> &column_def) {
        return column_def->name() == column_name;
    });
    if (iter == column_defs_.end()) {
        return nullptr;
    }
    return iter->get();
}

size_t TableInfo::GetColumnIdxByID(ColumnID column_id) const {
    auto iter = std::find_if(column_defs_.begin(), column_defs_.end(), [column_id](const std::shared_ptr<ColumnDef> &column_def) {
        return static_cast<ColumnID>(column_def->id()) == column_id;
    });
    if (iter == column_defs_.end()) {
        return -1;
    }
    return std::distance(column_defs_.begin(), iter);
}

} // namespace infinity