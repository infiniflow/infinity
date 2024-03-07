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

import stl;
import query_context;
import plan_fragment;
import physical_operator;

export module fragment_builder;

namespace infinity {

export class FragmentBuilder {
public:
    explicit FragmentBuilder(QueryContext *query_context_ptr) : query_context_ptr_(query_context_ptr) {}

    UniquePtr<PlanFragment> BuildFragment(PhysicalOperator *phys_op);

private:
    void BuildFragments(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr);

    void BuildExplain(PhysicalOperator *phys_op, PlanFragment *current_fragment_ptr);

    idx_t GetFragmentId() { return fragment_id_++; }

private:
    QueryContext *query_context_ptr_{};
    idx_t fragment_id_{};
};

} // namespace infinity