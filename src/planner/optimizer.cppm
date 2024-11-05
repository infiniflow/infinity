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

export module optimizer;

import stl;
import optimizer_rule;
import logical_node;
import base_statement;
import global_resource_usage;

namespace infinity {

class QueryContext;

export class Optimizer {
public:
    explicit Optimizer(QueryContext *query_context_ptr);

    ~Optimizer() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("Optimizer");
#endif
    }

    void AddRule(UniquePtr<OptimizerRule> rule);

    void optimize(SharedPtr<LogicalNode> &unoptimized_plan, StatementType statement_type);

public:
    QueryContext *query_context_ptr_{};

    Vector<UniquePtr<OptimizerRule>> rules_{};
};

} // namespace infinity
