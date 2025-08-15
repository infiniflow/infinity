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

export module infinity_core:bound_compact_statement;

import :stl;
import :logical_compact;
import :base_table_ref;
import :logical_node;
import :bind_context;
import :logger;

import third_party;

import compact_statement;

namespace infinity {

export struct BoundCompactStatement final {
public:
    BoundCompactStatement(std::shared_ptr<BindContext> bind_context, std::shared_ptr<BaseTableRef> base_table_ref, CompactStatementType compact_type)
        : bind_context_(std::move(bind_context)), base_table_ref_(base_table_ref), compact_type_(compact_type) {}

    std::vector<std::shared_ptr<LogicalNode>> BuildPlans(QueryContext *query_context);

private:
    std::shared_ptr<BindContext> bind_context_{};

    std::shared_ptr<BaseTableRef> base_table_ref_{};
    CompactStatementType compact_type_;
};

} // namespace infinity