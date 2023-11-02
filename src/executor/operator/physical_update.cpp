//
// Created by JinHai on 2022/7/28.
//

module;

import stl;
import std;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_collection_entry;
import query_context;
// import data_table;
import operator_state;
import db_entry;
import data_block;
import column_vector;
import expression_evaluator;
import expression_state;
import base_expression;

module physical_update;

namespace infinity {

void PhysicalUpdate::Init() {}

void PhysicalUpdate::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    DataBlock *data_block_ptr = input_state->input_data_block_;
    auto txn = query_context->GetTxn();
    auto db_name = TableCollectionEntry::GetDBEntry(table_entry_ptr_)->db_name_;
    auto table_name = table_entry_ptr_->table_collection_name_;
    Vector<RowID> row_ids;
    Vector<SharedPtr<ColumnVector>> column_vectors;
    for (SizeT i = 0; i < data_block_ptr->column_count(); i++) {
        SharedPtr<ColumnVector> column_vector = data_block_ptr->column_vectors[i];
        if (column_vector->data_type_->type() == LogicalType::kRowID) {
            row_ids.resize(column_vector->Size());
            Memcpy(row_ids.data(), column_vector->data_ptr_, column_vector->Size() * sizeof(RowID));
            break;
        } else {
            column_vectors.push_back(column_vector);
        }
    }
    if (!row_ids.empty()) {

        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);
        for (SizeT expr_idx = 0; expr_idx < update_columns_.size(); ++expr_idx) {
            SizeT column_idx = update_columns_[expr_idx].first;
            const SharedPtr<BaseExpression> &expr = update_columns_[expr_idx].second;
            SharedPtr<ColumnVector> output_column = ColumnVector::Make(column_vectors[column_idx]->data_type_);
            output_column->Initialize(ColumnVectorType::kFlat);
            output_column->Reserve(data_block_ptr->row_count());
            SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(expr);
            evaluator.Execute(expr, expr_state, output_column);
            column_vectors[column_idx] = output_column;
        }

        SharedPtr<DataBlock> output_data_block = DataBlock::Make();
        output_data_block->Init(column_vectors);
        txn->Append(*db_name, *table_name, output_data_block);
        txn->Delete(*db_name, *table_name, row_ids);

        output_state->count_++;
        output_state->sum_ += row_ids.size();
    }
    if (input_state->Complete())
        output_state->SetComplete();
}

void PhysicalUpdate::Execute(QueryContext *query_context) {}

} // namespace infinity
