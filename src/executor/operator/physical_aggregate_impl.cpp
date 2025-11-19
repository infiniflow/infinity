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

module infinity_core:physical_aggregate.impl;

import :physical_aggregate;
import :query_context;
import :table_def;
import :data_table;
import :operator_state;
import :data_block;
import :utility;
import :logger;
import :column_vector;
import :infinity_exception;
import :default_values;
import :expression_state;
import :expression_evaluator;
import :aggregate_expression;
import :status;

import std;
import third_party;

import logical_type;
import internal_types;
import column_def;
import data_type;

namespace infinity {

void PhysicalAggregate::Init(QueryContext *query_context) {}

bool PhysicalAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    OperatorState *prev_op_state = operator_state->prev_op_state_;
    auto *aggregate_operator_state = static_cast<AggregateOperatorState *>(operator_state);

    size_t group_count = groups_.size();
    bool task_completed = prev_op_state->Complete();

    if (group_count == 0) {
        // Aggregate without group by expression
        // e.g. SELECT count(a) FROM table;
        auto result = SimpleAggregateExecute(prev_op_state->data_block_array_,
                                             aggregate_operator_state->data_block_array_,
                                             aggregate_operator_state->states_,
                                             task_completed);
        prev_op_state->data_block_array_.clear();
        if (prev_op_state->Complete()) {
            aggregate_operator_state->SetComplete();
        }
        return result;
    }

    // 1. Execute group-by expressions to generate unique key.
    std::vector<std::shared_ptr<ColumnDef>> groupby_columns;
    groupby_columns.reserve(group_count);

    std::vector<std::shared_ptr<DataType>> groupby_types;
    groupby_types.reserve(group_count);

    for (i64 idx = 0; auto &expr : groups_) {
        std::shared_ptr<ColumnDef> col_def =
            std::make_shared<ColumnDef>(idx, std::make_shared<DataType>(expr->Type()), expr->Name(), std::set<ConstraintType>());
        groupby_columns.emplace_back(col_def);
        groupby_types.emplace_back(std::make_shared<DataType>(expr->Type()));
        ++idx;
    }

    std::shared_ptr<TableDef> groupby_tabledef = TableDef::Make(std::make_shared<std::string>("default_db"),
                                                                std::make_shared<std::string>("groupby"),
                                                                std::make_shared<std::string>(""),
                                                                groupby_columns);
    std::shared_ptr<DataTable> groupby_table = DataTable::Make(groupby_tabledef, TableType::kIntermediate);

    // Prepare the expression states
    std::vector<std::shared_ptr<ExpressionState>> expr_states;
    expr_states.reserve(group_count);
    for (const auto &expr : groups_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }

    size_t input_block_count = prev_op_state->data_block_array_.size();
    for (size_t block_idx = 0; block_idx < input_block_count; ++block_idx) {
        DataBlock *input_data_block = prev_op_state->data_block_array_[block_idx].get();

        groupby_table->data_blocks_.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock *output_data_block = groupby_table->data_blocks_.back().get();
        output_data_block->Init(groupby_types);

        ExpressionEvaluator groupby_executor;
        groupby_executor.Init(input_data_block);

        for (size_t expr_idx = 0; expr_idx < group_count; ++expr_idx) {
            groupby_executor.Execute(groups_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
        }
        output_data_block->Finalize();
    }

    // 2. Use the unique key to get the row list of the same key.
    HashTable &hash_table = aggregate_operator_state->hash_table_;
    if (!hash_table.Initialized()) {
        hash_table.Init(groupby_types);
    } else {
        hash_table.Clear();
    }

    size_t block_count = groupby_table->DataBlockCount();
    for (size_t block_id = 0; block_id < block_count; ++block_id) {
        const std::shared_ptr<DataBlock> &block_ptr = groupby_table->GetDataBlockById(block_id);
        hash_table.Append(block_ptr->column_vectors, block_id, block_ptr->row_count());
    }

    // 3. forlop each aggregates function on each group by bucket, to calculate the result according to the row list
    std::shared_ptr<DataTable> output_groupby_table = DataTable::Make(groupby_tabledef, TableType::kIntermediate);
    GenerateGroupByResult(groupby_table, output_groupby_table, hash_table);

    // generate output aggregate table
    size_t aggregates_count = aggregates_.size();
    if (aggregates_count > 0) {
        // Input table after group by, each block belong to one group. This is the prerequisites to execute aggregate function.
        std::vector<std::unique_ptr<DataBlock>> grouped_input_datablocks;
        GroupByInputTable(prev_op_state->data_block_array_, grouped_input_datablocks, hash_table);

        // Prepare the output table columns
        std::vector<std::shared_ptr<ColumnDef>> aggregate_columns;
        aggregate_columns.reserve(aggregates_count);

        // Prepare the expression states
        std::vector<std::shared_ptr<ExpressionState>> expr_states;
        expr_states.reserve(aggregates_.size());

        // Prepare the output block
        std::vector<std::shared_ptr<DataType>> output_types;
        output_types.reserve(aggregates_count);
        auto &agg_states = aggregate_operator_state->states_;

        for (i64 idx = 0; auto &expr : aggregates_) {
            // expression state
            expr_states.emplace_back(ExpressionState::CreateState(std::static_pointer_cast<AggregateExpression>(expr),
                                                                  agg_states[idx].get(),
                                                                  AggregateFlag::kRunAndFinish));
            std::shared_ptr<DataType> data_type = std::make_shared<DataType>(expr->Type());

            // column definition
            std::shared_ptr<ColumnDef> col_def = std::make_shared<ColumnDef>(idx, data_type, expr->Name(), std::set<ConstraintType>());
            aggregate_columns.emplace_back(col_def);

            // for output block
            output_types.emplace_back(data_type);

            ++idx;
        }

        // output aggregate table definition
        std::shared_ptr<TableDef> aggregate_tabledef = TableDef::Make(std::make_shared<std::string>("default_db"),
                                                                      std::make_shared<std::string>("aggregate"),
                                                                      std::make_shared<std::string>(""),
                                                                      aggregate_columns);
        std::shared_ptr<DataTable> output_aggregate_table = DataTable::Make(aggregate_tabledef, TableType::kAggregate);

        // Loop blocks
        size_t input_data_block_count = grouped_input_datablocks.size();
        for (size_t block_idx = 0; block_idx < input_data_block_count; ++block_idx) {
            std::shared_ptr<DataBlock> output_data_block = DataBlock::Make();
            output_data_block->Init(output_types, 1);
            DataBlock *input_block = grouped_input_datablocks[block_idx].get();
            // Loop aggregate expression
            ExpressionEvaluator evaluator;
            evaluator.Init(input_block);
            for (size_t expr_idx = 0; expr_idx < aggregates_count; ++expr_idx) {
                std::shared_ptr<ColumnVector> blocks_column = output_data_block->column_vectors[expr_idx];
                evaluator.Execute(aggregates_[expr_idx], expr_states[expr_idx], blocks_column);
                if (blocks_column.get() != output_data_block->column_vectors[expr_idx].get()) {
                    // column vector in blocks column might be changed to the column vector from column reference.
                    // This check and assignment is to make sure the right column vector are assign to output_data_block
                    output_data_block->column_vectors[expr_idx] = blocks_column;
                }
            }

            output_data_block->Finalize();
            output_aggregate_table->Append(output_data_block);
        }

        // Union output group by table with aggregate table
        output_groupby_table->UnionWith(output_aggregate_table);
    }

    // 4. generate the result to output
    output_groupby_table->ShrinkBlocks();
    for (size_t block_idx = 0; block_idx < output_groupby_table->DataBlockCount(); ++block_idx) {
        std::shared_ptr<DataBlock> output_data_block = output_groupby_table->GetDataBlockById(block_idx);
        aggregate_operator_state->data_block_array_.push_back(std::make_unique<DataBlock>(std::move(*output_data_block)));
    }

    prev_op_state->data_block_array_.clear();
    if (prev_op_state->Complete()) {
        aggregate_operator_state->SetComplete();
    }
    return true;
}

void PhysicalAggregate::GroupByInputTable(const std::vector<std::unique_ptr<DataBlock>> &input_datablocks,
                                          std::vector<std::unique_ptr<DataBlock>> &output_datablocks,
                                          HashTable &hash_table) {
    // 1. Get output table column types.
    std::vector<std::shared_ptr<DataType>> types = input_datablocks.front()->types();
    size_t column_count = input_datablocks.front()->column_count();

    // 2. Generate data blocks and append it into output table according to the group by hash table.
    // const std::vector<std::shared_ptr<DataBlock>> &input_datablocks = input_table->data_blocks_;
    for (const auto &item : hash_table.hash_table_) {

        // 2.1 Each hash bucket will be insert in to one data block
        std::unique_ptr<DataBlock> output_datablock = DataBlock::MakeUniquePtr();
        size_t datablock_size = 0;
        for (const auto &vec_pair : item.second) {
            datablock_size += vec_pair.second.size();
        }
        size_t datablock_capacity = Utility::NextPowerOfTwo(datablock_size);
        output_datablock->Init(types, datablock_capacity);

        // Loop each block
        size_t output_data_num = 0;
        for (const auto &vec_pair : item.second) {
            size_t input_block_id = vec_pair.first;

            // Forloop each column
            for (size_t column_id = 0; column_id < column_count; ++column_id) {
                // Loop each row of same block
                for (const auto input_offset : vec_pair.second) {

                    output_datablock->column_vectors[column_id]->AppendWith(*input_datablocks[input_block_id]->column_vectors[column_id],
                                                                            input_offset,
                                                                            1);
                    ++output_data_num;
                }
            }
        }

        if (output_data_num != datablock_size * column_count) {
            UnrecoverableError(fmt::format("Expected block size: {}, but only copied data size: {}", datablock_size * column_count, output_data_num));
            break;
        }

        output_datablock->Finalize();
        output_datablocks.push_back(std::move(output_datablock));
    }
}

void PhysicalAggregate::GenerateGroupByResult(const std::shared_ptr<DataTable> &input_table,
                                              std::shared_ptr<DataTable> &output_table,
                                              HashTable &hash_table) {

    size_t column_count = input_table->ColumnCount();
    std::vector<std::shared_ptr<DataType>> types;
    types.reserve(column_count);
    for (size_t column_id = 0; column_id < column_count; ++column_id) {
        std::shared_ptr<DataType> input_type = input_table->GetColumnTypeById(column_id);
        std::shared_ptr<DataType> output_type = output_table->GetColumnTypeById(column_id);
        if (*input_type != *output_type) {
            Status status = Status::DataTypeMismatch(input_type->ToString(), output_type->ToString());
            RecoverableError(status);
        }
        types.emplace_back(input_type);
    }

    std::shared_ptr<DataBlock> output_datablock = nullptr;
    const std::vector<std::shared_ptr<DataBlock>> &input_datablocks = input_table->data_blocks_;
    for (const auto &item : hash_table.hash_table_) {
        // Each hash bucket will generate one data block.
        output_datablock = DataBlock::Make();
        output_datablock->Init(types, 1);

        // Only get the first row(block id and row offset of the block) of the bucket
        size_t input_block_id = item.second.begin()->first;
        size_t input_offset = item.second.begin()->second.front();

        // Only the first position of the column vector has value.
        for (size_t column_id = 0; column_id < column_count; ++column_id) {
            output_datablock->column_vectors[column_id]->AppendWith(*input_datablocks[input_block_id]->column_vectors[column_id], input_offset, 1);
        }

        output_datablock->Finalize();
        output_table->Append(output_datablock);
    }
}

bool PhysicalAggregate::SimpleAggregateExecute(const std::vector<std::unique_ptr<DataBlock>> &input_blocks,
                                               std::vector<std::unique_ptr<DataBlock>> &output_blocks,
                                               std::vector<std::unique_ptr<char[]>> &states,
                                               bool task_completed) {
    size_t aggregates_count = aggregates_.size();
    if (aggregates_count <= 0) {
        UnrecoverableError("Simple Aggregate without aggregate expression.");
    }

    size_t input_block_count = input_blocks.size();

    if (input_block_count == 0) {
        // No input data
        LOG_TRACE("No input, no aggregate result");
        return true;
    }

    // Prepare the output table columns
    std::vector<std::shared_ptr<ColumnDef>> aggregate_columns;
    aggregate_columns.reserve(aggregates_count);

    // Prepare the expression states
    std::vector<std::shared_ptr<ExpressionState>> expr_states;
    expr_states.reserve(aggregates_.size());

    // Prepare the output block
    std::vector<std::shared_ptr<DataType>> output_types;
    output_types.reserve(aggregates_count);

    AggregateFlag flag = output_blocks.empty() ? (!task_completed ? AggregateFlag::kUninitialized : AggregateFlag::kRunAndFinish)
                                               : (!task_completed ? AggregateFlag::kRunning : AggregateFlag::kFinish);

    for (i64 idx = 0; auto &expr : aggregates_) {
        // expression state
        expr_states.emplace_back(ExpressionState::CreateState(std::static_pointer_cast<AggregateExpression>(expr), states[idx].get(), flag));

        std::shared_ptr<DataType> output_type = std::make_shared<DataType>(expr->Type());

        // column definition
        std::shared_ptr<ColumnDef> col_def = std::make_shared<ColumnDef>(idx, output_type, expr->Name(), std::set<ConstraintType>());
        aggregate_columns.emplace_back(col_def);

        // for output block
        output_types.emplace_back(output_type);

        ++idx;
    }

    if (output_blocks.empty()) {
        for (size_t block_idx = 0; block_idx < input_block_count; ++block_idx) {
            output_blocks.emplace_back(DataBlock::MakeUniquePtr());
            auto out_put_block = output_blocks.back().get();
            out_put_block->Init(*GetOutputTypes());
        }
    }

    for (size_t block_idx = 0; block_idx < input_block_count; ++block_idx) {
        DataBlock *input_data_block = input_blocks[block_idx].get();

        DataBlock *output_data_block = output_blocks[block_idx].get();

        ExpressionEvaluator evaluator;
        evaluator.Init(input_data_block);

        size_t expression_count = aggregates_count;
        // calculate every columns value
        for (size_t expr_idx = 0; expr_idx < expression_count; ++expr_idx) {
            LOG_TRACE("Physical aggregate Execute");
            evaluator.Execute(aggregates_[expr_idx], expr_states[expr_idx], output_data_block->column_vectors[expr_idx]);
        }
        if (task_completed) {
            // Finalize the output block (e.g. calculate the average value
            output_data_block->Finalize();
        }
    }
    return true;
}

std::shared_ptr<std::vector<std::string>> PhysicalAggregate::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result = std::make_shared<std::vector<std::string>>();
    size_t groups_count = groups_.size();
    size_t aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (size_t i = 0; i < groups_count; ++i) {
        result->emplace_back(groups_[i]->Name());
    }
    for (size_t i = 0; i < aggregates_count; ++i) {
        result->emplace_back(aggregates_[i]->Name());
    }
    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalAggregate::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    size_t groups_count = groups_.size();
    size_t aggregates_count = aggregates_.size();
    result->reserve(groups_count + aggregates_count);
    for (size_t i = 0; i < groups_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(groups_[i]->Type()));
    }
    for (size_t i = 0; i < aggregates_count; ++i) {
        result->emplace_back(std::make_shared<DataType>(aggregates_[i]->Type()));
    }
    return result;
}

std::vector<HashRange> PhysicalAggregate::GetHashRanges(i64 parallel_count) const {
    std::vector<HashRange> result;
    result.resize(parallel_count);
    return result;
}

} // namespace infinity
