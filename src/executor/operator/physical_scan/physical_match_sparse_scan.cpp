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

module physical_match_sparse_scan;

import stl;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import table_entry;
import match_sparse_expression;
import base_table_ref;
import data_type;
import common_query_filter;
import default_values;
import logical_type;
import match_sparse_scan_function_data;
import block_entry;
import logger;
import third_party;
import buffer_manager;
import expression_evaluator;
import expression_state;
import base_expression;
import column_vector;
import data_block;

namespace infinity {

PhysicalMatchSparseScan::PhysicalMatchSparseScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchSparseExpression> match_sparse_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalScanBase(id, PhysicalOperatorType::kMatchSparseScan, nullptr, nullptr, base_table_ref, load_metas), table_index_(table_index),
      match_sparse_expr_(std::move(match_sparse_expression)), common_query_filter_(common_query_filter) {}

void PhysicalMatchSparseScan::Init() { search_column_id_ = match_sparse_expr_->column_expr_->binding().column_idx; }

SharedPtr<Vector<String>> PhysicalMatchSparseScan::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatchSparseScan::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(MakeShared<DataType>(match_sparse_expr_->Type()));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

bool PhysicalMatchSparseScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *match_sparse_scan_state = static_cast<MatchSparseScanOperatorState *>(operator_state);
    MatchSparseScanFunctionData &function_data = match_sparse_scan_state->match_sparse_scan_function_data_;
    SharedPtr<DataBlock> query_data = function_data.query_data_;

    if (!function_data.evaluated_) {
        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);

        SharedPtr<BaseExpression> query_expr = match_sparse_expr_->query_sparse_expr_;

        Vector<SharedPtr<DataType>> output_types;
        output_types.push_back(MakeShared<DataType>(query_expr->Type()));
        query_data->Init(output_types);
        SharedPtr<ExpressionState> expr_state = ExpressionState::CreateState(query_expr);
        evaluator.Execute(query_expr, expr_state, query_data->column_vectors[0]);

        function_data.evaluated_ = true;
    }

    // BufferManager *buffer_mgr = query_context->storage()->buffer_manager();

    const Vector<GlobalBlockID> &block_ids = *function_data.global_block_ids_;
    auto &block_ids_idx = function_data.current_block_ids_idx_;
    // const BlockIndex *block_index = function_data.block_index_;

    if (auto task_id = block_ids_idx; task_id < block_ids.size()) {
        ++block_ids_idx;
        const auto [segment_id, block_id] = block_ids[task_id];

        // BlockOffset row_cnt = block_index->GetBlockOffset(segment_id, block_id);
        // const BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
        LOG_DEBUG(fmt::format("MatchSparseScan: segment_id: {}, block_id: {}", segment_id, block_id));

        // auto *block_column_entry = block_entry->GetColumnBlockEntry(search_column_id_);
        // auto column_vector = block_column_entry->GetColumnVector(buffer_mgr);
    }
    if (block_ids_idx >= block_ids.size()) {
        LOG_DEBUG(fmt::format("MatchSparseScan: {} task finished", block_ids_idx));
        match_sparse_scan_state->SetComplete();
    }
    return true;
}

} // namespace infinity
