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

#include <sstream>
#include <vector>

module match_sparse_expression;

import stl;
import logical_type;
import internal_types;
import third_party;
import column_expression;
import status;
import logger;
import infinity_exception;

namespace infinity {

DataType MatchSparseExpression::Type() const {
    const auto *col_sparse_info = static_cast<const SparseInfo *>(column_expr_->Type().type_info().get());
    auto logical_type = GetCommonLogicalType(sparse_info_->DataType(), col_sparse_info->DataType());
    if (logical_type == LogicalType::kInvalid) {
        Status status =
            Status::NotSupport(fmt::format("Unsupported query sparse data type: {}", EmbeddingT::EmbeddingDataType2String(sparse_info_->DataType())));
        LOG_ERROR(status.message());
        RecoverableError(std::move(status));
    }
    return DataType(LogicalType::kFloat);
}

String MatchSparseExpression::ToString() const {

    String sparse_str = SparseT::Sparse2String(query_sparse_.raw_data_ptr_,
                                               query_sparse_.raw_indice_ptr_,
                                               sparse_info_->DataType(),
                                               sparse_info_->IndexType(),
                                               query_sparse_.nnz_);

    std::stringstream ss;
    size_t opt_num = opt_params_.size();
    for (size_t i = 0; i < opt_num; ++i) {
        ss << opt_params_[i]->ToString();
        if (i < opt_num - 1) {
            ss << ", ";
        }
    }
    String opt_str = ss.str();

    return fmt::format("MATCH SPARSE ({}, [{}], {}, {}) WITH ({})",
                       column_expr_->Name(),
                       sparse_str,
                       MatchSparseExpr::MetricTypeToString(metric_type_),
                       topn_,
                       opt_str);
}

} // namespace infinity
