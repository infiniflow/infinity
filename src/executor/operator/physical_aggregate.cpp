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

#include <memory>
#include <string>
#include <vector>

module physical_aggregate;

import stl;
import txn;
import query_context;
import table_def;
import data_table;

import operator_state;
import data_block;
import utility;
import logger;
import column_vector;
import third_party;
import infinity_exception;
import default_values;

import expression_state;
import expression_evaluator;
import aggregate_expression;
import status;
import logical_type;
import internal_types;
import column_def;

namespace infinity {

void PhysicalAggregate::Init() {}

bool PhysicalAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    OperatorState *prev_op_state = operator_state->prev_op_state_;
    auto *aggregate_operator_state = static_cast<AggregateOperatorState *>(operator_state);

    // 1. Execute group-by expressions to generate unique key.
    // ExpressionEvaluator groupby_executor;
    // groupby_executor.Init(groups_);

    Vector<SharedPtr<ColumnDef>> groupby_columns;
    SizeT group_count = groups_.size();

    if (group_count == 0) {
        // Aggregate without group by expression
        // e.g. SELECT count(a) FROM table;
        auto result = SimpleAggregateExecute(prev_op_state->data_block_array_,
                                             aggregate_operator_state->data_block_array_,
                                             aggregate_operator_state->states_,
                                             prev_op_state->Complete());
        prev_op_state->data_block_array_.clear();
        if (prev_op_state->Complete()) {
            aggregate_operator_state->SetComplete();
        }
        return result;
    }
#if 0
    groupby_columns.reserve(group_count);

    Vector<DataType> types;
    types.reserve(group_count);

    for(i64 idx = 0; auto& expr: groups_) {
        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx,
                                                             MakeShared<DataType>(expr->Type()),
                                                             expr->Name(),
                                                             std::set<ConstraintType>());
        groupby_columns.emplace_back(col_def);
        types.emplace_back(expr->Type());
        ++ idx;
    }

    SharedPtr<TableDef> groupby_tabledef = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("groupby"), groupby_columns);
    SharedPtr<DataTable> groupby_table = DataTable::Make(groupby_tabledef, TableType::kIntermediate);

    groupby_executor.Execute(input_table_, groupby_table);

    // 2. Use the unique key to get the row list of the same key.
    hash_table_.Init(types);

    SizeT block_count = groupby_table->DataBlockCount();
    for(SizeT block_id = 0; block_id < block_count; ++ block_id) {
        const SharedPtr<DataBlock>& block_ptr = groupby_table->GetDataBlockById(block_id);
        hash_table_.Append(block_ptr->column_vectors, block_id, block_ptr->row_count());
    }

    // 3. forlop each aggregates function on each group by bucket, to calculate the result according to the row list
    SharedPtr<DataTable> output_groupby_table = DataTable::Make(groupby_tabledef, TableType::kIntermediate);
    GenerateGroupByResult(groupby_table, output_groupby_table);


    // input table after group by, each block belong to one group. This is the prerequisites to execute aggregate function.
    SharedPtr<DataTable> grouped_input_table;
    {
        SizeT column_count = input_table_->ColumnCount();
        Vector<SharedPtr<ColumnDef>> columns;
        columns.reserve(column_count);
        for(SizeT idx = 0; idx < column_count; ++ idx) {
            SharedPtr<DataType> col_type = input_table_->GetColumnTypeById(idx);
            String col_name = input_table_->GetColumnNameById(idx);

            SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, col_type, col_name, std::set<ConstraintType>());
            columns.emplace_back(col_def);
        }

        SharedPtr<TableDef> table_def = TableDef::Make(MakeShared<String>("default_db"), MakeShared<String>("grouped_input"), columns);

        grouped_input_table = DataTable::Make(table_def, TableType::kGroupBy);
    }
    GroupByInputTable(input_table_, grouped_input_table);

    // generate output aggregate table
    SizeT aggregates_count = aggregates_.size();
    if(aggregates_count > 0) {
        SharedPtr<DataTable> output_aggregate_table{};

        // Prepare the output table columns
        Vector<SharedPtr<ColumnDef>> aggregate_columns;
        aggregate_columns.reserve(aggregates_count);

        // Prepare the expression states
        Vector<SharedPtr<ExpressionState>> expr_states;
        expr_states.reserve(aggregates_.size());

        // Prepare the output block
        Vector<SharedPtr<DataType>> output_types;
        output_types.reserve(aggregates_count);

        for(i64 idx = 0; auto& expr: aggregates_) {
            // expression state
            expr_states.emplace_back(ExpressionState::CreateState(expr));
            SharedPtr<DataType> data_type = MakeShared<DataType>(expr->Type());

            // column definition
            SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx,
                                                                 data_type,
                                                                 expr->Name(),
                                                                 std::set<ConstraintType>());
            aggregate_columns.emplace_back(col_def);

            // for output block
            output_types.emplace_back(data_type);

            ++ idx;
        }

        // output aggregate table definition
        SharedPtr<TableDef> aggregate_tabledef = TableDef::Make(MakeShared<String>("default_db"),
                                                                MakeShared<String>("aggregate"),
                                                                aggregate_columns);
        output_aggregate_table = DataTable::Make(aggregate_tabledef, TableType::kAggregate);

        // Loop blocks
        HashMap<u64, SharedPtr<DataBlock>> block_map;
        SizeT input_data_block_count = grouped_input_table->DataBlockCount();
        for(SizeT block_idx = 0; block_idx < input_data_block_count; ++ block_idx) {
            SharedPtr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types);
            Vector<SharedPtr<DataBlock>> input_blocks{grouped_input_table->GetDataBlockById(block_idx)};
//            block_map[groupby_index_] = block_ptr;
//            block_map[input_table_index_] = block_ptr;
            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            evaluator.Init(input_blocks);
            for(SizeT expr_idx = 0; expr_idx < aggregates_count; ++ expr_idx) {
                Vector<SharedPtr<ColumnVector>> blocks_column;
                blocks_column.emplace_back(output_data_block->column_vectors[expr_idx]);
                evaluator.Execute(aggregates_[expr_idx],
                                  expr_states[expr_idx],
                                  blocks_column);
                if(blocks_column[0].get() != output_data_block->column_vectors[expr_idx].get()) {
                    // column vector in blocks column might be changed to the column vector from column reference.
                    // This check and assignment is to make sure the right column vector are assign to output_data_block
                    output_data_block->column_vectors[expr_idx] = blocks_column[0];
                }
            }

            output_data_block->Finalize();
            output_aggregate_table->Append(output_data_block);
        }

        // Union output group by table with aggregate table
        output_groupby_table->UnionWith(output_aggregate_table);
    }

    // 4. generate the result to output
    this->output_ = output_groupby_table;
#endif
    return true;
}

void PhysicalAggregate::GroupByInputTable(const SharedPtr<DataTable> &input_table, SharedPtr<DataTable> &grouped_input_table) {
    SizeT column_count = input_table->ColumnCount();

    // 1. Get output table column types.
    Vector<SharedPtr<DataType>> types;
    types.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        SharedPtr<DataType> input_type = input_table->GetColumnTypeById(column_id);
        SharedPtr<DataType> output_type = grouped_input_table->GetColumnTypeById(column_id);
        if (*input_type != *output_type) {
            Status status = Status::DataTypeMismatch(input_type->ToString(), output_type->ToString());
            RecoverableError(status);
        }
        types.emplace_back(input_type);
    }

    // 2. Generate data blocks and append it into output table according to the group by hash table.
    const Vector<SharedPtr<DataBlock>> &input_datablocks = input_table->data_blocks_;
    for (const auto &item : hash_table_.hash_table_) {

        // 2.1 Each hash bucket will be insert in to one data block
        SharedPtr<DataBlock> output_datablock = DataBlock::Make();
        SizeT datablock_size = 0;
        for (const auto &vec_pair : item.second) {
            datablock_size += vec_pair.second.size();
        }
        SizeT datablock_capacity = Utility::NextPowerOfTwo(datablock_size);
        output_datablock->Init(types, datablock_capacity);

        // Loop each block
        SizeT output_row_idx = 0;
        for (const auto &vec_pair : item.second) {
            SizeT input_block_id = vec_pair.first;

            // Loop each row of same block
            for (const auto input_offset : vec_pair.second) {

                // Forloop each column
                for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                    switch (types[column_id]->type()) {
                        case LogicalType::kBoolean: {
                            ((BooleanT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((BooleanT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kTinyInt: {
                            ((TinyIntT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((TinyIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kSmallInt: {
                            ((SmallIntT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((SmallIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kInteger: {
                            ((IntegerT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((IntegerT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kBigInt: {
                            ((BigIntT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((BigIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kHugeInt: {
                            String error_message = "Not implement: HugeInt data shuffle";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kFloat: {
                            ((FloatT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((FloatT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kDouble: {
                            ((DoubleT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((DoubleT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kDecimal: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kVarchar: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kDate: {
                            ((DateT *)(output_datablock->column_vectors[column_id]->data()))[output_row_idx] =
                                ((DateT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                            break;
                        }
                        case LogicalType::kTime: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kDateTime: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kTimestamp: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kInterval: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        case LogicalType::kMixed: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                        default: {
                            String error_message = "Not implement: data shuffle.";
                            UnrecoverableError(error_message);
                            break;
                        }
                    }
                }

                ++output_row_idx;
            }
        }

        if (output_row_idx != datablock_size) {
            String error_message = fmt::format("Expected block size: {}, but only copied data size: {}", datablock_size, output_row_idx);
            UnrecoverableError(error_message);
            break;
        }

        for (SizeT column_id = 0; column_id < column_count; ++column_id) {
            output_datablock->column_vectors[column_id]->Finalize(datablock_size);
        }

        output_datablock->Finalize();
        grouped_input_table->Append(output_datablock);
    }
}

void PhysicalAggregate::GenerateGroupByResult(const SharedPtr<DataTable> &input_table, SharedPtr<DataTable> &output_table) {

    SizeT column_count = input_table->ColumnCount();
    Vector<SharedPtr<DataType>> types;
    types.reserve(column_count);
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        SharedPtr<DataType> input_type = input_table->GetColumnTypeById(column_id);
        SharedPtr<DataType> output_type = output_table->GetColumnTypeById(column_id);
        if (*input_type != *output_type) {
            Status status = Status::DataTypeMismatch(input_type->ToString(), output_type->ToString());
            RecoverableError(status);
        }
        types.emplace_back(input_type);
    }

    SharedPtr<DataBlock> output_datablock = nullptr;
    const Vector<SharedPtr<DataBlock>> &input_datablocks = input_table->data_blocks_;
//    SizeT row_count = hash_table_.hash_table_.size();
#if 1
    for (SizeT block_row_idx = 0; const auto &item : hash_table_.hash_table_) {
        // Each hash bucket will generate one data block.
        output_datablock = DataBlock::Make();
        output_datablock->Init(types);

        // Only get the first row(block id and row offset of the block) of the bucket
        SizeT input_block_id = item.second.begin()->first;
        SizeT input_offset = item.second.begin()->second.front();

        // Only the first position of the column vector has value.
        for (SizeT column_id = 0; column_id < column_count; ++column_id) {
            switch (types[column_id]->type()) {
                case LogicalType::kBoolean: {
                    ((BooleanT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((BooleanT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kTinyInt: {
                    ((TinyIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((TinyIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kSmallInt: {
                    ((SmallIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((SmallIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kInteger: {
                    ((IntegerT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((IntegerT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kBigInt: {
                    ((BigIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((BigIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kHugeInt: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kFloat: {
                    ((FloatT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((FloatT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kDouble: {
                    ((DoubleT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((DoubleT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kDecimal: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kVarchar: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kDate: {
                    ((DateT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((DateT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kTime: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kDateTime: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kTimestamp: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kInterval: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                case LogicalType::kMixed: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
                default: {
                    String error_message = "Not implement: data shuffle.";
                    UnrecoverableError(error_message);
                    break;
                }
            }

            output_datablock->column_vectors[column_id]->Finalize(block_row_idx + 1);
        }

        output_datablock->Finalize();
        output_table->Append(output_datablock);
    }
#else
    for (SizeT row_id = 0, block_row_idx = 0; const auto &item : hash_table_.hash_table_) {
        // DEFAULT VECTOR SIZE buckets will generate one data block.
        if (row_id % DEFAULT_VECTOR_SIZE == 0) {
            if (output_datablock.get() != nullptr) {
                for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                    output_datablock->column_vectors[column_id]->tail_index_ = block_row_idx;
                }

                output_datablock->Finalize();
                output_table->Append(output_datablock);
                block_row_idx = 0;
            }

            output_datablock = DataBlock::Make();
            output_datablock->Init(types);
        }

        SizeT input_block_id = item.second.begin()->first;
        SizeT input_offset = item.second.begin()->second.front();

        for (SizeT column_id = 0; column_id < column_count; ++column_id) {
            switch (types[column_id].type()) {
                case LogicalType::kBoolean: {
                    ((BooleanT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((BooleanT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kTinyInt: {
                    ((TinyIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((TinyIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kSmallInt: {
                    ((SmallIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((SmallIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kInteger: {
                    ((IntegerT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((IntegerT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kBigInt: {
                    ((BigIntT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((BigIntT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kHugeInt: {
                    NotImplementError("HugeInt data shuffle isn't implemented.")
                }
                case LogicalType::kFloat: {
                    ((FloatT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((FloatT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kDouble: {
                    ((DoubleT *)(output_datablock->column_vectors[column_id]->data()))[block_row_idx] =
                        ((DoubleT *)(input_datablocks[input_block_id]->column_vectors[column_id]->data()))[input_offset];
                    break;
                }
                case LogicalType::kDecimal: {
                    NotImplementError("Decimal data shuffle isn't implemented.")
                }
                case LogicalType::kVarchar: {
                    NotImplementError("Varchar data shuffle isn't implemented.")
                }
                case LogicalType::kDate: {
                    NotImplementError("Date data shuffle isn't implemented.")
                }
                case LogicalType::kTime: {
                    NotImplementError("Time data shuffle isn't implemented.")
                }
                case LogicalType::kDateTime: {
                    NotImplementError("Datetime data shuffle isn't implemented.")
                }
                case LogicalType::kTimestamp: {
                    NotImplementError("Timestamp data shuffle isn't implemented.")
                }
                case LogicalType::kTimestampTZ: {
                    NotImplementError("TimestampTZ data shuffle isn't implemented.")
                }
                case LogicalType::kInterval: {
                    NotImplementError("Interval data shuffle isn't implemented.")
                }
                case LogicalType::kMixed: {
                    NotImplementError("Heterogeneous data shuffle isn't implemented.")
                }
                default: {
                    ExecutorError("Unexpected data type")
                }
            }
        }

        ++block_row_idx;
        ++row_id;

        if (row_id == row_count) {
            // All hash table data are checked
            for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                output_datablock->column_vectors[column_id]->tail_index_ = block_row_idx;
            }

            output_datablock->Finalize();
            output_table->Append(output_datablock);
            break;
        }
    }
#endif
}

bool PhysicalAggregate::SimpleAggregateExecute(const Vector<UniquePtr<DataBlock>> &input_blocks,
                                               Vector<UniquePtr<DataBlock>> &output_blocks,
                                               Vector<UniquePtr<char[]>> &states,
                                               bool task_completed) {
    SizeT aggregates_count = aggregates_.size();
    if (aggregates_count <= 0) {
        String error_message = "Simple Aggregate without aggregate expression.";
        UnrecoverableError(error_message);
    }

    SizeT input_block_count = input_blocks.size();

    if (input_block_count == 0) {
        // No input data
        LOG_TRACE("No input, no aggregate result");
        return true;
    }

    // Prepare the output table columns
    Vector<SharedPtr<ColumnDef>> aggregate_columns;
    aggregate_columns.reserve(aggregates_count);

    // Prepare the expression states
    Vector<SharedPtr<ExpressionState>> expr_states;
    expr_states.reserve(aggregates_.size());

    // Prepare the output block
    Vector<SharedPtr<DataType>> output_types;
    output_types.reserve(aggregates_count);

    AggregateFlag flag = output_blocks.empty() ? (!task_completed ? AggregateFlag::kUninitialized : AggregateFlag::kRunAndFinish)
                                               : (!task_completed ? AggregateFlag::kRunning : AggregateFlag::kFinish);

    for (i64 idx = 0; auto &expr : aggregates_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(std::static_pointer_cast<AggregateExpression>(expr), states[idx].get(), flag));

        SharedPtr<DataType> output_type = MakeShared<DataType>(expr->Type());

        // column definition
        SharedPtr<ColumnDef> col_def = MakeShared<ColumnDef>(idx, output_type, expr->Name(), std::set<ConstraintType>());
        aggregate_columns.emplace_back(col_def);

        // for output block
        output_types.emplace_back(output_type);

        ++idx;
    }

    if (output_blocks.empty()) {
        for (SizeT block_idx = 0; block_idx < input_block_count; ++block_idx) {
            output_blocks.emplace_back(DataBlock::MakeUniquePtr());
            auto out_put_block = output_blocks.back().get();
            out_put_block->Init(*GetOutputTypes());
        }
    }

    for (SizeT block_idx = 0; block_idx < input_block_count; ++block_idx) {
        DataBlock *input_data_block = input_blocks[block_idx].get();

        DataBlock *output_data_block = output_blocks[block_idx].get();

        ExpressionEvaluator evaluator;
        evaluator.Init(input_data_block);

        SizeT expression_count = aggregates_count;
        // calculate every columns value
        for (SizeT expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
            LOG_TRACE("Physical aggregate Execute");
            evaluator.Execute(aggregates_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
        }
        if (task_completed) {
            // Finalize the output block (e.g. calculate the average value
            output_data_block->Finalize();
        }
        // {
        //     auto row = input_data_block->row_count();
        //     if (row == 0) {
        //         LOG_WARN("EEE");
        //     } else {
        //         auto v = input_data_block->GetValue(0, 0);
        //         auto ti = v.value_.tiny_int;
        //         auto si = v.value_.small_int;
        //         auto i = v.value_.integer;
        //         LOG_WARN(fmt::format("Agg Input: {}, {},{} {} {}", u64(input_data_block), row, ti, si, i));
        //     }
        // }
        // {
        //     auto row = output_data_block->row_count();
        //     if (row == 0) {
        //         auto row1 = input_data_block->row_count();
        //         LOG_WARN(fmt::format("FFF {}", row1));
        //     } else {
        //         auto v = output_data_block->GetValue(0, 0);
        //         auto ti = v.value_.tiny_int;
        //         auto si = v.value_.small_int;
        //         auto i = v.value_.integer;
        //         LOG_WARN(fmt::format("Agg Output: {}, {},{} {} {}", u64(output_data_block), row, ti, si, i));
        //     }
        // }
    }
    return true;
}

SharedPtr<Vector<String>> PhysicalAggregate::GetOutputNames() const {
    SharedPtr<Vector<String>> result = MakeShared<Vector<String>>();
    SizeT groups_count = groups_.size();
    SizeT aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (SizeT i = 0; i < groups_count; ++i) {
        result->emplace_back(groups_[i]->Name());
    }
    for (SizeT i = 0; i < aggregates_count; ++i) {
        result->emplace_back(aggregates_[i]->Name());
    }
    return result;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalAggregate::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result = MakeShared<Vector<SharedPtr<DataType>>>();
    SizeT groups_count = groups_.size();
    SizeT aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (SizeT i = 0; i < groups_count; ++i) {
        result->emplace_back(MakeShared<DataType>(groups_[i]->Type()));
    }
    for (SizeT i = 0; i < aggregates_count; ++i) {
        result->emplace_back(MakeShared<DataType>(aggregates_[i]->Type()));
    }
    return result;
}

Vector<HashRange> PhysicalAggregate::GetHashRanges(i64 parallel_count) const {
    Vector<HashRange> result;
    result.resize(parallel_count);
    return result;
}

} // namespace infinity
