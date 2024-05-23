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

std::string MatchTensorExpr::ToString() const {
    if (!alias_.empty()) {
        return alias_;
    }
    if (options_text_.empty()) {
        return fmt::format("MATCH TENSOR ({}, [{}], {}, {})",
                           column_expr_->ToString(),
                           tensor_expr_->ToString(),
                           embedding_data_type_,
                           search_method_);
    }
    return fmt::format("MATCH TENSOR ({}, [{}], {}, {}, EXTRA OPTION : '{}')",
                       column_expr_->ToString(),
                       tensor_expr_->ToString(),
                       embedding_data_type_,
                       search_method_,
                       options_text_);
}

} // namespace infinity
