//
// Created by jinhai on 23-9-22.
//

module;

#include <vector>

import stl;
// import txn;
import query_context;
// import table_def;
// import table;
import parser;
import logger;
// import physical_operator_type;
import operator_state;
import global_block_id;
import block_index;
import base_table_ref;
import block_entry;
import knn_scan_data;
import column_buffer;
import block_column_entry;
import knn_distance;
import third_party;
import infinity_assert;
import infinity_exception;
import table_collection_entry;

module physical_knn_scan;

namespace infinity {

void PhysicalKnnScan::Init() {}

void PhysicalKnnScan::Execute(QueryContext *query_context) {}

void PhysicalKnnScan::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto *knn_scan_input_state = static_cast<KnnScanInputState *>(input_state);
    auto *knn_scan_output_state = static_cast<KnnScanOutputState *>(output_state);

    ExecuteInternal(query_context, knn_scan_input_state, knn_scan_output_state);
#if 0
    KnnScanFunction* knn_scan_func = (KnnScanFunction*)base_table_ref_->table_func_.get();
    knn_scan_func->main_function_(query_context,
                                  knn_scan_input_state->knn_scan_function_data_.get(),
                                  *knn_scan_output_state->data_block_);
#endif
}

TableCollectionEntry *PhysicalKnnScan::table_collection_ptr() const { return base_table_ref_->table_entry_ptr_; }

String PhysicalKnnScan::TableAlias() const { return base_table_ref_->alias_; }

BlockIndex *PhysicalKnnScan::GetBlockIndex() const { return base_table_ref_->block_index_.get(); }

Vector<SizeT> &PhysicalKnnScan::ColumnIDs() const { return base_table_ref_->column_ids_; }

Vector<SharedPtr<Vector<GlobalBlockID>>> PhysicalKnnScan::PlanBlockEntries(i64 parallel_count) const {
    BlockIndex *block_index = base_table_ref_->block_index_.get();

    u64 all_block_count = block_index->BlockCount();
    u64 block_per_task = all_block_count / parallel_count;
    u64 residual = all_block_count % parallel_count;

    Vector<SharedPtr<Vector<GlobalBlockID>>> result(parallel_count, nullptr);
    for (u64 task_id = 0, global_block_id = 0, residual_idx = 0; task_id < parallel_count; ++task_id) {
        result[task_id] = MakeShared<Vector<GlobalBlockID>>();
        for (u64 block_id_in_task = 0; block_id_in_task < block_per_task; ++block_id_in_task) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
        }
        if (residual_idx < residual) {
            result[task_id]->emplace_back(block_index->global_blocks_[global_block_id++]);
            ++residual_idx;
        }
    }
    return result;
}

SizeT PhysicalKnnScan::BlockEntryCount() const { return base_table_ref_->block_index_->BlockCount(); }

void PhysicalKnnScan::ExecuteInternal(QueryContext *query_context, KnnScanInputState *input_state, KnnScanOutputState *output_state) {
    auto *knn_scan_function_data_ptr = input_state->knn_scan_function_data_.get();
    BlockIndex *block_index = knn_scan_function_data_ptr->block_index_;
    Vector<GlobalBlockID> *block_ids = knn_scan_function_data_ptr->global_block_ids_.get();
    const Vector<SizeT> &knn_column_ids = knn_scan_function_data_ptr->knn_column_ids_;
    if (knn_column_ids.size() != 1) {
        Error<ExecutorException>("More than one knn column", __FILE_NAME__, __LINE__);
    }

    SizeT knn_column_id = knn_column_ids[0];

    i64 &block_ids_idx = knn_scan_function_data_ptr->current_block_ids_idx_;
    i32 segment_id = block_ids->at(block_ids_idx).segment_id_;
    i16 block_id = block_ids->at(block_ids_idx).block_id_;

    BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);
    i16 row_count = current_block_entry->row_count_;

    Vector<ColumnBuffer> columns_buffer;
    columns_buffer.reserve(current_block_entry->columns_.size());
    SizeT column_count = current_block_entry->columns_.size();
    for (SizeT column_id = 0; column_id < column_count; ++column_id) {
        columns_buffer.emplace_back(
            BlockColumnEntry::GetColumnData(current_block_entry->columns_[column_id].get(), query_context->storage()->buffer_manager()));
    }

    switch (knn_scan_function_data_ptr->knn_distance_type_) {

        case KnnDistanceType::kInvalid: {
            Error<ExecutorException>("Invalid Knn distance type", __FILE_NAME__, __LINE__);
        }
        case KnnDistanceType::kL2: {
            switch (knn_scan_function_data_ptr->elem_type_) {
                case EmbeddingDataType::kElemFloat: {

                    KnnDistance<f32> *knn_flat_l2 = static_cast<KnnDistance<f32> *>(knn_scan_function_data_ptr->knn_distance_.get());

                    knn_flat_l2->Search((f32 *)(columns_buffer[knn_column_id].GetAll()), row_count, segment_id, block_id);

                    if (block_ids_idx == block_ids->size() - 1) {
                        // Last block, Get the result according to the topk row.
                        knn_flat_l2->End();

                        for (i64 query_idx = 0; query_idx < knn_flat_l2->QueryCount(); ++query_idx) {

                            f32 *top_distance = knn_flat_l2->GetDistanceByIdx(query_idx);
                            RowID *row_id = knn_flat_l2->GetIDByIdx(query_idx);

                            i64 result_count = std::min(knn_flat_l2->TotalBaseCount(), knn_flat_l2->TopK());

                            for (i64 top_idx = 0; top_idx < result_count; ++top_idx) {
                                SizeT id = query_idx * knn_flat_l2->QueryCount() + top_idx;
                                LOG_TRACE(Format("Row offset: {}: {}: {}, distance {}",
                                                 row_id[id].segment_id_,
                                                 row_id[id].block_id_,
                                                 row_id[id].block_offset_,
                                                 top_distance[id]));

                                BlockEntry *block_entry = block_index->segment_block_index_[row_id[id].segment_id_][row_id[id].block_id_];

                                SizeT column_id = 0;
                                for (; column_id < column_count; ++column_id) {
                                    ColumnBuffer column_buffer = BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(),
                                                                                                 query_context->storage()->buffer_manager());

                                    ptr_t ptr = column_buffer.GetValueAt(row_id[id].block_offset_, *output_types_->at(column_id));
                                    output_state->data_block_->AppendValueByPtr(column_id, ptr);
                                }

                                output_state->data_block_->AppendValueByPtr(column_id++, (ptr_t)&top_distance[id]);
                                output_state->data_block_->AppendValueByPtr(column_id, (ptr_t)&row_id[id]);
                            }

                            for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                                LOG_TRACE(
                                    Format("Output Column ID: {}, Name: {}", base_table_ref_->column_ids_[column_id], output_names_->at(column_id)));
                            }
                            // Get row from input data block
                            //                            output_state->data_block_->AppendValue()
                        }
                    }
                    break;
                }
                case EmbeddingDataType::kElemBit: {
                    //            KnnFlat::SearchBit(&knn_scan_function_data_ptr->knn_flat_,
                    //                               knn_scan_function_data_ptr->dimension_,
                    //                               (i8*)(column_buffer.GetAll()),
                    //                               row_count,
                    //                               (i8*)knn_scan_function_data_ptr->query_embedding_,
                    //                               knn_scan_function_data_ptr->query_embedding_count_,
                    //                               knn_scan_function_data_ptr->topk_,
                    //                               nullptr,
                    //                               nullptr,
                    //                               EmbeddingDataType::kElemBit,
                    //                               knn_scan_function_data_ptr->knn_distance_type_);
                    //
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchBit(&knn_scan_function_data_ptr->knn_flat_,
                    //                                         knn_scan_function_data_ptr->topk_,
                    //                                         knn_scan_function_data_ptr->knn_distance_type_);
                    //            }

                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    //            KnnFlat::SearchDouble(&knn_scan_function_data_ptr->knn_flat_,
                    //                                  knn_scan_function_data_ptr->dimension_,
                    //                                  (f64*)(column_buffer.GetAll()),
                    //                                  row_count,
                    //                                  (f64*)knn_scan_function_data_ptr->query_embedding_,
                    //                                  knn_scan_function_data_ptr->query_embedding_count_,
                    //                                  knn_scan_function_data_ptr->topk_,
                    //                                  nullptr,
                    //                                  nullptr,
                    //                                  knn_scan_function_data_ptr->knn_distance_type_);
                    //
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchDouble(&knn_scan_function_data_ptr->knn_flat_,
                    //                                            knn_scan_function_data_ptr->topk_,
                    //                                            knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    //            KnnFlat::SearchInt64(&knn_scan_function_data_ptr->knn_flat_,
                    //                                 knn_scan_function_data_ptr->dimension_,
                    //                                 (i64*)(column_buffer.GetAll()),
                    //                                 row_count,
                    //                                 (i64*)knn_scan_function_data_ptr->query_embedding_,
                    //                                 knn_scan_function_data_ptr->query_embedding_count_,
                    //                                 knn_scan_function_data_ptr->topk_,
                    //                                 nullptr,
                    //                                 nullptr,
                    //                                 knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt64(&knn_scan_function_data_ptr->knn_flat_,
                    //                                            knn_scan_function_data_ptr->topk_,
                    //                                            knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    //            KnnFlat::SearchInt32(&knn_scan_function_data_ptr->knn_flat_,
                    //                                 knn_scan_function_data_ptr->dimension_,
                    //                                 (i32*)(column_buffer.GetAll()),
                    //                                 row_count,
                    //                                 (i32*)knn_scan_function_data_ptr->query_embedding_,
                    //                                 knn_scan_function_data_ptr->query_embedding_count_,
                    //                                 knn_scan_function_data_ptr->topk_,
                    //                                 nullptr,
                    //                                 nullptr,
                    //                                 knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt32(&knn_scan_function_data_ptr->knn_flat_,
                    //                                           knn_scan_function_data_ptr->topk_,
                    //                                           knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    //            KnnFlat::SearchInt16(&knn_scan_function_data_ptr->knn_flat_,
                    //                                 knn_scan_function_data_ptr->dimension_,
                    //                                 (i16*)(column_buffer.GetAll()),
                    //                                 row_count,
                    //                                 (i16*)knn_scan_function_data_ptr->query_embedding_,
                    //                                 knn_scan_function_data_ptr->query_embedding_count_,
                    //                                 knn_scan_function_data_ptr->topk_,
                    //                                 nullptr,
                    //                                 nullptr,
                    //                                 knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt16(&knn_scan_function_data_ptr->knn_flat_,
                    //                                           knn_scan_function_data_ptr->topk_,
                    //                                           knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    //            KnnFlat::SearchInt8(&knn_scan_function_data_ptr->knn_flat_,
                    //                                knn_scan_function_data_ptr->dimension_,
                    //                                (i8*)(column_buffer.GetAll()),
                    //                                row_count,
                    //                                (i8*)knn_scan_function_data_ptr->query_embedding_,
                    //                                knn_scan_function_data_ptr->query_embedding_count_,
                    //                                knn_scan_function_data_ptr->topk_,
                    //                                nullptr,
                    //                                nullptr,
                    //                                knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt8(&knn_scan_function_data_ptr->knn_flat_,
                    //                                          knn_scan_function_data_ptr->topk_,
                    //                                          knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    Error<ExecutorException>("Invalid element data type", __FILE_NAME__, __LINE__);
                }
            }
            break;
        }
        case KnnDistanceType::kCosine: {
            Error<ExecutorException>("Not implemented cosine", __FILE_NAME__, __LINE__);
        }
        case KnnDistanceType::kInnerProduct: {
            switch (knn_scan_function_data_ptr->elem_type_) {
                case EmbeddingDataType::kElemFloat: {

                    KnnDistance<f32> *knn_flat_ip = static_cast<KnnDistance<f32> *>(knn_scan_function_data_ptr->knn_distance_.get());

                    knn_flat_ip->Search((f32 *)(columns_buffer[knn_column_id].GetAll()), row_count, segment_id, block_id);

                    if (block_ids_idx == block_ids->size() - 1) {
                        // Last segment, Get the result according to the topk row.
                        knn_flat_ip->End();

                        for (i64 query_idx = 0; query_idx < knn_flat_ip->QueryCount(); ++query_idx) {

                            f32 *top_distance = knn_flat_ip->GetDistanceByIdx(query_idx);
                            RowID *row_id = knn_flat_ip->GetIDByIdx(query_idx);

                            i64 result_count = std::min(knn_flat_ip->TotalBaseCount(), knn_flat_ip->TopK());

                            for (i64 top_idx = 0; top_idx < result_count; ++top_idx) {
                                SizeT id = query_idx * knn_flat_ip->TotalBaseCount() + top_idx;
                                LOG_TRACE(Format("Row offset: {}: {}: {}, distance {}",
                                                 row_id[id].segment_id_,
                                                 row_id[id].block_id_,
                                                 row_id[id].block_offset_,
                                                 top_distance[id]));

                                BlockEntry *block_entry = block_index->segment_block_index_[row_id[id].segment_id_][row_id[id].block_id_];

                                SizeT column_id = 0;
                                for (; column_id < column_count; ++column_id) {
                                    ColumnBuffer column_buffer = BlockColumnEntry::GetColumnData(block_entry->columns_[column_id].get(),
                                                                                                 query_context->storage()->buffer_manager());

                                    ptr_t ptr = column_buffer.GetValueAt(row_id[id].block_offset_, *output_types_->at(column_id));
                                    output_state->data_block_->AppendValueByPtr(column_id, ptr);
                                }

                                output_state->data_block_->AppendValueByPtr(column_id++, (ptr_t)&top_distance[id]);
                                output_state->data_block_->AppendValueByPtr(column_id, (ptr_t)&row_id[id]);
                            }

                            for (SizeT column_id = 0; column_id < column_count; ++column_id) {
                                LOG_TRACE(
                                    Format("Output Column ID: {}, Name: {}", base_table_ref_->column_ids_[column_id], output_names_->at(column_id)));
                            }
                        }
                    }
                    break;
                }
                case EmbeddingDataType::kElemBit: {
                    //            KnnFlat::SearchBit(&knn_scan_function_data_ptr->knn_flat_,
                    //                               knn_scan_function_data_ptr->dimension_,
                    //                               (i8*)(column_buffer.GetAll()),
                    //                               row_count,
                    //                               (i8*)knn_scan_function_data_ptr->query_embedding_,
                    //                               knn_scan_function_data_ptr->query_embedding_count_,
                    //                               knn_scan_function_data_ptr->topk_,
                    //                               nullptr,
                    //                               nullptr,
                    //                               EmbeddingDataType::kElemBit,
                    //                               knn_scan_function_data_ptr->knn_distance_type_);
                    //
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchBit(&knn_scan_function_data_ptr->knn_flat_,
                    //                                         knn_scan_function_data_ptr->topk_,
                    //                                         knn_scan_function_data_ptr->knn_distance_type_);
                    //            }

                    break;
                }
                case EmbeddingDataType::kElemDouble: {
                    //            KnnFlat::SearchDouble(&knn_scan_function_data_ptr->knn_flat_,
                    //                                  knn_scan_function_data_ptr->dimension_,
                    //                                  (f64*)(column_buffer.GetAll()),
                    //                                  row_count,
                    //                                  (f64*)knn_scan_function_data_ptr->query_embedding_,
                    //                                  knn_scan_function_data_ptr->query_embedding_count_,
                    //                                  knn_scan_function_data_ptr->topk_,
                    //                                  nullptr,
                    //                                  nullptr,
                    //                                  knn_scan_function_data_ptr->knn_distance_type_);
                    //
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchDouble(&knn_scan_function_data_ptr->knn_flat_,
                    //                                            knn_scan_function_data_ptr->topk_,
                    //                                            knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt64: {
                    //            KnnFlat::SearchInt64(&knn_scan_function_data_ptr->knn_flat_,
                    //                                 knn_scan_function_data_ptr->dimension_,
                    //                                 (i64*)(column_buffer.GetAll()),
                    //                                 row_count,
                    //                                 (i64*)knn_scan_function_data_ptr->query_embedding_,
                    //                                 knn_scan_function_data_ptr->query_embedding_count_,
                    //                                 knn_scan_function_data_ptr->topk_,
                    //                                 nullptr,
                    //                                 nullptr,
                    //                                 knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt64(&knn_scan_function_data_ptr->knn_flat_,
                    //                                            knn_scan_function_data_ptr->topk_,
                    //                                            knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt32: {
                    //            KnnFlat::SearchInt32(&knn_scan_function_data_ptr->knn_flat_,
                    //                                 knn_scan_function_data_ptr->dimension_,
                    //                                 (i32*)(column_buffer.GetAll()),
                    //                                 row_count,
                    //                                 (i32*)knn_scan_function_data_ptr->query_embedding_,
                    //                                 knn_scan_function_data_ptr->query_embedding_count_,
                    //                                 knn_scan_function_data_ptr->topk_,
                    //                                 nullptr,
                    //                                 nullptr,
                    //                                 knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt32(&knn_scan_function_data_ptr->knn_flat_,
                    //                                           knn_scan_function_data_ptr->topk_,
                    //                                           knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt16: {
                    //            KnnFlat::SearchInt16(&knn_scan_function_data_ptr->knn_flat_,
                    //                                 knn_scan_function_data_ptr->dimension_,
                    //                                 (i16*)(column_buffer.GetAll()),
                    //                                 row_count,
                    //                                 (i16*)knn_scan_function_data_ptr->query_embedding_,
                    //                                 knn_scan_function_data_ptr->query_embedding_count_,
                    //                                 knn_scan_function_data_ptr->topk_,
                    //                                 nullptr,
                    //                                 nullptr,
                    //                                 knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt16(&knn_scan_function_data_ptr->knn_flat_,
                    //                                           knn_scan_function_data_ptr->topk_,
                    //                                           knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInt8: {
                    //            KnnFlat::SearchInt8(&knn_scan_function_data_ptr->knn_flat_,
                    //                                knn_scan_function_data_ptr->dimension_,
                    //                                (i8*)(column_buffer.GetAll()),
                    //                                row_count,
                    //                                (i8*)knn_scan_function_data_ptr->query_embedding_,
                    //                                knn_scan_function_data_ptr->query_embedding_count_,
                    //                                knn_scan_function_data_ptr->topk_,
                    //                                nullptr,
                    //                                nullptr,
                    //                                knn_scan_function_data_ptr->knn_distance_type_);
                    //            if(segments_idx == segment_indexes->size() - 1) {
                    //                // Last segment, Get the result according to the topk row.
                    //                KnnFlat::FinishSearchInt8(&knn_scan_function_data_ptr->knn_flat_,
                    //                                          knn_scan_function_data_ptr->topk_,
                    //                                          knn_scan_function_data_ptr->knn_distance_type_);
                    //            }
                    break;
                }
                case EmbeddingDataType::kElemInvalid: {
                    Error<ExecutorException>("Invalid element data type", __FILE_NAME__, __LINE__);
                }
            }
            break;
        }
        case KnnDistanceType::kHamming: {
            Error<ExecutorException>("Not implemented Hamming", __FILE_NAME__, __LINE__);
        }
    }

    if (block_ids_idx >= block_ids->size() - 1) {
        // Last segment, Get the result according to the topk row.
        output_state->SetComplete();
    }
    output_state->data_block_->Finalize();
}

} // namespace infinity
