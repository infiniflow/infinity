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

export module infinity_core:dependent_join_flattener;

import :stl;
import :bind_context;
import :logical_node;
import :column_binding;

namespace infinity {

export class DependentJoinFlattener {
public:
    explicit DependentJoinFlattener(const std::shared_ptr<BindContext> &bind_context_ptr, QueryContext *query_context)
        : bind_context_ptr_(bind_context_ptr), query_context_(query_context) {}

    bool DetectCorrelatedExpressions(const std::shared_ptr<LogicalNode> &logical_node);

    std::shared_ptr<LogicalNode> PushDependentJoin(const std::shared_ptr<LogicalNode> &logical_node);

    inline size_t CorrelatedColumnBaseIndex() const { return correlated_expression_offset_; }

private:
    std::shared_ptr<LogicalNode> PushDependentJoinInternal(const std::shared_ptr<LogicalNode> &logical_node);

    std::shared_ptr<LogicalNode> BuildNoCorrelatedInternal(const std::shared_ptr<LogicalNode> &logical_node);

private:
    const std::shared_ptr<BindContext> &bind_context_ptr_;
    QueryContext *query_context_{};
    std::unordered_map<u64, bool> operator2correlated_expression_map_;
    ColumnBinding base_binding_{};
    size_t correlated_expression_offset_{};
};

} // namespace infinity
