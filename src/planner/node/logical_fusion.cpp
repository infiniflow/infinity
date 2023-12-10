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

import stl;
import base_table_ref;
import column_binding;
import logical_node_type;
import fusion_expression;
import parser;
import default_values;

module logical_fusion;

namespace infinity {

LogicalFusion::LogicalFusion(u64 node_id,
                             SharedPtr<FusionExpression> fusion_expr)
    : LogicalNode(node_id, LogicalNodeType::kFusion), fusion_expr_(fusion_expr) {}

String LogicalFusion::ToString(i64 &space) const {
    std::stringstream ss;
    String arrow_str;
    if (space != 0) {
        arrow_str = String(space - 2, ' ');
        arrow_str += "-> LogicalFusion ";
    } else {
        arrow_str = "LogicalFusion ";
    }
    ss << arrow_str << fusion_expr_->ToString();
    return ss.str();
}

} // namespace infinity
