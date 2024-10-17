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

module physical_drop_index;

import stl;
import query_context;
import operator_state;

import table_def;
import data_table;
import status;
import infinity_exception;
import logical_type;
import column_def;

namespace infinity {

void PhysicalDropIndex::Init() {}

bool PhysicalDropIndex::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto txn = query_context->GetTxn();
    Status status = txn->DropIndexByName(*schema_name_, *table_name_, *index_name_, conflict_type_);

    if (!status.ok()) {
        operator_state->status_ = status;
    }

    // Generate the result
    Vector<SharedPtr<ColumnDef>> column_defs = {
        MakeShared<ColumnDef>(0, MakeShared<DataType>(LogicalType::kInteger), "OK", std::set<ConstraintType>())};

    auto result_table_def_ptr = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("Tables"), nullptr, column_defs);
    output_ = MakeShared<DataTable>(result_table_def_ptr, TableType::kDataTable);
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
