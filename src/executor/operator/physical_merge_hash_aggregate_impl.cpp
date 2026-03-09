// Copyright(C) 2026 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:physical_merge_hash_aggregate.impl;

import :physical_merge_hash_aggregate;
import :query_context;
import :operator_state;
import :data_table;
import :data_block;
import :base_expression;
import :reference_expression;
import :column_expression;
import :expression_evaluator;
import :expression_state;
import :aggregate_expression;
import :aggregate_utils;
import :load_meta;
import :infinity_exception;
import :table_def;
import :utility;
import :column_vector;

import std;
import third_party;

import logical_type;
import internal_types;
import column_def;
import data_type;

namespace infinity {

void PhysicalMergeHashAggregate::Init(QueryContext *query_context) {
    groupby_types_.reserve(groups_.size());
    for (auto &expr : groups_) {
        groupby_types_.emplace_back(std::make_shared<DataType>(expr->Type()));
    }

    aggregate_types_.reserve(aggregates_.size());
    for (auto &expr : aggregates_) {
        aggregate_types_.emplace_back(std::make_shared<DataType>(expr->Type()));
    }

    hash_key_size_ = CalculateHashKeySize(groupby_types_);
}

bool PhysicalMergeHashAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    // MergeHashAggregate takes data from HashAggregate and performs global deduplication + final aggregation
    auto *merge_agg_op_state = static_cast<MergeHashAggregateOperatorState *>(operator_state);

    // Process input data incrementally if available
    if (merge_agg_op_state->input_data_block_ != nullptr) {
        auto &block = merge_agg_op_state->input_data_block_;
        if (!block) {
            UnrecoverableError("Input data block pointer is null in MergeHashAggregate");
        }
        if (!block->Initialized()) {
            UnrecoverableError("Input data block is not initialized in MergeHashAggregate");
        }
        ProcessInputBlockIncremental(block.get(), merge_agg_op_state);
        merge_agg_op_state->input_data_block_.reset();
    } else if (merge_agg_op_state->prev_op_state_ != nullptr && !merge_agg_op_state->prev_op_state_->data_block_array_.empty()) {
        for (auto &block : merge_agg_op_state->prev_op_state_->data_block_array_) {
            if (block && block->Initialized()) {
                ProcessInputBlockIncremental(block.get(), merge_agg_op_state);
            }
        }
        merge_agg_op_state->prev_op_state_->data_block_array_.clear();
    }

    // If input is not complete, return false to wait for more data
    if (!merge_agg_op_state->input_complete_) {
        return false;
    }

    GenerateOutputFromHashTable(merge_agg_op_state);
    merge_agg_op_state->SetComplete();
    return true;
}

void PhysicalMergeHashAggregate::ProcessInputBlockIncremental(DataBlock *input_block, MergeHashAggregateOperatorState *op_state) {
    size_t group_count = groups_.size();
    size_t row_count = input_block->row_count();
    if (row_count == 0) {
        return;
    }

    std::vector<std::shared_ptr<ColumnVector>> groupby_cols;
    if (group_count > 0) {
        groupby_cols.reserve(group_count);
        for (size_t col_idx = 0; col_idx < group_count; ++col_idx) {
            groupby_cols.push_back(input_block->column_vectors_[col_idx]);
        }
    }

    std::vector<std::shared_ptr<DataType>> src_types;
    src_types.reserve(input_block->column_count());
    for (size_t col = 0; col < input_block->column_count(); ++col) {
        src_types.push_back(input_block->column_vectors_[col]->data_type());
    }

    auto single_row_block = DataBlock::MakeUniquePtr();
    single_row_block->Init(src_types, 1);
    auto temp_output = DataBlock::MakeUniquePtr();
    temp_output->Init(aggregate_types_, 1);

    std::vector<std::vector<std::shared_ptr<ColumnVector>>> col_vec;
    std::vector<std::vector<std::shared_ptr<DataType>>> col_type;
    col_vec.resize(aggregates_.size());
    col_type.resize(aggregates_.size());

    for (size_t agg_idx = 0; agg_idx < aggregates_.size(); ++agg_idx) {
        auto agg_expr = std::static_pointer_cast<AggregateExpression>(aggregates_[agg_idx]);
        const auto &args = agg_expr->arguments();
        if (args.empty()) {
            continue;
        }

        if (agg_expr->distinct()) {
            for (const auto &arg : args) {
                size_t col_idx = 0;
                if (arg->type() == ExpressionType::kColumn) {
                    auto col_expr = std::static_pointer_cast<ColumnExpression>(arg);
                    col_idx = col_expr->binding().column_idx;
                } else if (arg->type() == ExpressionType::kReference) {
                    auto ref_expr = std::static_pointer_cast<ReferenceExpression>(arg);
                    col_idx = ref_expr->column_index();
                } else {
                    continue;
                }
                col_vec[agg_idx].push_back(input_block->column_vectors_[col_idx]);
                col_type[agg_idx].push_back(input_block->column_vectors_[col_idx]->data_type());
            }
        }
    }

    for (size_t row_id = 0; row_id < row_count; ++row_id) {
        // Compute group key - extract only the groupby columns
        std::string group_key;
        if (group_count > 0) {
            BuildHashKey(groupby_cols, row_id, groupby_types_, group_key);
        }

        auto it = op_state->hash_table_.find(group_key);
        if (it == op_state->hash_table_.end()) {
            MergeHashAggregateOperatorState::GroupAggregateData group_data;

            if (group_count > 0) {
                group_data.groupby_columns.resize(group_count);
                for (size_t col_idx = 0; col_idx < group_count; ++col_idx) {
                    // Create a new column vector for this groupby column
                    auto col_vec = ColumnVector::Make(input_block->column_vectors_[col_idx]->data_type());
                    col_vec->Initialize(ColumnVectorType::kFlat, 1);
                    col_vec->AppendWith(*input_block->column_vectors_[col_idx], row_id, 1);
                    group_data.groupby_columns[col_idx] = std::move(col_vec);
                }
            }

            group_data.distinct_values.resize(aggregates_.size());
            group_data.agg_states.resize(aggregates_.size());
            for (size_t agg_idx = 0; agg_idx < aggregates_.size(); ++agg_idx) {
                auto agg_expr = std::static_pointer_cast<AggregateExpression>(aggregates_[agg_idx]);
                group_data.agg_states[agg_idx] = agg_expr->aggregate_function_.InitState();
                agg_expr->aggregate_function_.init_func_(group_data.agg_states[agg_idx].get());
            }

            it = op_state->hash_table_.emplace(std::move(group_key), std::move(group_data)).first;
        }

        auto &group_data = it->second;

        // Process each aggregate
        for (size_t agg_idx = 0; agg_idx < aggregates_.size(); ++agg_idx) {
            auto agg_expr = std::static_pointer_cast<AggregateExpression>(aggregates_[agg_idx]);
            const auto &args = agg_expr->arguments();
            if (args.empty()) {
                continue;
            }

            // For DISTINCT aggregates: check if this value is already in the distinct set
            // For non-DISTINCT aggregates: always update the aggregate (no deduplication)
            if (agg_expr->distinct()) {
                std::string value_key;
                BuildHashKey(col_vec[agg_idx], row_id, col_type[agg_idx], value_key);

                auto &distinct_set = group_data.distinct_values[agg_idx];
                if (distinct_set.find(value_key) != distinct_set.end()) {
                    continue;
                }
                distinct_set.insert(value_key);
            }

            for (size_t col = 0; col < input_block->column_count(); ++col) {
                single_row_block->column_vectors_[col]->AppendWith(*input_block->column_vectors_[col], row_id, 1);
            }
            single_row_block->Finalize();

            // Update aggregate state with this value
            // For DISTINCT: only called for new distinct values
            // For non-DISTINCT: called for all values
            auto expr_state = ExpressionState::CreateState(agg_expr, group_data.agg_states[agg_idx].get(), AggregateFlag::kRunning);
            ExpressionEvaluator agg_evaluator;
            agg_evaluator.Init(single_row_block.get());
            agg_evaluator.Execute(aggregates_[agg_idx], expr_state, temp_output->column_vectors_[agg_idx]);
            single_row_block->Reset();
        }
    }
}

void PhysicalMergeHashAggregate::GenerateOutputFromHashTable(MergeHashAggregateOperatorState *op_state) {
    size_t group_count = groups_.size();

    std::vector<std::shared_ptr<DataType>> output_types;
    if (group_count > 0) {
        output_types = groupby_types_;
    }
    for (const auto &agg_type : aggregate_types_) {
        output_types.emplace_back(agg_type);
    }

    if (group_count == 0) {
        auto it = op_state->hash_table_.find("");
        if (it == op_state->hash_table_.end() || it->second.distinct_values.empty()) {
            // No data processed, create output with default aggregate values
            if (!aggregates_.empty()) {
                std::unique_ptr<DataBlock> output_block;
                GenerateDefaultAggregateOutput(aggregates_, output_block);
                op_state->data_block_array_.push_back(std::move(output_block));
            }
            return;
        }

        auto &group_data = it->second;
        auto output_block = DataBlock::MakeUniquePtr();
        output_block->Init(aggregate_types_, 1);

        // Compute final aggregates from states
        for (size_t agg_idx = 0; agg_idx < aggregates_.size(); ++agg_idx) {
            auto agg_expr = std::static_pointer_cast<AggregateExpression>(aggregates_[agg_idx]);
            const char *result_ptr = agg_expr->aggregate_function_.finalize_func_(group_data.agg_states[agg_idx].get());
            output_block->column_vectors_[agg_idx]->AppendByPtr(result_ptr);
        }

        output_block->Finalize();
        op_state->data_block_array_.push_back(std::move(output_block));
    } else {
        // Handle group by case
        for (auto &[group_key, group_data] : op_state->hash_table_) {
            if (group_data.distinct_values.empty()) {
                continue;
            }

            auto final_block = DataBlock::MakeUniquePtr();
            final_block->Init(output_types, 1);

            for (size_t col_idx = 0; col_idx < group_count; ++col_idx) {
                final_block->column_vectors_[col_idx] = group_data.groupby_columns[col_idx];
            }

            // For each aggregate, compute final result
            for (size_t agg_idx = 0; agg_idx < aggregates_.size(); ++agg_idx) {
                auto agg_expr = std::static_pointer_cast<AggregateExpression>(aggregates_[agg_idx]);
                const char *result_ptr = agg_expr->aggregate_function_.finalize_func_(group_data.agg_states[agg_idx].get());
                final_block->column_vectors_[group_count + agg_idx]->AppendByPtr(result_ptr);
            }

            final_block->Finalize();
            op_state->data_block_array_.push_back(std::move(final_block));
        }
    }
}

std::shared_ptr<std::vector<std::string>> PhysicalMergeHashAggregate::GetOutputNames() const {
    auto result = std::make_shared<std::vector<std::string>>();
    result->reserve(groups_.size() + aggregates_.size());
    for (const auto &expr : groups_) {
        result->push_back(expr->Name());
    }
    for (const auto &expr : aggregates_) {
        result->push_back(expr->Name());
    }
    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalMergeHashAggregate::GetOutputTypes() const {
    auto result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    result->reserve(groups_.size() + aggregates_.size());
    for (const auto &expr : groups_) {
        result->push_back(std::make_shared<DataType>(expr->Type()));
    }
    for (const auto &expr : aggregates_) {
        result->push_back(std::make_shared<DataType>(expr->Type()));
    }
    return result;
}

} // namespace infinity
