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

module infinity_core:physical_match_tensor_scan.impl;

import :physical_match_tensor_scan;
import :query_context;
import :operator_state;
import :physical_operator;
import :physical_operator_type;
import :query_context;
import :operator_state;
import :data_block;
import :column_vector;
import :expression_evaluator;
import :expression_state;
import :base_expression;
import :column_expression;
import :match_tensor_expression;
import :default_values;
import :infinity_exception;
import :base_table_ref;
import :load_meta;
import :status;
import :logger;
import :physical_index_scan;
import :filter_value_type_classification;
import :roaring_bitmap;
import :emvb_index_in_mem;
import :emvb_index;
import :knn_filter;
import :global_block_id;
import :block_index;
import :buffer_manager;
import :buffer_handle;
import :match_tensor_scan_function_data;
import :mlas_matrix_multiply;
import :physical_fusion;
import :filter_value_type_classification;
import :logical_match_tensor_scan;
import :simd_functions;
import :knn_expression;
import :result_cache_manager;
import :buffer_obj;
import :table_meeta;
import :table_index_meeta;
import :segment_index_meta;
import :chunk_index_meta;
import :segment_meta;
import :block_meta;
import :table_meeta;
import :new_txn;
import :new_catalog;
import :index_base;
import :column_meta;
import :mem_index;

import std.compat;
import third_party;

import data_type;
import knn_expr;
import create_index_info;
import match_tensor_expr;
import search_options;
import column_def;
import internal_types;
import type_info;
import embedding_info;
import logical_type;

namespace infinity {

using AlignedMatchTensorExprHolderT =
    std::pair<std::unique_ptr<void, decltype([](void *ptr) { std::free(ptr); })>, std::unique_ptr<MatchTensorExpression>>;

AlignedMatchTensorExprHolderT GetMatchTensorExprForCalculation(MatchTensorExpression &src_match_tensor_expr, EmbeddingDataType column_embedding_type);

PhysicalMatchTensorScan::PhysicalMatchTensorScan(const u64 id,
                                                 const u64 table_index,
                                                 std::shared_ptr<BaseTableRef> base_table_ref,
                                                 std::shared_ptr<MatchTensorExpression> match_tensor_expression,
                                                 const std::shared_ptr<CommonQueryFilter> &common_query_filter,
                                                 const u32 topn,
                                                 const std::optional<f32> knn_threshold,
                                                 const std::shared_ptr<MatchTensorScanIndexOptions> &index_options,
                                                 std::shared_ptr<std::vector<LoadMeta>> load_metas)
    : PhysicalFilterScanBase(id,
                             PhysicalOperatorType::kMatchTensorScan,
                             nullptr,
                             nullptr,
                             table_index,
                             std::move(base_table_ref),
                             common_query_filter,
                             std::move(load_metas)),
      src_match_tensor_expr_(std::move(match_tensor_expression)), topn_(topn), knn_threshold_(knn_threshold), index_options_(index_options) {
    search_column_id_ = std::numeric_limits<ColumnID>::max();
}

PhysicalMatchTensorScan::~PhysicalMatchTensorScan() = default;

void PhysicalMatchTensorScan::Init(QueryContext *query_context) {}

std::shared_ptr<std::vector<std::string>> PhysicalMatchTensorScan::GetOutputNames() const {
    std::shared_ptr<std::vector<std::string>> result_names = std::make_shared<std::vector<std::string>>();
    result_names->reserve(base_table_ref_->column_names_->size() + 2);
    for (auto &name : *base_table_ref_->column_names_) {
        result_names->emplace_back(name);
    }
    result_names->emplace_back(COLUMN_NAME_SCORE);
    result_names->emplace_back(COLUMN_NAME_ROW_ID);
    return result_names;
}

std::shared_ptr<std::vector<std::shared_ptr<DataType>>> PhysicalMatchTensorScan::GetOutputTypes() const {
    std::shared_ptr<std::vector<std::shared_ptr<DataType>>> result_types = std::make_shared<std::vector<std::shared_ptr<DataType>>>();
    result_types->reserve(base_table_ref_->column_types_->size() + 2);
    for (auto &type : *base_table_ref_->column_types_) {
        result_types->emplace_back(type);
    }
    result_types->emplace_back(std::make_shared<DataType>(src_match_tensor_expr_->Type()));
    result_types->emplace_back(std::make_shared<DataType>(LogicalType::kRowID));
    return result_types;
}

ColumnID PhysicalMatchTensorScan::SearchColumnID() const {
    if (search_column_id_ == std::numeric_limits<ColumnID>::max()) {
        UnrecoverableError("Search column id is not set. Init() error.");
    }
    return search_column_id_;
}

void PhysicalMatchTensorScan::CheckColumn() {
    search_column_id_ = src_match_tensor_expr_->column_expr_->binding().column_idx;
    const ColumnDef *column_def = base_table_ref_->table_info_->GetColumnDefByIdx(search_column_id_);
    const auto &column_type_ptr = column_def->type();
    if (const auto l_type = column_type_ptr->type(); l_type != LogicalType::kTensor and l_type != LogicalType::kTensorArray) {
        UnrecoverableError(fmt::format("Column {} is not a tensor or tensorarray column", column_def->name()));
    }
    const auto &type_info = column_type_ptr->type_info();
    if (type_info->type() != TypeInfoType::kEmbedding) {
        UnrecoverableError(fmt::format("Column {} is not a tensor column", column_def->name()));
    }
    const auto *embedding_info = static_cast<const EmbeddingInfo *>(type_info.get());
    if (embedding_info->Dimension() != src_match_tensor_expr_->tensor_basic_embedding_dimension_) {
        UnrecoverableError(
            fmt::format("Column {} embedding dimension not match with query {}", column_def->name(), src_match_tensor_expr_->ToString()));
    }
    // check column basic embedding data type and query embedding data type
    // apply necessary cast
    if (auto [new_search_ptr, new_search_expr] = GetMatchTensorExprForCalculation(*src_match_tensor_expr_, embedding_info->Type()); new_search_ptr) {
        calc_match_tensor_aligned_holder_ = std::shared_ptr<void>(new_search_ptr.release(), std::free);
        calc_match_tensor_expr_holder_ = std::move(new_search_expr);
        calc_match_tensor_expr_ = calc_match_tensor_expr_holder_.get();
    } else {
        calc_match_tensor_expr_ = src_match_tensor_expr_.get();
    }
}

void PhysicalMatchTensorScan::PlanWithIndex(QueryContext *query_context) {
    Status status;
    size_t search_column_id = SearchColumnID();
    auto &search_column_name = base_table_ref_->table_info_->column_defs_[search_column_id]->name();

    block_metas_ = std::make_unique<std::vector<BlockMeta *>>();
    segment_index_metas_ = std::make_unique<std::vector<std::shared_ptr<SegmentIndexMeta>>>();

    TableMeeta *table_meta = base_table_ref_->block_index_->table_meta_.get();

    std::set<SegmentID> index_entry_map;

    if (!src_match_tensor_expr_->ignore_index_) {
        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        if (!status.ok()) {
            RecoverableError(status);
        }

        if (src_match_tensor_expr_->index_name_.empty()) {
            LOG_TRACE("Try to find an index to use");
            for (size_t i = 0; i < index_id_strs_ptr->size(); ++i) {
                const std::string &index_id_str = (*index_id_strs_ptr)[i];
                const std::string &index_name_str = (*index_names_ptr)[i];
                auto table_index_meta = std::make_unique<TableIndexMeeta>(index_id_str, index_name_str, *table_meta);

                std::shared_ptr<IndexBase> index_base;
                std::tie(index_base, status) = table_index_meta->GetIndexBase();
                if (!status.ok()) {
                    RecoverableError(status);
                }

                if (index_base->column_name() != search_column_name) {
                    // search_column_id isn't in this table index
                    continue;
                }
                // check index type
                if (auto index_type = index_base->index_type_; index_type != IndexType::kEMVB) {
                    LOG_TRACE(fmt::format("MatchTensorScan: PlanWithIndex(): Skipping non-knn index."));
                    continue;
                }
                table_index_meta_ = std::move(table_index_meta);
                break;
            }
        } else {
            LOG_TRACE(fmt::format("Use index: {}", src_match_tensor_expr_->index_name_));
            auto iter = std::find(index_names_ptr->begin(), index_names_ptr->end(), src_match_tensor_expr_->index_name_);
            if (iter == index_names_ptr->end()) {
                Status status = Status::IndexNotExist(src_match_tensor_expr_->index_name_);
                RecoverableError(std::move(status));
            }
            const std::string &index_id_str = (*index_id_strs_ptr)[iter - index_names_ptr->begin()];
            auto table_index_meta = std::make_unique<TableIndexMeeta>(index_id_str, src_match_tensor_expr_->index_name_, *table_meta);

            std::shared_ptr<IndexBase> index_base;
            std::tie(index_base, status) = table_index_meta->GetIndexBase();
            if (!status.ok()) {
                RecoverableError(status);
            }

            if (index_base->column_name() != search_column_name) {
                // search_column_id isn't in this table index
                LOG_ERROR(fmt::format("Column {} not found", index_base->column_name()));
                Status error_status = Status::ColumnNotExist(index_base->column_name());
                RecoverableError(std::move(error_status));
            }
            // check index type
            if (auto index_type = index_base->index_type_; index_type != IndexType::kEMVB) {
                Status error_status = Status::InvalidIndexType("invalid index");
                RecoverableError(std::move(error_status));
            }
            table_index_meta_ = std::move(table_index_meta);
        }
        // Fill the segment with index
        if (table_index_meta_) {
            std::vector<SegmentID> *segment_ids_ptr = nullptr;
            std::tie(segment_ids_ptr, status) = table_index_meta_->GetSegmentIndexIDs1();
            if (!status.ok()) {
                RecoverableError(status);
            }
            index_entry_map.insert(segment_ids_ptr->begin(), segment_ids_ptr->end());
        }
    }

    // Generate task set: index segment and no index block
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    for (const auto &[segment_id, segment_info] : block_index->new_segment_block_index_) {
        if (auto iter = index_entry_map.find(segment_id); iter != index_entry_map.end()) {
            segment_index_metas_->push_back(std::make_shared<SegmentIndexMeta>(segment_id, *table_index_meta_));
        } else {
            const auto &block_map = segment_info.block_map();
            for (const auto &block_meta : block_map) {
                block_metas_->emplace_back(block_meta.get());
            }
        }
    }
    LOG_TRACE(fmt::format("MatchTensorScan: brute force task: {}, index task: {}", block_metas_->size(), segment_index_metas_->size()));

    return;
}

std::vector<std::shared_ptr<std::vector<GlobalBlockID>>> PhysicalMatchTensorScan::PlanBlockEntries(i64 parallel_count) const {
    UnrecoverableError("PhysicalMatchTensorScan:: use PlanWithIndex instead of PlanBlockEntries!");
    return {};
}

// TODO: how many threads for brute force scan?
size_t PhysicalMatchTensorScan::TaskletCount() {
    return (block_metas_ ? block_metas_->size() : 0) + (segment_index_metas_ ? segment_index_metas_->size() : 0);
}

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
        UnrecoverableError("TensorScan output data block array should be empty");
    }

    Status status;
    NewTxn *new_txn = query_context->GetNewTxn();
    const TxnTimeStamp begin_ts = new_txn->BeginTS();
    const TxnTimeStamp commit_ts = new_txn->CommitTS();
    if (!common_query_filter_->TryFinishBuild()) {
        // not ready, abort and wait for next time
        return;
    }
    MatchTensorScanFunctionData &function_data = *(operator_state->match_tensor_scan_function_data_);
    if (function_data.finished_) [[unlikely]] {
        UnrecoverableError("MatchTensorScanFunctionData is finished");
    }
    const BlockIndex *block_index = base_table_ref_->block_index_.get();
    if (const u32 task_job_index = task_executed_++; task_job_index < segment_index_metas_->size()) {
        // use index
        SegmentIndexMeta &segment_index_meta = *(*segment_index_metas_)[task_job_index];
        SegmentID segment_id = segment_index_meta.segment_id();
        // SegmentEntry *segment_entry = nullptr;
        SegmentOffset segment_row_count = 0;
        SegmentMeta *segment_meta = nullptr;
        const auto &segment_index_hashmap = base_table_ref_->block_index_->new_segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            UnrecoverableError(fmt::format("Cannot find SegmentMeta for segment id: {}", segment_id));
        } else {
            // segment_entry = iter->second.segment_entry_;
            segment_meta = iter->second.segment_meta_.get();
            std::tie(segment_row_count, status) = segment_meta->GetRowCnt1();
            if (!status.ok()) {
                UnrecoverableError(fmt::format("GetRowCnt1 failed: {}", status.message()));
            }
        }

        bool has_some_result = false;
        Bitmask segment_bitmask;
        if (common_query_filter_->AlwaysTrue()) {
            has_some_result = true;
            segment_bitmask.SetAllTrue();
        } else {
            if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
                LOG_TRACE(
                    fmt::format("MatchTensorScan: index {}/{} not skipped after common_query_filter", task_job_index, segment_index_metas_->size()));
                segment_bitmask = it->second;
                if (segment_row_count != segment_bitmask.count()) {
                    UnrecoverableError(
                        fmt::format("Segment row count {} not match with bitmask size {}", segment_row_count, segment_bitmask.count()));
                }
                has_some_result = true;
            }
        }

        if (has_some_result) {
            LOG_TRACE(
                fmt::format("MatchTensorScan: index {}/{} not skipped after common_query_filter", task_job_index, segment_index_metas_->size()));
            // TODO: now only have EMVB index
            auto [chunk_ids_ptr, status] = segment_index_meta.GetChunkIDs1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            std::shared_ptr<MemIndex> mem_index = segment_index_meta.GetMemIndex();
            std::shared_ptr<EMVBIndexInMem> emvb_index_in_mem = mem_index == nullptr ? nullptr : mem_index->GetEMVBIndex();
            // 1. in mem index
            if (emvb_index_in_mem) {
                // TODO: fix the parameters
                const auto result = emvb_index_in_mem->SearchWithBitmask(reinterpret_cast<const f32 *>(calc_match_tensor_expr_->query_embedding_.ptr),
                                                                         calc_match_tensor_expr_->num_of_embedding_in_query_tensor_,
                                                                         topn_,
                                                                         segment_bitmask,
                                                                         block_index,
                                                                         begin_ts,
                                                                         index_options_->emvb_centroid_nprobe_,
                                                                         index_options_->emvb_threshold_first_,
                                                                         index_options_->emvb_n_doc_to_score_,
                                                                         index_options_->emvb_n_doc_out_second_stage_,
                                                                         index_options_->emvb_threshold_final_);
                std::visit(
                    Overload{[segment_id, &function_data](const std::tuple<u32, std::unique_ptr<f32[]>, std::unique_ptr<u32[]>> &index_result) {
                                 const auto &[result_num, score_ptr, row_id_ptr] = index_result;
                                 for (u32 i = 0; i < result_num; ++i) {
                                     function_data.result_handler_->AddResult(0, score_ptr[i], RowID(segment_id, row_id_ptr[i]));
                                 }
                             },
                             [this, begin_ts, commit_ts, segment_id, &function_data, &segment_meta](const std::pair<u32, u32> &in_mem_result) {
                                 const auto &[start_offset, total_row_count] = in_mem_result;
                                 BlockID block_id = start_offset / DEFAULT_BLOCK_CAPACITY;
                                 BlockOffset block_offset = start_offset % DEFAULT_BLOCK_CAPACITY;
                                 u32 row_leftover = total_row_count;
                                 do {
                                     const u32 row_to_read = std::min<u32>(row_leftover, DEFAULT_BLOCK_CAPACITY - block_offset);
                                     Bitmask block_bitmask;
                                     if (this->CalculateFilterBitmask(segment_id, block_id, row_to_read, block_bitmask)) {
                                         BlockMeta block_meta(block_id, *segment_meta);
                                         Status status = NewCatalog::SetBlockDeleteBitmask(block_meta, begin_ts, commit_ts, block_bitmask);
                                         if (!status.ok()) {
                                             UnrecoverableError(status.message());
                                         }
                                         ColumnMeta column_meta(this->search_column_id_, block_meta);
                                         ColumnVector column_vector;
                                         status = NewCatalog::GetColumnVector(column_meta,
                                                                              column_meta.get_column_def(),
                                                                              row_to_read,
                                                                              ColumnVectorMode::kReadOnly,
                                                                              column_vector);
                                         if (!status.ok()) {
                                             UnrecoverableError(status.message());
                                         }

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
            for (ChunkID chunk_id : *chunk_ids_ptr) {
                ChunkIndexMeta chunk_index_meta(chunk_id, segment_index_meta);
                BufferObj *index_buffer = nullptr;
                Status status = chunk_index_meta.GetIndexBuffer(index_buffer);
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }
                BufferHandle index_handle = index_buffer->Load();
                const auto *emvb_index = static_cast<const EMVBIndex *>(index_handle.GetData());

                const auto [result_num, score_ptr, row_id_ptr] =
                    emvb_index->SearchWithBitmask(reinterpret_cast<const f32 *>(calc_match_tensor_expr_->query_embedding_.ptr),
                                                  calc_match_tensor_expr_->num_of_embedding_in_query_tensor_,
                                                  topn_,
                                                  segment_bitmask,
                                                  block_index,
                                                  begin_ts,
                                                  index_options_->emvb_centroid_nprobe_,
                                                  index_options_->emvb_threshold_first_,
                                                  index_options_->emvb_n_doc_to_score_,
                                                  index_options_->emvb_n_doc_out_second_stage_,
                                                  index_options_->emvb_threshold_final_);
                for (u32 i = 0; i < result_num; ++i) {
                    function_data.result_handler_->AddResult(0, score_ptr[i], RowID(segment_id, row_id_ptr[i]));
                }
            }
        }
    } else if (const u32 task_job_block = task_job_index - segment_index_metas_->size(); task_job_block < block_metas_->size()) {
        BlockMeta *block_meta = (*block_metas_)[task_job_block];
        BlockID block_id = block_meta->block_id();
        SegmentID segment_id = block_meta->segment_meta().segment_id();
        BlockOffset row_count = block_index->GetBlockOffset(segment_id, block_id);

        Bitmask bitmask;
        if (this->CalculateFilterBitmask(segment_id, block_id, row_count, bitmask)) {
            Status status = NewCatalog::SetBlockDeleteBitmask(*block_meta, begin_ts, commit_ts, bitmask);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            ColumnMeta column_meta(this->search_column_id_, *block_meta);
            ColumnVector column_vector;
            status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, column_vector);
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            // output score will always be float type
            CalculateScoreOnColumnVector(column_vector, segment_id, block_id, 0, row_count, bitmask, *calc_match_tensor_expr_, function_data);
        }
    } else {
        // all task Complete
        const u32 result_n = function_data.End();
        float *result_scores = function_data.score_result_.get();
        RowID *result_row_ids = function_data.row_id_result_.get();
        SetOutput(std::vector<char *>{reinterpret_cast<char *>(result_scores)},
                  std::vector<RowID *>{result_row_ids},
                  sizeof(std::remove_pointer_t<decltype(result_scores)>),
                  result_n,
                  query_context,
                  operator_state);
        operator_state->SetComplete();
    }
    return;
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
        auto output_ptr = std::make_unique_for_overwrite<float[]>(query_embedding_num * target_embedding_num);
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
        auto target_buffer = std::make_unique_for_overwrite<float[]>(basic_embedding_dimension * target_embedding_num);
        for (u32 i = 0; i < basic_embedding_dimension * target_embedding_num; ++i) {
            target_buffer[i] = static_cast<float>(src_target_type_ptr[i]);
        }
        auto output_ptr = std::make_unique_for_overwrite<float[]>(query_embedding_num * target_embedding_num);
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
        std::vector<std::pair<std::span<const char>, size_t>> tensor_array = column_vector.GetTensorArrayRaw(block_offset);
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
            UnrecoverableError("Invalid search method!");
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
                UnrecoverableError("Invalid column type! target column is not Tensor or TensorArray type.");
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
            UnrecoverableError("Invalid embedding data type!");
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
    std::vector<MatchTensorRerankDoc> &rerank_docs_;
    BufferManager *buffer_mgr_;
    const DataType *column_data_type_;
    const ColumnID column_id_;
    const BlockIndex *block_index_;
    const MatchTensorExpression &match_tensor_expr_;
    RerankerParameterPack(std::vector<MatchTensorRerankDoc> &rerank_docs,
                          BufferManager *buffer_mgr,
                          const DataType *column_data_type,
                          const ColumnID column_id,
                          const BlockIndex *block_index,
                          const MatchTensorExpression &match_tensor_expr)
        : rerank_docs_(rerank_docs), buffer_mgr_(buffer_mgr), column_data_type_(column_data_type), column_id_(column_id), block_index_(block_index),
          match_tensor_expr_(match_tensor_expr) {}
};

template <typename CalcutateScoreOfRowOp>
void GetRerankerScore(std::vector<MatchTensorRerankDoc> &rerank_docs,
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
        ColumnVector column_vec;
        BlockMeta *block_meta = block_index->new_segment_block_index_.at(segment_id).block_map().at(block_id).get();
        ColumnMeta column_meta(column_id, *block_meta);
        auto [block_row_cnt, status] = block_meta->GetRowCnt1();
        if (!status.ok()) {
            UnrecoverableError("GetRowCnt1 failed!");
        }
        status = NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), block_row_cnt, ColumnVectorMode::kReadOnly, column_vec);
        if (!status.ok()) {
            UnrecoverableError("GetRowCnt1 failed!");
        }

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
            UnrecoverableError("Invalid search method!");
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
                UnrecoverableError("Invalid column type! target column is not Tensor or TensorArray type.");
            }
        }
    }
};

void CalculateFusionMatchTensorRerankerScores(std::vector<MatchTensorRerankDoc> &rerank_docs,
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
inline auto GetAVX512AlignedMemory(const size_t bytes) {
    const auto alloc_bytes = ((bytes + 63u) & (~static_cast<size_t>(63))) + 128u; // need to be multiple of 64, with extra 128 bytes
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
                                                                src_match_tensor_expr.optional_filter_,
                                                                src_match_tensor_expr.index_name_,
                                                                src_match_tensor_expr.ignore_index_);
    }
    return result;
}

} // namespace infinity
