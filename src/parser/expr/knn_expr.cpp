//
// Created by jinhai on 23-9-21.
//

#include "knn_expr.h"
#include "spdlog/fmt/fmt.h"

namespace infinity {

KnnExpr::~KnnExpr() {
    if (column_expr_ != nullptr) {
        delete column_expr_;
        column_expr_ = nullptr;
    }

    if (embedding_data_ptr_ != nullptr) {
        switch (embedding_data_type_) {
            case EmbeddingDataType::kElemDouble: {
                double *data_ptr = static_cast<double *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemFloat: {
                float *data_ptr = static_cast<float *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemBit:
            case EmbeddingDataType::kElemInt8: {
                int8_t *data_ptr = static_cast<int8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt16: {
                int16_t *data_ptr = static_cast<int16_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt32: {
                int32_t *data_ptr = static_cast<int32_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt64: {
                int64_t *data_ptr = static_cast<int64_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInvalid: {
                //                LOG_CRITICAL("Unexpected embedding data type")
                int8_t *data_ptr = static_cast<int8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
        }
        embedding_data_ptr_ = nullptr;
    }
}

std::string KnnExpr::ToString() const {

    if (!alias_.empty()) {
        return alias_;
    }

    std::string expr_str =
        fmt::format("KNN({}, {}, {}, Float32, {})", column_expr_->ToString(), "xxxxxx", dimension_, KnnDistanceType2Str(distance_type_));

    return expr_str;
}

std::string KnnExpr::KnnDistanceType2Str(KnnDistanceType knn_distance_type) {
    switch (knn_distance_type) {
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
            ParserError("Invalid knn distance type") break;
        }
    }
}

} // namespace infinity
