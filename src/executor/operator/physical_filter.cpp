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
import expression_state;
import data_block;
import logger;
import third_party;

import infinity_exception;

module physical_filter;

namespace infinity {

void PhysicalFilter::Init() {
    //    executor.Init({condition_});
    //    input_table_ = left_->output();
    //    ExecutorAssert(input_table_ != nullptr, "No input.");
    //
    //    SizeT column_count = input_table_->ColumnCount();
    //    Vector<SharedPtr<ColumnDef>> columns;
    //    columns.reserve(column_count);
    //    for(SizeT idx = 0; idx < column_count; ++ idx) {
    //        DataType col_type = input_table_->GetColumnTypeById(idx);
    //        String col_name = input_table_->GetColumnNameById(idx);
    //
    //        SharedPtr<ColumnDef> col_def = ColumnDef::Make(col_name, idx, col_type, Set<ConstrainType>());
    //        columns.emplace_back(col_def);
    //    }
    //
    //    SharedPtr<TableDef> table_def = TableDef::Make("filter", columns, false);
    //
    //    output_ = DataTable::Make(table_def, TableType::kIntermediate);
}

void PhysicalFilter::Execute(QueryContext *, OperatorState *operator_state) {
    if(operator_state->data_block_.get() == nullptr) {
        operator_state->data_block_ = DataBlock::Make();

        // Performance issue here.
        operator_state->data_block_->Init(*GetOutputTypes());
    }

    SharedPtr<ExpressionState> condition_state = ExpressionState::CreateState(condition_);

    auto* prev_op_state = operator_state->prev_op_state_;
    auto* filter_operator_state = static_cast<FilterOperatorState *>(operator_state);

    SizeT selected_count = selector_.Select(condition_,
                                            condition_state,
                                            prev_op_state->data_block_.get(),
                                            filter_operator_state->data_block_.get(),
                                            prev_op_state->data_block_->row_count());

    LOG_TRACE(Format("{} rows after filter", selected_count));
    if (prev_op_state->Complete()) {
        prev_op_state->data_block_.reset();
        filter_operator_state->SetComplete();
    }
    return;
}

} // namespace infinity
