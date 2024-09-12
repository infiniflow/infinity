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

void MatchSparseExpr::SetSearchColumn(ParsedExpr *&column_expr) {
    column_expr_.reset(column_expr);
    column_expr = nullptr;
}

void MatchSparseExpr::SetQuerySparse(ConstantExpr *&raw_sparse_expr) {
    query_sparse_expr_.reset(raw_sparse_expr);
    query_sparse_expr_->TrySortSparseVec(nullptr);
    raw_sparse_expr = nullptr;
}

void MatchSparseExpr::SetMetricType(char *&raw_metric_type) {
    std::string_view metric_type(raw_metric_type);
    std::unique_ptr<char[], decltype([](char *p) { std::free(p); })> raw_metric_type_ptr(raw_metric_type);
    raw_metric_type = nullptr;
    SetMetricTypeInner(metric_type);
}

void MatchSparseExpr::SetMetricType(const std::string &metric_type) {
    SetMetricTypeInner(metric_type);
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
    delete opt_params;
    opt_params = nullptr;
}

void MatchSparseExpr::SetOptionalFilter(ParsedExpr *&filter_expr) {
    filter_expr_.reset(filter_expr);
    filter_expr = nullptr;
}

std::string MatchSparseExpr::ToString() const {
    std::string column_str = column_expr_->ToString();
    std::string query_str = query_sparse_expr_->ToString();
    std::string method_str = MetricTypeToString(metric_type_);

    std::stringstream ss;
    size_t opt_num = opt_params_.size();
    for (size_t i = 0; i < opt_num; ++i) {
        ss << opt_params_[i]->ToString();
        if (i < opt_num - 1) {
            ss << ", ";
        }
    }
    std::string opt_str = std::move(ss).str();
    const auto filter_str = filter_expr_ ? fmt::format(", WHERE {}", filter_expr_->ToString()) : "";
    return fmt::format("MATCH SPARSE ({}, [{}], {}, {}{}) WITH ({})", column_str, query_str, method_str, topn_, filter_str, opt_str);
}

void MatchSparseExpr::SetMetricTypeInner(std::string_view metric_type) {
    if (metric_type == "ip") {
        metric_type_ = SparseMetricType::kInnerProduct;
    } else {
        ParserError(fmt::format("Unrecognized search method: {}", metric_type));
        metric_type_ = SparseMetricType::kInvalid;
    }
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