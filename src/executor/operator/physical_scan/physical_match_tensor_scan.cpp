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
import column_expression;
import knn_expr;
import create_index_info;
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
import segment_index_entry;
import chunk_index_entry;
import emvb_index_in_mem;
import emvb_index;
import knn_filter;
import global_block_id;
import block_index;
import column_def;
import internal_types;
import fix_heap;
import type_info;
import embedding_info;
import buffer_manager;
import buffer_handle;
import match_tensor_scan_function_data;
import mlas_matrix_multiply;
import physical_fusion;
import filter_value_type_classification;

namespace infinity {

PhysicalMatchTensorScan::PhysicalMatchTensorScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchTensorExpression> match_tensor_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 u32 topn,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalFilterScanBase(id, PhysicalOperatorType::kMatchTensorScan, nullptr, nullptr, base_table_ref, common_query_filter, load_metas),
      table_index_(table_index), match_tensor_expr_(std::move(match_tensor_expression)), topn_(topn) {
    search_column_id_ = std::numeric_limits<ColumnID>::max();
}

void PhysicalMatchTensorScan::Init() {}

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

ColumnID PhysicalMatchTensorScan::SearchColumnID() const {
    if (search_column_id_ == std::numeric_limits<ColumnID>::max()) {
        String error_message = "Search column id is not set. Init() error.";
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
    return search_column_id_;
}

void PhysicalMatchTensorScan::CheckColumn() {
    search_column_id_ = match_tensor_expr_->column_expr_->binding().column_idx;
    const ColumnDef *column_def = base_table_ref_->table_entry_ptr_->GetColumnDefByID(search_column_id_);
    const auto &column_type_ptr = column_def->type();
    if (const auto l_type = column_type_ptr->type(); l_type != LogicalType::kTensor and l_type != LogicalType::kTensorArray) {
        String error_message = fmt::format("Column {} is not a tensor or tensorarray column", column_def->name());
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
        String error_message =
            fmt::format("Column {} embedding dimension not match with query {}", column_def->name(), match_tensor_expr_->ToString());
        LOG_CRITICAL(error_message);
        UnrecoverableError(error_message);
    }
}

void PhysicalMatchTensorScan::PlanWithIndex(QueryContext *query_context) {
    Txn *txn = query_context->GetTxn();
    const TransactionID txn_id = txn->TxnID();
    const TxnTimeStamp begin_ts = txn->BeginTS();
    const auto search_column_id = SearchColumnID();

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    Map<u32, SharedPtr<SegmentIndexEntry>> index_entry_map;

    for (auto map_guard = table_entry->IndexMetaMap(); auto &[index_name, table_index_meta] : *map_guard) {
        auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
        if (!status.ok()) {
            // Table index entry isn't found
            LOG_ERROR(status.message());
            continue;
        }
        if (const String column_name = table_index_entry->index_base()->column_name();
            table_entry->GetColumnIdByName(column_name) != search_column_id) {
            // search_column_id isn't in this table index
            continue;
        }
        // check index type
        if (auto index_type = table_index_entry->index_base()->index_type_; index_type != IndexType::kEMVB) {
            // Skip non-EMVB index
            continue;
        }
        // Fill the segment with index
        index_entry_map = table_index_entry->index_by_segment();
        // found one index
        break;
    }

    // Generate task set: index segment and no index block
    for (BlockIndex *block_index = base_table_ref_->block_index_.get(); const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        if (auto iter = index_entry_map.find(segment_id); iter != index_entry_map.end()) {
            index_entries_.emplace_back(iter->second.get());
        } else {
            for (const auto &block_map = segment_info.block_map_; const auto *block_entry : block_map) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(search_column_id);
                block_column_entries_.emplace_back(block_column_entry);
            }
        }
    }
    LOG_TRACE(fmt::format("MatchTensorScan: brute force task: {}, index task: {}", block_column_entries_.size(), index_entries_.size()));
}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalMatchTensorScan::PlanBlockEntries(i64 parallel_count) const {
    UnrecoverableError("PhysicalMatchTensorScan:: use PlanWithIndex instead of PlanBlockEntries!");
    return {};
}

SizeT PhysicalMatchTensorScan::TaskletCount() { return block_column_entries_.size() + index_entries_.size(); }

bool PhysicalMatchTensorScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *match_tensor_scan_operator_state = static_cast<MatchTensorScanOperatorState *>(operator_state);
    ExecuteInner(query_context, match_tensor_scan_operator_state);
    return true;
}

void CalculateScoreOnColumnVector(ColumnVector &column_vector,
                                  SegmentID segment_id,
                                  BlockID block_id,
                                  u32 start_offset,
                                  u32 row_count,
                                  const Bitmask &bitmask,
                                  const MatchTensorExpression &match_tensor_expr,
                                  MatchTensorScanFunctionData &function_data);

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
    const BlockIndex *block_index = base_table_ref_->block_index_.get();
    if (const u32 task_job_index = task_executed_++; task_job_index < index_entries_.size()) {
        // use index
        auto *index_entry = index_entries_[task_job_index];
        const auto segment_id = index_entry->segment_id();
        SegmentEntry *segment_entry = nullptr;
        const auto &segment_index_hashmap = base_table_ref_->block_index_->segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            String error_message = fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id);
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        } else {
            segment_entry = iter->second.segment_entry_;
        }
        if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
            LOG_TRACE(fmt::format("MatchTensorScan: index {}/{} not skipped after common_query_filter", task_job_index, index_entries_.size()));
            const auto segment_row_count = segment_entry->row_count();
            Bitmask segment_bitmask;
            const std::variant<Vector<u32>, Bitmask> &filter_result = it->second;
            if (std::holds_alternative<Vector<u32>>(filter_result)) {
                const Vector<u32> &filter_result_vector = std::get<Vector<u32>>(filter_result);
                segment_bitmask.Initialize(std::ceil(segment_row_count));
                segment_bitmask.SetAllFalse();
                for (u32 row_id : filter_result_vector) {
                    segment_bitmask.SetTrue(row_id);
                }
            } else {
                segment_bitmask.ShallowCopy(std::get<Bitmask>(filter_result));
            }
            // TODO: now only have EMVB index
            const Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<EMVBIndexInMem>> emvb_snapshot = index_entry->GetEMVBIndexSnapshot();
            // 1. in mem index
            {
                const EMVBIndexInMem *emvb_index_in_mem = std::get<1>(emvb_snapshot).get();
                // TODO: fix the parameters
                const auto result = emvb_index_in_mem->SearchWithBitmask(reinterpret_cast<const f32 *>(match_tensor_expr_->query_embedding_.ptr),
                                                                         match_tensor_expr_->num_of_embedding_in_query_tensor_,
                                                                         topn_,
                                                                         segment_bitmask,
                                                                         segment_entry,
                                                                         block_index,
                                                                         begin_ts,
                                                                         2,
                                                                         0.4f,
                                                                         topn_ * 10,
                                                                         topn_ * 5,
                                                                         0.5f);
                std::visit(Overload{[segment_id, &function_data](const Tuple<u32, UniquePtr<f32[]>, UniquePtr<u32[]>> &index_result) {
                                        const auto &[result_num, score_ptr, row_id_ptr] = index_result;
                                        for (u32 i = 0; i < result_num; ++i) {
                                            function_data.result_handler_->AddResult(0, score_ptr[i], RowID(segment_id, row_id_ptr[i]));
                                        }
                                    },
                                    [this, buffer_mgr, begin_ts, segment_id, &segment_entry, &function_data](const Pair<u32, u32> &in_mem_result) {
                                        const auto &[start_offset, total_row_count] = in_mem_result;
                                        BlockID block_id = start_offset / DEFAULT_BLOCK_CAPACITY;
                                        BlockOffset block_offset = start_offset % DEFAULT_BLOCK_CAPACITY;
                                        u32 row_leftover = total_row_count;
                                        BlocksGuard block_guard = segment_entry->GetBlocksGuard();
                                        do {
                                            const u32 row_to_read = std::min<u32>(row_leftover, DEFAULT_BLOCK_CAPACITY - block_offset);
                                            Bitmask block_bitmask;
                                            if (this->CalculateFilterBitmask(segment_id, block_id, row_to_read, block_bitmask)) {
                                                const BlockEntry *block_entry = block_guard.block_entries_[block_id].get();
                                                block_entry->SetDeleteBitmask(begin_ts, block_bitmask);
                                                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(this->search_column_id_);
                                                auto column_vector = block_column_entry->GetColumnVector(buffer_mgr);
                                                // output score will always be float type
                                                CalculateScoreOnColumnVector(column_vector,
                                                                             segment_id,
                                                                             block_id,
                                                                             block_offset,
                                                                             row_to_read,
                                                                             block_bitmask,
                                                                             *(this->match_tensor_expr_),
                                                                             function_data);
                                            }
                                            // prepare next block
                                            row_leftover -= row_to_read;
                                            ++block_id;
                                            block_offset = 0;
                                        } while (row_leftover);
                                    }},
                           result);
            }
            // 2. chunk index
            for (const auto &chunk_index_entry : std::get<0>(emvb_snapshot)) {
                if (chunk_index_entry->CheckVisible(txn)) {
                    const BufferHandle index_handle = chunk_index_entry->GetIndex();
                    const auto *emvb_index = static_cast<const EMVBIndex *>(index_handle.GetData());
                    // TODO: fix the parameters
                    const auto [result_num, score_ptr, row_id_ptr] =
                        emvb_index->SearchWithBitmask(reinterpret_cast<const f32 *>(match_tensor_expr_->query_embedding_.ptr),
                                                      match_tensor_expr_->num_of_embedding_in_query_tensor_,
                                                      topn_,
                                                      segment_bitmask,
                                                      segment_entry,
                                                      block_index,
                                                      begin_ts,
                                                      2,
                                                      0.4f,
                                                      topn_ * 10,
                                                      topn_ * 5,
                                                      0.5f);
                    for (u32 i = 0; i < result_num; ++i) {
                        function_data.result_handler_->AddResult(0, score_ptr[i], RowID(segment_id, row_id_ptr[i]));
                    }
                }
            }
        }
    } else if (const u32 task_job_block = task_job_index - index_entries_.size(); task_job_block < block_column_entries_.size()) {
        auto *block_column_entry = block_column_entries_[task_job_block];
        const BlockEntry *block_entry = block_column_entry->GetBlockEntry();
        const BlockID block_id = block_entry->block_id();
        const SegmentEntry *segment_entry = block_entry->GetSegmentEntry();
        const SegmentID segment_id = segment_entry->segment_id();
        BlockOffset row_count = block_index->GetBlockOffset(segment_id, block_id);
        Bitmask bitmask;
        if (this->CalculateFilterBitmask(segment_id, block_id, row_count, bitmask)) {
            block_entry->SetDeleteBitmask(begin_ts, bitmask);
            auto column_vector = block_column_entry->GetColumnVector(buffer_mgr);
            // output score will always be float type
            CalculateScoreOnColumnVector(column_vector, segment_id, block_id, 0, row_count, bitmask, *match_tensor_expr_, function_data);
        }
    } else {
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

template <typename TensorElemT, typename QueryElemT>
struct MaxSimOp;

template <>
struct MaxSimOp<float, float> {
    static float Score(const char *query_tensor_ptr,
                       const char *target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * target_embedding_num);
        matrixA_multiply_transpose_matrixB_output_to_C(reinterpret_cast<const float *>(query_tensor_ptr),
                                                       reinterpret_cast<const float *>(target_tensor_ptr),
                                                       query_embedding_num,
                                                       target_embedding_num,
                                                       basic_embedding_dimension,
                                                       output_ptr.get());
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const float *query_ip_ptr = output_ptr.get() + query_i * target_embedding_num;
            float max_score_i = std::numeric_limits<float>::lowest();
            for (u32 k = 0; k < target_embedding_num; ++k) {
                max_score_i = std::max(max_score_i, query_ip_ptr[k]);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

template <typename TensorElemT, typename QueryElemT>
    requires(!std::is_same_v<TensorElemT, bool> && !std::is_same_v<QueryElemT, bool>)
struct MaxSimOp<TensorElemT, QueryElemT> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto query_tensor_ptr = reinterpret_cast<const QueryElemT *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const TensorElemT *>(raw_target_tensor_ptr);
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * target_embedding_num);
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            float max_score_i = std::numeric_limits<float>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
                const auto target_ptr = target_tensor_ptr + target_j * basic_embedding_dimension;
                float score_ij = 0.0f;
                for (u32 k = 0; k < basic_embedding_dimension; ++k) {
                    score_ij += static_cast<float>(query_ptr[k]) * static_cast<float>(target_ptr[k]);
                }
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

template <>
struct MaxSimOp<bool, bool> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto query_tensor_ptr = reinterpret_cast<const u8 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * target_embedding_num);
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            u32 max_score_i = 0;
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto query_ptr = query_tensor_ptr + query_i * unit_embedding_bytes;
                const auto target_ptr = target_tensor_ptr + target_j * unit_embedding_bytes;
                u32 score_ij = 0;
                auto is_true = [](const u8 *ptr, const u32 k) -> bool { return ptr[k / 8] & (1u << (k % 8)); };
                for (u32 k = 0; k < basic_embedding_dimension; ++k) {
                    if (is_true(query_ptr, k) && is_true(target_ptr, k)) {
                        ++score_ij;
                    }
                }
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

template <typename QueryElemT>
struct MaxSimOp<bool, QueryElemT> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto query_tensor_ptr = reinterpret_cast<const QueryElemT *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * target_embedding_num);
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            float max_score_i = std::numeric_limits<float>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
                const auto target_ptr = target_tensor_ptr + target_j * unit_embedding_bytes;
                float score_ij = 0.0f;
                auto is_true = [](const u8 *ptr, const u32 k) -> bool { return ptr[k / 8] & (1u << (k % 8)); };
                for (u32 k = 0; k < basic_embedding_dimension; ++k) {
                    if (is_true(target_ptr, k)) {
                        score_ij += static_cast<float>(query_ptr[k]);
                    }
                }
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

template <typename TensorElemT>
struct MaxSimOp<TensorElemT, bool> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto query_tensor_ptr = reinterpret_cast<const u8 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const TensorElemT *>(raw_target_tensor_ptr);
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * target_embedding_num);
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            float max_score_i = std::numeric_limits<float>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto query_ptr = query_tensor_ptr + query_i * unit_embedding_bytes;
                const auto target_ptr = target_tensor_ptr + target_j * basic_embedding_dimension;
                float score_ij = 0.0f;
                auto is_true = [](const u8 *ptr, const u32 k) -> bool { return ptr[k / 8] & (1u << (k % 8)); };
                for (u32 k = 0; k < basic_embedding_dimension; ++k) {
                    if (is_true(query_ptr, k)) {
                        score_ij += static_cast<float>(target_ptr[k]);
                    }
                }
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

template <typename Op>
struct CalcutateScoreOfTensorRow {
    static float Execute(ColumnVector &column_vector,
                         const u32 block_offset,
                         const char *query_tensor_ptr,
                         const u32 query_embedding_num,
                         const u32 basic_embedding_dimension) {
        auto tensor_ptr = reinterpret_cast<const TensorT *>(column_vector.data());
        FixHeapManager *tensor_heap_mgr = column_vector.buffer_->fix_heap_mgr_.get();
        const auto [embedding_num, chunk_id, chunk_offset] = tensor_ptr[block_offset];
        const char *tensor_data_ptr = tensor_heap_mgr->GetRawPtrFromChunk(chunk_id, chunk_offset);
        return Op::Score(query_tensor_ptr, tensor_data_ptr, query_embedding_num, embedding_num, basic_embedding_dimension);
    }
};

template <typename Op>
struct CalcutateScoreOfTensorArrayRow {
    static float Execute(ColumnVector &column_vector,
                         const u32 block_offset,
                         const char *query_tensor_ptr,
                         const u32 query_embedding_num,
                         const u32 basic_embedding_dimension) {
        auto tensor_array_ptr = reinterpret_cast<const TensorArrayT *>(column_vector.data());
        FixHeapManager *tensor_array_heap_mgr = column_vector.buffer_->fix_heap_mgr_.get();
        FixHeapManager *tensor_heap_mgr = column_vector.buffer_->fix_heap_mgr_1_.get();
        const auto [tensor_num, tensor_array_chunk_id, tensor_array_chunk_offset] = tensor_array_ptr[block_offset];
        Vector<TensorT> tensors(tensor_num);
        tensor_array_heap_mgr->ReadFromHeap(reinterpret_cast<char *>(tensors.data()),
                                            tensor_array_chunk_id,
                                            tensor_array_chunk_offset,
                                            tensor_num * sizeof(TensorT));
        float maxsim_score = std::numeric_limits<float>::lowest();
        for (const auto [embedding_num, tensor_chunk_id, tensor_chunk_offset] : tensors) {
            const char *tensor_data_ptr = tensor_heap_mgr->GetRawPtrFromChunk(tensor_chunk_id, tensor_chunk_offset);
            const float tensor_score = Op::Score(query_tensor_ptr, tensor_data_ptr, query_embedding_num, embedding_num, basic_embedding_dimension);
            maxsim_score = std::max(maxsim_score, tensor_score);
        }
        return maxsim_score;
    }
};

template <typename CalcutateScoreOfRowOp>
void ExecuteScanOnColumn(ColumnVector &column_vector,
                         const SegmentID segment_id,
                         const BlockID block_id,
                         const u32 start_block_offset,
                         const u32 row_count,
                         const Bitmask &bitmask,
                         const MatchTensorExpression &match_tensor_expr,
                         MatchTensorScanFunctionData &function_data) {
    const char *query_tensor_ptr = match_tensor_expr.query_embedding_.ptr;
    const u32 query_embedding_num = match_tensor_expr.num_of_embedding_in_query_tensor_;
    const u32 basic_embedding_dimension = match_tensor_expr.tensor_basic_embedding_dimension_;
    const u32 segment_offset_start = block_id * DEFAULT_BLOCK_CAPACITY;
    const u32 end_block_offset = start_block_offset + row_count;
    for (u32 i = start_block_offset; i < end_block_offset; ++i) {
        if (bitmask.IsTrue(i)) {
            const float maxsim_score =
                CalcutateScoreOfRowOp::Execute(column_vector, i, query_tensor_ptr, query_embedding_num, basic_embedding_dimension);
            function_data.result_handler_->AddResult(0, maxsim_score, RowID(segment_id, segment_offset_start + i));
        }
    }
}

struct TensorScanParameterPack {
    ColumnVector &column_vector_;
    const SegmentID segment_id_;
    const BlockID block_id_;
    const u32 start_block_offset_;
    const u32 row_count_;
    const Bitmask &bitmask_;
    const MatchTensorExpression &match_tensor_expr_;
    MatchTensorScanFunctionData &function_data_;
    TensorScanParameterPack(ColumnVector &column_vector,
                            const SegmentID segment_id,
                            const BlockID block_id,
                            const u32 start_block_offset,
                            const u32 row_count,
                            const Bitmask &bitmask,
                            const MatchTensorExpression &match_tensor_expr,
                            MatchTensorScanFunctionData &function_data)
        : column_vector_(column_vector), segment_id_(segment_id), block_id_(block_id), start_block_offset_(start_block_offset), row_count_(row_count),
          bitmask_(bitmask), match_tensor_expr_(match_tensor_expr), function_data_(function_data) {}
};

template <template <typename> typename CalcutateScoreOfRow, typename ColumnElemT, typename QueryElemT>
void CalculateScoreOnColumnVectorT(TensorScanParameterPack &parameter_pack) {
    switch (parameter_pack.match_tensor_expr_.search_method_) {
        case MatchTensorSearchMethod::kMaxSim: {
            return ExecuteScanOnColumn<CalcutateScoreOfRow<MaxSimOp<ColumnElemT, QueryElemT>>>(parameter_pack.column_vector_,
                                                                                               parameter_pack.segment_id_,
                                                                                               parameter_pack.block_id_,
                                                                                               parameter_pack.start_block_offset_,
                                                                                               parameter_pack.row_count_,
                                                                                               parameter_pack.bitmask_,
                                                                                               parameter_pack.match_tensor_expr_,
                                                                                               parameter_pack.function_data_);
        }
        case MatchTensorSearchMethod::kInvalid: {
            const auto error_message = "Invalid search method!";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
    }
}

template <typename... T>
struct ExecuteMatchTensorScanTypes {
    static void Execute(TensorScanParameterPack &parameter_pack) {
        switch (parameter_pack.column_vector_.data_type()->type()) {
            case LogicalType::kTensor: {
                return CalculateScoreOnColumnVectorT<CalcutateScoreOfTensorRow, T...>(parameter_pack);
            }
            case LogicalType::kTensorArray: {
                return CalculateScoreOnColumnVectorT<CalcutateScoreOfTensorArrayRow, T...>(parameter_pack);
            }
            default: {
                const auto error_message = "Invalid column type! target column is not Tensor or TensorArray type.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
        }
    }
};

template <template <typename...> typename T, typename U>
struct ExecuteHelper;

template <template <typename...> typename ExecuteT, typename... T>
struct ExecuteHelper<ExecuteT, TypeList<T...>> {
    template <typename Params>
    static void Execute(Params &parameter_pack) {
        ExecuteT<T...>::Execute(parameter_pack);
    }
};

template <template <typename...> typename ExecuteT, typename Typelist, typename Params>
void ElemTypeDispatch(Params &parameter_pack) {
    ExecuteHelper<ExecuteT, Typelist>::Execute(parameter_pack);
}

template <template <typename...> typename ExecuteT, typename Typelist, typename Params, typename... Args>
void ElemTypeDispatch(Params &parameter_pack, EmbeddingDataType type_enum, Args... extra_types) {
    switch (type_enum) {
        case EmbeddingDataType::kElemBit: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<bool>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemInt8: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<i8>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemInt16: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<i16>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemInt32: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<i32>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemInt64: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<i64>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemFloat: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<float>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemDouble: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<double>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemInvalid: {
            const auto error_message = "Invalid embedding data type!";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
        }
    }
}

void CalculateScoreOnColumnVector(ColumnVector &column_vector,
                                  const SegmentID segment_id,
                                  const BlockID block_id,
                                  const u32 start_offset,
                                  const u32 row_count,
                                  const Bitmask &bitmask,
                                  const MatchTensorExpression &match_tensor_expr,
                                  MatchTensorScanFunctionData &function_data) {
    TensorScanParameterPack parameter_pack(column_vector, segment_id, block_id, start_offset, row_count, bitmask, match_tensor_expr, function_data);
    auto column_elem_type = static_cast<const EmbeddingInfo *>(parameter_pack.column_vector_.data_type()->type_info().get())->Type();
    auto query_elem_type = parameter_pack.match_tensor_expr_.embedding_data_type_;
    ElemTypeDispatch<ExecuteMatchTensorScanTypes, TypeList<>>(parameter_pack, column_elem_type, query_elem_type);
}

struct RerankerParameterPack {
    Vector<MatchTensorRerankDoc> &rerank_docs_;
    BufferManager *buffer_mgr_;
    const DataType *column_data_type_;
    const ColumnID column_id_;
    const BlockIndex *block_index_;
    const MatchTensorExpression &match_tensor_expr_;
    RerankerParameterPack(Vector<MatchTensorRerankDoc> &rerank_docs,
                          BufferManager *buffer_mgr,
                          const DataType *column_data_type,
                          const ColumnID column_id,
                          const BlockIndex *block_index,
                          const MatchTensorExpression &match_tensor_expr)
        : rerank_docs_(rerank_docs), buffer_mgr_(buffer_mgr), column_data_type_(column_data_type), column_id_(column_id), block_index_(block_index),
          match_tensor_expr_(match_tensor_expr) {}
};

template <typename CalcutateScoreOfRowOp>
void GetRerankerScore(Vector<MatchTensorRerankDoc> &rerank_docs,
                      BufferManager *buffer_mgr,
                      const ColumnID column_id,
                      const BlockIndex *block_index,
                      const char *query_tensor_ptr,
                      const u32 query_embedding_num,
                      const u32 basic_embedding_dimension) {
    for (auto &doc : rerank_docs) {
        const RowID row_id = doc.row_id_;
        const SegmentID segment_id = row_id.segment_id_;
        const SegmentOffset segment_offset = row_id.segment_offset_;
        const BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
        const BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
        BlockColumnEntry *block_column_entry =
            block_index->segment_block_index_.at(segment_id).block_map_.at(block_id)->GetColumnBlockEntry(column_id);
        auto column_vec = block_column_entry->GetColumnVector(buffer_mgr);
        doc.score_ = CalcutateScoreOfRowOp::Execute(column_vec, block_offset, query_tensor_ptr, query_embedding_num, basic_embedding_dimension);
    }
}

template <template <typename> typename CalcutateScoreOfRow, typename ColumnElemT, typename QueryElemT>
void RerankerScoreT(RerankerParameterPack &parameter_pack) {
    const char *query_tensor_ptr = parameter_pack.match_tensor_expr_.query_embedding_.ptr;
    const u32 query_embedding_num = parameter_pack.match_tensor_expr_.num_of_embedding_in_query_tensor_;
    const u32 basic_embedding_dimension = parameter_pack.match_tensor_expr_.tensor_basic_embedding_dimension_;
    switch (parameter_pack.match_tensor_expr_.search_method_) {
        case MatchTensorSearchMethod::kMaxSim: {
            return GetRerankerScore<CalcutateScoreOfRow<MaxSimOp<ColumnElemT, QueryElemT>>>(parameter_pack.rerank_docs_,
                                                                                            parameter_pack.buffer_mgr_,
                                                                                            parameter_pack.column_id_,
                                                                                            parameter_pack.block_index_,
                                                                                            query_tensor_ptr,
                                                                                            query_embedding_num,
                                                                                            basic_embedding_dimension);
        }
        case MatchTensorSearchMethod::kInvalid: {
            const auto error_message = "Invalid search method!";
            LOG_CRITICAL(error_message);
            UnrecoverableError(error_message);
            break;
        }
    }
}

template <typename... T>
struct ExecuteMatchTensorRerankerTypes {
    static void Execute(RerankerParameterPack &parameter_pack) {
        switch (parameter_pack.column_data_type_->type()) {
            case LogicalType::kTensor: {
                return RerankerScoreT<CalcutateScoreOfTensorRow, T...>(parameter_pack);
            }
            case LogicalType::kTensorArray: {
                return RerankerScoreT<CalcutateScoreOfTensorArrayRow, T...>(parameter_pack);
            }
            default: {
                const auto error_message = "Invalid column type! target column is not Tensor or TensorArray type.";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
        }
    }
};

void CalculateFusionMatchTensorRerankerScores(Vector<MatchTensorRerankDoc> &rerank_docs,
                                              BufferManager *buffer_mgr,
                                              const DataType *column_data_type,
                                              const ColumnID column_id,
                                              const BlockIndex *block_index,
                                              const MatchTensorExpression &match_tensor_expr) {
    RerankerParameterPack parameter_pack(rerank_docs, buffer_mgr, column_data_type, column_id, block_index, match_tensor_expr);
    auto column_elem_type = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get())->Type();
    auto query_elem_type = parameter_pack.match_tensor_expr_.embedding_data_type_;
    ElemTypeDispatch<ExecuteMatchTensorRerankerTypes, TypeList<>>(parameter_pack, column_elem_type, query_elem_type);
}

} // namespace infinity
