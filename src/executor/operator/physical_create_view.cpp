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
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;

module physical_create_view;

namespace infinity {

void PhysicalCreateView::Init() {}

void PhysicalCreateView::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) { output_state->SetComplete(); }

void PhysicalCreateView::Execute(QueryContext *query_context) {

    Txn *txn = query_context->GetTxn();

    txn->CreateView(create_view_info_->schema_name_, create_view_info_->view_name_, create_view_info_->conflict_type_);

    //    SharedPtr<View> view_ptr = MakeShared<View>(create_view_info_,
    //                                                GetOutputNames(),
    //                                                GetOutputTypes());
    //    InfinityContext::instance().catalog()->CreateView(create_view_info_->schema_name_,
    //                                               view_ptr,
    //                                               create_view_info_->conflict_type_);
    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", HashSet<ConstraintType>())};

    SharedPtr<TableDef> result_table_def_ptr = MakeShared<TableDef>(MakeShared<String>("default"), MakeShared<String>("Tables"), column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
}

} // namespace infinity
