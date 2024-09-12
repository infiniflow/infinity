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

#include "knn_expr.h"
#include "spdlog/fmt/fmt.h"

namespace infinity {

KnnExpr::~KnnExpr() {
    if (column_expr_ != nullptr) {
        delete column_expr_;
        column_expr_ = nullptr;
    }

    if(opt_params_ != nullptr) {
        for(auto* param_ptr: *opt_params_) {
            delete param_ptr;
            param_ptr = nullptr;
        }
        delete opt_params_;
        opt_params_ = nullptr;
    }

    if (!own_memory_) {
        return;
    }
    if (embedding_data_ptr_ != nullptr) {
        switch (embedding_data_type_) {
            case EmbeddingDataType::kElemDouble: {
                double *data_ptr = reinterpret_cast<double *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemFloat: {
                float *data_ptr = reinterpret_cast<float *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemFloat16: {
                auto *data_ptr = reinterpret_cast<Float16T *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemBFloat16: {
                auto *data_ptr = reinterpret_cast<BFloat16T *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemBit:
            case EmbeddingDataType::kElemInt8: {
                int8_t *data_ptr = reinterpret_cast<int8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemUInt8: {
                uint8_t *data_ptr = reinterpret_cast<uint8_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt16: {
                int16_t *data_ptr = reinterpret_cast<int16_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt32: {
                int32_t *data_ptr = reinterpret_cast<int32_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInt64: {
                int64_t *data_ptr = reinterpret_cast<int64_t *>(embedding_data_ptr_);
                delete[] data_ptr;
                break;
            }
            case EmbeddingDataType::kElemInvalid: {
                //                LOG_CRITICAL("Unexpected embedding data type")
                int8_t *data_ptr = reinterpret_cast<int8_t *>(embedding_data_ptr_);
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
    const auto filter_str = filter_expr_ ? fmt::format(", WHERE {}", filter_expr_->ToString()) : "";
    auto embedding_data_ptr = static_cast<char *>(embedding_data_ptr_);
    EmbeddingType tmp_embedding_type(std::move(embedding_data_ptr), false);
    std::string expr_str = fmt::format("MATCH VECTOR ({}, {}, {}, {}, {}{})",
                                       column_expr_->ToString(),
                                       EmbeddingType::Embedding2String(tmp_embedding_type, embedding_data_type_, dimension_),
                                       EmbeddingType::EmbeddingDataType2String(embedding_data_type_),
                                       KnnDistanceType2Str(distance_type_),
                                       topn_,
                                       filter_str);
    if (!opt_params_->empty()) {
        expr_str += '(';
        for (size_t i = 0; i < opt_params_->size(); ++i) {
            expr_str += fmt::format("{}={}", (*opt_params_)[i]->param_name_, (*opt_params_)[i]->param_value_);
            if (i != opt_params_->size() - 1) {
                expr_str += ", ";
            }
        }
        expr_str += ')';
    }
    return expr_str;
}

bool KnnExpr::InitDistanceType(const char *distance_type) {
    if (strcmp(distance_type, "l2") == 0) {
        distance_type_ = infinity::KnnDistanceType::kL2;
    } else if (strcmp(distance_type, "ip") == 0) {
        distance_type_ = infinity::KnnDistanceType::kInnerProduct;
    } else if (strcmp(distance_type, "cosine") == 0) {
        distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if (strcmp(distance_type, "cos") == 0) {
        distance_type_ = infinity::KnnDistanceType::kCosine;
    } else if (strcmp(distance_type, "hamming") == 0) {
        distance_type_ = infinity::KnnDistanceType::kHamming;
    } else {
        return false;
    }
    return true;
}

bool KnnExpr::InitEmbedding(const char *data_type, const ConstantExpr *query_vec) {
    if (strcmp(data_type, "float") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat;
        if (!(query_vec->double_array_.empty())) {
            dimension_ = query_vec->double_array_.size();
            embedding_data_ptr_ = new float[dimension_];
            for (long i = 0; i < dimension_; ++i) {
                ((float *)(embedding_data_ptr_))[i] = query_vec->double_array_[i];
            }
        }
        if (!(query_vec->long_array_.empty())) {
            dimension_ = query_vec->long_array_.size();
            embedding_data_ptr_ = new float[dimension_];
            for (long i = 0; i < dimension_; ++i) {
                ((float *)(embedding_data_ptr_))[i] = query_vec->long_array_[i];
            }
        }
    } else if (strcmp(data_type, "float16") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        embedding_data_type_ = infinity::EmbeddingDataType::kElemFloat16;
        if (!(query_vec->double_array_.empty())) {
            dimension_ = query_vec->double_array_.size();
            embedding_data_ptr_ = new Float16T[dimension_];
            for (long i = 0; i < dimension_; ++i) {
                ((Float16T *)(embedding_data_ptr_))[i] = static_cast<float>(query_vec->double_array_[i]);
            }
        }
        if (!(query_vec->long_array_.empty())) {
            dimension_ = query_vec->long_array_.size();
            embedding_data_ptr_ = new Float16T[dimension_];
            for (long i = 0; i < dimension_; ++i) {
                ((Float16T *)(embedding_data_ptr_))[i] = static_cast<float>(query_vec->long_array_[i]);
            }
        }
    } else if (strcmp(data_type, "bfloat16") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        embedding_data_type_ = infinity::EmbeddingDataType::kElemBFloat16;
        if (!(query_vec->double_array_.empty())) {
            dimension_ = query_vec->double_array_.size();
            embedding_data_ptr_ = new BFloat16T[dimension_];
            for (long i = 0; i < dimension_; ++i) {
                ((BFloat16T *)(embedding_data_ptr_))[i] = static_cast<float>(query_vec->double_array_[i]);
            }
        }
        if (!(query_vec->long_array_.empty())) {
            dimension_ = query_vec->long_array_.size();
            embedding_data_ptr_ = new BFloat16T[dimension_];
            for (long i = 0; i < dimension_; ++i) {
                ((BFloat16T *)(embedding_data_ptr_))[i] = static_cast<float>(query_vec->long_array_[i]);
            }
        }
    } else if (strcmp(data_type, "tinyint") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->long_array_.size();
        embedding_data_type_ = infinity::EmbeddingDataType::kElemInt8;
        embedding_data_ptr_ = new char[dimension_];

        for (long i = 0; i < dimension_; ++i) {
            ((char *)embedding_data_ptr_)[i] = query_vec->long_array_[i];
        }
    } else if (strcmp(data_type, "unsigned tinyint") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->long_array_.size();
        embedding_data_type_ = infinity::EmbeddingDataType::kElemUInt8;
        embedding_data_ptr_ = new uint8_t[dimension_];
        for (long i = 0; i < dimension_; ++i) {
            ((uint8_t *)embedding_data_ptr_)[i] = query_vec->long_array_[i];
        }
    } else if (strcmp(data_type, "smallint") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->long_array_.size();
        embedding_data_type_ = infinity::EmbeddingDataType::kElemInt16;
        embedding_data_ptr_ = new short int[dimension_];

        for (long i = 0; i < dimension_; ++i) {
            ((short int *)embedding_data_ptr_)[i] = query_vec->long_array_[i];
        }
    } else if (strcmp(data_type, "integer") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->long_array_.size();
        embedding_data_type_ = infinity::EmbeddingDataType::kElemInt32;
        embedding_data_ptr_ = new int[dimension_];

        for (long i = 0; i < dimension_; ++i) {
            ((int *)embedding_data_ptr_)[i] = query_vec->long_array_[i];
        }
    } else if (strcmp(data_type, "bigint") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->long_array_.size();
        embedding_data_type_ = infinity::EmbeddingDataType::kElemInt64;
        embedding_data_ptr_ = new long[dimension_];

        memcpy(embedding_data_ptr_, (void *)query_vec->long_array_.data(), dimension_ * sizeof(long));
    } else if (strcmp(data_type, "bit") == 0 and distance_type_ == infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->long_array_.size();
        if (dimension_ % 8 == 0) {
            embedding_data_type_ = infinity::EmbeddingDataType::kElemBit;
            long embedding_size = dimension_ / 8;
            char *char_ptr = new char[embedding_size];
            uint8_t *data_ptr = reinterpret_cast<uint8_t *>(char_ptr);
            embedding_data_ptr_ = char_ptr;
            for (long i = 0; i < embedding_size; ++i) {
                uint8_t embedding_unit = 0;
                for (long bit_idx = 0; bit_idx < 8; ++bit_idx) {
                    if (query_vec->long_array_[i * 8 + bit_idx] == 1) {
                        embedding_unit |= (uint8_t(1) << bit_idx);
                    } else if (query_vec->long_array_[i * 8 + bit_idx] == 0) {
                        // no-op
                    } else {
                        return false;
                    }
                }
                data_ptr[i] = embedding_unit;
            }
        } else {
            return false;
        }
    } else if (strcmp(data_type, "double") == 0 and distance_type_ != infinity::KnnDistanceType::kHamming) {
        dimension_ = query_vec->double_array_.size();
        embedding_data_type_ = infinity::EmbeddingDataType::kElemDouble;
        embedding_data_ptr_ = new double[dimension_];

        memcpy(embedding_data_ptr_, (void *)query_vec->double_array_.data(), dimension_ * sizeof(double));
    } else {
        return false;
    }
    return true;
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
            ParserError("Invalid knn distance type");
            break;
        }
    }
    return std::string();
}

} // namespace infinity
