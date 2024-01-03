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

#include <memory>

import stl;
import data_block;
import table_function;
import query_context;
import catalog;
import knn_scan_data;
import block_index;
import global_block_id;

import infinity_exception;
import column_buffer;
import parser;
import knn_distance;
import third_party;
import logger;

module knn_scan;

namespace infinity {

void KnnScanFunc(QueryContext *, TableFunctionData *, DataBlock &) {
    //     auto *knn_scan_function_data_ptr = static_cast<KnnScanFunctionData *>(table_function_data_ptr);
    //     const BlockIndex *block_index = knn_scan_function_data_ptr->block_index_;
    //     Vector<GlobalBlockID> *block_ids = knn_scan_function_data_ptr->global_block_ids_.get();
    //     const Vector<SizeT> &knn_column_ids = knn_scan_function_data_ptr->knn_column_ids_;
    //     if (knn_column_ids.size() != 1) {
    //         Error<ExecutorException>("More than one knn column");
    //     }

    //     SizeT knn_column_id = knn_column_ids[0];

    //     i64 &block_ids_idx = knn_scan_function_data_ptr->current_block_ids_idx_;
    //     u32 segment_id = block_ids->at(block_ids_idx).segment_id_;
    //     u16 block_id = block_ids->at(block_ids_idx).block_id_;

    //     BlockEntry *current_block_entry = block_index->GetBlockEntry(segment_id, block_id);
    //     i64 row_count = current_block_entry->row_count_;

    //     ColumnBuffer column_buffer =
    //         BlockColumnEntry::GetColumnData(current_block_entry->columns_[knn_column_id].get(), query_context->storage()->buffer_manager());
    //     switch (knn_scan_function_data_ptr->knn_distance_type_) {

    //         case KnnDistanceType::kInvalid: {
    //             Error<ExecutorException>("Invalid Knn distance type");
    //         }
    //         case KnnDistanceType::kL2: {
    //             switch (knn_scan_function_data_ptr->elem_type_) {
    //                 case EmbeddingDataType::kElemFloat: {

    //                     KnnDistance<f32> *knn_flat_l2 = static_cast<KnnDistance<f32> *>(knn_scan_function_data_ptr->knn_distance_.get());

    //                     knn_flat_l2->Search((f32 *)(column_buffer.GetAll()), row_count, segment_id, block_id);

    //                     if (block_ids_idx == block_ids->size() - 1) {
    //                         // Last block, Get the result according to the topk row.
    //                         knn_flat_l2->End();

    //                         for (i64 query_idx = 0; query_idx < knn_scan_function_data_ptr->query_embedding_count_; ++query_idx) {

    //                             f32 *top_distance = knn_flat_l2->GetDistanceByIdx(query_idx);
    //                             RowID *row_id = knn_flat_l2->GetIDByIdx(query_idx);

    //                             for (i64 top_idx = 0; top_idx < knn_scan_function_data_ptr->topk_; ++top_idx) {
    //                                 SizeT id = query_idx * knn_scan_function_data_ptr->query_embedding_count_ + top_idx;
    //                                 LOG_TRACE(
    //                                     Format("Row offset: {}: {}, distance {}", row_id[id].segment_id_, row_id[id].segment_offset_,
    //                                     top_distance[id]));
    //                             }
    //                         }
    //                     }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemBit: {
    //                     //            KnnFlat::SearchBit(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                               knn_scan_function_data_ptr->dimension_,
    //                     //                               (i8*)(column_buffer.GetAll()),
    //                     //                               row_count,
    //                     //                               (i8*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                               knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                               knn_scan_function_data_ptr->topk_,
    //                     //                               nullptr,
    //                     //                               nullptr,
    //                     //                               EmbeddingDataType::kElemBit,
    //                     //                               knn_scan_function_data_ptr->knn_distance_type_);
    //                     //
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchBit(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                         knn_scan_function_data_ptr->topk_,
    //                     //                                         knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }

    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemDouble: {
    //                     //            KnnFlat::SearchDouble(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                  knn_scan_function_data_ptr->dimension_,
    //                     //                                  (f64*)(column_buffer.GetAll()),
    //                     //                                  row_count,
    //                     //                                  (f64*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                  knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                  knn_scan_function_data_ptr->topk_,
    //                     //                                  nullptr,
    //                     //                                  nullptr,
    //                     //                                  knn_scan_function_data_ptr->knn_distance_type_);
    //                     //
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchDouble(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                            knn_scan_function_data_ptr->topk_,
    //                     //                                            knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt64: {
    //                     //            KnnFlat::SearchInt64(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                 knn_scan_function_data_ptr->dimension_,
    //                     //                                 (i64*)(column_buffer.GetAll()),
    //                     //                                 row_count,
    //                     //                                 (i64*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                 knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                 knn_scan_function_data_ptr->topk_,
    //                     //                                 nullptr,
    //                     //                                 nullptr,
    //                     //                                 knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt64(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                            knn_scan_function_data_ptr->topk_,
    //                     //                                            knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt32: {
    //                     //            KnnFlat::SearchInt32(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                 knn_scan_function_data_ptr->dimension_,
    //                     //                                 (i32*)(column_buffer.GetAll()),
    //                     //                                 row_count,
    //                     //                                 (i32*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                 knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                 knn_scan_function_data_ptr->topk_,
    //                     //                                 nullptr,
    //                     //                                 nullptr,
    //                     //                                 knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt32(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                           knn_scan_function_data_ptr->topk_,
    //                     //                                           knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt16: {
    //                     //            KnnFlat::SearchInt16(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                 knn_scan_function_data_ptr->dimension_,
    //                     //                                 (u16*)(column_buffer.GetAll()),
    //                     //                                 row_count,
    //                     //                                 (u16*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                 knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                 knn_scan_function_data_ptr->topk_,
    //                     //                                 nullptr,
    //                     //                                 nullptr,
    //                     //                                 knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt16(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                           knn_scan_function_data_ptr->topk_,
    //                     //                                           knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt8: {
    //                     //            KnnFlat::SearchInt8(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                knn_scan_function_data_ptr->dimension_,
    //                     //                                (i8*)(column_buffer.GetAll()),
    //                     //                                row_count,
    //                     //                                (i8*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                knn_scan_function_data_ptr->topk_,
    //                     //                                nullptr,
    //                     //                                nullptr,
    //                     //                                knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt8(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                          knn_scan_function_data_ptr->topk_,
    //                     //                                          knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInvalid: {
    //                     Error<ExecutorException>("Invalid element data type");
    //                 }
    //             }
    //             break;
    //         }
    //         case KnnDistanceType::kCosine: {
    //             Error<ExecutorException>("Not implemented Cosine");
    //         }
    //         case KnnDistanceType::kInnerProduct: {
    //             switch (knn_scan_function_data_ptr->elem_type_) {
    //                 case EmbeddingDataType::kElemFloat: {

    //                     KnnDistance<f32> *knn_flat_ip = static_cast<KnnDistance<f32> *>(knn_scan_function_data_ptr->knn_distance_.get());

    //                     knn_flat_ip->Search((f32 *)(column_buffer.GetAll()), row_count, segment_id, block_id);

    //                     if (block_ids_idx == block_ids->size() - 1) {
    //                         // Last segment, Get the result according to the topk row.
    //                         knn_flat_ip->End();

    //                         for (i64 query_idx = 0; query_idx < knn_scan_function_data_ptr->query_embedding_count_; ++query_idx) {

    //                             f32 *top_distance = knn_flat_ip->GetDistanceByIdx(query_idx);
    //                             RowID *row_id = knn_flat_ip->GetIDByIdx(query_idx);

    //                             for (i64 top_idx = 0; top_idx < knn_scan_function_data_ptr->topk_; ++top_idx) {
    //                                 SizeT id = query_idx * knn_scan_function_data_ptr->query_embedding_count_ + top_idx;
    //                                 LOG_TRACE(
    //                                     Format("Row offset: {}: {}, distance {}", row_id[id].segment_id_, row_id[id].segment_offset_,
    //                                     top_distance[id]));
    //                             }
    //                         }
    //                     }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemBit: {
    //                     //            KnnFlat::SearchBit(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                               knn_scan_function_data_ptr->dimension_,
    //                     //                               (i8*)(column_buffer.GetAll()),
    //                     //                               row_count,
    //                     //                               (i8*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                               knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                               knn_scan_function_data_ptr->topk_,
    //                     //                               nullptr,
    //                     //                               nullptr,
    //                     //                               EmbeddingDataType::kElemBit,
    //                     //                               knn_scan_function_data_ptr->knn_distance_type_);
    //                     //
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchBit(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                         knn_scan_function_data_ptr->topk_,
    //                     //                                         knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }

    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemDouble: {
    //                     //            KnnFlat::SearchDouble(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                  knn_scan_function_data_ptr->dimension_,
    //                     //                                  (f64*)(column_buffer.GetAll()),
    //                     //                                  row_count,
    //                     //                                  (f64*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                  knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                  knn_scan_function_data_ptr->topk_,
    //                     //                                  nullptr,
    //                     //                                  nullptr,
    //                     //                                  knn_scan_function_data_ptr->knn_distance_type_);
    //                     //
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchDouble(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                            knn_scan_function_data_ptr->topk_,
    //                     //                                            knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt64: {
    //                     //            KnnFlat::SearchInt64(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                 knn_scan_function_data_ptr->dimension_,
    //                     //                                 (i64*)(column_buffer.GetAll()),
    //                     //                                 row_count,
    //                     //                                 (i64*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                 knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                 knn_scan_function_data_ptr->topk_,
    //                     //                                 nullptr,
    //                     //                                 nullptr,
    //                     //                                 knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt64(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                            knn_scan_function_data_ptr->topk_,
    //                     //                                            knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt32: {
    //                     //            KnnFlat::SearchInt32(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                 knn_scan_function_data_ptr->dimension_,
    //                     //                                 (i32*)(column_buffer.GetAll()),
    //                     //                                 row_count,
    //                     //                                 (i32*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                 knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                 knn_scan_function_data_ptr->topk_,
    //                     //                                 nullptr,
    //                     //                                 nullptr,
    //                     //                                 knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt32(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                           knn_scan_function_data_ptr->topk_,
    //                     //                                           knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt16: {
    //                     //            KnnFlat::SearchInt16(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                 knn_scan_function_data_ptr->dimension_,
    //                     //                                 (u16*)(column_buffer.GetAll()),
    //                     //                                 row_count,
    //                     //                                 (u16*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                 knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                 knn_scan_function_data_ptr->topk_,
    //                     //                                 nullptr,
    //                     //                                 nullptr,
    //                     //                                 knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt16(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                           knn_scan_function_data_ptr->topk_,
    //                     //                                           knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInt8: {
    //                     //            KnnFlat::SearchInt8(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                knn_scan_function_data_ptr->dimension_,
    //                     //                                (i8*)(column_buffer.GetAll()),
    //                     //                                row_count,
    //                     //                                (i8*)knn_scan_function_data_ptr->query_embedding_,
    //                     //                                knn_scan_function_data_ptr->query_embedding_count_,
    //                     //                                knn_scan_function_data_ptr->topk_,
    //                     //                                nullptr,
    //                     //                                nullptr,
    //                     //                                knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            if(segments_idx == segment_indexes->size() - 1) {
    //                     //                // Last segment, Get the result according to the topk row.
    //                     //                KnnFlat::FinishSearchInt8(&knn_scan_function_data_ptr->knn_flat_,
    //                     //                                          knn_scan_function_data_ptr->topk_,
    //                     //                                          knn_scan_function_data_ptr->knn_distance_type_);
    //                     //            }
    //                     break;
    //                 }
    //                 case EmbeddingDataType::kElemInvalid: {
    //                     Error<ExecutorException>("Invalid element data type");
    //                 }
    //             }
    //             break;
    //         }
    //         case KnnDistanceType::kHamming: {
    //             Error<ExecutorException>("Not implemented Hamming");
    //         }
    //     }

    //     if (block_ids_idx == block_ids->size() - 1) {
    //         // Last segment, Get the result according to the topk row.
    //     }
    // #if 0
    //     SizeT& read_offset = knn_scan_function_data_ptr->current_read_offset_;

    //     if(segments_idx >= segment_indexes->size()) {
    //         // No data or all data is read
    //         return ;
    //     }

    //     // Here we assume output is a fresh data block, we have never written anything into it.
    //     auto write_capacity = output.capacity();
    //     while(write_capacity > 0 && segments_idx < segment_indexes->size()) {
    //         SegmentEntry* current_segment_entry = segment_entries[segments_idx];
    //         auto remaining_rows = current_segment_entry->current_row_ - read_offset;
    //         auto write_size = Min(write_capacity, remaining_rows);

    //         SizeT output_column_id{0};
    //         for (auto column_id : column_ids) {
    //             ColumnBuffer column_buffer = ColumnDataEntry::GetColumnData(current_segment_entry->columns_[column_id].get(),
    //                                                                      query_context->storage()->buffer_manager());
    //             output.column_vectors[output_column_id ++]->AppendWith(column_buffer, read_offset, write_size);
    //         }

    //         // write_size = already read size = already write size
    //         write_capacity -= write_size;
    //         remaining_rows -= write_size;
    //         read_offset += write_size;

    //         // we have read all data from current segment, move to next block
    //         if (remaining_rows == 0) {
    //             ++ segments_idx;
    //             read_offset = 0;
    //         }
    //     }
    // #endif
    //     output.Finalize();
}

SharedPtr<KnnScanFunction> KnnScanFunction::Make(NewCatalog *catalog, const String &func_name) {
    SharedPtr<TableFunction> table_func = NewCatalog::GetTableFunctionByName(catalog, func_name);
    SharedPtr<KnnScanFunction> knn_scan_func = std::static_pointer_cast<KnnScanFunction>(table_func);
    return knn_scan_func;
}

void RegisterKnnScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<KnnScanFunction> table_scan_ptr = MakeShared<KnnScanFunction>("knn_scan", KnnScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), table_scan_ptr);
}

} // namespace infinity
