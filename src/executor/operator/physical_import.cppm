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

export module infinity_core:physical_import;

import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :zsv;
import :load_meta;
import :infinity_exception;
import :column_vector;
import :logger;
import :meta_info;

import third_party;

import constant_expr;
import sparse_info;
import internal_types;
import statement_common;
import data_type;

namespace infinity {

struct DataBlock;

export class PhysicalImport : public PhysicalOperator {
public:
    explicit PhysicalImport(u64 id,
                            const std::shared_ptr<TableInfo> &table_info,
                            std::string file_path,
                            bool header,
                            char delimiter,
                            CopyFileType type,
                            std::shared_ptr<std::vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id, load_metas), table_info_(table_info), file_type_(type),
          file_path_(std::move(file_path)), header_(header), delimiter_(delimiter) {}

    ~PhysicalImport() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline std::shared_ptr<std::vector<std::string>> GetOutputNames() const final { return output_names_; }

    inline std::shared_ptr<std::vector<std::shared_ptr<DataType>>> GetOutputTypes() const final { return output_types_; }

    void NewImportFVECS(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportCSR(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportBVECS(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportTypeVecs(QueryContext *query_context,
                           ImportOperatorState *import_op_state,
                           EmbeddingDataType embedding_data_type,
                           std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportCSV(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportJSON(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportJSONL(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    void NewImportPARQUET(QueryContext *query_context, ImportOperatorState *import_op_state, std::vector<std::shared_ptr<DataBlock>> &data_blocks);

    inline const TableInfo *table_info() const { return table_info_.get(); }

    inline CopyFileType FileType() const { return file_type_; }

    inline const std::string &file_path() const { return file_path_; }

    inline bool header() const { return header_; }

    inline char delimiter() const { return delimiter_; }
private:
    static void NewCSVHeaderHandler(void *);

    static void NewCSVRowHandler(void *);

    void JSONLRowHandler(std::string_view line_json, std::vector<std::shared_ptr<ColumnVector>> &column_vectors);

    void ParquetValueHandler(const std::shared_ptr<arrow::Array> &array, ColumnVector &column_vector, u64 value_idx);

private:
    std::shared_ptr<std::vector<std::string>> output_names_{};
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types_{};

    std::shared_ptr<TableInfo> table_info_{};
    CopyFileType file_type_{CopyFileType::kInvalid};
    std::string file_path_{};
    bool header_{false};
    char delimiter_{','};
};

export std::shared_ptr<ConstantExpr> BuildConstantExprFromJson(std::string_view json_object);
export std::shared_ptr<ConstantExpr> BuildConstantSparseExprFromJson(std::string_view json_object, const SparseInfo *sparse_info);

} // namespace infinity