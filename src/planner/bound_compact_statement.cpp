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

module bound_compact_statement;

import stl;
import logical_compact;
import base_table_ref;
import block_index;
import logical_node;
import query_context;
import bind_context;
import logger;
import third_party;
import compact_statement;

namespace infinity {

Vector<SharedPtr<LogicalNode>> BoundCompactStatement::BuildPlans(QueryContext *query_context) {
    Vector<SharedPtr<LogicalNode>> res;
    const SharedPtr<BindContext> &bind_context = this->bind_context_;

    auto compact_node = MakeShared<LogicalCompact>(bind_context->GetNewLogicalNodeId(), base_table_ref_, compact_type_);
    res.emplace_back(compact_node);
    return res;
}

} // namespace infinity