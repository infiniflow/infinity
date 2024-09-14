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

#include <bit>
#include <cstdlib>
#include <memory>
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
import roaring_bitmap;
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
import logical_match_tensor_scan;
import simd_functions;
import knn_expression;
import search_options;

namespace infinity {

using AlignedMatchTensorExprHolderT =
    std::pair<std::unique_ptr<void, decltype([](void *ptr) { std::free(ptr); })>, std::unique_ptr<MatchTensorExpression>>;

AlignedMatchTensorExprHolderT GetMatchTensorExprForCalculation(MatchTensorExpression &src_match_tensor_expr, EmbeddingDataType column_embedding_type);

PhysicalMatchTensorScan::PhysicalMatchTensorScan(u64 id,
                                                 u64 table_index,
                                                 SharedPtr<BaseTableRef> base_table_ref,
                                                 SharedPtr<MatchTensorExpression> match_tensor_expression,
                                                 const SharedPtr<CommonQueryFilter> &common_query_filter,
                                                 u32 topn,
                                                 const MatchTensorScanIndexOptions &index_options,
                                                 SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalFilterScanBase(id, PhysicalOperatorType::kMatchTensorScan, nullptr, nullptr, base_table_ref, common_query_filter, load_metas),
      table_index_(table_index), src_match_tensor_expr_(std::move(match_tensor_expression)), topn_(topn), index_options_(index_options) {
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
    result_types->emplace_back(MakeShared<DataType>(src_match_tensor_expr_->Type()));
    result_types->emplace_back(MakeShared<DataType>(LogicalType::kRowID));
    return result_types;
}

ColumnID PhysicalMatchTensorScan::SearchColumnID() const {
    if (search_column_id_ == std::numeric_limits<ColumnID>::max()) {
        String error_message = "Search column id is not set. Init() error.";
        UnrecoverableError(error_message);
    }
    return search_column_id_;
}

void PhysicalMatchTensorScan::CheckColumn() {
    search_column_id_ = src_match_tensor_expr_->column_expr_->binding().column_idx;
    const ColumnDef *column_def = base_table_ref_->table_entry_ptr_->GetColumnDefByIdx(search_column_id_);
    const auto &column_type_ptr = column_def->type();
    if (const auto l_type = column_type_ptr->type(); l_type != LogicalType::kTensor and l_type != LogicalType::kTensorArray) {
        String error_message = fmt::format("Column {} is not a tensor or tensorarray column", column_def->name());
        UnrecoverableError(error_message);
    }
    const auto &type_info = column_type_ptr->type_info();
    if (type_info->type() != TypeInfoType::kEmbedding) {
        String error_message = fmt::format("Column {} is not a tensor column", column_def->name());
        UnrecoverableError(error_message);
    }
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_info.get());
    if (embedding_info->Dimension() != src_match_tensor_expr_->tensor_basic_embedding_dimension_) {
        String error_message =
            fmt::format("Column {} embedding dimension not match with query {}", column_def->name(), src_match_tensor_expr_->ToString());
        UnrecoverableError(error_message);
    }
    // check column basic embedding data type and query embedding data type
    // apply necessary cast
    if (auto [new_search_ptr, new_search_expr] = GetMatchTensorExprForCalculation(*src_match_tensor_expr_, embedding_info->Type()); new_search_ptr) {
        calc_match_tensor_aligned_holder_ = SharedPtr<void>(new_search_ptr.release(), std::free);
        calc_match_tensor_expr_holder_ = std::move(new_search_expr);
        calc_match_tensor_expr_ = calc_match_tensor_expr_holder_.get();
    } else {
        calc_match_tensor_expr_ = src_match_tensor_expr_.get();
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
            // already dropped
            LOG_WARN(status.message());
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
    if (!block_column_entries_.empty()) {
        // check unused option text
        if (const SearchOptions options(src_match_tensor_expr_->options_text_); options.size() != options.options_.count("topn")) {
            RecoverableError(Status::SyntaxError(fmt::format(R"(Input option text "{}" has unused part.)", src_match_tensor_expr_->options_text_)));
        }
    }
    LOG_TRACE(fmt::format("MatchTensorScan: brute force task: {}, index task: {}", block_column_entries_.size(), index_entries_.size()));
}

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalMatchTensorScan::PlanBlockEntries(i64 parallel_count) const {
    UnrecoverableError("PhysicalMatchTensorScan:: use PlanWithIndex instead of PlanBlockEntries!");
    return {};
}

// TODO: how many threads for brute force scan?
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
        UnrecoverableError(error_message);
    }
    const BlockIndex *block_index = base_table_ref_->block_index_.get();
    if (const u32 task_job_index = task_executed_++; task_job_index < index_entries_.size()) {
        // use index
        auto *index_entry = index_entries_[task_job_index];
        const auto segment_id = index_entry->segment_id();
        SegmentEntry *segment_entry = nullptr;
        SegmentOffset segment_row_count = 0;
        const auto &segment_index_hashmap = base_table_ref_->block_index_->segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            String error_message = fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id);
            UnrecoverableError(error_message);
        } else {
            segment_entry = iter->second.segment_entry_;
            segment_row_count = iter->second.segment_offset_;
        }

        bool has_some_result = false;
        Bitmask segment_bitmask;
        if (common_query_filter_->AlwaysTrue()) {
            has_some_result = true;
            segment_bitmask.SetAllTrue();
        } else {
            if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
                LOG_TRACE(fmt::format("MatchTensorScan: index {}/{} not skipped after common_query_filter", task_job_index, index_entries_.size()));
                segment_bitmask = it->second;
                if (segment_row_count != segment_bitmask.count()) {
                    UnrecoverableError(
                        fmt::format("Segment row count {} not match with bitmask size {}", segment_row_count, segment_bitmask.count()));
                }
                has_some_result = true;
            }
        }

        if (has_some_result) {
            LOG_TRACE(fmt::format("MatchTensorScan: index {}/{} not skipped after common_query_filter", task_job_index, index_entries_.size()));
            // TODO: now only have EMVB index
            const Tuple<Vector<SharedPtr<ChunkIndexEntry>>, SharedPtr<EMVBIndexInMem>> emvb_snapshot = index_entry->GetEMVBIndexSnapshot();
            // 1. in mem index
            if (const EMVBIndexInMem *emvb_index_in_mem = std::get<SharedPtr<EMVBIndexInMem>>(emvb_snapshot).get(); emvb_index_in_mem) {
                // TODO: fix the parameters
                const auto result = emvb_index_in_mem->SearchWithBitmask(reinterpret_cast<const f32 *>(calc_match_tensor_expr_->query_embedding_.ptr),
                                                                         calc_match_tensor_expr_->num_of_embedding_in_query_tensor_,
                                                                         topn_,
                                                                         segment_bitmask,
                                                                         segment_entry,
                                                                         block_index,
                                                                         begin_ts,
                                                                         index_options_.emvb_centroid_nprobe_,
                                                                         index_options_.emvb_threshold_first_,
                                                                         index_options_.emvb_n_doc_to_score_,
                                                                         index_options_.emvb_n_doc_out_second_stage_,
                                                                         index_options_.emvb_threshold_final_);
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
                                                auto column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);
                                                // output score will always be float type
                                                CalculateScoreOnColumnVector(column_vector,
                                                                             segment_id,
                                                                             block_id,
                                                                             block_offset,
                                                                             row_to_read,
                                                                             block_bitmask,
                                                                             *(this->calc_match_tensor_expr_),
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
            for (const auto &chunk_index_entry : std::get<Vector<SharedPtr<ChunkIndexEntry>>>(emvb_snapshot)) {
                if (chunk_index_entry->CheckVisible(txn)) {
                    const BufferHandle index_handle = chunk_index_entry->GetIndex();
                    const auto *emvb_index = static_cast<const EMVBIndex *>(index_handle.GetData());
                    // TODO: fix the parameters
                    const auto [result_num, score_ptr, row_id_ptr] =
                        emvb_index->SearchWithBitmask(reinterpret_cast<const f32 *>(calc_match_tensor_expr_->query_embedding_.ptr),
                                                      calc_match_tensor_expr_->num_of_embedding_in_query_tensor_,
                                                      topn_,
                                                      segment_bitmask,
                                                      segment_entry,
                                                      block_index,
                                                      begin_ts,
                                                      index_options_.emvb_centroid_nprobe_,
                                                      index_options_.emvb_threshold_first_,
                                                      index_options_.emvb_n_doc_to_score_,
                                                      index_options_.emvb_n_doc_out_second_stage_,
                                                      index_options_.emvb_threshold_final_);
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
            auto column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);
            // output score will always be float type
            CalculateScoreOnColumnVector(column_vector, segment_id, block_id, 0, row_count, bitmask, *calc_match_tensor_expr_, function_data);
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
                auto column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);
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

// TensorElemT: bit, QueryElemT: bit (unaligned).
// TensorElemT: bit, QueryElemT: f32, i32, i64 (aligned).
// TensorElemT: u8, QueryElemT: u8 (unaligned).
// TensorElemT: i8, QueryElemT: i8 (unaligned).
// TensorElemT: f32, f64, f16, bf16, QueryElemT: f32 (aligned).
template <typename TensorElemT, typename QueryElemT>
struct MaxSimOp {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        UnrecoverableError("Unreachable code!");
        return {};
    }
};

// TensorElemT: bit, QueryElemT: bit (unaligned)
// TODO: hamming distance?
template <>
struct MaxSimOp<bool, bool> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        if (unit_embedding_bytes % 4 == 0) {
            if (reinterpret_cast<uintptr_t>(raw_query_tensor_ptr) % alignof(u32) == 0 &&
                reinterpret_cast<uintptr_t>(raw_target_tensor_ptr) % alignof(u32) == 0) {
                const auto u32_cnt = unit_embedding_bytes / 4;
                const auto query_tensor_u32_ptr = reinterpret_cast<const u32 *>(raw_query_tensor_ptr);
                const auto target_tensor_u32_ptr = reinterpret_cast<const u32 *>(raw_target_tensor_ptr);
                u32 maxsim_score = 0;
                for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
                    const auto query_ptr = query_tensor_u32_ptr + query_i * u32_cnt;
                    auto min_score_i = std::numeric_limits<u32>::max();
                    for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                        const auto target_ptr = target_tensor_u32_ptr + target_j * u32_cnt;
                        u32 score_ij = 0;
                        for (u32 k = 0; k < u32_cnt; ++k) {
                            score_ij += std::popcount(query_ptr[k] ^ target_ptr[k]);
                        }
                        min_score_i = std::min(min_score_i, score_ij);
                    }
                    maxsim_score += min_score_i;
                }
                // hamming distance, higher score means more different
                return -static_cast<float>(maxsim_score);
            }
            // memory allocated by new char[] should be aligned to any basic type
            LOG_ERROR("MaxSimOp<bool, bool> Score: input tensor is not aligned to int");
        }
        const auto query_tensor_ptr = reinterpret_cast<const u8 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        u32 maxsim_score = 0;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const auto query_ptr = query_tensor_ptr + query_i * unit_embedding_bytes;
            auto min_score_i = std::numeric_limits<u32>::max();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto target_ptr = target_tensor_ptr + target_j * unit_embedding_bytes;
                u32 score_ij = 0;
                for (u32 k = 0; k < unit_embedding_bytes; ++k) {
                    score_ij += std::popcount(static_cast<u32>(query_ptr[k] ^ target_ptr[k]));
                }
                min_score_i = std::min(min_score_i, score_ij);
            }
            maxsim_score += min_score_i;
        }
        // hamming distance, higher score means more different
        return -static_cast<float>(maxsim_score);
    }
};

// TensorElemT: bit, QueryElemT: f32 (aligned)
template <>
struct MaxSimOp<bool, f32> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto f32_bit_ip_func_ptr = GetSIMD_FUNCTIONS().MaxSimF32BitIP_func_ptr_;
        const auto query_tensor_ptr = reinterpret_cast<const f32 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        float maxsim_score = 0.0f;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
            auto max_score_i = std::numeric_limits<float>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto target_ptr = target_tensor_ptr + target_j * unit_embedding_bytes;
                const auto score_ij = f32_bit_ip_func_ptr(query_ptr, target_ptr, basic_embedding_dimension);
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

// TensorElemT: bit, QueryElemT: i32 (aligned)
template <>
struct MaxSimOp<bool, i32> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto i32_bit_ip_func_ptr = GetSIMD_FUNCTIONS().MaxSimI32BitIP_func_ptr_;
        const auto query_tensor_ptr = reinterpret_cast<const i32 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        i32 maxsim_score = 0;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
            auto max_score_i = std::numeric_limits<i32>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto target_ptr = target_tensor_ptr + target_j * unit_embedding_bytes;
                const auto score_ij = i32_bit_ip_func_ptr(query_ptr, target_ptr, basic_embedding_dimension);
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

// TensorElemT: bit, QueryElemT: i64 (aligned)
template <>
struct MaxSimOp<bool, i64> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto i64_bit_ip_func_ptr = GetSIMD_FUNCTIONS().MaxSimI64BitIP_func_ptr_;
        const auto query_tensor_ptr = reinterpret_cast<const i64 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        const auto unit_embedding_bytes = basic_embedding_dimension / 8;
        i64 maxsim_score = 0;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
            auto max_score_i = std::numeric_limits<i64>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto target_ptr = target_tensor_ptr + target_j * unit_embedding_bytes;
                const auto score_ij = i64_bit_ip_func_ptr(query_ptr, target_ptr, basic_embedding_dimension);
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

// TensorElemT: u8, QueryElemT: u8 (unaligned)
template <>
struct MaxSimOp<u8, u8> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto u8ip_func_ptr = GetSIMD_FUNCTIONS().HNSW_U8IP_ptr_;
        const auto query_tensor_ptr = reinterpret_cast<const u8 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const u8 *>(raw_target_tensor_ptr);
        i32 maxsim_score = 0;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
            auto max_score_i = std::numeric_limits<i32>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto target_ptr = target_tensor_ptr + target_j * basic_embedding_dimension;
                const i32 score_ij = u8ip_func_ptr(query_ptr, target_ptr, basic_embedding_dimension);
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

// TensorElemT: i8, QueryElemT: i8 (unaligned)
template <>
struct MaxSimOp<i8, i8> {
    static float Score(const char *raw_query_tensor_ptr,
                       const char *raw_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        const auto i8ip_func_ptr = GetSIMD_FUNCTIONS().HNSW_I8IP_ptr_;
        const auto query_tensor_ptr = reinterpret_cast<const i8 *>(raw_query_tensor_ptr);
        const auto target_tensor_ptr = reinterpret_cast<const i8 *>(raw_target_tensor_ptr);
        i32 maxsim_score = 0;
        for (u32 query_i = 0; query_i < query_embedding_num; ++query_i) {
            const auto query_ptr = query_tensor_ptr + query_i * basic_embedding_dimension;
            auto max_score_i = std::numeric_limits<i32>::lowest();
            for (u32 target_j = 0; target_j < target_embedding_num; ++target_j) {
                const auto target_ptr = target_tensor_ptr + target_j * basic_embedding_dimension;
                const i32 score_ij = i8ip_func_ptr(query_ptr, target_ptr, basic_embedding_dimension);
                max_score_i = std::max(max_score_i, score_ij);
            }
            maxsim_score += max_score_i;
        }
        return maxsim_score;
    }
};

// TensorElemT: f32, QueryElemT: f32 (aligned)
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

// TensorElemT: f64, f16, bf16, QueryElemT: f32 (aligned)
template <typename TensorElemT>
    requires(IsAnyOf<TensorElemT, f64, Float16T, BFloat16T>)
struct MaxSimOp<TensorElemT, float> {
    static float Score(const char *query_tensor_ptr,
                       const char *src_target_tensor_ptr,
                       const u32 query_embedding_num,
                       const u32 target_embedding_num,
                       const u32 basic_embedding_dimension) {
        auto src_target_type_ptr = reinterpret_cast<const TensorElemT *>(src_target_tensor_ptr);
        auto target_buffer = MakeUniqueForOverwrite<float[]>(basic_embedding_dimension * target_embedding_num);
        for (u32 i = 0; i < basic_embedding_dimension * target_embedding_num; ++i) {
            target_buffer[i] = static_cast<float>(src_target_type_ptr[i]);
        }
        auto output_ptr = MakeUniqueForOverwrite<float[]>(query_embedding_num * target_embedding_num);
        matrixA_multiply_transpose_matrixB_output_to_C(reinterpret_cast<const float *>(query_tensor_ptr),
                                                       target_buffer.get(),
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

template <typename Op>
struct CalcutateScoreOfTensorRow {
    static float Execute(ColumnVector &column_vector,
                         const u32 block_offset,
                         const char *query_tensor_ptr,
                         const u32 query_embedding_num,
                         const u32 basic_embedding_dimension) {
        const auto [raw_data, embedding_num] = column_vector.GetTensorRaw(block_offset);
        return Op::Score(query_tensor_ptr, raw_data.data(), query_embedding_num, embedding_num, basic_embedding_dimension);
    }
};

template <typename Op>
struct CalcutateScoreOfTensorArrayRow {
    static float Execute(ColumnVector &column_vector,
                         const u32 block_offset,
                         const char *query_tensor_ptr,
                         const u32 query_embedding_num,
                         const u32 basic_embedding_dimension) {
        float maxsim_score = std::numeric_limits<float>::lowest();
        Vector<Pair<Span<const char>, SizeT>> tensor_array = column_vector.GetTensorArrayRaw(block_offset);
        for (const auto &[raw_data, embedding_num] : tensor_array) {
            const float tensor_score = Op::Score(query_tensor_ptr, raw_data.data(), query_embedding_num, embedding_num, basic_embedding_dimension);
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
        case EmbeddingDataType::kElemUInt8: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<u8>>>(parameter_pack, extra_types...);
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
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<f32>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemDouble: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<f64>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemFloat16: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<Float16T>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemBFloat16: {
            return ElemTypeDispatch<ExecuteT, AddTypeList<Typelist, TypeList<BFloat16T>>>(parameter_pack, extra_types...);
        }
        case EmbeddingDataType::kElemInvalid: {
            const auto error_message = "Invalid embedding data type!";
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
        auto column_vec = block_column_entry->GetConstColumnVector(buffer_mgr);
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
                                              MatchTensorExpression &src_match_tensor_expr) {
    const auto column_elem_type = static_cast<const EmbeddingInfo *>(column_data_type->type_info().get())->Type();
    const auto [new_search_ptr, new_search_expr] = GetMatchTensorExprForCalculation(src_match_tensor_expr, column_elem_type);
    const auto *match_tensor_expr_ptr = new_search_expr ? new_search_expr.get() : &src_match_tensor_expr;
    RerankerParameterPack parameter_pack(rerank_docs, buffer_mgr, column_data_type, column_id, block_index, *match_tensor_expr_ptr);
    const auto query_elem_type = parameter_pack.match_tensor_expr_.embedding_data_type_;
    ElemTypeDispatch<ExecuteMatchTensorRerankerTypes, TypeList<>>(parameter_pack, column_elem_type, query_elem_type);
}

// For AVX512
inline auto GetAVX512AlignedMemory(const SizeT bytes) {
    const auto alloc_bytes = ((bytes + 63u) & (~static_cast<SizeT>(63))) + 128u; // need to be multiple of 64, with extra 128 bytes
    auto ptr = std::aligned_alloc(64, alloc_bytes);
    if (!ptr) {
        UnrecoverableError("Out of memory!");
    }
    return ptr;
}

struct ExecuteCast512AlignedParamPack {
    char *src_embedding_ptr = nullptr;
    u32 dimension = 0;
    void *result_ptr = nullptr;
    ExecuteCast512AlignedParamPack() = default;
    ExecuteCast512AlignedParamPack(const ExecuteCast512AlignedParamPack &) = delete;
    ExecuteCast512AlignedParamPack(ExecuteCast512AlignedParamPack &&) = delete;
    ExecuteCast512AlignedParamPack &operator=(const ExecuteCast512AlignedParamPack &) = delete;
    ExecuteCast512AlignedParamPack &operator=(ExecuteCast512AlignedParamPack &&) = delete;
};

template <typename SRC_T, typename DST_T>
struct ExecuteCast512Aligned {
    static void Execute(ExecuteCast512AlignedParamPack &param) { UnrecoverableError("Unreachable code!"); }
};

template <typename SRC_T, typename DST_T>
    requires((std::is_same_v<DST_T, i32> && IsAnyOf<SRC_T, u8, i8, i16, i32>) || (std::is_same_v<DST_T, i64> && IsAnyOf<SRC_T, i64>) ||
             (std::is_same_v<DST_T, f32> && IsAnyOf<SRC_T, f32, f64, Float16T, BFloat16T>))
struct ExecuteCast512Aligned<SRC_T, DST_T> {
    static void Execute(ExecuteCast512AlignedParamPack &param) {
        auto *src_embedding_ptr = reinterpret_cast<const SRC_T *>(param.src_embedding_ptr);
        auto *result_aligned = GetAVX512AlignedMemory(param.dimension * sizeof(DST_T));
        param.result_ptr = result_aligned;
        auto *dst_embedding_ptr = static_cast<DST_T *>(result_aligned);
        for (u32 i = 0; i < param.dimension; ++i) {
            dst_embedding_ptr[i] = static_cast<DST_T>(src_embedding_ptr[i]);
        }
    }
};

// u8, i8, i16, i32 -> i32
// i64 -> i64
// f32, f64, float16, bfloat16 -> f32
void *GetAlignedCast(char *src_embedding_ptr, u32 dimension, EmbeddingDataType src_embedding_data_type, EmbeddingDataType new_embedding_data_type) {
    ExecuteCast512AlignedParamPack param;
    param.src_embedding_ptr = src_embedding_ptr;
    param.dimension = dimension;
    ElemTypeDispatch<ExecuteCast512Aligned, TypeList<>>(param, src_embedding_data_type, new_embedding_data_type);
    return param.result_ptr;
}

AlignedMatchTensorExprHolderT GetMatchTensorExprForCalculation(MatchTensorExpression &src_match_tensor_expr,
                                                               const EmbeddingDataType column_embedding_type) {
    // check column basic embedding data type and query embedding data type
    // apply necessary cast
    const auto src_query_embedding_type = src_match_tensor_expr.embedding_data_type_;
    EmbeddingDataType new_query_embedding_type = EmbeddingDataType::kElemInvalid;
    switch (column_embedding_type) {
        case EmbeddingDataType::kElemBit: {
            // accept all query embedding types
            switch (src_query_embedding_type) {
                case EmbeddingDataType::kElemBit: {
                    // use the old query embedding
                    // no need for alignment
                    // TODO: hamming distance?
                    break;
                }
                case EmbeddingDataType::kElemUInt8:
                case EmbeddingDataType::kElemInt8:
                case EmbeddingDataType::kElemInt16:
                case EmbeddingDataType::kElemInt32: {
                    // cast to aligned i32
                    new_query_embedding_type = EmbeddingDataType::kElemInt32;
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    // cast to aligned i64
                    new_query_embedding_type = EmbeddingDataType::kElemInt64;
                    break;
                }
                case EmbeddingDataType::kElemFloat:
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemFloat16:
                case EmbeddingDataType::kElemBFloat16: {
                    // cast to aligned f32
                    new_query_embedding_type = EmbeddingDataType::kElemFloat;
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    UnrecoverableError("Invalid embedding data type");
                    break;
                }
            }
            break;
        }
        case EmbeddingDataType::kElemUInt8:
        case EmbeddingDataType::kElemInt8: {
            // expect query embedding to be the same type
            if (src_query_embedding_type != column_embedding_type) {
                UnrecoverableError(fmt::format("Query embedding with data type: {} which doesn't match with column basic embedding type {}.",
                                               EmbeddingInfo::EmbeddingDataTypeToString(src_query_embedding_type),
                                               EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
            }
            // use the old query embedding
            // no need for alignment
            break;
        }
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64: {
            // TODO: not supported yet
            UnrecoverableError(fmt::format("Cannot query on column with basic embedding data type: {}.",
                                           EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
            break;
        }
        case EmbeddingDataType::kElemFloat:
        case EmbeddingDataType::kElemDouble:
        case EmbeddingDataType::kElemFloat16:
        case EmbeddingDataType::kElemBFloat16: {
            // expect query embedding to be also float type
            switch (src_query_embedding_type) {
                case EmbeddingDataType::kElemFloat:
                case EmbeddingDataType::kElemDouble:
                case EmbeddingDataType::kElemFloat16:
                case EmbeddingDataType::kElemBFloat16: {
                    // cast to aligned f32
                    new_query_embedding_type = EmbeddingDataType::kElemFloat;
                    break;
                }
                default: {
                    UnrecoverableError(fmt::format("Query embedding with data type: {} which doesn't match with column basic embedding type {}.",
                                                   EmbeddingInfo::EmbeddingDataTypeToString(src_query_embedding_type),
                                                   EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type)));
                    break;
                }
            }
            break;
        }
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError("Invalid embedding data type");
            break;
        }
    }
    AlignedMatchTensorExprHolderT result;
    if (new_query_embedding_type != EmbeddingDataType::kElemInvalid) {
        const auto aligned_ptr = GetAlignedCast(src_match_tensor_expr.query_embedding_.ptr,
                                                src_match_tensor_expr.dimension_,
                                                src_query_embedding_type,
                                                new_query_embedding_type);
        result.first.reset(aligned_ptr);
        result.second = std::make_unique<MatchTensorExpression>(src_match_tensor_expr.arguments(),
                                                                src_match_tensor_expr.search_method_,
                                                                new_query_embedding_type,
                                                                src_match_tensor_expr.dimension_,
                                                                EmbeddingT(static_cast<char *>(aligned_ptr), false),
                                                                src_match_tensor_expr.tensor_basic_embedding_dimension_,
                                                                src_match_tensor_expr.options_text_,
                                                                src_match_tensor_expr.optional_filter_);
    }
    return result;
}

} // namespace infinity
