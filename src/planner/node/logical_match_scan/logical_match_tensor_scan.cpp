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

module logical_match_tensor_scan;

import stl;
import base_table_ref;
import column_binding;
import logical_node_type;
import match_tensor_expression;
import default_values;
import logical_type;
import internal_types;
import third_party;
import explain_logical_plan;
import search_options;
import infinity_exception;
import status;
import logger;

namespace infinity {

void LogicalMatchTensorScan::InitExtraOptions() {
    auto match_tensor_expr = static_cast<MatchTensorExpression *>(query_expression_.get());
    SearchOptions options(match_tensor_expr->options_text_);
    // topn option
    auto top_n_it = options.options_.find("topn");
    if (top_n_it == options.options_.end()) {
        top_n_it = options.options_.find("top_n");
        if (top_n_it == options.options_.end()) {
            top_n_it = options.options_.find("topk");
            if (top_n_it == options.options_.end()) {
                top_n_it = options.options_.find("top_k");
            }
        }
    }
    if (top_n_it != options.options_.end()) {
        const int top_n_option = std::stoi(top_n_it->second);
        if (top_n_option <= 0) {
            Status status = Status::SyntaxError("topn must be a positive integer");
            LOG_ERROR(status.message());
            RecoverableError(status);
        }
        topn_ = top_n_option;
    } else {
        topn_ = DEFAULT_MATCH_TENSOR_OPTION_TOP_N;
    }
}

} // namespace infinity
