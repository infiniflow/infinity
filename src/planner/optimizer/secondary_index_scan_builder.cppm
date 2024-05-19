//  Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      https://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

module;

export module secondary_index_scan_builder;

import stl;
import logical_node;
import query_context;
import optimizer_rule;

namespace infinity {

export class SecondaryIndexScanBuilder final : public OptimizerRule {
public:
    ~SecondaryIndexScanBuilder() final = default;

    void ApplyToPlan(QueryContext *query_context_ptr, SharedPtr<LogicalNode> &logical_plan) final;

    String name() const final { return "Build SecondaryIndexScan"; }
};

} // namespace infinity