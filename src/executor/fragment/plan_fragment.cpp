//
// Created by jinhai on 23-4-27.
//

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
