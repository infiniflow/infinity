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

export module infinity_core:bg_query_state;

import :stl;
import :logical_node_visitor;
import :logical_node;

namespace infinity {
class PhysicalOperator;
class PlanFragment;
class Notifier;

export struct BGQueryState {
    std::vector<std::shared_ptr<LogicalNode>> logical_plans{};
    std::vector<std::unique_ptr<PhysicalOperator>> physical_plans{};
    std::shared_ptr<PlanFragment> plan_fragment{};
    std::unique_ptr<Notifier> notifier{};
};

} // namespace infinity
