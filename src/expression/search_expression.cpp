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
module search_expression;
import stl;
import expression_type;
import scalar_function;
import match_expression;
import knn_expression;
import match_tensor_expression;
import fusion_expression;


namespace infinity {

SearchExpression::SearchExpression(Vector<SharedPtr<MatchExpression>> &match_exprs,
                                   Vector<SharedPtr<KnnExpression>> &knn_exprs,
                                   Vector<SharedPtr<MatchTensorExpression>> &match_tensor_exprs,
                                   SharedPtr<FusionExpression> fusion_expr)
    : BaseExpression(ExpressionType::kSearch, Vector<SharedPtr<BaseExpression>>()), match_exprs_(match_exprs), knn_exprs_(knn_exprs),
      match_tensor_exprs_(match_tensor_exprs), fusion_expr_(fusion_expr) {}

String SearchExpression::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }

    OStringStream oss;
    oss << "SEARCH(";
    int cnt = 0;
    for (auto &match_expr : match_exprs_) {
        if (cnt != 0)
            oss << ", ";
        cnt++;
        oss << match_expr->ToString();
    }
    for (auto &knn_expr : knn_exprs_) {
        if (cnt != 0)
            oss << ", ";
        cnt++;
        oss << knn_expr->ToString();
    }
    for (auto &match_tensor_expr : match_tensor_exprs_) {
        if (cnt != 0)
            oss << ", ";
        cnt++;
        oss << match_tensor_expr->ToString();
    }
    if (fusion_expr_.get() != nullptr) {
        if (cnt != 0)
            oss << ", ";
        cnt++;
        oss << fusion_expr_->ToString();
    }
    oss << ")";
    return oss.str();
}

} // namespace infinity
