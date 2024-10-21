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

module cached_index_scan;

import logical_index_scan;
import physical_index_scan;
import logical_node_type;

namespace infinity {

CachedIndexScan::CachedIndexScan(TxnTimeStamp query_ts, const LogicalIndexScan *logical_index_scan)
    : CachedScanBase(logical_index_scan->operator_type(), logical_index_scan->base_table_ref_.get(), query_ts, logical_index_scan->GetOutputNames()),
      filter_expression_(logical_index_scan->index_filter_) {}

CachedIndexScan::CachedIndexScan(TxnTimeStamp query_ts, const PhysicalIndexScan *physical_index_scan)
    : CachedScanBase(LogicalNodeType::kIndexScan, physical_index_scan, query_ts), filter_expression_(physical_index_scan->FilterExpression()) {}

u64 CachedIndexScan::Hash() const {
    u64 h = 0;
    h ^= CachedScanBase::Hash();
    h ^= filter_expression_->Hash();
    return h;
}

bool CachedIndexScan::Eq(const CachedNodeBase &other_base) const {
    if (!CachedScanBase::Eq(other_base)) {
        return false;
    }
    if (type() != other_base.type()) {
        return false;
    }
    const auto &other = static_cast<const CachedIndexScan &>(other_base);
    return filter_expression_->Eq(*other.filter_expression_);
}

} // namespace infinity
