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

#include <string>

module physical_sort;

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

import physical_operator_type;
import operator_state;
import data_block;
import infinity_exception;
import third_party;
import status;
import physical_top;
import logger;

namespace infinity {

struct BlockRawIndex {
    BlockRawIndex(u32 block_idx, u32 offset) : block_idx_(block_idx), offset_(offset) {}
    ~BlockRawIndex() = default;
    u32 block_idx_;
    u32 offset_;
};

class Comparator {
public:
    explicit Comparator(const CompareTwoRowAndPreferLeft &prefer_left_function,
                        const Vector<UniquePtr<DataBlock>> &order_by_blocks,
                        const Vector<SharedPtr<BaseExpression>> &expressions,
                        Vector<SharedPtr<ExpressionState>> &expr_states)
        : prefer_left_function_(prefer_left_function), order_by_blocks_(order_by_blocks), expressions_(expressions), expr_states_(expr_states) {}

    void Init() {
        if (order_by_blocks_.empty()) {
            return;
        }
        eval_results_ = PhysicalTop::GetEvalColumns(expressions_, expr_states_, order_by_blocks_);
    }

    bool Compare(BlockRawIndex left_index, BlockRawIndex right_index) {
        auto &left = eval_results_[left_index.block_idx_];
        auto &right = eval_results_[right_index.block_idx_];
        return prefer_left_function_.Compare(left, left_index.offset_, right, right_index.offset_);
    }

private:
    const CompareTwoRowAndPreferLeft &prefer_left_function_;
    const Vector<UniquePtr<DataBlock>> &order_by_blocks_;
    const Vector<SharedPtr<BaseExpression>> &expressions_;
    Vector<SharedPtr<ExpressionState>> &expr_states_;

    // Blocks -> Expressions
    Vector<Vector<SharedPtr<ColumnVector>>> eval_results_;
};

Vector<BlockRawIndex> MergeTwoIndexes(Vector<BlockRawIndex> &&indexes_a, Vector<BlockRawIndex> &&indexes_b, Comparator &comparator) {
    if (indexes_a.empty() || indexes_b.empty()) {
        return indexes_a.empty() ? indexes_b : indexes_a;
    }
    Vector<BlockRawIndex> merged_indexes;
    merged_indexes.reserve(indexes_a.size() + indexes_b.size());

    auto ptr_a = indexes_a.begin();
    auto ptr_b = indexes_b.begin();

    while (ptr_a != indexes_a.end() && ptr_b != indexes_b.end()) {
        if (comparator.Compare(*ptr_a, *ptr_b)) {
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

Vector<BlockRawIndex> MergeIndexes(Vector<Vector<BlockRawIndex>> &indexes_group, SizeT l, SizeT r, Comparator &comparator) {
    if (l > r or r >= indexes_group.size())
        return Vector<BlockRawIndex>();
    if (l == r)
        return indexes_group[l];
    SizeT mid = (l + r) >> 1;
    return MergeTwoIndexes(MergeIndexes(indexes_group, l, mid, comparator), MergeIndexes(indexes_group, mid + 1, r, comparator), comparator);
}

void CopyWithIndexes(const Vector<UniquePtr<DataBlock>> &input_blocks,
                     Vector<UniquePtr<DataBlock>> &output_blocks,
                     const Vector<BlockRawIndex> &block_indexes) {
    if (input_blocks.empty()) {
        return;
    }

    SizeT start_block_index = output_blocks.size();
    auto block_count = (block_indexes.size() + DEFAULT_BLOCK_CAPACITY - 1) / DEFAULT_BLOCK_CAPACITY;

    // copy with block_indexes and push to unmerge_sorted_blocks
    for (SizeT i = 0; i < block_count; ++i) {
        auto sorted_datablock = DataBlock::MakeUniquePtr();
        sorted_datablock->Init(input_blocks[0]->types());

        output_blocks.push_back(std::move(sorted_datablock));
    }
    for (SizeT index_idx = 0; index_idx < block_indexes.size(); ++index_idx) {
        auto &block_index = block_indexes[index_idx];
        const Vector<SharedPtr<ColumnVector>> &output_column_vectors =
            output_blocks[(index_idx / DEFAULT_BLOCK_CAPACITY) + start_block_index]->column_vectors;

        for (SizeT column_id = 0; column_id < output_column_vectors.size(); ++column_id) {
            output_column_vectors[column_id]->AppendWith(*input_blocks[block_index.block_idx_]->column_vectors[column_id].get(),
                                                         block_index.offset_,
                                                         1);
        }
    }
    for (SizeT i = 0; i < block_count; ++i) {
        output_blocks[i + start_block_index]->Finalize();
    }
}

void PhysicalSort::Init() {
    auto sort_expr_count = order_by_types_.size();
    if (sort_expr_count != expressions_.size()) {
        String error_message = "order_by_types_.size() != expressions_.size()";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    Vector<std::function<std::strong_ordering(const SharedPtr<ColumnVector> &, u32, const SharedPtr<ColumnVector> &, u32)>> sort_functions;
    sort_functions.reserve(sort_expr_count);
    for (u32 i = 0; i < sort_expr_count; ++i) {
        sort_functions.emplace_back(PhysicalTop::GenerateSortFunction(order_by_types_[i], expressions_[i]));
    }
    prefer_left_function_ = CompareTwoRowAndPreferLeft(std::move(sort_functions));
}

bool PhysicalSort::Execute(QueryContext *, OperatorState *operator_state) {
    auto *prev_op_state = operator_state->prev_op_state_;
    auto *sort_operator_state = static_cast<SortOperatorState *>(operator_state);

    // Generate block indexes
    Vector<BlockRawIndex> block_indexes;
    auto pre_op_state = operator_state->prev_op_state_;

    block_indexes.reserve(pre_op_state->data_block_array_.size() * DEFAULT_BLOCK_CAPACITY);
    // filling block_indexes
    for (u32 block_id = 0; block_id < pre_op_state->data_block_array_.size(); block_id++) {
        for (u32 offset = 0; offset < pre_op_state->data_block_array_[block_id]->row_count(); offset++) {
            block_indexes.emplace_back(block_id, offset);
        }
    }
    auto &expr_states = (static_cast<SortOperatorState *>(operator_state))->expr_states_;
    auto block_comparator = Comparator(prefer_left_function_, pre_op_state->data_block_array_, expressions_, expr_states);

    block_comparator.Init();
    // sort block_indexes
    std::sort(block_indexes.begin(), block_indexes.end(), [&block_comparator](BlockRawIndex x, BlockRawIndex y) -> bool {
        // Be careful! std::sort needs a strict ordering comparator. ("<" instead of "<=")
        return !block_comparator.Compare(y, x);
    });

    CopyWithIndexes(pre_op_state->data_block_array_, sort_operator_state->unmerge_sorted_blocks_, block_indexes);
    prev_op_state->data_block_array_.clear();

    if (!prev_op_state->Complete()) {
        return false;
    }
    auto &unmerge_sorted_blocks = sort_operator_state->unmerge_sorted_blocks_;
    auto merge_comparator = Comparator(prefer_left_function_, unmerge_sorted_blocks, expressions_, expr_states);
    Vector<Vector<BlockRawIndex>> indexes_group;

    merge_comparator.Init();
    indexes_group.reserve(unmerge_sorted_blocks.size());

    for (u32 block_id = 0; block_id < unmerge_sorted_blocks.size(); ++block_id) {
        Vector<BlockRawIndex> indexes;
        indexes.reserve(unmerge_sorted_blocks[block_id]->row_count());

        for (u32 offset = 0; offset < unmerge_sorted_blocks[block_id]->row_count(); ++offset) {
            indexes.emplace_back(block_id, offset);
        }
        indexes_group.push_back(std::move(indexes));
    }
    auto merge_indexes = MergeIndexes(indexes_group, 0, indexes_group.size() - 1, merge_comparator);
    indexes_group.clear();

    CopyWithIndexes(sort_operator_state->unmerge_sorted_blocks_, sort_operator_state->data_block_array_, merge_indexes);
    sort_operator_state->unmerge_sorted_blocks_.clear();
    sort_operator_state->SetComplete();
    return true;
}

} // namespace infinity
