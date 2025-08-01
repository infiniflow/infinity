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

export module infinity_core:logical_export;

import :stl;
import :logical_node_type;
import :column_binding;
import :logical_node;
import data_type;
import internal_types;
import statement_common;
import :meta_info;

namespace infinity {

struct BlockIndex;

export class LogicalExport : public LogicalNode {
public:
    explicit LogicalExport(u64 node_id,
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
                           SharedPtr<BlockIndex> block_index);

    ~LogicalExport() override;

    [[nodiscard]] Vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] SharedPtr<Vector<String>> GetOutputNames() const final;

    [[nodiscard]] SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final;

    String ToString(i64 &space) const final;

    inline String name() final { return "LogicalExport"; }

    const SharedPtr<TableInfo> &table_info() const { return table_info_; }

    SharedPtr<TableInfo> table_info() { return table_info_; }

    [[nodiscard]] CopyFileType FileType() const { return file_type_; }

    [[nodiscard]] inline const String &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const String &table_name() const { return table_name_; }

    [[nodiscard]] inline const String &file_path() const { return file_path_; }

    [[nodiscard]] bool header() const { return header_; }

    [[nodiscard]] char delimiter() const { return delimiter_; }

    [[nodiscard]] SizeT offset() const { return offset_; }

    [[nodiscard]] SizeT limit() const { return limit_; }

    [[nodiscard]] SizeT row_limit() const { return row_limit_; }

    [[nodiscard]] const Vector<u64> &column_idx_array() const { return column_idx_array_; }

    [[nodiscard]] SharedPtr<BlockIndex> block_index() const { return block_index_; }

private:
    SharedPtr<TableInfo> table_info_{};

    String schema_name_{"default_db"};
    String table_name_{};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};
    CopyFileType file_type_{CopyFileType::kInvalid};
    SizeT offset_{};
    SizeT limit_{};
    SizeT row_limit_{};
    Vector<u64> column_idx_array_;
    SharedPtr<BlockIndex> block_index_{};
};

} // namespace infinity