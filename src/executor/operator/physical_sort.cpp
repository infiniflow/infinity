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

#include <algorithm>
#include <string>

import stl;
import txn;
import query_context;
import table_def;
import data_table;
import data_block;
import default_values;
import column_vector;
import expression_state;
import base_expression;
import expression_evaluator;
import parser;
import physical_operator_type;
import operator_state;
import data_block;
import infinity_exception;
import third_party;

module physical_sort;

namespace infinity {

#define COMPARE(type)                                                                                                                                \
    type left_value = ((type *)(left_result_vector->data()))[left_index.offset];                                                                     \
    type right_value = ((type *)(right_result_vector->data()))[right_index.offset];                                                                  \
    if (left_value == right_value) {                                                                                                                 \
        continue;                                                                                                                                    \
    }                                                                                                                                                \
    if (order_type == OrderType::kAsc) {                                                                                                             \
        return left_value < right_value;                                                                                                             \
    } else {                                                                                                                                         \
        return left_value > right_value;                                                                                                             \
    }

class Comparator {
public:
    explicit Comparator(const Vector<UniquePtr<DataBlock>> &order_by_blocks,
                        const Vector<OrderType> &order_by_types,
                        Vector<SharedPtr<BaseExpression>> expressions)
        : order_by_blocks_(order_by_blocks), order_by_types_(order_by_types), expressions_(expressions) {}

    bool operator()(BlockRawIndex left_index, BlockRawIndex right_index) {
        SizeT exprs_count = expressions_.size();
        for (SizeT expr_idx = 0; expr_idx < exprs_count; ++expr_idx) {
            auto expr = expressions_[expr_idx];
            DataType type = expr->Type();
            OrderType order_type = order_by_types_[expr_idx];

            const UniquePtr<DataBlock> &left_block = order_by_blocks_[left_index.block_idx];
            SharedPtr<ColumnVector> left_result_vector = EvalOrderVector(left_block.get(), left_index.block_idx, expr);

            const UniquePtr<DataBlock> &right_block = order_by_blocks_[right_index.block_idx];
            SharedPtr<ColumnVector> right_result_vector = EvalOrderVector(right_block.get(), right_index.block_idx, expr);

            switch (type.type()) {
                case kBoolean: {
                    COMPARE(BooleanT)
                }
                case kTinyInt: {
                    COMPARE(TinyIntT)
                }
                case kSmallInt: {
                    COMPARE(SmallIntT)
                }
                case kInteger: {
                    COMPARE(IntegerT)
                }
                case kBigInt: {
                    COMPARE(BigIntT)
                }
                case kFloat: {
                    COMPARE(FloatT)
                }
                case kDouble: {
                    COMPARE(DoubleT)
                }
                default: {
                    Error<NotImplementException>(Format("{} not implemented.", type.type()));
                }
            }
        }
        return true;
    }

private:
    SharedPtr<ColumnVector> EvalOrderVector(DataBlock *block, SizeT block_idx, SharedPtr<BaseExpression> &expr) {
        auto eval_key = MakeShared<Pair<SizeT, SharedPtr<BaseExpression>>>(block_idx, expr);
        if (eval_cache_.contains(eval_key)) {
            return eval_cache_[eval_key];
        } else {
            ExpressionEvaluator expr_evaluator;
            auto state = ExpressionState::CreateState(expr);
            SharedPtr<ColumnVector> result_vector = MakeShared<ColumnVector>(MakeShared<DataType>(expr->Type()));
            result_vector->Initialize();

            expr_evaluator.Init(block);
            expr_evaluator.Execute(expr, state, result_vector);

            eval_cache_[eval_key] = result_vector;

            return result_vector;
        }
    }

    const Vector<UniquePtr<DataBlock>> &order_by_blocks_;

    const Vector<OrderType> &order_by_types_;
    Vector<SharedPtr<BaseExpression>> expressions_;

    // K: BlockIdx & Expression
    // V: ColumnVector
    HashMap<SharedPtr<Pair<SizeT, SharedPtr<BaseExpression>>>, SharedPtr<ColumnVector>> eval_cache_{};
};

Vector<BlockRawIndex> MergeTwoIndexes(Vector<BlockRawIndex> &indexes_a, Vector<BlockRawIndex> &indexes_b, Comparator &comparator) {
    if (indexes_a.empty() || indexes_b.empty()) {
        return indexes_a.empty() ? indexes_b : indexes_a;
    }
    Vector<BlockRawIndex> merged_indexes;
    merged_indexes.reserve(indexes_a.size() + indexes_b.size());

    auto ptr_a = indexes_a.begin();
    auto ptr_b = indexes_b.begin();

    while (ptr_a != indexes_a.end() && ptr_b != indexes_b.end()) {
        if (comparator(*ptr_a, *ptr_b)) {
            merged_indexes.push_back(*ptr_a);
            ++ptr_a;
        } else {
            merged_indexes.push_back(*ptr_b);
            ++ptr_b;
        }
    }
    if (ptr_a != indexes_a.end()) {
        merged_indexes.insert(merged_indexes.end(), ptr_a, indexes_a.end());
    }
    if (ptr_b != indexes_b.end()) {
        merged_indexes.insert(merged_indexes.end(), ptr_b, indexes_b.end());
    }

    return merged_indexes;
}

Vector<BlockRawIndex> MergeIndexes(Vector<Vector<BlockRawIndex>> &&indexes_group, Comparator &comparator) {
    Vector<BlockRawIndex> ans;
    for (SizeT i = 0; i < indexes_group.size(); ++i) {
        ans = MergeTwoIndexes(ans, indexes_group[i], comparator);
    }
    return ans;
}

void CopyWithIndexes(const Vector<UniquePtr<DataBlock>> &input_blocks,
                     Vector<UniquePtr<DataBlock>> &output_blocks,
                     const Vector<BlockRawIndex> &block_indexes,
                     const SharedPtr<Vector<SharedPtr<DataType>>> &types) {
    auto block_count = (block_indexes.size() + DEFAULT_BLOCK_CAPACITY) / DEFAULT_BLOCK_CAPACITY;

    // copy with block_indexes and push to unmerge_sorted_blocks
    for (SizeT i = 0; i < block_count; ++i) {
        auto sorted_datablock = DataBlock::MakeUniquePtr();
        sorted_datablock->Init(*types.get());

        output_blocks.push_back(Move(sorted_datablock));
    }
    for (SizeT index_idx = 0; index_idx < block_indexes.size(); ++index_idx) {
        auto &block_index = block_indexes[index_idx];
        const Vector<SharedPtr<ColumnVector>> &output_column_vectors = output_blocks[index_idx / DEFAULT_BLOCK_CAPACITY]->column_vectors;

        for (SizeT column_id = 0; column_id < output_column_vectors.size(); ++column_id) {
            output_column_vectors[column_id]->AppendValue(input_blocks[block_index.block_idx]->GetValue(column_id, block_index.offset));
        }
    }
    for (SizeT i = 0; i < block_count; ++i) {
        output_blocks[i]->Finalize();
    }
}

void PhysicalSort::Init() {}

bool PhysicalSort::Execute(QueryContext *, OperatorState *operator_state) {
    auto *prev_op_state = operator_state->prev_op_state_;
    auto *sort_operator_state = static_cast<SortOperatorState *>(operator_state);

    // Generate block indexes
    Vector<BlockRawIndex> block_indexes;
    auto pre_op_state = operator_state->prev_op_state_;

    // filling block_indexes
    for (SizeT block_id = 0; block_id < pre_op_state->data_block_array_.size(); block_id++) {
        for (SizeT offset = 0; offset < pre_op_state->data_block_array_[block_id]->row_count(); offset++) {
            BlockRawIndex block_index(block_id, offset);

            block_indexes.push_back(block_index);
        }
    }

    // sort block_indexes
    std::sort(block_indexes.begin(), block_indexes.end(), Comparator(pre_op_state->data_block_array_, order_by_types_, expressions_));

    auto types = left_->GetOutputTypes();

    CopyWithIndexes(pre_op_state->data_block_array_, sort_operator_state->unmerge_sorted_blocks_, block_indexes, types);
    prev_op_state->data_block_array_.clear();

    if (prev_op_state->Complete()) {
        auto &unmerge_sorted_blocks = sort_operator_state->unmerge_sorted_blocks_;
        auto merge_comparator = Comparator(unmerge_sorted_blocks, order_by_types_, expressions_);

        Vector<Vector<BlockRawIndex>> indexes_group;
        indexes_group.reserve(unmerge_sorted_blocks.size());
        for (SizeT block_id = 0; block_id < unmerge_sorted_blocks.size(); ++block_id) {
            Vector<BlockRawIndex> indexes;
            indexes.reserve(unmerge_sorted_blocks[block_id]->row_count());

            for (SizeT offset = 0; offset < unmerge_sorted_blocks[block_id]->row_count(); ++offset) {
                BlockRawIndex block_index(block_id, offset);
                indexes.push_back(block_index);
            }
            indexes_group.push_back(indexes);

            auto output_datablock = DataBlock::MakeUniquePtr();
            output_datablock->Init(*types.get());

            sort_operator_state->data_block_array_.push_back(Move(output_datablock));
        }
        auto merge_indexes = MergeIndexes(Move(indexes_group), merge_comparator);

        CopyWithIndexes(sort_operator_state->unmerge_sorted_blocks_, sort_operator_state->data_block_array_, merge_indexes, types);
        sort_operator_state->unmerge_sorted_blocks_.clear();
        sort_operator_state->SetComplete();
    }
    return true;
}

} // namespace infinity
