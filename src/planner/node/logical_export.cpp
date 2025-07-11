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

module infinity_core;

import :stl;
import :column_binding;
import :block_index;
import internal_types;
import statement_common;
import data_type;

namespace infinity {

LogicalExport::LogicalExport(u64 node_id,
                             const SharedPtr<TableInfo> &table_info,
                             String schema_name,
                             String table_name,
                             String file_path,
                             bool header,
                             char delimiter,
                             CopyFileType type,
                             SizeT offset,
                             SizeT limit,
                             SizeT row_limit,
                             Vector<u64> column_idx_array,
                             SharedPtr<BlockIndex> block_index)
    : LogicalNode(node_id, LogicalNodeType::kExport), table_info_(table_info), schema_name_(std::move(schema_name)),
      table_name_(std::move(table_name)), file_path_(std::move(file_path)), header_(header), delimiter_(delimiter), file_type_(type), offset_(offset),
      limit_(limit), row_limit_(row_limit), column_idx_array_(std::move(column_idx_array)), block_index_(std::move(block_index)) {}

LogicalExport::~LogicalExport() = default;

Vector<ColumnBinding> LogicalExport::GetColumnBindings() const { return {}; }

SharedPtr<Vector<String>> LogicalExport::GetOutputNames() const { return MakeShared<Vector<String>>(); }

SharedPtr<Vector<SharedPtr<DataType>>> LogicalExport::GetOutputTypes() const { return MakeShared<Vector<SharedPtr<DataType>>>(); }

String LogicalExport::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space > 3) {
        space -= 4;
        arrow_str = "->  ";
    }

    ss << String(space, ' ') << "-> "
       << "Export from: " << file_path_;

    switch (file_type_) {
        case CopyFileType::kCSV: {
            ss << "(CSV) ";
            break;
        }
        case CopyFileType::kJSON: {
            ss << "(JSON) ";
            break;
        }
        case CopyFileType::kFVECS: {
            ss << "(FVECS) ";
            break;
        }
        case CopyFileType::kJSONL: {
            ss << "(JSONL) ";
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

    ss << "to " << schema_name_ << '.' << table_name_ << ", offset " << offset_;
    if (limit_ != 0) {
        ss << " limit " << limit_;
    }

    space += arrow_str.size();

    return ss.str();
}

} // namespace infinity
