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

module infinity_core:physical_hash_aggregate.impl;

import :physical_hash_aggregate;
import :query_context;
import :operator_state;
import :data_table;
import :data_block;
import :base_expression;
import :expression_evaluator;
import :expression_state;
import :aggregate_utils;
import :load_meta;
import :infinity_exception;
import :table_def;
import :utility;

import std;
import third_party;

import logical_type;
import internal_types;
import column_def;
import data_type;

namespace infinity {

void PhysicalHashAggregate::Init(QueryContext *query_context) {
    groupby_types_.reserve(groups_.size());
    groupby_columns_.reserve(groups_.size());
    for (i64 idx = 0; auto &expr : groups_) {
        groupby_types_.emplace_back(std::make_shared<DataType>(expr->Type()));
        groupby_columns_.emplace_back(
            std::make_shared<ColumnDef>(idx, std::make_shared<DataType>(expr->Type()), expr->Name(), std::set<ConstraintType>()));
    }

    distinct_types_.reserve(distinct_columns_.size());
    distinct_column_defs_.reserve(distinct_columns_.size());
    for (i64 idx = 0; auto &expr : distinct_columns_) {
        distinct_types_.emplace_back(std::make_shared<DataType>(expr->Type()));
        distinct_column_defs_.emplace_back(
            std::make_shared<ColumnDef>(idx, std::make_shared<DataType>(expr->Type()), expr->Name(), std::set<ConstraintType>()));
    }

    output_types_.reserve(non_distinct_columns_.size());
    non_distinct_column_defs_.reserve(non_distinct_columns_.size());
    for (i64 idx = 0; auto &expr : non_distinct_columns_) {
        output_types_.emplace_back(std::make_shared<DataType>(expr->Type()));
        non_distinct_column_defs_.emplace_back(
            std::make_shared<ColumnDef>(idx, std::make_shared<DataType>(expr->Type()), expr->Name(), std::set<ConstraintType>()));
    }

    // Calculate hash key size (for group_by + distinct columns)
    hash_key_size_ = CalculateHashKeySize(groupby_types_) + CalculateHashKeySize(distinct_types_);
}

bool PhysicalHashAggregate::Execute(QueryContext *query_context, OperatorState *operator_state) {
    OperatorState *prev_op_state = operator_state->prev_op_state_;
    auto *hash_agg_operator_state = static_cast<HashAggregateOperatorState *>(operator_state);

    // 1. Execute group-by, distinct, and non-distinct expressions to generate output columns
    // all_types includes: groupby + distinct + non-distinct
    std::vector<std::shared_ptr<DataType>> all_types;
    all_types.reserve(groupby_types_.size() + distinct_types_.size() + output_types_.size());
    for (const auto &type : groupby_types_) {
        all_types.emplace_back(type);
    }
    for (const auto &type : distinct_types_) {
        all_types.emplace_back(type);
    }
    for (const auto &type : output_types_) {
        all_types.emplace_back(type);
    }

    // Prepare the expression states
    std::vector<std::shared_ptr<ExpressionState>> expr_states;
    expr_states.reserve(groups_.size() + distinct_columns_.size() + non_distinct_columns_.size());
    for (const auto &expr : groups_) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    for (const auto &expr : distinct_columns_) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }
    for (const auto &expr : non_distinct_columns_) {
        expr_states.emplace_back(ExpressionState::CreateState(expr));
    }

    // Create intermediate table for group_by + distinct + non-distinct columns
    std::vector<std::shared_ptr<ColumnDef>> all_columns;
    all_columns.reserve(groupby_columns_.size() + distinct_column_defs_.size() + non_distinct_column_defs_.size());
    for (const auto &col : groupby_columns_) {
        all_columns.emplace_back(col);
    }
    for (const auto &col : distinct_column_defs_) {
        all_columns.emplace_back(col);
    }
    for (const auto &col : non_distinct_column_defs_) {
        all_columns.emplace_back(col);
    }

    std::shared_ptr<TableDef> tabledef = TableDef::Make(std::make_shared<std::string>("default_db"),
                                                        std::make_shared<std::string>("hash_agg"),
                                                        std::make_shared<std::string>(""),
                                                        all_columns);
    std::shared_ptr<DataTable> intermediate_table = DataTable::Make(tabledef, TableType::kIntermediate);

    // For hash key calculation, only use groupby + distinct types
    std::vector<std::shared_ptr<DataType>> key_types;
    key_types.reserve(groupby_types_.size() + distinct_types_.size());
    for (const auto &type : groupby_types_) {
        key_types.emplace_back(type);
    }
    for (const auto &type : distinct_types_) {
        key_types.emplace_back(type);
    }

    // Execute expressions on each input block
    size_t input_block_count = prev_op_state->data_block_array_.size();
    for (size_t block_idx = 0; block_idx < input_block_count; ++block_idx) {
        DataBlock *input_data_block = prev_op_state->data_block_array_[block_idx].get();

        intermediate_table->data_blocks_.emplace_back(DataBlock::MakeUniquePtr());
        DataBlock *output_data_block = intermediate_table->data_blocks_.back().get();
        output_data_block->Init(all_types);

        ExpressionEvaluator evaluator;
        evaluator.Init(input_data_block);

        size_t expr_idx = 0;
        for (size_t i = 0; i < groups_.size(); ++i, ++expr_idx) {
            evaluator.Execute(groups_[i], expr_states[expr_idx], output_data_block->column_vectors_[expr_idx]);
        }

        for (size_t i = 0; i < distinct_columns_.size(); ++i, ++expr_idx) {
            evaluator.Execute(distinct_columns_[i], expr_states[expr_idx], output_data_block->column_vectors_[expr_idx]);
        }

        for (size_t i = 0; i < non_distinct_columns_.size(); ++i, ++expr_idx) {
            evaluator.Execute(non_distinct_columns_[i], expr_states[expr_idx], output_data_block->column_vectors_[expr_idx]);
        }

        output_data_block->Finalize();
    }

    // 2. Build hash table for deduplication
    auto &hash_table = hash_agg_operator_state->hash_table_;
    hash_table.clear();

    // If there are mixed aggregates (mixed distinct/non-distinct case),
    // don't deduplicate at this level - pass all rows through.
    // MergeHashAggregate will handle deduplication for distinct aggregates.
    if (non_distinct_columns_.size() > 0) {
        size_t block_count = intermediate_table->DataBlockCount();
        for (size_t block_id = 0; block_id < block_count; ++block_id) {
            const std::shared_ptr<DataBlock> &block_ptr = intermediate_table->GetDataBlockById(block_id);
            hash_agg_operator_state->data_block_array_.push_back(std::make_unique<DataBlock>(std::move(*block_ptr)));
        }
    } else {
        // Deduplicate based on distinct columns, or (group_by + distinct) combination
        const size_t key_size = hash_key_size_;
        std::string hash_key;
        if (key_size) {
            hash_key.reserve(key_size);
        }

        size_t block_count = intermediate_table->DataBlockCount();
        for (size_t block_id = 0; block_id < block_count; ++block_id) {
            const std::shared_ptr<DataBlock> &block_ptr = intermediate_table->GetDataBlockById(block_id);
            size_t row_count = block_ptr->row_count();
            for (size_t row_id = 0; row_id < row_count; ++row_id) {
                std::vector<std::shared_ptr<ColumnVector>> key_columns(block_ptr->column_vectors_.begin(),
                                                                       block_ptr->column_vectors_.begin() + key_types.size());
                BuildHashKey(key_columns, row_id, key_types, hash_key);
                if (hash_table.find(hash_key) == hash_table.end()) {
                    hash_table[hash_key] = std::make_pair(block_id, row_id);
                }
            }
        }

        // 3. Generate output with deduplicated rows - batch into blocks
        constexpr size_t kBatchSize = 1024;
        std::shared_ptr<DataBlock> current_block = DataBlock::Make();
        current_block->Init(all_types, kBatchSize);

        size_t row_count = 0;
        for (const auto &[hash_key, block_row_pair] : hash_table) {
            size_t block_id = block_row_pair.first;
            size_t row_id = block_row_pair.second;

            const std::shared_ptr<DataBlock> &src_block = intermediate_table->GetDataBlockById(block_id);

            // If current block is full, finalize it and start a new one
            if (row_count >= kBatchSize) {
                current_block->Finalize();
                hash_agg_operator_state->data_block_array_.push_back(std::make_unique<DataBlock>(std::move(*current_block)));
                current_block = DataBlock::Make();
                current_block->Init(all_types, kBatchSize);
                row_count = 0;
            }

            for (size_t col_id = 0; col_id < all_columns.size(); ++col_id) {
                current_block->column_vectors_[col_id]->AppendWith(*src_block->column_vectors_[col_id], row_id, 1);
            }
            row_count++;
        }

        // Finalize the last block if it has any rows
        if (row_count > 0) {
            current_block->Finalize();
            hash_agg_operator_state->data_block_array_.push_back(std::make_unique<DataBlock>(std::move(*current_block)));
        }
    }

    prev_op_state->data_block_array_.clear();

    hash_agg_operator_state->SetComplete();

    return true;
}

std::shared_ptr<std::vector<std::string>> PhysicalHashAggregate::GetOutputNames() const {
    auto result = std::make_shared<std::vector<std::string>>();
    result->reserve(groups_.size() + distinct_columns_.size() + non_distinct_columns_.size());
    for (const auto &expr : groups_) {
        result->push_back(expr->Name());
    }
    for (const auto &expr : distinct_columns_) {
        result->push_back(expr->Name());
    }
    for (const auto &expr : non_distinct_columns_) {
        result->push_back(expr->Name());
    }
    return result;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalHashAggregate::GetOutputTypes() const {
    auto result = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    result->reserve(groups_.size() + distinct_columns_.size() + non_distinct_columns_.size());
    for (const auto &expr : groups_) {
        result->push_back(std::make_shared<DataType>(expr->Type()));
    }
    for (const auto &expr : distinct_columns_) {
        result->push_back(std::make_shared<DataType>(expr->Type()));
    }
    for (const auto &expr : non_distinct_columns_) {
        result->push_back(std::make_shared<DataType>(expr->Type()));
    }
    return result;
}

std::vector<HashRange> PhysicalHashAggregate::GetHashRanges(i64 parallel_count) const {
    std::vector<HashRange> result;
    result.resize(parallel_count);
    return result;
}

} // namespace infinity
