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
import base_expression;
import logical_show;

export module physical_show;

namespace infinity {

export class PhysicalShow : public PhysicalOperator {
public:
    explicit PhysicalShow(u64 id, ShowType type, String db_name, String object_name, u64 table_index)
        : PhysicalOperator(PhysicalOperatorType::kShow, nullptr, nullptr, id), scan_type_(type), db_name_(Move(db_name)),
          object_name_(Move(object_name)), table_index_(table_index) {}

    ~PhysicalShow() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *output_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline ShowType scan_type() const { return scan_type_; }

    inline const String &db_name() const { return db_name_; };

    inline const String &object_name() const { return object_name_; };

private:
    void ExecuteShowTable(QueryContext *query_context);

    void ExecuteShowViews(QueryContext *query_context);

    void ExecuteShowColumns(QueryContext *query_context);

    void ExecuteShowTableDetail(QueryContext *query_context, const Vector<SharedPtr<ColumnDef>> &table_columns);

    void ExecuteShowViewDetail(QueryContext *query_context,
                               const SharedPtr<Vector<SharedPtr<DataType>>> &column_types,
                               const SharedPtr<Vector<String>> &column_names);

    void ExecuteShowDatabases(QueryContext *query_context, ShowOperatorState* operator_state);

    /// Execute push based show table
    void ExecuteShowTable(QueryContext *query_context, ShowOperatorState* operator_state);

    /// Execute push based describe table
    void ExecuteShowColumns(QueryContext *query_context, ShowOperatorState* operator_state);

    void ExecuteShowIndexes(QueryContext *query_context, ShowOperatorState* operator_state);

    void ExecuteShowProfiles(QueryContext *query_context, ShowOperatorState* operator_state);

    void ExecuteShowConfigs(QueryContext *query_context, ShowOperatorState* operator_state);

private:
    ShowType scan_type_{ShowType::kInvalid};
    String db_name_{};
    String object_name_{};
    u64 table_index_{};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
