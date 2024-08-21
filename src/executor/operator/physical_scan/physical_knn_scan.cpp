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

#include <cstdlib>
#include <memory>
#include <string>

module physical_knn_scan;

import stl;
import query_context;
import operator_state;
import physical_operator_type;
import base_table_ref;
import table_entry;
import block_column_entry;
import segment_index_entry;
import load_meta;
import knn_expression;
import data_type;
import common_query_filter;
import physical_filter_scan_base;
import logical_type;
import internal_types;
import logger;
import block_index;
import knn_scan_data;
import knn_filter;
import knn_distance;
import third_party;
import txn;
import infinity_exception;
import default_values;
import column_expression;
import embedding_info;
import buffer_manager;
import merge_knn;
import knn_result_handler;
import ann_ivf_flat;
import annivfflat_index_data;
import buffer_handle;
import data_block;
import bitmask;
import column_vector;
import index_hnsw;
import status;
import create_index_info;
import knn_expr;
import block_entry;
import segment_entry;
import abstract_hnsw;
import physical_match_tensor_scan;
import hnsw_alg;

namespace infinity {

auto GetKnnExprForCalculation(const KnnExpression &src_knn_expr, const EmbeddingDataType column_embedding_type) {
    // check column basic embedding data type and query embedding data type
    // apply necessary cast
    const auto src_query_embedding_type = src_knn_expr.embedding_data_type_;
    EmbeddingDataType new_query_embedding_type = EmbeddingDataType::kElemInvalid;
    switch (column_embedding_type) {
        case EmbeddingDataType::kElemUInt8:
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
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64: {
            // TODO: not supported yet
            RecoverableError(Status::NotSupport(fmt::format("Cannot query on column with basic embedding data type: {}.",
                                                            EmbeddingInfo::EmbeddingDataTypeToString(column_embedding_type))));
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
    Pair<std::unique_ptr<void, decltype([](void *ptr) { std::free(ptr); })>, EmbeddingDataType> result = {nullptr, EmbeddingDataType::kElemInvalid};
    if (new_query_embedding_type != EmbeddingDataType::kElemInvalid) {
        const auto aligned_ptr =
            GetAlignedCast(src_knn_expr.query_embedding_.ptr, src_knn_expr.dimension_, src_query_embedding_type, new_query_embedding_type);
        result.first.reset(aligned_ptr);
        result.second = new_query_embedding_type;
    }
    return result;
}

void PhysicalKnnScan::Init() {
    KnnExpression *knn_expr = knn_expression_.get();
    const auto *column_expr = static_cast<const ColumnExpression *>(knn_expr->arguments()[0].get());
    const auto column_data_type = column_expr->Type();
    switch (column_data_type.type()) {
        case LogicalType::kEmbedding:
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
    if (auto [new_query_embedding_ptr, new_query_embedding_type] = GetKnnExprForCalculation(*knn_expr, column_elem_type);
        new_query_embedding_type != EmbeddingDataType::kElemInvalid) {
        // have new query ptr
        const auto new_query_embedding_ptr_ = new_query_embedding_ptr.release();
        real_knn_query_embedding_holder_ = SharedPtr<void>(new_query_embedding_ptr_, std::free);
        real_knn_query_embedding_ptr_ = new_query_embedding_ptr_;
        real_knn_query_elem_type_ = new_query_embedding_type;
    } else {
        real_knn_query_embedding_ptr_ = knn_expr->query_embedding_.ptr;
        real_knn_query_elem_type_ = knn_expr->embedding_data_type_;
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

SizeT PhysicalKnnScan::BlockScanTaskCount() const {
    const u32 block_cnt = block_column_entries_size_;
    SizeT brute_task_n = 1;
    for (const auto &[block_n, job_n] : block_parallel_options_) {
        if (block_cnt < block_n) {
            break;
        }
        brute_task_n = job_n;
    }
    return brute_task_n;
}

SizeT PhysicalKnnScan::TaskletCount() { return BlockScanTaskCount() + index_entries_size_; }

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
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64:
        case EmbeddingDataType::kElemInvalid: {
            UnrecoverableError(
                fmt::format("KnnScan: Execute(): Invalid embedding data type {}.", EmbeddingT::EmbeddingDataType2String(column_elem_type_)));
        }
    }
}

template <LogicalType t, typename ColumnDataType, typename QueryDataType, template <typename, typename> typename C, typename DistanceDataType>
struct ExecuteDispatchHelper {
    static void Execute(PhysicalKnnScan *knn_op, QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
        UnrecoverableError("Unexpected ColumnDataType and QueryDataType type match.");
    }
};

template <LogicalType t, typename ColumnDataType, typename QueryDataType, template <typename, typename> typename C, typename DistanceDataType>
    requires(((IsAnyOf<ColumnDataType, u8, i8> && std::same_as<ColumnDataType, QueryDataType>) ||
              (IsAnyOf<ColumnDataType, f32, f64, Float16T, BFloat16T> && std::same_as<QueryDataType, f32>)) &&
             std::same_as<DistanceDataType, f32>)
struct ExecuteDispatchHelper<t, ColumnDataType, QueryDataType, C, DistanceDataType> {
    static void Execute(PhysicalKnnScan *knn_op, QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
        return knn_op->ExecuteInternalByColumnDataTypeAndQueryDataType<t, ColumnDataType, QueryDataType, C, DistanceDataType>(
            query_context,
            knn_scan_operator_state);
    }
};

template <LogicalType t, typename ColumnDataT>
void PhysicalKnnScan::ExecuteInternalByColumnDataType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
    static_assert(IsAnyOf<ColumnDataT, u8, i8, f32, f64, Float16T, BFloat16T>, "Unexpected ColumnDataT type.");
    const auto query_elem_type = knn_scan_operator_state->knn_scan_function_data_->knn_scan_shared_data_->query_elem_type_;
    const auto query_dist_type = knn_scan_operator_state->knn_scan_function_data_->knn_scan_shared_data_->knn_distance_type_;
    auto knn_distance_error = [query_dist_type] {
        RecoverableError(Status::NotSupport(fmt::format("Not implemented KNN distance: {}", KnnExpression::KnnDistanceType2Str(query_dist_type))));
    };
    switch (query_elem_type) {
        case EmbeddingDataType::kElemFloat: {
            switch (query_dist_type) {
                case KnnDistanceType::kL2:
                case KnnDistanceType::kHamming: {
                    return ExecuteDispatchHelper<t, ColumnDataT, f32, CompareMax, f32>::Execute(this, query_context, knn_scan_operator_state);
                }
                case KnnDistanceType::kCosine:
                case KnnDistanceType::kInnerProduct: {
                    return ExecuteDispatchHelper<t, ColumnDataT, f32, CompareMin, f32>::Execute(this, query_context, knn_scan_operator_state);
                }
                default: {
                    return knn_distance_error();
                }
            }
            break;
        }
        case EmbeddingDataType::kElemUInt8: {
            switch (query_dist_type) {
                case KnnDistanceType::kL2:
                case KnnDistanceType::kHamming: {
                    return ExecuteDispatchHelper<t, ColumnDataT, u8, CompareMax, f32>::Execute(this, query_context, knn_scan_operator_state);
                }
                case KnnDistanceType::kCosine:
                case KnnDistanceType::kInnerProduct: {
                    return ExecuteDispatchHelper<t, ColumnDataT, u8, CompareMin, f32>::Execute(this, query_context, knn_scan_operator_state);
                }
                default: {
                    return knn_distance_error();
                }
            }
            break;
        }
        case EmbeddingDataType::kElemInt8: {
            switch (query_dist_type) {
                case KnnDistanceType::kL2:
                case KnnDistanceType::kHamming: {
                    return ExecuteDispatchHelper<t, ColumnDataT, i8, CompareMax, f32>::Execute(this, query_context, knn_scan_operator_state);
                }
                case KnnDistanceType::kCosine:
                case KnnDistanceType::kInnerProduct: {
                    return ExecuteDispatchHelper<t, ColumnDataT, i8, CompareMin, f32>::Execute(this, query_context, knn_scan_operator_state);
                }
                default: {
                    return knn_distance_error();
                }
            }
            break;
        }
        case EmbeddingDataType::kElemFloat16:
        case EmbeddingDataType::kElemBFloat16:
        case EmbeddingDataType::kElemDouble: {
            // bug
            UnrecoverableError(fmt::format("BUG: Query embedding data type: {} should be cast to Float before knn search!",
                                           EmbeddingT::EmbeddingDataType2String(query_elem_type)));
        }
        case EmbeddingDataType::kElemBit:
        case EmbeddingDataType::kElemInt16:
        case EmbeddingDataType::kElemInt32:
        case EmbeddingDataType::kElemInt64:
        case EmbeddingDataType::kElemInvalid: {
            RecoverableError(Status::NotSupport(
                fmt::format("Not implemented query embedding data type: {}", EmbeddingT::EmbeddingDataType2String(query_elem_type))));
        }
    }
}

TableEntry *PhysicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

SizeT PhysicalKnnScan::GetColumnID() const {
    KnnExpression *knn_expr = knn_expression_.get();
    ColumnExpression *column_expr = static_cast<ColumnExpression *>(knn_expr->arguments()[0].get());
    return column_expr->binding().column_idx;
}

void PhysicalKnnScan::PlanWithIndex(QueryContext *query_context) { // TODO: return base entry vector
    InitBlockParallelOption();                                     // PlanWithIndex() will be called in physical planner
    Txn *txn = query_context->GetTxn();
    TransactionID txn_id = txn->TxnID();
    TxnTimeStamp begin_ts = txn->BeginTS();

    SizeT knn_column_id = GetColumnID();

    block_column_entries_ = MakeUnique<Vector<BlockColumnEntry *>>();
    index_entries_ = MakeUnique<Vector<SegmentIndexEntry *>>();

    TableEntry *table_entry = base_table_ref_->table_entry_ptr_;
    Map<u32, SharedPtr<SegmentIndexEntry>> index_entry_map;

    if (knn_expression_->ignore_index_) {
        LOG_TRACE("Not use index"); // No index need to check
    } else {
        auto map_guard = table_entry->IndexMetaMap();
        if (knn_expression_->using_index_.empty()) {
            LOG_TRACE("Try to find a index to use");
            for (auto &[index_name, table_index_meta] : *map_guard) {
                auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
                if (!status.ok()) {
                    // already dropped
                    LOG_WARN(status.message());
                    continue;
                }

                const String column_name = table_index_entry->index_base()->column_name();
                SizeT column_id = table_entry->GetColumnIdByName(column_name);
                if (column_id != knn_column_id) {
                    // knn_column_id isn't in this table index
                    continue;
                }
                // check index type
                if (auto index_type = table_index_entry->index_base()->index_type_;
                    index_type != IndexType::kIVFFlat and index_type != IndexType::kHnsw) {
                    LOG_TRACE(fmt::format("KnnScan: PlanWithIndex(): Skipping non-knn index."));
                    continue;
                }

                // Fill the segment with index
                index_entry_map = table_index_entry->index_by_segment();
            }
        } else {
            LOG_TRACE(fmt::format("Use index: {}", knn_expression_->using_index_));
            auto iter = (*map_guard).find(knn_expression_->using_index_);
            if (iter == (*map_guard).end()) {
                Status status = Status::IndexNotExist(knn_expression_->using_index_);
                RecoverableError(std::move(status));
            }

            auto &table_index_meta = iter->second;

            auto [table_index_entry, status] = table_index_meta->GetEntryNolock(txn_id, begin_ts);
            if (!status.ok()) {
                // already dropped
                RecoverableError(std::move(status));
            }

            const String column_name = table_index_entry->index_base()->column_name();
            SizeT column_id = table_entry->GetColumnIdByName(column_name);
            if (column_id != knn_column_id) {
                // knn_column_id isn't in this table index
                LOG_ERROR(fmt::format("Column {} not found", column_name));
                Status error_status = Status::ColumnNotExist(column_name);
                RecoverableError(std::move(error_status));
            }
            // check index type
            if (auto index_type = table_index_entry->index_base()->index_type_;
                index_type != IndexType::kIVFFlat and index_type != IndexType::kHnsw) {
                LOG_ERROR("Invalid index type");
                Status error_status = Status::InvalidIndexType();
                RecoverableError(std::move(error_status));
            }

            // Fill the segment with index
            index_entry_map = table_index_entry->index_by_segment();
        }
    }

    // Generate task set: index segment and no index block
    BlockIndex *block_index = base_table_ref_->block_index_.get();
    for (const auto &[segment_id, segment_info] : block_index->segment_block_index_) {
        if (auto iter = index_entry_map.find(segment_id); iter != index_entry_map.end()) {
            index_entries_->emplace_back(iter->second.get());
        } else {
            const auto &block_map = segment_info.block_map_;
            for (const auto *block_entry : block_map) {
                BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(knn_column_id);
                block_column_entries_->emplace_back(block_column_entry);
            }
        }
    }
    block_column_entries_size_ = block_column_entries_->size();
    index_entries_size_ = index_entries_->size();
    LOG_TRACE(fmt::format("KnnScan: brute force task: {}, index task: {}", block_column_entries_size_, index_entries_size_));
}

SizeT PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

template <LogicalType t, typename ColumnDataType, typename QueryDataType, template <typename, typename> typename C, typename DistanceDataType>
struct BruteForceBlockScan;

template <LogicalType t, typename ColumnDataType, typename QueryDataType, template <typename, typename> typename C, typename DistanceDataType>
void PhysicalKnnScan::ExecuteInternalByColumnDataTypeAndQueryDataType(QueryContext *query_context, KnnScanOperatorState *knn_scan_operator_state) {
    // knn expr output data type is always f32
    static_assert(std::is_same_v<DistanceDataType, f32>);
    // allowed match:
    // ColumnDataType: u8, QueryDataType: u8
    // ColumnDataType: i8, QueryDataType: i8
    // ColumnDataType: f32, QueryDataType: f32
    // ColumnDataType: f64, float16, bfloat16, QueryDataType: f32
    static_assert((IsAnyOf<ColumnDataType, u8, i8> && std::is_same_v<ColumnDataType, QueryDataType>) ||
                      (IsAnyOf<ColumnDataType, f32, f64, Float16T, BFloat16T> && std::is_same_v<QueryDataType, f32>),
                  "Unexpected ColumnDataType and QueryDataType type match.");

    Txn *txn = query_context->GetTxn();
    TxnTimeStamp begin_ts = txn->BeginTS();

    if (!common_query_filter_->TryFinishBuild(txn)) {
        // not ready, abort and wait for next time
        return;
    }

    auto knn_scan_function_data = knn_scan_operator_state->knn_scan_function_data_.get();
    auto knn_scan_shared_data = knn_scan_function_data->knn_scan_shared_data_;

    auto dist_func = dynamic_cast<KnnDistance1<QueryDataType, DistanceDataType> *>(knn_scan_function_data->knn_distance_.get());
    auto merge_heap = dynamic_cast<MergeKnn<QueryDataType, C, DistanceDataType> *>(knn_scan_function_data->merge_knn_base_.get());
    auto knn_query_ptr = static_cast<const QueryDataType *>(knn_scan_shared_data->query_embedding_);
    const auto embedding_dim = knn_scan_shared_data->dimension_;
    if (!dist_func || !merge_heap) {
        const auto err = "Invalid dynamic cast";
        LOG_ERROR(err);
        UnrecoverableError(err);
    }

    const SizeT index_task_n = knn_scan_shared_data->index_entries_->size();
    const SizeT brute_task_n = knn_scan_shared_data->block_column_entries_->size();
    BlockIndex *block_index = knn_scan_shared_data->table_ref_->block_index_.get();
    BufferManager *buffer_mgr = query_context->storage()->buffer_manager();
    SizeT knn_column_id = GetColumnID();

    if (u64 block_column_idx =
            knn_scan_function_data->execute_block_scan_job_ ? knn_scan_shared_data->current_block_idx_++ : std::numeric_limits<u64>::max();
        block_column_idx < brute_task_n) {
        LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{}", knn_scan_function_data->task_id_, block_column_idx + 1, brute_task_n));
        // brute force
        // TODO: now will try to finish all block scan job in the task
        UniquePtr<QueryDataType[]> buffer_ptr_for_cast;
        do {
            BlockColumnEntry *block_column_entry = knn_scan_shared_data->block_column_entries_->at(block_column_idx);
            const BlockEntry *block_entry = block_column_entry->block_entry();
            const auto block_id = block_entry->block_id();
            const SegmentID segment_id = block_entry->GetSegmentEntry()->segment_id();
            const auto row_count = block_entry->row_count();
            Bitmask bitmask;
            if (this->CalculateFilterBitmask(segment_id, block_id, row_count, bitmask)) {
                // LOG_TRACE(fmt::format("KnnScan: {} brute force {}/{} not skipped after common_query_filter",
                //                       knn_scan_function_data->task_id_,
                //                       block_column_idx + 1,
                //                       brute_task_n));
                block_entry->SetDeleteBitmask(begin_ts, bitmask);
                ColumnVector column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);
                BruteForceBlockScan<t, ColumnDataType, QueryDataType, C, DistanceDataType>::Execute(merge_heap,
                                                                                                    dist_func,
                                                                                                    knn_query_ptr,
                                                                                                    embedding_dim,
                                                                                                    buffer_ptr_for_cast,
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
        SegmentIndexEntry *segment_index_entry = knn_scan_shared_data->index_entries_->at(index_idx);

        auto segment_id = segment_index_entry->segment_id();
        SegmentEntry *segment_entry = nullptr;
        const auto &segment_index_hashmap = base_table_ref_->block_index_->segment_block_index_;
        if (auto iter = segment_index_hashmap.find(segment_id); iter == segment_index_hashmap.end()) {
            String error_message = fmt::format("Cannot find SegmentEntry for segment id: {}", segment_id);
            UnrecoverableError(error_message);
        } else {
            segment_entry = iter->second.segment_entry_;
        }

        bool has_some_result = false;
        Bitmask bitmask;
        bool use_bitmask = false;
        if (common_query_filter_->AlwaysTrue()) {
            has_some_result = true;
            bitmask.SetAllTrue();
        } else {
            auto it = common_query_filter_->filter_result_.find(segment_id);
            if (it != common_query_filter_->filter_result_.end()) {
                LOG_TRACE(fmt::format("KnnScan: {} index {}/{} not skipped after common_query_filter",
                                      knn_scan_function_data->task_id_,
                                      index_idx + 1,
                                      index_task_n));
                auto segment_row_count = segment_entry->row_count();
                const std::variant<Vector<u32>, Bitmask> &filter_result = it->second;
                if (std::holds_alternative<Vector<u32>>(filter_result)) {
                    const Vector<u32> &filter_result_vector = std::get<Vector<u32>>(filter_result);
                    bitmask.Initialize(std::ceil(segment_row_count));
                    bitmask.SetAllFalse();
                    for (u32 row_id : filter_result_vector) {
                        bitmask.SetTrue(row_id);
                    }
                } else {
                    bitmask.ShallowCopy(std::get<Bitmask>(filter_result));
                }
                has_some_result = true;
                use_bitmask = !bitmask.IsAllTrue();
            }
        }

        if (has_some_result) {
            switch (segment_index_entry->table_index_entry()->index_base()->index_type_) {
                case IndexType::kIVFFlat: {
                    if constexpr (!(t == LogicalType::kEmbedding && std::is_same_v<ColumnDataType, f32>)) {
                        UnrecoverableError("Invalid data type");
                    } else {
                        BufferHandle index_handle = segment_index_entry->GetIndex();
                        auto index = static_cast<const AnnIVFFlatIndexData<ColumnDataType> *>(index_handle.GetData());
                        i32 n_probes = 1;
                        auto IVFFlatScanTemplate = [&]<typename AnnIVFFlatType, typename... OptionalFilter>(OptionalFilter &&...filter) {
                            AnnIVFFlatType ann_ivfflat_query(knn_query_ptr,
                                                             knn_scan_shared_data->query_count_,
                                                             knn_scan_shared_data->topk_,
                                                             knn_scan_shared_data->dimension_,
                                                             knn_scan_shared_data->query_elem_type_);
                            ann_ivfflat_query.Begin();
                            ann_ivfflat_query.Search(index, segment_id, n_probes, std::forward<OptionalFilter>(filter)...);
                            ann_ivfflat_query.EndWithoutSort();
                            auto dists = ann_ivfflat_query.GetDistances();
                            auto row_ids = ann_ivfflat_query.GetIDs();
                            // TODO: now only work for one query
                            // FIXME: cant work for multiple queries
                            auto result_count = std::lower_bound(dists,
                                                                 dists + knn_scan_shared_data->topk_,
                                                                 AnnIVFFlatType::InvalidValue(),
                                                                 AnnIVFFlatType::CompareDist) -
                                                dists;
                            merge_heap->Search(dists, row_ids, result_count);
                        };
                        auto IVFFlatScan = [&]<typename... OptionalFilter>(OptionalFilter &&...filter) {
                            switch (knn_scan_shared_data->knn_distance_type_) {
                                case KnnDistanceType::kL2: {
                                    IVFFlatScanTemplate.template operator()<AnnIVFFlatL2<ColumnDataType>>(std::forward<OptionalFilter>(filter)...);
                                    break;
                                }
                                case KnnDistanceType::kInnerProduct: {
                                    IVFFlatScanTemplate.template operator()<AnnIVFFlatIP<ColumnDataType>>(std::forward<OptionalFilter>(filter)...);
                                    break;
                                }
                                case KnnDistanceType::kCosine: {
                                    IVFFlatScanTemplate.template operator()<AnnIVFFlatCOS<ColumnDataType>>(std::forward<OptionalFilter>(filter)...);
                                    break;
                                }
                                default: {
                                    Status status = Status::NotSupport("Not implemented KNN distance");
                                    RecoverableError(status);
                                }
                            }
                        };
                        if (use_bitmask) {
                            BitmaskFilter<SegmentOffset> filter(bitmask);
                            IVFFlatScan(filter);
                        } else {
                            IVFFlatScan();
                        }
                    }
                    break;
                }
                case IndexType::kHnsw: {
                    if constexpr (!(t == LogicalType::kEmbedding && IsAnyOf<ColumnDataType, u8, i8, f32> &&
                                    std::is_same_v<ColumnDataType, QueryDataType>)) {
                        UnrecoverableError("Invalid data type");
                    } else {
                        auto hnsw_search = [&](auto *hnsw_index, bool with_lock) {
                            bool rerank = false;
                            KnnSearchOption search_option;
                            for (const auto &opt_param : knn_scan_shared_data->opt_params_) {
                                if (opt_param.param_name_ == "ef") {
                                    u64 ef = std::stoull(opt_param.param_value_);
                                    search_option.ef_ = ef;
                                } else if (opt_param.param_name_ == "rerank") {
                                    rerank = true;
                                }
                            }

                            i64 result_n = -1;
                            for (u64 query_idx = 0; query_idx < knn_scan_shared_data->query_count_; ++query_idx) {
                                const auto *query = static_cast<const QueryDataType *>(knn_scan_shared_data->query_embedding_) +
                                                    query_idx * knn_scan_shared_data->dimension_;

                                SizeT result_n1 = 0;
                                UniquePtr<DistanceDataType[]> d_ptr = nullptr;
                                UniquePtr<SegmentOffset[]> l_ptr = nullptr;
                                if (use_bitmask) {
                                    BitmaskFilter<SegmentOffset> filter(bitmask);
                                    if (with_lock) {
                                        std::tie(result_n1, d_ptr, l_ptr) =
                                            hnsw_index->template KnnSearch<BitmaskFilter<SegmentOffset>, true>(query,
                                                                                                               knn_scan_shared_data->topk_,
                                                                                                               filter,
                                                                                                               search_option);
                                    } else {
                                        std::tie(result_n1, d_ptr, l_ptr) =
                                            hnsw_index->template KnnSearch<BitmaskFilter<SegmentOffset>, false>(query,
                                                                                                                knn_scan_shared_data->topk_,
                                                                                                                filter,
                                                                                                                search_option);
                                    }
                                } else {
                                    SegmentOffset max_segment_offset = block_index->GetSegmentOffset(segment_id);
                                    if (!with_lock) {
                                        std::tie(result_n1, d_ptr, l_ptr) =
                                            hnsw_index->template KnnSearch<false>(query, knn_scan_shared_data->topk_, search_option);
                                    } else {
                                        AppendFilter filter(max_segment_offset);
                                        std::tie(result_n1, d_ptr, l_ptr) =
                                            hnsw_index->template KnnSearch<AppendFilter, true>(query,
                                                                                               knn_scan_shared_data->topk_,
                                                                                               filter,
                                                                                               search_option);
                                    }
                                }

                                if (result_n < 0) {
                                    result_n = result_n1;
                                } else if (result_n != (i64)result_n1) {
                                    String error_message = "KnnScan: result_n mismatch";
                                    UnrecoverableError(error_message);
                                }

                                if (rerank && t == LogicalType::kEmbedding) { // TODO: rerank for multivector
                                    Vector<SizeT> idxes(result_n);
                                    std::iota(idxes.begin(), idxes.end(), 0);
                                    std::sort(idxes.begin(), idxes.end(), [&](SizeT i, SizeT j) {
                                        return l_ptr[i] < l_ptr[j];
                                    }); // sort by segment offset
                                    BlockID prev_block_id = -1;
                                    ColumnVector column_vector;
                                    for (SizeT idx : idxes) {
                                        SegmentOffset segment_offset = l_ptr[idx];
                                        BlockID block_id = segment_offset / DEFAULT_BLOCK_CAPACITY;
                                        BlockOffset block_offset = segment_offset % DEFAULT_BLOCK_CAPACITY;
                                        if (block_id != prev_block_id) {
                                            prev_block_id = block_id;
                                            BlockEntry *block_entry = block_index->GetBlockEntry(segment_id, block_id);
                                            BlockColumnEntry *block_column_entry = block_entry->GetColumnBlockEntry(knn_column_id);
                                            column_vector = block_column_entry->GetConstColumnVector(buffer_mgr);
                                        }
                                        const auto *data = reinterpret_cast<const ColumnDataType *>(column_vector.data());
                                        data += block_offset * knn_scan_shared_data->dimension_;
                                        merge_heap->Search(query,
                                                           data,
                                                           knn_scan_shared_data->dimension_,
                                                           dist_func->dist_func_,
                                                           segment_id,
                                                           segment_offset);
                                    }
                                } else {
                                    switch (knn_scan_shared_data->knn_distance_type_) {
                                        case KnnDistanceType::kInvalid: {
                                            String error_message = "Invalid distance type";
                                            UnrecoverableError(error_message);
                                        }
                                        case KnnDistanceType::kL2:
                                        case KnnDistanceType::kHamming: {
                                            break;
                                        }
                                        // FIXME:
                                        case KnnDistanceType::kCosine:
                                        case KnnDistanceType::kInnerProduct: {
                                            for (i64 i = 0; i < result_n; ++i) {
                                                d_ptr[i] = -d_ptr[i];
                                            }
                                            break;
                                        }
                                    }

                                    auto row_ids = MakeUniqueForOverwrite<RowID[]>(result_n);
                                    for (i64 i = 0; i < result_n; ++i) {
                                        row_ids[i] = RowID{segment_id, l_ptr[i]};
                                    }

                                    merge_heap->Search(0, d_ptr.get(), row_ids.get(), result_n);
                                }
                            }
                        };
                        auto abstract_hnsw_search = [&](const AbstractHnsw &abstract_hnsw, bool with_lock) {
                            std::visit(
                                [&](auto &&arg) {
                                    using T = std::decay_t<decltype(arg)>;
                                    if constexpr (std::is_same_v<T, std::nullptr_t>) {
                                        UnrecoverableError("Invalid index type");
                                    } else if constexpr (!std::is_same_v<ColumnDataType, typename std::remove_pointer_t<T>::DataType>) {
                                        UnrecoverableError("Invalid data type");
                                    } else {
                                        hnsw_search(arg, with_lock);
                                    }
                                },
                                abstract_hnsw);
                        };

                        auto [chunk_index_entries, memory_hnsw_index] = segment_index_entry->GetHnswIndexSnapshot();
                        for (auto &chunk_index_entry : chunk_index_entries) {
                            if (chunk_index_entry->CheckVisible(txn)) {
                                BufferHandle index_handle = chunk_index_entry->GetIndex();
                                const auto *abstract_hnsw = reinterpret_cast<const AbstractHnsw *>(index_handle.GetData());
                                abstract_hnsw_search(*abstract_hnsw, false);
                            }
                        }
                        if (memory_hnsw_index.get() != nullptr) {
                            const AbstractHnsw &abstract_hnsw = memory_hnsw_index->get();
                            abstract_hnsw_search(abstract_hnsw, true);
                        }
                    }
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

        merge_heap->End();
        i64 result_n = std::min(knn_scan_shared_data->topk_, merge_heap->total_count());

        SizeT query_n = knn_scan_shared_data->query_count_;
        Vector<char *> result_dists_list;
        Vector<RowID *> row_ids_list;
        for (SizeT query_id = 0; query_id < query_n; ++query_id) {
            result_dists_list.emplace_back(reinterpret_cast<char *>(merge_heap->GetDistancesByIdx(query_id)));
            row_ids_list.emplace_back(merge_heap->GetIDsByIdx(query_id));
        }

        this->SetOutput(result_dists_list, row_ids_list, sizeof(DistanceDataType), result_n, query_context, knn_scan_operator_state);
        knn_scan_operator_state->SetComplete();
    }
}

template <typename ColumnDataType, typename QueryDataType, template <typename, typename> typename C, typename DistanceDataType>
struct BruteForceBlockScan<LogicalType::kEmbedding, ColumnDataType, QueryDataType, C, DistanceDataType> {
    static void Execute(MergeKnn<QueryDataType, C, DistanceDataType> *merge_heap,
                        KnnDistance1<QueryDataType, DistanceDataType> *dist_func,
                        const QueryDataType *knn_query_ptr,
                        const u32 embedding_dim,
                        UniquePtr<QueryDataType[]> &buffer_ptr_for_cast,
                        const ColumnVector &column_vector,
                        const SegmentID segment_id,
                        const BlockID block_id,
                        const BlockOffset row_count,
                        const Bitmask &bitmask) {
        auto data = reinterpret_cast<const ColumnDataType *>(column_vector.data());
        const QueryDataType *target_ptr = nullptr;
        if constexpr (std::is_same_v<ColumnDataType, QueryDataType>) {
            target_ptr = data;
        } else {
            if (!buffer_ptr_for_cast) {
                buffer_ptr_for_cast = MakeUniqueForOverwrite<QueryDataType[]>(DEFAULT_BLOCK_CAPACITY * embedding_dim);
            }
            const SizeT total_elem_num = row_count * embedding_dim;
            for (SizeT i = 0; i < total_elem_num; ++i) {
                buffer_ptr_for_cast[i] = static_cast<QueryDataType>(data[i]);
            }
            target_ptr = buffer_ptr_for_cast.get();
        }
        merge_heap->Search(knn_query_ptr, target_ptr, embedding_dim, dist_func->dist_func_, row_count, segment_id, block_id, bitmask);
    }
};

template <typename ColumnDataType, typename QueryDataType, template <typename, typename> typename C, typename DistanceDataType>
struct BruteForceBlockScan<LogicalType::kMultiVector, ColumnDataType, QueryDataType, C, DistanceDataType> {
    static void Execute(MergeKnn<QueryDataType, C, DistanceDataType> *merge_heap,
                        KnnDistance1<QueryDataType, DistanceDataType> *dist_func,
                        const QueryDataType *knn_query_ptr,
                        const u32 embedding_dim,
                        UniquePtr<QueryDataType[]> &buffer_ptr_for_cast,
                        const ColumnVector &column_vector,
                        const SegmentID segment_id,
                        const BlockID block_id,
                        const BlockOffset row_count,
                        const Bitmask &bitmask) {
        const QueryDataType *target_ptr = nullptr;
        if constexpr (!std::is_same_v<ColumnDataType, QueryDataType>) {
            if (!buffer_ptr_for_cast) {
                buffer_ptr_for_cast = MakeUniqueForOverwrite<QueryDataType[]>(embedding_dim);
            }
            target_ptr = buffer_ptr_for_cast.get();
        }
        using Compare = C<DistanceDataType, RowID>;
        for (BlockOffset row_id = 0; row_id < row_count; ++row_id) {
            if (bitmask.IsTrue(row_id)) {
                const auto [data_span, embedding_num] = column_vector.GetMultiVectorRaw(row_id);
                auto result_dist = Compare::InitialValue();
                auto raw_data_ptr = reinterpret_cast<const ColumnDataType *>(data_span.data());
                for (u32 i = 0; i < embedding_num; ++i) {
                    if constexpr (!std::is_same_v<ColumnDataType, QueryDataType>) {
                        for (u32 j = 0; j < embedding_dim; ++j) {
                            buffer_ptr_for_cast[j] = static_cast<QueryDataType>(raw_data_ptr[j]);
                        }
                    } else {
                        target_ptr = raw_data_ptr;
                    }
                    const auto new_dist = dist_func->dist_func_(knn_query_ptr, target_ptr, embedding_dim);
                    static_assert(std::is_same_v<decltype(result_dist), std::decay_t<decltype(new_dist)>>);
                    result_dist = Compare::Compare(result_dist, new_dist) ? new_dist : result_dist;
                    raw_data_ptr += embedding_dim;
                }
                const RowID db_row_id(segment_id, static_cast<SegmentOffset>(block_id) * DEFAULT_BLOCK_CAPACITY + row_id);
                merge_heap->Search(0, &result_dist, &db_row_id, 1);
            }
        }
    }
};

} // namespace infinity
