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

module;

#include <sstream>

module infinity_core:logical_import.impl;

import :logical_import;

import :stl;
import :column_binding;
import internal_types;
import :logical_import;
import statement_common;
import data_type;

namespace infinity {

Vector<ColumnBinding> LogicalImport::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalImport::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalImport::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalImport::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << String(space, ' ') << "-> "
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
