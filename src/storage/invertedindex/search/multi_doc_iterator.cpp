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

#include <iostream>

module multi_doc_iterator;

import stl;
import infinity_exception;
import logger;

namespace infinity {

void MultiDocIterator::PrintTree(std::ostream &os, const String &prefix, bool is_final) const {
    os << prefix;
    os << (is_final ? "└──" : "├──");
    os << Name();
    {
        auto [level, cost] = GetEstimateIterateCost();
        os << " (estimate_iterate_cost: " << level << ", " << cost << ")";
    }
    os << " (bm25_score_upper_bound: " << BM25ScoreUpperBound() << ")";
    os << " (threshold: " << Threshold() << ")";
    os << " (children count: " << children_.size() << ")";
    os << '\n';
    std::string next_prefix = prefix + (is_final ? "    " : "│   ");
    for (u32 i = 0; i + 1 < children_.size(); ++i) {
        children_[i]->PrintTree(os, next_prefix, false);
    }
    children_.back()->PrintTree(os, next_prefix, true);
}

} // namespace infinity