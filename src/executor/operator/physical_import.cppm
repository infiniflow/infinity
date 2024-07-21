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

export module physical_import;

import stl;

import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import third_party;
import txn;
import txn_store;
import constant_expr;
import table_entry;
import segment_entry;
import block_entry;
import zsv;
import load_meta;
import infinity_exception;
import column_vector;
import internal_types;
import statement_common;
import data_type;
import logger;

namespace infinity {

class ZxvParserCtx {
public:
    ZsvParser parser_;
    SizeT row_count_{};
    SharedPtr<String> err_msg_{};
    TableEntry *const table_entry_{};
    Txn *const txn_{};
    SharedPtr<SegmentEntry> segment_entry_{};
    UniquePtr<BlockEntry> block_entry_{};
    Vector<ColumnVector> column_vectors_{};
    const char delimiter_{};

public:
    ZxvParserCtx(TableEntry *table_entry,
                 Txn *txn,
                 SharedPtr<SegmentEntry> segment_entry,
                 UniquePtr<BlockEntry> block_entry,
                 Vector<ColumnVector> &&column_vectors,
                 char delimiter)
        : row_count_(0), err_msg_(nullptr), table_entry_(table_entry), txn_(txn), segment_entry_(segment_entry), block_entry_(std::move(block_entry)),
          column_vectors_(std::move(column_vectors)), delimiter_(delimiter) {}
};

export class PhysicalImport : public PhysicalOperator {
public:
    explicit PhysicalImport(u64 id,
                            TableEntry *table_entry,
                            String file_path,
                            bool header,
                            char delimiter,
                            CopyFileType type,
                            SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id, load_metas), table_entry_(table_entry), file_type_(type),
          file_path_(std::move(file_path)), header_(header), delimiter_(delimiter) {}

    ~PhysicalImport() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    SizeT TaskletCount() override {
        String error_message = "Not implement: TaskletCount not Implement";
        UnrecoverableError(error_message);
        return 0;
    }

    void ImportFVECS(QueryContext *query_context, ImportOperatorState *import_op_state);

    void ImportCSR(QueryContext *query_context, ImportOperatorState *import_op_state);

    void ImportBVECS(QueryContext *query_context, ImportOperatorState *import_op_state);

    /// for push based execution
    void ImportCSV(QueryContext *query_context, ImportOperatorState *import_op_state);

    /// for push based execution
    void ImportJSON(QueryContext *query_context, ImportOperatorState *import_op_state);

    void ImportJSONL(QueryContext *query_context, ImportOperatorState *import_op_state);

    void ImportPARQUET(QueryContext *query_context, ImportOperatorState *import_op_state);

    inline const TableEntry *table_entry() const { return table_entry_; }

    inline CopyFileType FileType() const { return file_type_; }

    inline const String &file_path() const { return file_path_; }

    inline bool header() const { return header_; }

    inline char delimiter() const { return delimiter_; }

    static void SaveSegmentData(TableEntry *table_entry, Txn *txn, SharedPtr<SegmentEntry> segment_entry);

private:
    static void CSVHeaderHandler(void *);

    static void CSVRowHandler(void *);

    void JSONLRowHandler(const nlohmann::json &line_json, Vector<ColumnVector> &column_vectors);

    void ParquetValueHandler(const SharedPtr<arrow::Array> &array, ColumnVector &column_vector, u64 value_idx);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    TableEntry *table_entry_{};
    CopyFileType file_type_{CopyFileType::kCSV};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};
};

export SharedPtr<ConstantExpr> BuildConstantExprFromJson(const nlohmann::json &json_object);

} // namespace infinity