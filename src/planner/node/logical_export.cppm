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
import :meta_info;

import data_type;
import internal_types;
import statement_common;

namespace infinity {

struct BlockIndex;

export class LogicalExport : public LogicalNode {
public:
    explicit LogicalExport(u64 node_id,
                           const std::shared_ptr<TableInfo> &table_info,
                           std::string schema_name,
                           std::string table_name,
                           std::string file_path,
                           bool header,
                           char delimiter,
                           CopyFileType type,
                           size_t offset,
                           size_t limit,
                           size_t row_limit,
                           std::vector<u64> column_idx_array,
                           std::shared_ptr<BlockIndex> block_index);

    ~LogicalExport() override;

    [[nodiscard]] std::vector<ColumnBinding> GetColumnBindings() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::string>> GetOutputNames() const final;

    [[nodiscard]] std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final;

    std::string ToString(i64 &space) const final;

    inline std::string name() final { return "LogicalExport"; }

    const std::shared_ptr<TableInfo> &table_info() const { return table_info_; }

    std::shared_ptr<TableInfo> table_info() { return table_info_; }

    [[nodiscard]] CopyFileType FileType() const { return file_type_; }

    [[nodiscard]] inline const std::string &schema_name() const { return schema_name_; }

    [[nodiscard]] inline const std::string &table_name() const { return table_name_; }

    [[nodiscard]] inline const std::string &file_path() const { return file_path_; }

    [[nodiscard]] bool header() const { return header_; }

    [[nodiscard]] char delimiter() const { return delimiter_; }

    [[nodiscard]] size_t offset() const { return offset_; }

    [[nodiscard]] size_t limit() const { return limit_; }

    [[nodiscard]] size_t row_limit() const { return row_limit_; }

    [[nodiscard]] const std::vector<u64> &column_idx_array() const { return column_idx_array_; }

    [[nodiscard]] std::shared_ptr<BlockIndex> block_index() const { return block_index_; }

private:
    std::shared_ptr<TableInfo> table_info_{};

    std::string schema_name_{"default_db"};
    std::string table_name_{};
    std::string file_path_{};
    bool header_{false};
    char delimiter_{','};
    CopyFileType file_type_{CopyFileType::kInvalid};
    size_t offset_{};
    size_t limit_{};
    size_t row_limit_{};
    std::vector<u64> column_idx_array_;
    std::shared_ptr<BlockIndex> block_index_{};
};

} // namespace infinity