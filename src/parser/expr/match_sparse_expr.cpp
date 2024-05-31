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

#include "match_sparse_expr.h"
#include "spdlog/fmt/fmt.h"
#include "type/internal_types.h"

namespace infinity {

template <typename SourceType, typename TargetType>
void SetQuerySparseT2(const std::vector<int64_t> &source_idx,
                      const std::vector<SourceType> &source_data,
                      std::unique_ptr<char[]> &target_idx,
                      std::unique_ptr<char[]> &target_data,
                      int64_t &max_indice) {
    size_t nnz = source_idx.size();
    target_idx = std::make_unique<char[]>(nnz * sizeof(int64_t));
    target_data = std::make_unique<char[]>(nnz * sizeof(TargetType));
    auto *target_idx_ptr = reinterpret_cast<int64_t *>(target_idx.get());
    auto *target_data_ptr = reinterpret_cast<TargetType *>(target_data.get());
    for (size_t i = 0; i < nnz; ++i) {
        target_idx_ptr[i] = source_idx[i];
        target_data_ptr[i] = static_cast<TargetType>(source_data[i]);
        max_indice = std::max(max_indice, source_idx[i]);
    }
}

template <typename TargetType>
void SetQuerySparseT(ConstantExpr *raw_sparse_expr,
                     uint32_t &nnz_ref,
                     std::unique_ptr<char[]> &target_idx,
                     std::unique_ptr<char[]> &target_data,
                     int64_t &max_indice) {
    uint32_t nnz = 0;
    switch (raw_sparse_expr->literal_type_) {
        case LiteralType::kLongSparseArray: {
            nnz = raw_sparse_expr->long_sparse_array_.first.size();
            SetQuerySparseT2<int64_t, TargetType>(raw_sparse_expr->long_sparse_array_.first,
                                                  raw_sparse_expr->long_sparse_array_.second,
                                                  target_idx,
                                                  target_data,
                                                  max_indice);
            break;
        }
        case LiteralType::kDoubleSparseArray: {
            nnz = raw_sparse_expr->double_sparse_array_.first.size();
            SetQuerySparseT2<double, TargetType>(raw_sparse_expr->double_sparse_array_.first,
                                                 raw_sparse_expr->double_sparse_array_.second,
                                                 target_idx,
                                                 target_data,
                                                 max_indice);
            break;
        }
        // TODO: bit type sparse, empty sparse
        default: {
            ParserError("Unexpected case!");
        }
    }
    nnz_ref = nnz;
}

void MatchSparseExpr::SetSearchColumn(ParsedExpr *&column_expr) {
    column_expr_.reset(column_expr);
    column_expr = nullptr;
}

void MatchSparseExpr::SetQuerySparse(char *&raw_data_type, ConstantExpr *raw_sparse_expr) {
    std::string_view data_type(raw_data_type);
    std::unique_ptr<char[], decltype([](char *p) { std::free(p); })> raw_data_type_ptr(raw_data_type);
    raw_data_type = nullptr;
    embedding_data_type_ = EmbeddingT::String2EmbeddingDataType(data_type);
    switch (embedding_data_type_) {
        case kElemBit: {
            SetQuerySparseT<bool>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        case kElemInt8: {
            SetQuerySparseT<uint8_t>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        case kElemInt16: {
            SetQuerySparseT<uint16_t>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        case kElemInt32: {
            SetQuerySparseT<uint32_t>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        case kElemInt64: {
            SetQuerySparseT<uint64_t>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        case kElemFloat: {
            SetQuerySparseT<float>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        case kElemDouble: {
            SetQuerySparseT<double>(raw_sparse_expr, nnz_, query_sparse_indice_ptr_, query_sparse_data_ptr_, max_indice_);
            break;
        }
        default: {
            ParserError(fmt::format("Invalid embedding data type: {}", data_type));
        }
    }
}

void MatchSparseExpr::SetMetricType(char *&raw_metric_type) {
    std::string_view metric_type(raw_metric_type);
    std::unique_ptr<char[], decltype([](char *p) { std::free(p); })> raw_metric_type_ptr(raw_metric_type);
    raw_metric_type = nullptr;
    if (metric_type == "ip") {
        metric_type_ = SparseMetricType::kInnerProduct;
    } else {
        ParserError(fmt::format("Unrecognized search method: {}", metric_type));
    }
}

void MatchSparseExpr::SetOptParams(size_t topn, std::vector<InitParameter *> *&opt_params) {
    topn_ = topn;

    size_t opt_size = opt_params->size();
    opt_params_.resize(opt_size);
    for (size_t i = 0; i < opt_size; ++i) {
        auto &param = (*opt_params)[i];
        opt_params_[i].reset(param);
        param = nullptr;
    }
}

std::string MatchSparseExpr::ToString() const {
    std::string column_str = column_expr_->ToString();
    std::string query_str =
        SparseT::Sparse2String(query_sparse_data_ptr_.get(), query_sparse_indice_ptr_.get(), embedding_data_type_, embedding_indice_type_, nnz_);
    std::string data_type_str = EmbeddingT::EmbeddingDataType2String(embedding_data_type_);
    std::string method_str = MetricTypeToString(metric_type_);

    std::stringstream ss;
    size_t opt_num = opt_params_.size();
    for (size_t i = 0; i < opt_num; ++i) {
        ss << opt_params_[i]->ToString();
        if (i < opt_num - 1) {
            ss << ", ";
        }
    }
    std::string opt_str = ss.str();

    return fmt::format("MATCH SPARSE ({}, [{}], {}, {}, {}) WITH ({})", column_str, query_str, data_type_str, method_str, topn_, opt_str);
}

std::string MatchSparseExpr::MetricTypeToString(SparseMetricType metric) {
    switch (metric) {
        case SparseMetricType::kInnerProduct:
            return "INNER_PRODUCT";
        default:
            return "INVALID";
    }
}

} // namespace infinity