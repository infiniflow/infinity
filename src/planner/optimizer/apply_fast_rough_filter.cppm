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

export module infinity_core:apply_fast_rough_filter;

import :logical_node;
import :optimizer_rule;

namespace infinity {

export class ApplyFastRoughFilter final : public OptimizerRule {
public:
    ~ApplyFastRoughFilter() final = default;

    void ApplyToPlan(QueryContext *, std::shared_ptr<LogicalNode> &logical_plan) final;

    std::string name() const final { return "Apply FastRoughFilter"; }
};

} // namespace infinity
