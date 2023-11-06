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

import std;
import stl;
import parser;
import physical_source;
import physical_sink;
import data_table;
import query_context;

module plan_fragment;

namespace infinity {

void PlanFragment::SetSourceNode(QueryContext *query_context,
                                 SourceType source_type,
                                 const SharedPtr<Vector<String>> &names,
                                 const SharedPtr<Vector<SharedPtr<DataType>>> &types) {
    source_ = MakeUnique<PhysicalSource>(query_context->GetNextNodeID(), source_type, names, types);
}

void PlanFragment::SetSinkNode(QueryContext *query_context,
                               SinkType sink_type,
                               const SharedPtr<Vector<String>> &names,
                               const SharedPtr<Vector<SharedPtr<DataType>>> &types) {
    sink_ = MakeUnique<PhysicalSink>(query_context->GetNextNodeID(), sink_type, names, types);
}

SharedPtr<Vector<String>> PlanFragment::ToString() {
    auto result = MakeShared<Vector<String>>();
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

SharedPtr<DataTable> PlanFragment::GetResult() { return context_->GetResult(); }

} // namespace infinity
