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

export module infinity_core:physical_export;

import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :load_meta;
import :infinity_exception;
import :meta_info;
import :logger;
import :column_vector;
import :knn_filter;

import third_party;

import internal_types;
import statement_common;
import data_type;
import column_def;

namespace infinity {

struct BlockIndex;

export class PhysicalExport : public PhysicalOperator {
public:
    explicit PhysicalExport(u64 id,
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
                            std::shared_ptr<BlockIndex> block_index,
                            std::shared_ptr<std::vector<LoadMeta>> load_metas);

    ~PhysicalExport() override;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    size_t ExportToCSV(QueryContext *query_context, ExportOperatorState *export_op_state);

    size_t ExportToJSONL(QueryContext *query_context, ExportOperatorState *export_op_state);

    size_t ExportToFileInner(QueryContext *query_context, ExportOperatorState *export_op_state, std::function<std::string(const std::vector<ColumnVector> &, size_t)> line_to_string);

    size_t ExportToFVECS(QueryContext *query_context, ExportOperatorState *export_op_state);

    size_t ExportToPARQUET(QueryContext *query_context, ExportOperatorState *export_op_state);

    inline CopyFileType FileType() const { return file_type_; }

    inline const std::string &file_path() const { return file_path_; }

    inline const std::string &schema_name() const { return schema_name_; }

    inline const std::string &table_name() const { return table_name_; }

    inline bool header() const { return header_; }

    inline char delimiter() const { return delimiter_; }

private:
    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};

    std::shared_ptr<TableInfo> table_info_{};
    CopyFileType file_type_{CopyFileType::kInvalid};
    std::string file_path_{};
    std::string table_name_{};
    std::string schema_name_{"default_db"};
    bool header_{false};
    char delimiter_{','};
    size_t offset_{};
    size_t limit_{};
    size_t row_limit_{};
    std::vector<u64> column_idx_array_;
    std::shared_ptr<BlockIndex> block_index_{};
};

} // namespace infinity
