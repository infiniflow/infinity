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

export module infinity_core:physical_import;

import :stl;
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
                            const SharedPtr<TableInfo> &table_info,
                            String file_path,
                            bool header,
                            char delimiter,
                            CopyFileType type,
                            SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id, load_metas), table_info_(table_info), file_type_(type),
          file_path_(std::move(file_path)), header_(header), delimiter_(delimiter) {}

    ~PhysicalImport() override = default;

    void Init(QueryContext *query_context) override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    void NewImportFVECS(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportCSR(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportBVECS(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportTypeVecs(QueryContext *query_context,
                           ImportOperatorState *import_op_state,
                           EmbeddingDataType embedding_data_type,
                           Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportCSV(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportJSON(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportJSONL(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    void NewImportPARQUET(QueryContext *query_context, ImportOperatorState *import_op_state, Vector<SharedPtr<DataBlock>> &data_blocks);

    inline const TableInfo *table_info() const { return table_info_.get(); }

    inline CopyFileType FileType() const { return file_type_; }

    inline const String &file_path() const { return file_path_; }

    inline bool header() const { return header_; }

    inline char delimiter() const { return delimiter_; }
private:
    static void NewCSVHeaderHandler(void *);

    static void NewCSVRowHandler(void *);

    void JSONLRowHandler(std::string_view line_json, Vector<SharedPtr<ColumnVector>> &column_vectors);

    void ParquetValueHandler(const SharedPtr<arrow::Array> &array, ColumnVector &column_vector, u64 value_idx);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    SharedPtr<TableInfo> table_info_{};
    CopyFileType file_type_{CopyFileType::kInvalid};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};
};

export SharedPtr<ConstantExpr> BuildConstantExprFromJson(std::string_view json_object);
export SharedPtr<ConstantExpr> BuildConstantSparseExprFromJson(std::string_view json_object, const SparseInfo *sparse_info);

} // namespace infinity