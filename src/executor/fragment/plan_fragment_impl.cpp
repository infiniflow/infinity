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

module infinity_core:plan_fragment.impl;

import :plan_fragment;
import :stl;
import :physical_source;
import :physical_sink;
import :data_table;
import :query_context;
import :plan_fragment;

import std;
import third_party;

import data_type;

namespace infinity {

void PlanFragment::SetSourceNode(QueryContext *query_context,
                                 SourceType source_type,
                                 const std::shared_ptr<std::vector<std::string>> &names,
                                 const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &types) {
    source_ = std::make_unique<PhysicalSource>(query_context->GetNextNodeID(), source_type, names, types, nullptr);
}

void PlanFragment::SetSinkNode(QueryContext *query_context,
                               SinkType sink_type,
                               const std::shared_ptr<std::vector<std::string>> &names,
                               const std::shared_ptr<std::vector<std::shared_ptr<DataType>>> &types) {
    sink_ = std::make_unique<PhysicalSink>(query_context->GetNextNodeID(), sink_type, names, types, nullptr);
}

std::shared_ptr<std::vector<std::string>> PlanFragment::ToString() {
    auto result = std::make_shared<std::vector<std::string>>();
    if (source_.get() != nullptr) {
        result->push_back(source_->GetName());
    }
    for (auto &op : operators_) {
        result->push_back(op->GetName());
    }
    if (sink_.get() != nullptr) {
        result->push_back(sink_->GetName());
    }
    return result;
}

std::shared_ptr<DataTable> PlanFragment::GetResult() { return context_->GetResult(); }

void PlanFragment::AddNext(std::shared_ptr<PlanFragment> root, PlanFragment *next) {
    std::vector<PlanFragment *> next_leaves;
    next->GetStartFragments(next_leaves);
    for (auto &leaf : next_leaves) {
        leaf->AddChild(root);
    }
}

size_t PlanFragment::GetStartFragments(std::vector<PlanFragment *> &leaf_fragments) {
    size_t all_fragment_n = 0;
    std::unordered_set<PlanFragment *> visited;
    std::function<void(PlanFragment *)> TraversePlanFragmentGraph = [&](PlanFragment *fragment) {
        if (visited.find(fragment) != visited.end()) {
            return;
        }
        visited.insert(fragment);
        if (fragment->GetContext()) {
            all_fragment_n += fragment->GetContext()->Tasks().size();
        }
        if (!fragment->HasChild()) {
            leaf_fragments.emplace_back(fragment);
            return;
        }
        for (auto &child : fragment->Children()) {
            TraversePlanFragmentGraph(child.get());
        }
    };
    TraversePlanFragmentGraph(this);
    return all_fragment_n;
}

} // namespace infinity
