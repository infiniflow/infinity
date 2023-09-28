//
// Created by jinhai on 23-9-17.
//

#include "knn_scan.h"
#include "storage/indexstore/knn_flat/knn_flat_ip.h"
#include "storage/indexstore/knn_flat/knn_flat_ip_reservoir.h"
#include "storage/indexstore/knn_flat/knn_flat_ip_blas.h"
#include "storage/indexstore/knn_flat/knn_flat_ip_blas_reservoir.h"

namespace infinity {

void
KnnScanFunc(QueryContext* query_context,
            TableFunctionData* table_function_data_ptr,
            DataBlock &output) {

    auto* knn_scan_function_data_ptr
            = static_cast<KnnScanFunctionData*>(table_function_data_ptr);
    const Vector<SegmentEntry*>& segment_entries = *knn_scan_function_data_ptr->segment_entries_ptr_;
    const Vector<u64>* segment_indexes = knn_scan_function_data_ptr->segment_indexes_.get();
    const Vector<SizeT>& knn_column_ids = knn_scan_function_data_ptr->knn_column_ids_;
    if(knn_column_ids.size() != 1) {
        ExecutorError("More than one knn column")
    }

    SizeT knn_column_id = knn_column_ids[0];

    i64& segments_idx = knn_scan_function_data_ptr->current_segment_idx_;

    SegmentEntry* current_segment_entry = segment_entries[segments_idx];
    i64 row_count = current_segment_entry->current_row_;

    ColumnBuffer column_buffer = ColumnDataEntry::GetColumnData(current_segment_entry->columns_[knn_column_id].get(),
                                                                query_context->storage()->buffer_manager());
    switch(knn_scan_function_data_ptr->elem_type_) {
        case EmbeddingDataType::kElemFloat: {

            KnnDistance<f32>* knn_flat_ip = nullptr;

            if(knn_scan_function_data_ptr->query_embedding_count_ < faiss::distance_compute_blas_threshold) {
                if(knn_scan_function_data_ptr->topk_ < faiss::distance_compute_min_k_reservoir) {
                    knn_flat_ip = static_cast<KnnFlatIP<f32>*>(knn_scan_function_data_ptr->knn_distance_.get());
                } else {
                    knn_flat_ip = static_cast<KnnFlatIPReservoir<f32>*>(knn_scan_function_data_ptr->knn_distance_.get());
                }
            } else {
                if(knn_scan_function_data_ptr->topk_ < faiss::distance_compute_min_k_reservoir) {
                    knn_flat_ip = static_cast<KnnFlatIPBlas<f32>*>(knn_scan_function_data_ptr->knn_distance_.get());
                } else {
                    knn_flat_ip = static_cast<KnnFlatIPBlasReservoir<f32>*>(knn_scan_function_data_ptr->knn_distance_.get());
                }
            }

            knn_flat_ip->Search((f32*)(column_buffer.GetAll()),
                                row_count,
                                segments_idx);

            if(segments_idx == segment_indexes->size() - 1) {
                // Last segment, Get the result according to the topk row.
                knn_flat_ip->End();

                for(i64 query_idx = 0; query_idx < knn_scan_function_data_ptr->query_embedding_count_; ++ query_idx) {

                    f32* top_distance = knn_flat_ip->GetDistanceByIdx(query_idx);
                    CompoundID* compound_id = knn_flat_ip->GetIDByIdx(query_idx);

                    for(i64 top_idx = 0; top_idx < knn_scan_function_data_ptr->topk_; ++ top_idx) {
                        SizeT id = query_idx * knn_scan_function_data_ptr->query_embedding_count_ + top_idx;
                        LOG_TRACE("Row offset: {}: {}, distance {}",
                                  compound_id[id].segment_id_,
                                  compound_id[id].segment_offset_,
                                  top_distance[id]);
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
            ExecutorError("Invalid element data type")
        }
    }

    if(segments_idx == segment_indexes->size() - 1) {
        // Last segment, Get the result according to the topk row.
    }
#if 0
    SizeT& read_offset = knn_scan_function_data_ptr->current_read_offset_;

    if(segments_idx >= segment_indexes->size()) {
        // No data or all data is read
        return ;
    }

    // Here we assume output is a fresh data block, we have never written anything into it.
    auto write_capacity = output.capacity();
    while(write_capacity > 0 && segments_idx < segment_indexes->size()) {
        SegmentEntry* current_segment_entry = segment_entries[segments_idx];
        auto remaining_rows = current_segment_entry->current_row_ - read_offset;
        auto write_size = std::min(write_capacity, remaining_rows);

        SizeT output_column_id{0};
        for (auto column_id : column_ids) {
            ColumnBuffer column_buffer = ColumnDataEntry::GetColumnData(current_segment_entry->columns_[column_id].get(),
                                                                     query_context->storage()->buffer_manager());
            output.column_vectors[output_column_id ++]->AppendWith(column_buffer, read_offset, write_size);
        }

        // write_size = already read size = already write size
        write_capacity -= write_size;
        remaining_rows -= write_size;
        read_offset += write_size;

        // we have read all data from current segment, move to next block
        if (remaining_rows == 0) {
            ++ segments_idx;
            read_offset = 0;
        }
    }
#endif
    output.Finalize();
}

void
RegisterKnnScanFunction(const UniquePtr<NewCatalog> &catalog_ptr) {

    SharedPtr<KnnScanFunction> table_scan_ptr = MakeShared<KnnScanFunction>("knn_scan", KnnScanFunc);

    NewCatalog::AddTableFunction(catalog_ptr.get(), table_scan_ptr);

}


}
