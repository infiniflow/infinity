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

export module bound_compact_statement;

import stl;
import logical_compact;
import base_table_ref;
import logical_node;
import query_context;
import bind_context;
import logger;
import third_party;
import table_entry;
import compact_statement;

namespace infinity {

export struct BoundCompactStatement final {
public:
    BoundCompactStatement(SharedPtr<BindContext> bind_context, SharedPtr<BaseTableRef> base_table_ref, CompactStatementType compact_type)
        : bind_context_(std::move(bind_context)), base_table_ref_(base_table_ref), compact_type_(compact_type) {}

    Vector<SharedPtr<LogicalNode>> BuildPlans(QueryContext *query_context);

private:
    SharedPtr<BindContext> bind_context_{};

    SharedPtr<BaseTableRef> base_table_ref_{};
    CompactStatementType compact_type_;
};

} // namespace infinity