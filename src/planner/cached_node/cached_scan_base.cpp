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

#include <vector>

module cached_scan_base;

import base_table_ref;
import physical_scan_base;

namespace infinity {

CachedScanBase::CachedScanBase(LogicalNodeType type,
                               const BaseTableRef *base_table_ref,
                               TxnTimeStamp query_ts,
                               SharedPtr<Vector<String>> output_names)
    : CachedNodeBase(type, output_names), schema_name_(base_table_ref->schema_name()), table_name_(base_table_ref->table_name()),
      column_ids_(base_table_ref->column_ids_), query_ts_(query_ts) {}

CachedScanBase::CachedScanBase(LogicalNodeType type, const PhysicalScanBase *physical_scan_base, TxnTimeStamp query_ts)
    : CachedNodeBase(type, physical_scan_base->GetOutputNames()), schema_name_(physical_scan_base->base_table_ref_->schema_name()),
      table_name_(physical_scan_base->base_table_ref_->table_name()), column_ids_(physical_scan_base->base_table_ref_->column_ids_),
      query_ts_(query_ts) {}

u64 CachedScanBase::Hash() const {
    u64 h = CachedNodeBase::Hash();
    h = h ^ std::hash<String>{}(*schema_name_);
    h = h ^ std::hash<String>{}(*table_name_);
    h = h ^ std::hash<TxnTimeStamp>{}(query_ts_);
    return h;
}

bool CachedScanBase::Eq(const CachedNodeBase &other_base) const {
    if (!CachedNodeBase::Eq(other_base)) {
        return false;
    }
    const auto &other = static_cast<const CachedScanBase &>(other_base);
    return *schema_name_ == *other.schema_name_ && *table_name_ == *other.table_name_ && query_ts_ == other.query_ts_;
}

} // namespace infinity