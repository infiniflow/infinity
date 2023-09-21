//
// Created by jinhai on 23-9-21.
//

#include "knn_expr.h"

namespace infinity {

KnnExpr::~KnnExpr() {
    if(column_expr_ != nullptr) {
        delete column_expr_;
        column_expr_ = nullptr;
    }

    if(embedding_data_ptr_ != nullptr) {
        switch(embedding_data_type_) {
            case EmbeddingDataType::kElemDouble: {
                f64* data_ptr = static_cast<f64*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemFloat: {
                f32* data_ptr = static_cast<f32*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemBit:
            case EmbeddingDataType::kElemInt8: {
                i8* data_ptr = static_cast<i8*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt16: {
                i16* data_ptr = static_cast<i16*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt32: {
                i32* data_ptr = static_cast<i32*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt64: {
                i64* data_ptr = static_cast<i64*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInvalid: {
                LOG_CRITICAL("Unexpected embedding data type")
                i8* data_ptr = static_cast<i8*>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
        }
        embedding_data_ptr_ = nullptr;
    }
}

String
KnnExpr::ToString() const {

    if(!alias_.empty()) {
        return alias_;
    }

    String expr_str = fmt::format("KNN({}, {}, {}, Float32, {})",
                                  column_expr_->ToString(),
                                  "xxxxxx",
                                  dimension_,
                                  KnnDistanceType2Str(distance_type_));

    return expr_str;
}

String
KnnExpr::KnnDistanceType2Str(KnnDistanceType knn_distance_type) {
    switch(knn_distance_type) {
        case KnnDistanceType::kL2: {
            return "L2";
        }
        case KnnDistanceType::kInnerProduct: {
            return "InnerProduct";
        }
        case KnnDistanceType::kCosine: {
            return "Cosine";
        }
        case KnnDistanceType::kHamming: {
            return "Hamming";
        }
        case KnnDistanceType::kInvalid: {
            TypeError("Invalid knn distance type")
            break;
        }

    }
}

}
