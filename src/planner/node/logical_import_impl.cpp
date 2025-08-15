// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:logical_import.impl;

import :logical_import;
import :column_binding;
import :logical_import;

import std;

import internal_types;
import statement_common;
import data_type;

namespace infinity {

std::vector<ColumnBinding> LogicalImport::GetColumnBindings() const { return {}; }

std::shared_ptr<std::vector<std::string>> LogicalImport::GetOutputNames() const { return std::make_shared<std::vector<std::string>>(); }

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> LogicalImport::GetOutputTypes() const { return std::make_shared<std::vector<std::shared_ptr<DataType>>>(); }

std::string LogicalImport::ToString(i64 &space) const {
    std::stringstream ss;
    std::string arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << std::string(space, ' ') << "-> "
       << "Import from: " << file_path_;

    switch (file_type_) {
        case CopyFileType::kCSV: {
            ss << "(CSV) ";
            break;
        }
        case CopyFileType::kJSON: {
            ss << "(JSON) ";
            break;
        }
        case CopyFileType::kJSONL: {
            ss << "(JSONL) ";
            break;
        }
        case CopyFileType::kFVECS: {
            ss << "(FVECS) ";
            break;
        }
        case CopyFileType::kCSR: {
            ss << "(CSR) ";
            break;
        }
        case CopyFileType::kBVECS: {
            ss << "(BVECS) ";
            break;
        }
        case CopyFileType::kPARQUET: {
            ss << "(PARQUET) ";
            break;
        }
        case CopyFileType::kInvalid: {
            ss << "(Invalid) ";
            break;
        }
    }

    ss << "to " << *table_info_->db_name_ << '.' << *table_info_->table_name_;

    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
