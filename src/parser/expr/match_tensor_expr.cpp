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

#include "match_tensor_expr.h"
#include "spdlog/fmt/fmt.h"

namespace infinity {

std::string MatchTensorExpr::SearchMethodToString(MatchTensorSearchMethod method) {
    switch (method) {
        case MatchTensorSearchMethod::kMaxSim: {
            return "MAX_SIM";
        }
        case MatchTensorSearchMethod::kInvalid: {
            return "INVALID";
        }
    }
    ParserError("Unexpected case!");
    return "invalid";
}

std::string MatchTensorExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    char *emb_ptr = query_tensor_data_ptr_.get();
    const auto query_embedding = EmbeddingT(std::move(emb_ptr), false);
    const auto query_str = EmbeddingT::Embedding2String(query_embedding, embedding_data_type_, dimension_);
    const auto data_type_str = EmbeddingT::EmbeddingDataType2String(embedding_data_type_);
    const auto method_str = SearchMethodToString(search_method_enum_);
    if (options_text_.empty()) {
        return fmt::format("MATCH TENSOR ({}, [{}], {}, {})", column_expr_->ToString(), query_str, data_type_str, method_str);
    }
    return fmt::format("MATCH TENSOR ({}, [{}], {}, {}, EXTRA OPTION : '{}')",
                       column_expr_->ToString(),
                       query_str,
                       data_type_str,
                       method_str,
                       options_text_);
}

void MatchTensorExpr::SetSearchMethod(char *&raw_search_method) {
    const std::unique_ptr<char, decltype([](char *p) { std::free(p); })> raw_search_method_destructor(raw_search_method);
    raw_search_method = nullptr;
    SetSearchMethodStr(raw_search_method_destructor.get());
}

void MatchTensorExpr::SetSearchMethodStr(std::string search_method) {
    std::transform(search_method.begin(), search_method.end(), search_method.begin(), [](unsigned char c) { return std::tolower(c); });
    if (search_method == "maxsim" or search_method == "max_sim") {
        search_method_enum_ = MatchTensorSearchMethod::kMaxSim;
    } else {
        ParserError(fmt::format("Invalid search method: {}", search_method));
    }
}

void MatchTensorExpr::SetSearchColumn(ParsedExpr *&column_expr) noexcept {
    column_expr_.reset(column_expr);
    column_expr = nullptr;
}

void MatchTensorExpr::SetExtraOptions(char *&raw_options_text) {
    const std::unique_ptr<char, decltype([](char *p) { std::free(p); })> raw_options_text_destructor(raw_options_text);
    raw_options_text = nullptr;
    options_text_ = raw_options_text_destructor.get();
}

template <typename T>
std::unique_ptr<char[]>
GetConcatenatedTensorData(const ConstantExpr *tensor_expr_, const uint32_t tensor_column_basic_embedding_dim, uint32_t &query_total_dimension);

void MatchTensorExpr::SetQueryTensor(char *&raw_embedding_data_type, ConstantExpr *&raw_tensor_expr) {
    const std::unique_ptr<ConstantExpr> tensor_expr(raw_tensor_expr);
    raw_tensor_expr = nullptr;
    const std::unique_ptr<char, decltype([](char *p) { std::free(p); })> raw_embedding_data_type_destructor(raw_embedding_data_type);
    raw_embedding_data_type = nullptr;
    std::string embedding_data_type(raw_embedding_data_type_destructor.get());
    SetQueryTensorStr(std::move(embedding_data_type), tensor_expr.get());
}

void MatchTensorExpr::SetQueryTensorStr(std::string embedding_data_type, const ConstantExpr *tensor_expr) {
    std::transform(embedding_data_type.begin(), embedding_data_type.end(), embedding_data_type.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    uint32_t fake_assume_tensor_column_basic_embedding_dim = 0;
    switch (tensor_expr->literal_type_) {
        case LiteralType::kIntegerArray: {
            fake_assume_tensor_column_basic_embedding_dim = tensor_expr->long_array_.size();
            break;
        }
        case LiteralType::kDoubleArray: {
            fake_assume_tensor_column_basic_embedding_dim = tensor_expr->double_array_.size();
            break;
        }
        case LiteralType::kSubArrayArray: {
            switch (const auto &child_arr = tensor_expr->sub_array_array_[0]; child_arr->literal_type_) {
                case LiteralType::kIntegerArray: {
                    fake_assume_tensor_column_basic_embedding_dim = child_arr->long_array_.size();
                    break;
                }
                case LiteralType::kDoubleArray: {
                    fake_assume_tensor_column_basic_embedding_dim = child_arr->double_array_.size();
                    break;
                }
                default: {
                    ParserError("Wrong query tensor format!");
                    break;
                }
            }
            break;
        }
        default: {
            ParserError("Wrong query tensor format!");
            break;
        }
    }
    if (embedding_data_type == "bit") {
        embedding_data_type_ = EmbeddingDataType::kElemBit;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<bool>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else if (embedding_data_type == "int8") {
        embedding_data_type_ = EmbeddingDataType::kElemInt8;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<int8_t>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else if (embedding_data_type == "int16") {
        embedding_data_type_ = EmbeddingDataType::kElemInt16;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<int16_t>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else if (embedding_data_type == "int32" or embedding_data_type == "int") {
        embedding_data_type_ = EmbeddingDataType::kElemInt32;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<int32_t>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else if (embedding_data_type == "int64") {
        embedding_data_type_ = EmbeddingDataType::kElemInt64;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<int64_t>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else if (embedding_data_type == "float" or embedding_data_type == "float32" or embedding_data_type == "f32") {
        embedding_data_type_ = EmbeddingDataType::kElemFloat;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<float>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else if (embedding_data_type == "double" or embedding_data_type == "float64" or embedding_data_type == "f64") {
        embedding_data_type_ = EmbeddingDataType::kElemDouble;
        query_tensor_data_ptr_ = GetConcatenatedTensorData<double>(tensor_expr, fake_assume_tensor_column_basic_embedding_dim, dimension_);
    } else {
        ParserError(fmt::format("Invalid embedding data type: {}", embedding_data_type));
    }
}

template <typename T, typename U>
void FillConcatenatedTensorData(T *output_ptr, const std::vector<U> &data_array, const uint32_t expect_dim) {
    if (data_array.size() != expect_dim) {
        const auto error_info = fmt::format("Mismatch in tensor member dimension, expect: {}, but got: {}", expect_dim, data_array.size());
        ParserError(error_info);
    }
    for (uint32_t i = 0; i < expect_dim; ++i) {
        output_ptr[i] = data_array[i];
    }
}

template <typename T>
std::unique_ptr<char[]> GetConcatenatedTensorDataFromSubArray(const std::vector<std::shared_ptr<ConstantExpr>> &sub_array_array,
                                                              const uint32_t tensor_column_basic_embedding_dim,
                                                              uint32_t &query_total_dimension) {
    static_assert(!std::is_same_v<T, bool>);
    query_total_dimension = sub_array_array.size() * tensor_column_basic_embedding_dim;
    auto result = std::make_unique_for_overwrite<char[]>(query_total_dimension * sizeof(T));
    auto output_data = reinterpret_cast<T *>(result.get());
    for (uint32_t i = 0; i < sub_array_array.size(); ++i) {
        switch (sub_array_array[i]->literal_type_) {
            case LiteralType::kIntegerArray: {
                FillConcatenatedTensorData(output_data + i * tensor_column_basic_embedding_dim,
                                           sub_array_array[i]->long_array_,
                                           tensor_column_basic_embedding_dim);
                break;
            }
            case LiteralType::kDoubleArray: {
                FillConcatenatedTensorData(output_data + i * tensor_column_basic_embedding_dim,
                                           sub_array_array[i]->double_array_,
                                           tensor_column_basic_embedding_dim);
                break;
            }
            default: {
                const auto error_info = "Tensor subarray type should be IntegerArray or DoubleArray.";
                ParserError(error_info);
                break;
            }
        }
    }
    return result;
}

template <typename T, typename U>
void FillConcatenatedTensorDataBit(T *output_ptr, const std::vector<U> &data_array, const uint32_t expect_dim) {
    static_assert(std::is_same_v<T, uint8_t>);
    if (data_array.size() != expect_dim) {
        const auto error_info = fmt::format("Mismatch in tensor member dimension, expect: {}, but got: {}", expect_dim, data_array.size());
        ParserError(error_info);
    }
    for (uint32_t i = 0; i < expect_dim; ++i) {
        if (data_array[i]) {
            output_ptr[i / 8] |= (1u << (i % 8));
        }
    }
}

template <>
std::unique_ptr<char[]> GetConcatenatedTensorDataFromSubArray<bool>(const std::vector<std::shared_ptr<ConstantExpr>> &sub_array_array,
                                                                    const uint32_t tensor_column_basic_embedding_dim,
                                                                    uint32_t &query_total_dimension) {
    query_total_dimension = sub_array_array.size() * tensor_column_basic_embedding_dim;
    auto result = std::make_unique<char[]>(query_total_dimension / 8);
    auto output_data = reinterpret_cast<uint8_t *>(result.get());
    const uint32_t basic_u8_dim = tensor_column_basic_embedding_dim / 8;
    for (uint32_t i = 0; i < sub_array_array.size(); ++i) {
        switch (sub_array_array[i]->literal_type_) {
            case LiteralType::kIntegerArray: {
                FillConcatenatedTensorDataBit(output_data + i * basic_u8_dim, sub_array_array[i]->long_array_, tensor_column_basic_embedding_dim);
                break;
            }
            case LiteralType::kDoubleArray: {
                FillConcatenatedTensorDataBit(output_data + i * basic_u8_dim, sub_array_array[i]->double_array_, tensor_column_basic_embedding_dim);
                break;
            }
            default: {
                const auto error_info = "Tensor subarray type should be IntegerArray or DoubleArray.";
                ParserError(error_info);
                break;
            }
        }
    }
    return result;
}

template <typename T, typename U>
std::unique_ptr<char[]>
GetConcatenatedTensorData(const std::vector<U> &data_array, const uint32_t tensor_column_basic_embedding_dim, uint32_t &query_total_dimension) {
    query_total_dimension = data_array.size();
    if (query_total_dimension == 0 or query_total_dimension % tensor_column_basic_embedding_dim != 0) {
        const auto error_info = fmt::format("Query embedding with dimension: {} which doesn't match with tensor basic dimension {}",
                                            query_total_dimension,
                                            tensor_column_basic_embedding_dim);
        ParserError(error_info);
    }
    if constexpr (std::is_same_v<T, bool>) {
        auto result = std::make_unique<char[]>(query_total_dimension / 8);
        auto embedding_data_ptr = reinterpret_cast<uint8_t *>(result.get());
        for (uint32_t i = 0; i < query_total_dimension; ++i) {
            if (data_array[i]) {
                embedding_data_ptr[i / 8] |= (1u << (i % 8));
            }
        }
        return result;
    } else {
        auto result = std::make_unique_for_overwrite<char[]>(query_total_dimension * sizeof(T));
        auto embedding_data_ptr = reinterpret_cast<T *>(result.get());
        for (uint32_t i = 0; i < query_total_dimension; ++i) {
            embedding_data_ptr[i] = data_array[i];
        }
        return result;
    }
}

template <typename T>
std::unique_ptr<char[]>
GetConcatenatedTensorData(const ConstantExpr *tensor_expr_, const uint32_t tensor_column_basic_embedding_dim, uint32_t &query_total_dimension) {
    if constexpr (std::is_same_v<T, bool>) {
        if (tensor_column_basic_embedding_dim % 8 != 0) {
            ParserError("The tensor column basic embedding dimension should be multiple of 8");
        }
    }
    switch (tensor_expr_->literal_type_) {
        case LiteralType::kIntegerArray: {
            return GetConcatenatedTensorData<T>(tensor_expr_->long_array_, tensor_column_basic_embedding_dim, query_total_dimension);
        }
        case LiteralType::kDoubleArray: {
            return GetConcatenatedTensorData<T>(tensor_expr_->double_array_, tensor_column_basic_embedding_dim, query_total_dimension);
        }
        case LiteralType::kSubArrayArray: {
            return GetConcatenatedTensorDataFromSubArray<T>(tensor_expr_->sub_array_array_, tensor_column_basic_embedding_dim, query_total_dimension);
        }
        default: {
            ParserError("Unexpected case!");
            return nullptr;
        }
    }
}

} // namespace infinity
