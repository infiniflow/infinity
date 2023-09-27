//
// Created by jinhai on 23-9-23.
//

#pragma once

#include "common/types/internal_types.h"
#include "parser/parsed_expr/knn_expr.h"

namespace infinity {

class KnnFlat {
public:
    // bit embedding
    static void
    SearchBit(i64 dimension,
              i8* embedding_column,
              i64 column_row_count,
              i8* query_embedding,
              i64 query_embedding_count,
              i64 topk,
              i64* offset_array,
              i64* top_distance_array,
              EmbeddingDataType element_type,
              KnnDistanceType distance_type);

    // int8 embedding
    static void
    SearchInt8(i64 dimension,
               i8* embedding_column,
               i64 column_row_count,
               i8* query_embedding,
               i64 query_embedding_count,
               i64 topk,
               i64* offset_array,
               i64* top_distance_array,
               KnnDistanceType distance_type);

    // int16 embedding
    static void
    SearchInt16(i64 dimension,
                i16* embedding_column,
                i64 column_row_count,
                i16* query_embedding,
                i64 query_embedding_count,
                i64 topk,
                i64* offset_array,
                i64* top_distance_array,
                KnnDistanceType distance_type);

    // int32 embedding
    static void
    SearchInt32(i64 dimension,
                i32* embedding_column,
                i64 column_row_count,
                i32* query_embedding,
                i64 query_embedding_count,
                i64 topk,
                i64* offset_array,
                i64* top_distance_array,
                KnnDistanceType distance_type);

    // int64 embedding
    static void
    SearchInt64(i64 dimension,
                i64* embedding_column,
                i64 column_row_count,
                i64* query_embedding,
                i64 query_embedding_count,
                i64 topk,
                i64* offset_array,
                i64* top_distance_array,
                KnnDistanceType distance_type);

    // float32 embedding
    static void
    SearchFloat(i64 dimension,
                f32* embedding_column,
                i64 column_row_count,
                f32* query_embedding,
                i64 query_embedding_count,
                i64 topk,
                i64* offset_array,
                f32* top_distance_array,
                KnnDistanceType distance_type);

    // float64 embedding
    static void
    SearchDouble(i64 dimension,
                 f64* embedding_column,
                 i64 column_row_count,
                 f64* query_embedding,
                 i64 query_embedding_count,
                 i64 topk,
                 i64* offset_array,
                 f64* top_distance_array,
                 KnnDistanceType distance_type);
};

}
