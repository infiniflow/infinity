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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import third_party;
import txn;
import txn_store;
import table_collection_entry;
import segment_entry;
import zsv;

export module physical_import;


namespace infinity {

// class TableCollectionEntry;
// class Txn;
// class SegmentEntry;
// class ImportInputState;
// class ImportOutputState;

export class ParserContext {
public:
    ZsvParser parser_;
    SizeT row_count_{};
    SharedPtr<String> err_msg_{};
    TableCollectionEntry *const table_collection_entry_{};
    Txn *const txn_{};
    SharedPtr<SegmentEntry> segment_entry_{};
    const char delimiter_{};

public:
    ParserContext(TableCollectionEntry *table_collection_entry, Txn *txn, SharedPtr<SegmentEntry> &segment_entry, char delimiter)
        : row_count_(0), err_msg_(nullptr), table_collection_entry_(table_collection_entry), txn_(txn), segment_entry_(segment_entry),
          delimiter_(delimiter) {}
};

export class PhysicalImport : public PhysicalOperator {
public:
    explicit PhysicalImport(u64 id, TableCollectionEntry *table_collection_entry, String file_path, bool header, char delimiter, CopyFileType type)
        : PhysicalOperator(PhysicalOperatorType::kImport, nullptr, nullptr, id), table_collection_entry_(table_collection_entry),
          file_path_(Move(file_path)), header_(header), delimiter_(delimiter), file_type_(type) {}

    ~PhysicalImport() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    void ImportFVECS(QueryContext *query_context, ImportOperatorState* import_op_state);

    /// for push based execution
    void ImportCSV(QueryContext *query_context, ImportOperatorState* import_op_state);

    /// for push based execution
    void ImportJSON(QueryContext *query_context, ImportOperatorState* import_op_state);

    inline const TableCollectionEntry *table_collection_entry() const { return table_collection_entry_; }

    inline CopyFileType FileType() const { return file_type_; }

    inline const String &file_path() const { return file_path_; }

    inline bool header() const { return header_; }

    inline char delimiter() const { return delimiter_; }

    static void SaveSegmentData(TxnTableStore *txn_store, SharedPtr<SegmentEntry> &segment_entry);

private:
    static void CSVHeaderHandler(void *);

    static void CSVRowHandler(void *);

private:
    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};

    TableCollectionEntry *table_collection_entry_{};
    CopyFileType file_type_{CopyFileType::kCSV};
    String file_path_{};
    bool header_{false};
    char delimiter_{','};
};

} // namespace infinity