//
// Created by jinhai on 23-9-23.
//

#include "knn_flat.h"
#include "faiss/utils/distances.h"
#include "expression/knn_expression.h"

namespace infinity {

// bit embedding
void
KnnFlat::SearchBit(i64 dimension,
                   i8* embedding_column,
                   i64 column_row_count,
                   i8* query_embedding,
                   i64 query_embedding_count,
                   i64 topk,
                   i64* offset_array,
                   i64* top_distance_array,
                   EmbeddingDataType element_type,
                   KnnDistanceType distance_type) {
    NotImplementError("Search on bit embedding isn't supported.")
}

// int8 embedding
void
KnnFlat::SearchInt8(i64 dimension,
                    i8* embedding_column,
                    i64 column_row_count,
                    i8* query_embedding,
                    i64 query_embedding_count,
                    i64 topk,
                    i64* offset_array,
                    i64* top_distance_array,
                    KnnDistanceType distance_type) {
    NotImplementError("Search on int8 embedding isn't supported.")
}

// int16 embedding
void
KnnFlat::SearchInt16(i64 dimension,
                     i16* embedding_column,
                     i64 column_row_count,
                     i16* query_embedding,
                     i64 query_embedding_count,
                     i64 topk,
                     i64* offset_array,
                     i64* top_distance_array,
                     KnnDistanceType distance_type) {
    NotImplementError("Search on int16 embedding isn't supported.")
}

// int32 embedding
void
KnnFlat::SearchInt32(i64 dimension,
                     i32* embedding_column,
                     i64 column_row_count,
                     i32* query_embedding,
                     i64 query_embedding_count,
                     i64 topk,
                     i64* offset_array,
                     i64* top_distance_array,
                     KnnDistanceType distance_type) {
    NotImplementError("Search on int32 embedding isn't supported.")
}

// int64 embedding
void
KnnFlat::SearchInt64(i64 dimension,
                     i64* embedding_column,
                     i64 column_row_count,
                     i64* query_embedding,
                     i64 query_embedding_count,
                     i64 topk,
                     i64* offset_array,
                     i64* top_distance_array,
                     KnnDistanceType distance_type) {
    NotImplementError("Search on int64 embedding isn't supported.")
}

// float32 embedding
void
KnnFlat::SearchFloat(i64 dimension,
                     f32* embedding_column,
                     i64 column_row_count,
                     f32* query_embedding,
                     i64 query_embedding_count,
                     i64 topk,
                     i64* offset_array,
                     f32* top_distance_array,
                     KnnDistanceType distance_type) {
    using namespace faiss;
    IDSelector* sel = nullptr;

    switch(distance_type) {
        case KnnDistanceType::kHamming:
        case KnnDistanceType::kInvalid: {
            ExecutorError(fmt::format("Invalid Knn distance type: {}",
                                      KnnExpression::KnnDistanceType2Str(distance_type)));
        }
        case KnnDistanceType::kCosine: {
            NotImplementError("Cosine distance isn't implemented")
        }
        case KnnDistanceType::kInnerProduct: {
            float_minheap_array_t res = {size_t(query_embedding_count), size_t(topk), offset_array, top_distance_array};
            knn_inner_product(query_embedding,
                              embedding_column,
                              (u64)dimension,
                              query_embedding_count,
                              column_row_count,
                              &res,
                              sel);
            break;
        }
        case KnnDistanceType::kL2: {
            float_maxheap_array_t res = {size_t(query_embedding_count), size_t(topk), offset_array, top_distance_array};
            knn_L2sqr(query_embedding,
                      embedding_column,
                      (u64)dimension,
                      query_embedding_count,
                      column_row_count,
                      &res,
                      nullptr,
                      sel);
            break;
        }
    }
}

// float64 embedding
void
KnnFlat::SearchDouble(i64 dimension,
                      f64* embedding_column,
                      i64 column_row_count,
                      f64* query_embedding,
                      i64 query_embedding_count,
                      i64 topk,
                      i64* offset_array,
                      f64* top_distance_array,
                      KnnDistanceType distance_type) {
    NotImplementError("Search on float64 embedding isn't supported.")
}

}