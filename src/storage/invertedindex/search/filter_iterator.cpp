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

#include <cassert>
#include <iostream>
module filter_iterator;
import stl;
import doc_iterator;
import query_node;
import internal_types;
import table_entry;
import column_index_reader;
import explain_logical_plan;

namespace infinity {

void FilterIterator::PrintTree(std::ostream &os, const String &prefix, const bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "FilterIterator (fake_doc_freq: " << common_query_filter_->filter_result_count_ << ") (filter expression: ";
    String filter_str;
    if (common_query_filter_->original_filter_.get()) {
        ExplainLogicalPlan::Explain(common_query_filter_->original_filter_.get(), filter_str);
    } else {
        filter_str = "None";
    }
    os << filter_str << ")\n";
    const String next_prefix = prefix + (is_final ? "    " : "│   ");
    query_iterator_->PrintTree(os, next_prefix, true);
}

UniquePtr<DocIterator> FilterQueryNode::CreateSearch(const CreateSearchParams params) const {
    assert(common_query_filter_ != nullptr);
    if (!common_query_filter_->AlwaysTrue() && common_query_filter_->filter_result_count_ == 0)
        return nullptr;
    auto search_iter = query_tree_->CreateSearch(params);
    if (!search_iter) {
        return nullptr;
    }
    if (common_query_filter_->AlwaysTrue()) {
        return search_iter;
    }
    return MakeUnique<FilterIterator>(common_query_filter_, std::move(search_iter));
}

void FilterQueryNode::PrintTree(std::ostream &os, const String &prefix, const bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << "Filter (expression: ";
    String filter_str;
    if (filter_expression) {
        ExplainLogicalPlan::Explain(filter_expression, filter_str);
    } else {
        filter_str = "None";
    }
    os << filter_str << ") (filter_result_count: " << filter_result_count_ << ")\n";
}

} // namespace infinity
