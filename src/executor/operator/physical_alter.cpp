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

#include <vector>

module physical_alter;

import query_context;
import operator_state;
import third_party;
import txn;
import status;
import infinity_exception;
import value;
import bind_context;
import value_expression;
import expression_binder;
import defer_op;
import cast_function;
import bound_cast_func;
import base_expression;
import cast_expression;
import expression_evaluator;
import column_vector;
import expression_state;

namespace infinity {

void PhysicalRenameTable::Init() {}

bool PhysicalRenameTable::Execute(QueryContext *query_context, OperatorState *operator_state) {
    RecoverableError(Status::NotSupport("Rename table is not supported."));
    operator_state->SetComplete();
    return true;
}

void PhysicalAddColumns::Init() {}

bool PhysicalAddColumns::Execute(QueryContext *query_context, OperatorState *operator_state) {
    LOG_INFO(fmt::format("Locking table {} for add columns", *table_entry_->GetTableName()));
    table_entry_->SetLocked();
    LOG_INFO(fmt::format("Table {} is locked", *table_entry_->GetTableName()));

    DeferFn defer_fn([&]() {
        LOG_INFO(fmt::format("Table {} unlocked.", *table_entry_->GetTableName()));
        table_entry_->SetUnlock();
    });

    Txn *txn = query_context->GetTxn();

    ExpressionBinder tmp_binder(nullptr);
    Vector<Value> values;
    for (const auto &column_def : column_defs_) {
        if (!column_def->has_default_value()) {
            UnrecoverableError(fmt::format("Column {} has no default value", column_def->name()));
        }
        SharedPtr<ConstantExpr> default_expr = column_def->default_value();
        auto expr = tmp_binder.BuildValueExpr(*default_expr, nullptr, 0, false);
        auto *value_expr = static_cast<ValueExpression *>(expr.get());

        const SharedPtr<DataType> &column_type = column_def->type();
        if (value_expr->Type() == *column_type) {
            values.push_back(value_expr->GetValue());
        } else {
            const SharedPtr<DataType> &column_type = column_def->type();

            BoundCastFunc cast = CastFunction::GetBoundFunc(value_expr->Type(), *column_type);
            SharedPtr<BaseExpression> cast_expr = MakeShared<CastExpression>(cast, expr, *column_type);
            SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(cast_expr);
            SharedPtr<ColumnVector> output_column_vector = ColumnVector::Make(column_type);
            output_column_vector->Initialize(ColumnVectorType::kConstant, 1);
            ExpressionEvaluator evaluator;
            evaluator.Init(nullptr);
            evaluator.Execute(cast_expr, expr_state, output_column_vector);

            values.push_back(output_column_vector->GetValue(0));
        }
    }
    auto status = txn->AddColumns(table_entry_, column_defs_, values);
    if (!status.ok()) {
        RecoverableError(status);
    }
    operator_state->SetComplete();
    return true;
}

void PhysicalDropColumns::Init() {}

bool PhysicalDropColumns::Execute(QueryContext *query_context, OperatorState *operator_state) {
    LOG_INFO(fmt::format("Locking table {} for add columns", *table_entry_->GetTableName()));
    table_entry_->SetLocked();
    LOG_INFO(fmt::format("Table {} is locked", *table_entry_->GetTableName()));

    DeferFn defer_fn([&]() {
        LOG_INFO(fmt::format("Table {} unlocked.", *table_entry_->GetTableName()));
        table_entry_->SetUnlock();
    });

    Txn *txn = query_context->GetTxn();
    auto status = txn->DropColumns(table_entry_, column_names_);
    if (!status.ok()) {
        RecoverableError(status);
    }

    operator_state->SetComplete();
    return true;
}

} // namespace infinity
