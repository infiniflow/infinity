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
#include <vector>
module physical_match_tensor_scan;

import stl;
import txn;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;
import query_context;
import operator_state;
import data_block;
import column_vector;
import expression_evaluator;
import expression_state;
import base_expression;
import knn_expr;
import match_tensor_expr;
import match_tensor_expression;
import default_values;
import infinity_exception;
import third_party;
import base_table_ref;
import load_meta;
import block_entry;
import block_column_entry;
import logical_type;
import status;
import logger;
import physical_index_scan;
import filter_value_type_classification;
import bitmask;
import segment_entry;
import knn_filter;
import global_block_id;
import block_index;
import column_def;
import internal_types;
import fix_heap;
import type_info;
import embedding_info;
import buffer_manager;
import match_tensor_scan_function_data;
import mlas_matrix_multiply;

namespace infinity {

PhysicalMatchTensorScan::PhysicalMatchTensorScan(const u64 id,
                                                 const u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchTensorExpression> match_tensor_expr,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 const u32 topn,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kMatchTensorScan, nullptr, nullptr, id, load_metas), table_index_(table_index),
      base_table_ref_(std::move(base_table_ref)), match_tensor_expr_(std::move(match_tensor_expr)), common_query_filter_(common_query_filter),
      topn_(topn) {
    search_column_id_ = std::numeric_limits<ColumnID>::max();
}

void PhysicalMatchTensorScan::Init() {
    search_column_id_ = match_tensor_expr_->column_expr_->binding().column_idx;
    const ColumnDef *column_def = base_table_ref_->table_entry_ptr_->GetColumnDefByID(search_column_id_);
    const auto &column_type_ptr = column_def->type();
    if (column_type_ptr->type() != LogicalType::kTensor) {
        String error_message = fmt::format("Column {} is not a tensor column", column_def->name());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    const auto &type_info = column_type_ptr->type_info();
    if (type_info->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Column {} is not a tensor column", column_def->name());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_info.get());
    if (embedding_info->Dimension() != match_tensor_expr_->tensor_basic_embedding_dimension_) {
        String error_message = fmt::format("Column {} embedding dimension not match with query {}", column_def->name(), match_tensor_expr_->ToString());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    // TODO: now only support MaxSim
    if (match_tensor_expr_->search_method_ != MatchTensorMethod::kMaxSim) {
        String error_message = "Now only support MaxSim search.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    // TODO: now only support search on float32 tensor column
    if (embedding_info->Type() != EmbeddingDataType::kElemFloat) {
        String error_message = "Now only support tensor search on float column.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    // TODO: now only support float32 tensor query
    if (match_tensor_expr_->embedding_data_type_ != EmbeddingDataType::kElemFloat) {
        String error_message = "Now only support float32 tensor query.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

SharedPtr<Vector<String>> PhysicalMatchTensorScan::GetOutputNames() const {
    SharedPtr<Vector<String>> result_names = MakeShared<Vector<String>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

SharedPtr<Vector<SharedPtr<DataType>>> PhysicalMatchTensorScan::GetOutputTypes() const {
    SharedPtr<Vector<SharedPtr<DataType>>> result_types = MakeShared<Vector<SharedPtr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(MakeShared<DataType>(match_tensor_expr_->Type()));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

SizeT PhysicalMatchTensorScan::TaskletCount() { return base_table_ref_->block_index_->BlockCount(); }

BlockIndex *PhysicalMatchTensorScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

ColumnID PhysicalMatchTensorScan::SearchColumnID() const {
    if (search_column_id_ == std::numeric_limits<ColumnID>::max()) {
        String error_message = "Search column id is not set. Init() error.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    return search_column_id_;
}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalMatchTensorScan::PlanBlockEntries(i64 parallel_count) const {
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<GlobalBlockID> global_blocks;
    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        for (const auto *block_entry : segment_info.block_map_) {
            global_blocks.emplace_back(segment_id, block_entry->block_id());
        }
    }
    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (SizeT task_id = 0, global_block_id = 0, residual_idx = 0; (i64)task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->push_back(global_blocks[global_block_id++]);
        }
        if (residual_idx < residual) {
            result[task_id]->push_back(global_blocks[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

bool PhysicalMatchTensorScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *match_tensor_scan_operator_state = static_cast<MatchTensorScanOperatorState *>(operator_state);
    ExecuteInner(query_context, match_tensor_scan_operator_state);
    return true;
}

void PhysicalMatchTensorScan::ExecuteInner(QueryContext *query_context, MatchTensorScanOperatorState *operator_state) const {
    if (!operator_state->data_block_array_.empty()) {
        String error_message = "TensorScan output data block array should be empty";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    Txn *txn = query_context->GetTxn();
    const TxnTimeStamp begin_ts = txn->BeginTS();
    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    if (!common_query_filter_->TryFinishBuild(txn)) {
        // not ready, abort and wait for next time
        return;
    }
    MatchTensorScanFunctionData &function_data = *(operator_state->match_tensor_scan_function_data_);
    if (function_data.finished_) [[unlikely]] {
        String error_message = "MatchTensorScanFunctionData is finished";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    const auto search_column_id = SearchColumnID();
    const BlockIndex *block_index = function_data.block_index_;
    const Vector<GlobalBlockID> &block_ids = *function_data.global_block_ids_;
    auto &block_ids_idx = function_data.current_block_ids_idx_;
    if (const auto task_id = block_ids_idx; task_id < block_ids.size()) {
        ++block_ids_idx;
        const auto [segment_id, block_id] = block_ids[task_id];
        const BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
        if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
            // not skipped after common_query_filter
            const auto row_count = block_entry->row_count();
            // filter for segment
            const std::variant<Vector<u32>, Bitmask> &filter_result = it->second;
            Bitmask bitmask;
            bitmask.Initialize(std::bit_ceil(row_count));
            const u32 block_start_offset = block_id * DEFAULT_BLOCK_CAPACITY;
            const u32 block_end_offset = block_start_offset + row_count;
            if (std::holds_alternative<Vector<u32>>(filter_result)) {
                const Vector<u32> &filter_result_vector = std::get<Vector<u32>>(filter_result);
                const auto it1 = std::lower_bound(filter_result_vector.begin(), filter_result_vector.end(), block_start_offset);
                const auto it2 = std::lower_bound(filter_result_vector.begin(), filter_result_vector.end(), block_end_offset);
                bitmask.SetAllFalse();
                for (auto it = it1; it < it2; ++it) {
                    bitmask.SetTrue(*it - block_start_offset);
                }
            } else {
                u32 u64_start_offset = block_start_offset / 64;
                u32 u64_end_offset = (block_end_offset - 1) / 64;
                if (const u64 *filter_data = std::get<Bitmask>(filter_result).GetData(); filter_data) {
                    bitmask.SetAllFalse();
                    u64 *data = bitmask.GetData();
                    for (u32 i = u64_start_offset; i <= u64_end_offset; ++i) {
                        data[i - u64_start_offset] = filter_data[i];
                    }
                }
            }
            block_entry->SetDeleteBitmask(begin_ts, bitmask);
            auto *block_column_entry = block_entry->GetColumnBlockEntry(search_column_id);
            auto column_vector = block_column_entry->GetColumnVector(buffer_mgr);
            auto tensor_ptr = reinterpret_cast<const TensorT *>(column_vector.data());
            FixHeapManager *fix_heap_mgr = column_vector.buffer_->fix_heap_mgr_.get();
            // query tensor
            const char *query_tensor_ptr = match_tensor_expr_->query_embedding_.ptr;
            const u32 query_embedding_num = match_tensor_expr_->num_of_embedding_in_query_tensor_;
            const u32 basic_embedding_dimension = match_tensor_expr_->tensor_basic_embedding_dimension_;
            const u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
            for (u32 i = 0; i < row_count; ++i) {
                if (bitmask.IsTrue(i)) {
                    const auto [embedding_num, chunk_id, chunk_offset] = tensor_ptr[i];
                    const char *tensor_data_ptr = fix_heap_mgr->GetRawPtrFromChunk(chunk_id, chunk_offset);
                    // TODO: now only support MaxSim on float32 tensor (check Init())
                    // use mlas
                    auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * embedding_num);
                    matrixA_multiply_transpose_matrixB_output_to_C(reinterpret_cast<const float *>(query_tensor_ptr),
                                                                   reinterpret_cast<const float *>(tensor_data_ptr),
                                                                   query_embedding_num,
                                                                   embedding_num,
                                                                   basic_embedding_dimension,
                                                                   output_ptr.get());
                    float maxsim_score = 0.0f;
                    for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
                        const float *query_ip_ptr = output_ptr.get() + query_i * embedding_num;
                        float max_score = std::numeric_limits<float>::lowest();
                        for (u32 k = 0; k < embedding_num; ++k) {
                            max_score = std::max(max_score, query_ip_ptr[k]);
                        }
                        maxsim_score += max_score;
                    }
                    function_data.result_handler_->AddResult(0, maxsim_score, RowID(segment_id, segment_offset_start + i));
                }
            }
        }
    }
    if (block_ids_idx >= block_ids.size()) {
        LOG_TRACE(fmt::format("MatchTensorScan: {} task finished", block_ids_idx));
        // all task Complete
        const u32 result_n = function_data.End();
        const auto output_type_ptr = GetOutputTypes();
        {
            // prepare output data block
            const u32 total_data_row_count = result_n;
            u32 row_idx = 0;
            do {
                auto data_block = DataBlock::MakeUniquePtr();
                data_block->Init(*output_type_ptr);
                operator_state->data_block_array_.emplace_back(std::move(data_block));
                row_idx += DEFAULT_BLOCK_CAPACITY;
            } while (row_idx < total_data_row_count);
        }
        u32 output_block_row_id = 0;
        u32 output_block_idx = 0;
        DataBlock *output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
        const float *result_scores = function_data.score_result_.get();
        const RowID *result_row_ids = function_data.row_id_result_.get();
        for (u32 top_idx = 0; top_idx < result_n; ++top_idx) {
            const SegmentID segment_id = result_row_ids[top_idx].segment_id_;
            const SegmentOffset segment_offset = result_row_ids[top_idx].segment_offset_;
            const BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
            const BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
            BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
            if (block_entry == nullptr) {
                String error_message = fmt::format("Cannot find segment id: {}, block id: {}", segment_id, block_id);
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
            if (output_block_row_id == DEFAULT_BLOCK_CAPACITY) {
                output_block_ptr->Finalize();
                ++output_block_idx;
                output_block_ptr = operator_state->data_block_array_[output_block_idx].get();
                output_block_row_id = 0;
            }
            const SizeT column_n = base_table_ref_->column_ids_.size();
            for (SizeT i = 0; i < column_n; ++i) {
                const auto column_id = base_table_ref_->column_ids_[i];
                auto *block_column_entry = block_entry->GetColumnBlockEntry(column_id);
                auto column_vector = block_column_entry->GetColumnVector(buffer_mgr);
                output_block_ptr->column_vectors[i]->AppendWith(column_vector, block_offset, 1);
            }
            output_block_ptr->AppendValueByPtr(column_n, (ptr_t)&result_scores[top_idx]);
            output_block_ptr->AppendValueByPtr(column_n + 1, (ptr_t)&result_row_ids[top_idx]);
            ++output_block_row_id;
        }
        output_block_ptr->Finalize();
        operator_state->SetComplete();
    }
}

} // namespace infinity
