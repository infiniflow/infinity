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

module cached_match;

import base_table_ref;
import match_expression;
import base_expression;
import logical_node_type;
import logical_match;
import physical_match;

namespace infinity {

CachedMatch::CachedMatch(TxnTimeStamp query_ts, LogicalMatch *logical_match)
    : CachedScanBase(LogicalNodeType::kMatch,
                      logical_match->base_table_ref_.get(),
                      query_ts,
                      logical_match->GetOutputNames()),
      match_expr_(logical_match->match_expr_), filter_expression_(logical_match->filter_expression_), topn_(logical_match->top_n_) {}

CachedMatch::CachedMatch(TxnTimeStamp query_ts, PhysicalMatch *physical_match)
    : CachedScanBase(LogicalNodeType::kMatch,
                      physical_match->base_table_ref().get(),
                      query_ts,
                      physical_match->GetOutputNames()),
      match_expr_(physical_match->match_expr()), filter_expression_(physical_match->filter_expression()), topn_(physical_match->top_n()) {}

u64 CachedMatch::Hash() const {
    u64 h = CachedScanBase::Hash();
    h ^= match_expr_->Hash();
    h ^= std::hash<u32>()(topn_);
    if (filter_expression_) {
        h ^= filter_expression_->Hash();
    }
    return h;
}

bool CachedMatch::Eq(const CachedNodeBase &other_base) const {
    if (type() != other_base.type()) {
        return false;
    }
    const auto &other = static_cast<const CachedMatch &>(other_base);
    if (!CachedScanBase::Eq(other)) {
        return false;
    }
    if (!match_expr_->Eq(*other.match_expr_)) {
        return false;
    }
    if (topn_ != other.topn_) {
        return false;
    }
    if (filter_expression_ && other.filter_expression_) {
        return filter_expression_->Eq(*other.filter_expression_);
    }
    if (filter_expression_ || other.filter_expression_) {
        return false;
    }
    return true;
}

} // namespace infinity
