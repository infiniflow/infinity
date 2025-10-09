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

module infinity_core:physical_knn_scan.impl;

import :physical_knn_scan;
import :operator_state;
import :physical_operator_type;
import :knn_expression;
import :physical_filter_scan_base;
import :logger;
import :block_index;
import :knn_scan_data;
import :knn_filter;
import :infinity_exception;
import :column_expression;
import :buffer_manager;
import :merge_knn;
import :knn_result_handler;
import :buffer_obj;
import :data_block;
import :index_hnsw;
import :status;
import :hnsw_handler;
import :physical_match_tensor_scan;
import :hnsw_alg;
import :ivf_index_data_in_mem;
import :ivf_index_data;
import :ivf_index_search;
import :new_txn;
import :table_index_meta;
import :segment_index_meta;
import :table_meta;
import :segment_meta;
import :block_meta;
import :column_meta;
import :mem_index;
import :mlas_matrix_multiply;
import :infinity_type;
import :expression_evaluator;
import :expression_state;
import :value;
import :fixed_dimensional_encoding;
import :function_expression;

import std.compat;
import third_party;

import create_index_info;
import knn_expr;
import embedding_info;
import logical_type;
import internal_types;

namespace infinity {

auto GetKnnExprForCalculation(const KnnExpression &src_knn_expr, const EmbeddingDataType column_embedding_type) {
    // check column basic embedding data type and query embedding data type
    // apply necessary cast
    const auto src_query_embedding_type = src_knn_expr.embedding_data_type_;
    auto new_query_embedding_type = EmbeddingDataType::kElemInvalid;
    switch (column_embedding_type) {
        case EmbeddingDataType::kElemBit:
            [[fallthrough]];
        case EmbeddingDataType::kElemUInt8:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt8: {
            // expect query embedding to be the same type
            if (src_query_embedding_type != column_embedding_type) {
                RecoverableError(
                    Status::NotSupport(fmt::format("Query embedding with data type: {} which doesn't match with column basic embedding type {}.",
                                                   EmbeddingInfo::EmbeddingDataTypeToString(src_query_embedding_type),
                                                   EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type))));
            }
            // use the old query embedding
            // no need for alignment
            break;
        }
        case EmbeddingDataType::kElemInt16:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt32:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt64: {
            // TODO: not supported yet
            RecoverableError(Status::NotSupport(fmt::format("Cannot query on column with basic embedding data type: {}.",
                                                            EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type))));
            break;
        }
        case EmbeddingDataType::kElemFloat:
            [[fallthrough]];
        case EmbeddingDataType::kElemDouble:
            [[fallthrough]];
        case EmbeddingDataType::kElemFloat16:
            [[fallthrough]];
        case EmbeddingDataType::kElemBFloat16: {
            // expect query embedding to be also float type
            switch (src_query_embedding_type) {
                case EmbeddingDataType::kElemFloat:
                    [[fallthrough]];
                case EmbeddingDataType::kElemDouble:
                    [[fallthrough]];
                case EmbeddingDataType::kElemFloat16:
                    [[fallthrough]];
                case EmbeddingDataType::kElemBFloat16: {
                    // cast to aligned f32
                    new_query_embedding_type = EmbeddingDataType::kElemFloat;
                    break;
                }
                default: {
                    RecoverableError(
                        Status::NotSupport(fmt::format("Query embedding with data type: {} which doesn't match with column basic embedding type {}.",
                                                       EmbeddingInfo::EmbeddingDataTypeToString(src_query_embedding_type),
                                                       EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type))));
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
    std::pair<std::unique_ptr<void, decltype([](void *ptr) { std::free(ptr); })>, EmbeddingDataType> result = {nullptr,
                                                                                                               EmbeddingDataType::kElemInvalid};
    if (new_query_embedding_type != EmbeddingDataType::kElemInvalid) {
        const auto aligned_ptr =
            GetAlignedCast(src_knn_expr.query_embedding_.ptr, src_knn_expr.dimension_, src_query_embedding_type, new_query_embedding_type);
        result.first.reset(aligned_ptr);
        result.second = new_query_embedding_type;
    }
    return result;
}

PhysicalKnnScan::PhysicalKnnScan(u64 id,
                                 std::shared_ptr<BaseTableRef> base_table_ref,
                                 std::shared_ptr<KnnExpression> knn_expression,
                                 const std::shared_ptr<CommonQueryFilter> &common_query_filter,
                                 std::shared_ptr<std::vector<std::string>> output_names,
                                 std::shared_ptr<std::vector<std::shared_ptr<DataType>>> output_types,
                                 u64 knn_table_index,
                                 std::shared_ptr<std::vector<LoadMeta>> load_metas)
    : PhysicalFilterScanBase(id, PhysicalOperatorType::kKnnScan, nullptr, nullptr, knn_table_index, base_table_ref, common_query_filter, load_metas),
      knn_expression_(std::move(knn_expression)), output_names_(std::move(output_names)), output_types_(std::move(output_types)) {}

PhysicalKnnScan::~PhysicalKnnScan() = default;

void PhysicalKnnScan::Init(QueryContext *query_context) {
    auto *knn_expr = knn_expression_.get();
    const auto *column_expr = static_cast<const ColumnExpression *>(knn_expr->arguments()[0].get());
    const auto column_data_type = column_expr->Type();
    switch (column_data_type.type()) {
        case LogicalType::kEmbedding:
            [[fallthrough]];
        case LogicalType::kMultiVector: {
            // support knn
            column_logical_type_ = column_data_type.type();
            break;
        }
        default: {
            RecoverableError(Status::NotSupport(fmt::format("KnnScan: Init(): Column type {} is not embedding.", column_data_type.ToString())));
            break;
        }
    }
    const auto *embedding_type_info = static_cast<const EmbeddingInfo *>(column_data_type.type_info().get());
    const auto column_elem_type = embedding_type_info->Type();
    column_elem_type_ = column_elem_type;
    column_embedding_dimension_ = embedding_type_info->Dimension();
    // Check if we need to evaluate a function expression (e.g., FDE) to generate query embedding
    if (knn_expr->arguments().size() > 1) {
        // Function expression case - evaluate the function to get query embedding
        auto function_expr = knn_expr->arguments()[1]; // The FDE function expression

        // Create expression evaluator without input data block (for constant expressions)
        ExpressionEvaluator evaluator;
        evaluator.Init(nullptr);

        // Create expression state for the function
        std::shared_ptr<ExpressionState> expr_state = ExpressionState::CreateState(function_expr);

        // Create output column vector for function result
        std::shared_ptr<ColumnVector> output_column = ColumnVector::Make(std::make_shared<DataType>(function_expr->Type()));
        output_column->Initialize();

        // Evaluate the function expression
        evaluator.Execute(function_expr, expr_state, output_column);

        // Extract the embedding result
        if (output_column->Size() > 0) {
            Value embedding_value = output_column->GetValueByIndex(0);
            if (embedding_value.type().type() == LogicalType::kEmbedding) {
                auto embedding_info = static_cast<const EmbeddingInfo *>(embedding_value.type().type_info().get());
                std::span<char> embedding_data = embedding_value.GetEmbedding();
                real_knn_query_embedding_ptr_ = embedding_data.data();
                real_knn_query_elem_type_ = embedding_info->Type();
                real_query_embedding_dimension_ = embedding_info->Dimension(); // Store actual dimension
                // Store the embedding data to prevent deallocation
                function_query_embedding_holder_ = embedding_value;
            } else {
                RecoverableError(Status::SyntaxError("Function must return embedding type"));
            }
        } else {
            RecoverableError(Status::SyntaxError("Function returned no results"));
        }
    } else if (auto [new_query_embedding_ptr, new_query_embedding_type] = GetKnnExprForCalculation(*knn_expr, column_elem_type);
               new_query_embedding_type != EmbeddingDataType::kElemInvalid) {
        // have new query ptr
        const auto new_query_embedding_ptr_ = new_query_embedding_ptr.release();
        real_knn_query_embedding_holder_ = std::shared_ptr<void>(new_query_embedding_ptr_, std::free);
        real_knn_query_embedding_ptr_ = new_query_embedding_ptr_;
        real_knn_query_elem_type_ = new_query_embedding_type;
        real_query_embedding_dimension_ = knn_expr->dimension_; // Use KnnExpression dimension
    } else {
        real_knn_query_embedding_ptr_ = knn_expr->query_embedding_.ptr;
        real_knn_query_elem_type_ = knn_expr->embedding_data_type_;
        real_query_embedding_dimension_ = knn_expr->dimension_; // Use KnnExpression dimension
    }
    if (real_knn_query_elem_type_ != column_elem_type_) {
        // If query embedding type is not the same as column embedding type, cast it
        real_knn_query_embedding_holder_ = knn_expr->query_embedding_.Cast(real_knn_query_elem_type_, column_elem_type_, knn_expr->dimension_);
        real_knn_query_embedding_ptr_ = real_knn_query_embedding_holder_.get();
        real_knn_query_elem_type_ = column_elem_type_;
        real_query_embedding_dimension_ = knn_expr->dimension_; // Use KnnExpression dimension
    }
}

void PhysicalKnnScan::InitBlockParallelOption() {
    // TODO: Set brute force block parallel option
    // 0. [0, 50), 1 thread
    // 1. [50, 100), 2 thread
    block_parallel_options_.emplace_back(50, 2);
    // 2. [100, +Inf), 3 threads
    block_parallel_options_.emplace_back(100, 3);
}

size_t PhysicalKnnScan::BlockScanTaskCount() const {
    const u32 block_cnt = block_column_entries_size_;
    if (block_cnt <= 0)
        return 0;
    size_t brute_task_n = 1;
    for (const auto &[block_n, job_n] : block_parallel_options_) {
        if (block_cnt < block_n) {
            break;
        }
        brute_task_n = job_n;
    }
    return brute_task_n;
}

size_t PhysicalKnnScan::TaskletCount() {
    u32 tasklet_count = BlockScanTaskCount() + index_entries_size_;
    if (tasklet_count <= 0) {
        tasklet_count = 1;
    }
    return tasklet_count;
}

bool PhysicalKnnScan::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *knn_scan_operator_state = static_cast<KnnScanOperatorState *>(operator_state);
    switch (column_logical_type_) {
        case LogicalType::kEmbedding: {
            ExecuteInternalByColumnLogicalType<LogicalType::kEmbedding>(query_context, knn_scan_operator_state);
            break;
        }
        case LogicalType::kMultiVector: {
            ExecuteInternalByColumnLogicalType<LogicalType::kMultiVector>(query_context, knn_scan_operator_state);
            break;
        }
        default: {
            UnrecoverableError(fmt::format("KnnScan: Execute(): Invalid column logical type {}.", LogicalType2Str(column_logical_type_)));
        }
    }
    return true;
}

template <LogicalType t>
void PhysicalKnnScan::ExecuteInternalByColumnLogicalType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
    switch (column_elem_type_) {
        case EmbeddingDataType::kElemUInt8: {
            return ExecuteInternalByColumnDataType<t, u8>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemInt8: {
            return ExecuteInternalByColumnDataType<t, i8>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemFloat: {
            return ExecuteInternalByColumnDataType<t, f32>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemDouble: {
            return ExecuteInternalByColumnDataType<t, f64>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemFloat16: {
            return ExecuteInternalByColumnDataType<t, Float16T>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemBFloat16: {
            return ExecuteInternalByColumnDataType<t, BFloat16T>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemBit: {
            return ExecuteInternalByColumnDataType<t, u8>(query_context, knn_scan_operator_state);
        }
        case EmbeddingDataType::kElemInt16:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt32:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt64:
            [[fallthrough]];
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError(
                fmt::format("KnnScan: Execute(): Invalid embedding data type {}.", EmbeddingT::EmbeddingDataType2String(column_elem_type_)));
        }
    }
}

template <LogicalType t, typename ColumnDataT>
void PhysicalKnnScan::ExecuteInternalByColumnDataType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
    static_assert(IsAnyOf<ColumnDataT, u8, i8, f32, f64, Float16T, BFloat16T>, "Unexpected ColumnDataT type.");
    const auto query_elem_type = knn_scan_operator_state->knn_scan_function_data_->knn_scan_shared_data_->query_elem_type_;
    const auto query_dist_type = knn_scan_operator_state->knn_scan_function_data_->knn_scan_shared_data_->knn_distance_type_;
    auto knn_distance_error = [query_dist_type] {
        RecoverableError(Status::NotSupport(fmt::format("Not implemented KNN distance: {}", KnnExpression::KnnDistanceType2Str(query_dist_type))));
    };
    switch (query_elem_type) {
        case EmbeddingDataType::kElemBit: {
            if (query_dist_type != KnnDistanceType::kHamming) {
                RecoverableError(Status::NotSupport(fmt::format("Not implemented KNN distance type: {} for query embedding data type: {}",
                                                                KnnExpression::KnnDistanceType2Str(query_dist_type),
                                                                EmbeddingT::EmbeddingDataType2String(query_elem_type))));
            }
            break;
        }
        case EmbeddingDataType::kElemUInt8:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt8:
            [[fallthrough]];
        case EmbeddingDataType::kElemFloat16:
            [[fallthrough]];
        case EmbeddingDataType::kElemBFloat16:
            [[fallthrough]];
        case EmbeddingDataType::kElemFloat:
            [[fallthrough]];
        case EmbeddingDataType::kElemDouble:
            break;
        case EmbeddingDataType::kElemInt16:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt32:
            [[fallthrough]];
        case EmbeddingDataType::kElemInt64:
            [[fallthrough]];
        case EmbeddingDataType::kElemInvalid: {
            RecoverableError(Status::NotSupport(
                fmt::format("Not implemented query embedding data type: {}", EmbeddingT::EmbeddingDataType2String(query_elem_type))));
        }
    }
    switch (query_dist_type) {
        case KnnDistanceType::kL2:
            [[fallthrough]];
        case KnnDistanceType::kHamming: {
            return ExecuteInternalByColumnDataTypeAndQueryDataType<t, ColumnDataT, CompareMax, f32>(query_context, knn_scan_operator_state);
        }
        case KnnDistanceType::kCosine:
            [[fallthrough]];
        case KnnDistanceType::kInnerProduct: {
            return ExecuteInternalByColumnDataTypeAndQueryDataType<t, ColumnDataT, CompareMin, f32>(query_context, knn_scan_operator_state);
        }
        default: {
            return knn_distance_error();
        }
    }
}

// TableInfo *PhysicalKnnScan::table_info() const { return base_table_ref_->table_info_.get(); }

std::string PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

std::vector<size_t> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

size_t PhysicalKnnScan::GetColumnID() const {
    auto *knn_expr = knn_expression_.get();
    auto *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());
    return column_expr->binding().column_idx;
}

void PhysicalKnnScan::PlanWithIndex(QueryContext *query_context) { // TODO: return base entry vector
    InitBlockParallelOption();                                     // PlanWithIndex() will be called in physical planner

    size_t knn_column_id = GetColumnID();
    auto &knn_column_name = base_table_ref_->table_info_->column_defs_[knn_column_id]->name();

    block_metas_ = std::make_unique<std::vector<BlockMeta *>>();
    segment_index_metas_ = std::make_unique<std::vector<std::shared_ptr<SegmentIndexMeta>>>();

    TableMeta *table_meta = base_table_ref_->block_index_->table_meta_.get();

    std::set<SegmentID> index_entry_map;

    if (knn_expression_->ignore_index_) {
        LOG_TRACE("Not use index"); // No index need to check
    } else {
        std::vector<std::string> *index_id_strs_ptr = nullptr;
        std::vector<std::string> *index_names_ptr = nullptr;
        auto status = table_meta->GetIndexIDs(index_id_strs_ptr, &index_names_ptr);
        if (!status.ok()) {
            RecoverableError(status);
        }

        if (knn_expression_->using_index_.empty()) {
            LOG_TRACE("Try to find a index to use");
            for (size_t i = 0; i < index_id_strs_ptr->size(); ++i) {
                auto it = base_table_ref_->block_index_->table_index_meta_map_[i];

                std::shared_ptr<IndexBase> index_base;
                std::tie(index_base, status) = it->GetIndexBase();
                if (!status.ok()) {
                    RecoverableError(status);
                }

                if (index_base->column_name() != knn_column_name) {
                    // knn_column_id isn't in this table index
                    continue;
                }
                // check index type
                if (auto index_type = index_base->index_type_; index_type != IndexType::kIVF and index_type != IndexType::kHnsw) {
                    LOG_TRACE(fmt::format("KnnScan: PlanWithIndex(): Skipping non-knn index."));
                    continue;
                }
                table_index_meta_ = it;
                break;
            }
        } else {
            LOG_TRACE(fmt::format("Use index: {}", knn_expression_->using_index_));
            auto iter = std::find(index_names_ptr->begin(), index_names_ptr->end(), knn_expression_->using_index_);
            if (iter == index_names_ptr->end()) {
                RecoverableError(Status::IndexNotExist(knn_expression_->using_index_));
            }
            auto it = base_table_ref_->block_index_->table_index_meta_map_[iter - index_names_ptr->begin()];

            std::shared_ptr<IndexBase> index_base;
            std::tie(index_base, status) = it->GetIndexBase();
            if (!status.ok()) {
                RecoverableError(status);
            }

            if (index_base->column_name() != knn_column_name) {
                // knn_column_id isn't in this table index
                LOG_ERROR(fmt::format("Column {} not found", index_base->column_name()));
                RecoverableError(Status::ColumnNotExist(index_base->column_name()));
            }
            // check index type
            if (auto index_type = index_base->index_type_; index_type != IndexType::kIVF and index_type != IndexType::kHnsw) {
                RecoverableError(Status::InvalidIndexType("invalid index"));
            }

            table_index_meta_ = it;
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
    block_column_entries_size_ = block_metas_->size();
    index_entries_size_ = segment_index_metas_->size();
    LOG_TRACE(fmt::format("KnnScan: brute force task: {}, index task: {}", block_column_entries_size_, index_entries_size_));

    return;
}

size_t PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

template <LogicalType t, typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
struct BruteForceBlockScan;

template <typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
void MultiVectorSearchOneLine(MergeKnn<ColumnDataType, C, DistanceDataType> *merge_heap,
                              KnnDistance1<ColumnDataType, DistanceDataType> *dist_func,
                              const ColumnDataType *knn_query_ptr,
                              const u32 query_count,
                              const u32 embedding_dim,
                              const ColumnVector &column_vector,
                              SegmentID segment_id,
                              SegmentOffset segment_offset,
                              BlockOffset block_offset);

template <LogicalType t, typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
void PhysicalKnnScan::ExecuteInternalByColumnDataTypeAndQueryDataType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
    // knn expr output data type is always f32
    static_assert(std::is_same_v<DistanceDataType, f32>);

    if (!common_query_filter_->TryFinishBuild()) {
        // not ready, abort and wait for next time
        return;
    }

    auto knn_scan_function_data = knn_scan_operator_state->knn_scan_function_data_.get();
    auto knn_scan_shared_data = knn_scan_function_data->knn_scan_shared_data_;

    auto dist_func = dynamic_cast<KnnDistance1<ColumnDataType, DistanceDataType> *>(knn_scan_function_data->knn_distance_.get());
    auto merge_heap = dynamic_cast<MergeKnn<ColumnDataType, C, DistanceDataType> *>(knn_scan_function_data->merge_knn_base_.get());
    auto knn_query_ptr = static_cast<const ColumnDataType *>(knn_scan_shared_data->query_embedding_);
    const auto query_count = knn_scan_shared_data->query_count_;
    const auto embedding_dim =
        knn_scan_shared_data->query_elem_type_ == EmbeddingDataType::kElemBit ? (column_embedding_dimension_ >> 3) : column_embedding_dimension_;
    if (!dist_func || !merge_heap) {
        const auto err = "Invalid dynamic cast";
        LOG_ERROR(err);
        UnrecoverableError(err);
    }

    // merge_heap for HNSW scan. For multivector maxsim, query_count could be larger than 1.
    MergeKnn<ColumnDataType, C, DistanceDataType> *merge_heap_hnsw = nullptr;
    if (knn_scan_shared_data->query_count_ > 1) {
        merge_heap_hnsw = dynamic_cast<MergeKnn<ColumnDataType, C, DistanceDataType> *>(knn_scan_function_data->merge_knn_base_hnsw_.get());
    } else {
        merge_heap_hnsw = merge_heap;
    }

    size_t index_task_n = knn_scan_shared_data->segment_index_metas_->size();
    size_t brute_task_n = knn_scan_shared_data->block_metas_->size();
    auto *new_txn = query_context->GetNewTxn();
    auto begin_ts = new_txn->BeginTS();
    auto commit_ts = new_txn->CommitTS();

    auto *block_index = knn_scan_shared_data->table_ref_->block_index_.get();
    size_t knn_column_id = GetColumnID();

    std::unique_ptr<ColumnDataType[]> buffer_ptr_for_cast;
    if (u64 block_column_idx =
            knn_scan_function_data->execute_block_scan_job_ ? knn_scan_shared_data->current_block_idx_++ : std::numeric_limits<u64>::max();
        block_column_idx < brute_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{}", knn_scan_function_data->task_id_, block_column_idx + 1, brute_task_n));
        // brute force
        // TODO: now will try to finish all block scan job in the task
        do {
            BlockMeta *block_meta = knn_scan_shared_data->block_metas_->at(block_column_idx);
            ColumnMeta column_meta(knn_column_id, *block_meta);
            BlockID block_id = block_meta->block_id();
            SegmentID segment_id = block_meta->segment_meta().segment_id();
            auto [row_count, status] = block_meta->GetRowCnt1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            Bitmask bitmask;
            if (this->CalculateFilterBitmask(segment_id, block_id, row_count, bitmask)) {
                status = NewCatalog::SetBlockDeleteBitmask(*block_meta, begin_ts, commit_ts, bitmask);
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }
                ColumnVector column_vector;
                status =
                    NewCatalog::GetColumnVector(column_meta, column_meta.get_column_def(), row_count, ColumnVectorMode::kReadOnly, column_vector);
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }
                BruteForceBlockScan<t, ColumnDataType, C, DistanceDataType>::Execute(merge_heap,
                                                                                     dist_func,
                                                                                     knn_query_ptr,
                                                                                     query_count,
                                                                                     embedding_dim,
                                                                                     column_vector,
                                                                                     segment_id,
                                                                                     block_id,
                                                                                     row_count,
                                                                                     bitmask);
            }
            block_column_idx = knn_scan_shared_data->current_block_idx_++;
        } while (block_column_idx < brute_task_n);
    } else if (u64 index_idx = knn_scan_shared_data->current_index_idx_++; index_idx < index_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} index {}/{}", knn_scan_function_data->task_id_, index_idx + 1, index_task_n));
        // with index

        SegmentIndexMeta *segment_index_meta = knn_scan_shared_data->segment_index_metas_->at(index_idx).get();
        SegmentID segment_id = segment_index_meta->segment_id();

        const auto &segment_index_hashmap = base_table_ref_->block_index_->new_segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            UnrecoverableError(fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id));
        }

        bool has_some_result = false;
        Bitmask bitmask;
        bool use_bitmask = false;
        if (common_query_filter_->AlwaysTrue()) {
            has_some_result = true;
        } else {
            if (auto it = common_query_filter_->filter_result_.find(segment_id); it != common_query_filter_->filter_result_.end()) {
                LOG_TRACE(fmt::format("KnnScan: {} index {}/{} not skipped after common_query_filter",
                                      knn_scan_function_data->task_id_,
                                      index_idx + 1,
                                      index_task_n));
                bitmask = it->second;
                has_some_result = true;
                use_bitmask = !bitmask.IsAllTrue();
            }
        }

        auto get_chunks = [&segment_index_meta] {
            auto [chunk_ids_ptr, status] = segment_index_meta->GetChunkIDs1();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            std::shared_ptr<MemIndex> mem_index = segment_index_meta->GetMemIndex();
            return std::make_tuple(chunk_ids_ptr, mem_index);
        };

        if (has_some_result) {
            Status status;
            const IndexBase *index_base;
            std::shared_ptr<IndexBase> index_base_ptr;
            std::tie(index_base_ptr, status) = segment_index_meta->table_index_meta().GetIndexBase();
            if (!status.ok()) {
                UnrecoverableError(status.message());
            }
            index_base = index_base_ptr.get();

            switch (index_base->index_type_) {
                case IndexType::kIVF: {
                    const SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
                    const auto ivf_search_params = IVF_Search_Params::Make(knn_scan_function_data);
                    auto ivf_result_handler =
                        GetIVFSearchHandler<t, C, DistanceDataType>(ivf_search_params, use_bitmask, bitmask, max_segment_offset);
                    ivf_result_handler->Begin();
                    auto [chunk_ids_ptr, mem_index] = get_chunks();
                    for (ChunkID chunk_id : *chunk_ids_ptr) {
                        ChunkIndexMeta chunk_index_meta(chunk_id, *segment_index_meta);
                        BufferObj *index_buffer = nullptr;
                        status = chunk_index_meta.GetIndexBuffer(index_buffer);
                        if (!status.ok()) {
                            UnrecoverableError(status.message());
                        }
                        auto index_handle = index_buffer->Load();
                        const auto *ivf_chunk = static_cast<const IVFIndexInChunk *>(index_handle.GetData());
                        ivf_result_handler->Search(ivf_chunk);
                    }
                    if (mem_index) {
                        std::shared_ptr<IVFIndexInMem> memory_ivf_index = mem_index->GetIVFIndex();
                        if (memory_ivf_index != nullptr) {
                            ivf_result_handler->Search(memory_ivf_index.get());
                        }
                    }
                    auto [result_n, d_ptr, offset_ptr] = ivf_result_handler->EndWithoutSort();
                    auto row_ids = std::make_unique_for_overwrite<RowID[]>(result_n);
                    for (size_t i = 0; i < result_n; ++i) {
                        row_ids[i] = RowID{segment_id, offset_ptr[i]};
                    }
                    merge_heap->Search(0, d_ptr.get(), row_ids.get(), result_n);
                    break;
                }
                case IndexType::kHnsw: {
                    ExecuteHnswSearch<t, ColumnDataType, C, DistanceDataType>(query_context,
                                                                              knn_scan_operator_state,
                                                                              merge_heap_hnsw,
                                                                              dist_func,
                                                                              embedding_dim,
                                                                              segment_id,
                                                                              segment_index_meta,
                                                                              block_index,
                                                                              knn_column_id,
                                                                              use_bitmask,
                                                                              bitmask);
                    break;
                }
                default: {
                    RecoverableError(Status::NotSupport("Not implemented index type"));
                }
            }
        }
    }
    if (knn_scan_shared_data->current_index_idx_ >= index_task_n && knn_scan_shared_data->current_block_idx_ >= brute_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} task finished", knn_scan_function_data->task_id_));
        // all task Complete

        size_t query_n = knn_scan_shared_data->query_count_;
        std::vector<char *> result_dists_list;
        std::vector<RowID *> row_ids_list;

        if (query_n > 1) {
            merge_heap_hnsw->EndWithoutSort();
            std::vector<RowID> unique_row_ids = merge_heap_hnsw->GetUniqueIDs();
            for (size_t i = 0; i < unique_row_ids.size(); ++i) {
                SegmentID segment_id = unique_row_ids[i].segment_id_;
                SegmentOffset segment_offset = unique_row_ids[i].segment_offset_;
                BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;

                TableMeta *table_meta = base_table_ref_->block_index_->table_meta_.get();
                SegmentMeta segment_meta(segment_id, *table_meta);
                BlockMeta block_meta(block_id, segment_meta);
                ColumnMeta column_meta(knn_column_id, block_meta);
                ColumnVector column_vector;
                Status status = NewCatalog::GetColumnVector(column_meta,
                                                            column_meta.get_column_def(),
                                                            DEFAULT_BLOCK_CAPACITY,
                                                            ColumnVectorMode::kReadOnly,
                                                            column_vector);
                if (!status.ok()) {
                    UnrecoverableError(status.message());
                }

                MultiVectorSearchOneLine<ColumnDataType, C, DistanceDataType>(merge_heap,
                                                                              dist_func,
                                                                              knn_query_ptr,
                                                                              query_n,
                                                                              embedding_dim,
                                                                              column_vector,
                                                                              segment_id,
                                                                              segment_offset,
                                                                              block_offset);
            }
        }

        merge_heap->End();
        i64 result_n = merge_heap->GetSize();

        result_dists_list.emplace_back(reinterpret_cast<char *>(merge_heap->GetDistancesByIdx(0)));
        row_ids_list.emplace_back(merge_heap->GetIDsByIdx(0));

        this->SetOutput(result_dists_list, row_ids_list, sizeof(DistanceDataType), result_n, query_context, knn_scan_operator_state);
        knn_scan_operator_state->SetComplete();
    }
}

template <typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
struct BruteForceBlockScan<LogicalType::kEmbedding, ColumnDataType, C, DistanceDataType> {
    static void Execute(MergeKnn<ColumnDataType, C, DistanceDataType> *merge_heap,
                        KnnDistance1<ColumnDataType, DistanceDataType> *dist_func,
                        const ColumnDataType *knn_query_ptr,
                        const u32 query_count,
                        const u32 embedding_dim,
                        const ColumnVector &column_vector,
                        const SegmentID segment_id,
                        const BlockID block_id,
                        const BlockOffset row_count,
                        const Bitmask &bitmask) {
        const ColumnDataType *target_ptr = reinterpret_cast<const ColumnDataType *>(column_vector.data());
        merge_heap->Search(knn_query_ptr, target_ptr, embedding_dim, dist_func->dist_func_, row_count, segment_id, block_id, bitmask);
    }
};

template <typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
struct BruteForceBlockScan<LogicalType::kMultiVector, ColumnDataType, C, DistanceDataType> {
    static void Execute(MergeKnn<ColumnDataType, C, DistanceDataType> *merge_heap,
                        KnnDistance1<ColumnDataType, DistanceDataType> *dist_func,
                        const ColumnDataType *knn_query_ptr,
                        const u32 query_count,
                        const u32 embedding_dim,
                        const ColumnVector &column_vector,
                        const SegmentID segment_id,
                        const BlockID block_id,
                        const BlockOffset row_count,
                        const Bitmask &bitmask) {
        for (BlockOffset row_id = 0; row_id < row_count; ++row_id) {
            if (bitmask.IsTrue(row_id)) {
                SegmentOffset segment_offset = block_id * DEFAULT_BLOCK_CAPACITY + row_id;
                MultiVectorSearchOneLine<ColumnDataType, C, DistanceDataType>(merge_heap,
                                                                              dist_func,
                                                                              knn_query_ptr,
                                                                              query_count,
                                                                              embedding_dim,
                                                                              column_vector,
                                                                              segment_id,
                                                                              segment_offset,
                                                                              row_id);
            }
        }
    }
};

template <typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
void MultiVectorSearchOneLine(MergeKnn<ColumnDataType, C, DistanceDataType> *merge_heap,
                              KnnDistance1<ColumnDataType, DistanceDataType> *dist_func,
                              const ColumnDataType *knn_query_ptr,
                              const u32 query_count,
                              const u32 embedding_dim,
                              const ColumnVector &column_vector,
                              const SegmentID segment_id,
                              const SegmentOffset segment_offset,
                              const BlockOffset block_offset) {
    using Compare = C<DistanceDataType, RowID>;
    const ColumnDataType *query_ptr = knn_query_ptr;
    const auto [data_span, embedding_num] = column_vector.GetMultiVectorRaw(block_offset);
    DistanceDataType result_dist = 0;
    for (size_t i = 0; i < query_count; i++) {
        auto query_dist = Compare::InitialValue();
        const ColumnDataType *target_ptr = reinterpret_cast<const ColumnDataType *>(data_span.data());
        for (u32 j = 0; j < embedding_num; ++j) {
            const auto new_dist = dist_func->dist_func_(query_ptr, target_ptr, embedding_dim);
            static_assert(std::is_same_v<decltype(result_dist), std::decay_t<decltype(new_dist)>>);
            query_dist = Compare::Compare(query_dist, new_dist) ? new_dist : query_dist;
            target_ptr += embedding_dim;
        }
        result_dist += query_dist;
        query_ptr += embedding_dim;
    }
    const RowID db_row_id(segment_id, segment_offset);
    merge_heap->Search(0, &result_dist, &db_row_id, 1);
}

// HNSW search function extracted for reuse
template <LogicalType t, typename ColumnDataType, template <typename, typename> typename C, typename DistanceDataType>
void ExecuteHnswSearch(QueryContext *query_context,
                       KnnScanOperatorState *knn_scan_operator_state,
                       MergeKnn<ColumnDataType, C, DistanceDataType> *merge_heap,
                       KnnDistance1<ColumnDataType, DistanceDataType> *dist_func,
                       u32 embedding_dim,
                       SegmentID segment_id,
                       SegmentIndexMeta *segment_index_meta,
                       BlockIndex *block_index,
                       ColumnID knn_column_id,
                       bool use_bitmask,
                       const Bitmask &bitmask) {

    auto knn_scan_function_data = knn_scan_operator_state->knn_scan_function_data_.get();
    auto knn_scan_shared_data = knn_scan_function_data->knn_scan_shared_data_;

    auto get_chunks = [&]() -> std::pair<std::unique_ptr<std::vector<ChunkID>>, std::shared_ptr<MemIndex>> {
        std::shared_ptr<MemIndex> mem_index = segment_index_meta->GetMemIndex();
        auto [chunk_ids_ptr, status] = segment_index_meta->GetChunkIDs1();
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        // Create a copy of the chunk IDs instead of transferring ownership of the original pointer
        return {std::make_unique<std::vector<ChunkID>>(*chunk_ids_ptr), mem_index};
    };

    Status status;
    if constexpr (!(IsAnyOf<ColumnDataType, u8, i8, f32>)) {
        UnrecoverableError("Invalid data type");
        return;
    }

    auto hnsw_search = [&](const HnswHandlerPtr hnsw_handler, bool with_lock) {
        KnnSearchOption search_option;
        search_option.column_logical_type_ = t;
        for (const auto &opt_param : knn_scan_shared_data->opt_params_) {
            if (opt_param.param_name_ == "ef") {
                u64 ef = std::stoull(opt_param.param_value_);
                search_option.ef_ = ef;
            }
        }

        i64 result_n = -1;
        for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
            const auto *query = static_cast<const ColumnDataType *>(knn_scan_shared_data->query_embedding_) + query_idx * embedding_dim;

            size_t result_n1 = 0;
            std::unique_ptr<DistanceDataType[]> d_ptr = nullptr;
            std::unique_ptr<SegmentOffset[]> l_ptr = nullptr;
            if (use_bitmask) {
                BitmaskFilter<SegmentOffset> filter(bitmask);
                if (with_lock) {
                    std::tie(result_n1, d_ptr, l_ptr) =
                        hnsw_handler->SearchIndex<DistanceDataType, SegmentOffset, BitmaskFilter<SegmentOffset>, true>(query,
                                                                                                                       knn_scan_shared_data->topk_,
                                                                                                                       filter,
                                                                                                                       search_option);
                } else {
                    std::tie(result_n1, d_ptr, l_ptr) =
                        hnsw_handler->SearchIndex<DistanceDataType, SegmentOffset, BitmaskFilter<SegmentOffset>, false>(query,
                                                                                                                        knn_scan_shared_data->topk_,
                                                                                                                        filter,
                                                                                                                        search_option);
                }
            } else {
                if (!with_lock) {
                    std::tie(result_n1, d_ptr, l_ptr) =
                        hnsw_handler->SearchIndex<DistanceDataType, SegmentOffset, false>(query, knn_scan_shared_data->topk_, search_option);
                } else {
                    SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
                    AppendFilter filter(max_segment_offset);
                    std::tie(result_n1, d_ptr, l_ptr) =
                        hnsw_handler->SearchIndex<DistanceDataType, SegmentOffset, AppendFilter, true>(query,
                                                                                                       knn_scan_shared_data->topk_,
                                                                                                       filter,
                                                                                                       search_option);
                }
            }

            if (result_n < 0) {
                result_n = result_n1;
            } else if (result_n != static_cast<i64>(result_n1)) {
                UnrecoverableError("KnnScan: result_n mismatch");
            }

            switch (knn_scan_shared_data->knn_distance_type_) {
                case KnnDistanceType::kInvalid: {
                    UnrecoverableError("Invalid distance type");
                }
                case KnnDistanceType::kL2:
                    [[fallthrough]];
                case KnnDistanceType::kHamming: {
                    break;
                }
                // FIXME:
                case KnnDistanceType::kCosine:
                    [[fallthrough]];
                case KnnDistanceType::kInnerProduct: {
                    for (i64 i = 0; i < result_n; ++i) {
                        d_ptr[i] = -d_ptr[i];
                    }
                    break;
                }
            }

            auto row_ids = std::make_unique_for_overwrite<RowID[]>(result_n);
            for (i64 i = 0; i < result_n; ++i) {
                row_ids[i] = RowID{segment_id, l_ptr[i]};
            }

            merge_heap->Search(query_idx, d_ptr.get(), row_ids.get(), result_n);
        }
    };
    auto [chunk_ids_ptr, mem_index] = get_chunks();
    for (ChunkID chunk_id : *chunk_ids_ptr) {
        ChunkIndexMeta chunk_index_meta(chunk_id, *segment_index_meta);
        BufferObj *index_buffer = nullptr;
        status = chunk_index_meta.GetIndexBuffer(index_buffer);
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }
        auto index_handle = index_buffer->Load();
        const auto *hnsw_handler = reinterpret_cast<const HnswHandlerPtr *>(index_handle.GetData());
        hnsw_search(*hnsw_handler, false);
    }
    if (mem_index) {
        std::shared_ptr<HnswIndexInMem> memory_hnsw_index = mem_index->GetHnswIndex();
        if (memory_hnsw_index) {
            const HnswHandlerPtr hnsw_handler = memory_hnsw_index->get();
            hnsw_search(hnsw_handler, true);
        }
    }
}

} // namespace infinity
