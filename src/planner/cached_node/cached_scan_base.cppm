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

export module cached_scan_base;

import stl;
import cached_node_base;
import logical_node_type;

namespace infinity {

class BaseTableRef;
class PhysicalScanBase;

export class CachedScanBase : public CachedNodeBase {
public:
    CachedScanBase(LogicalNodeType type, const BaseTableRef *base_table_ref, TxnTimeStamp query_ts, SharedPtr<Vector<String>> output_names);

    CachedScanBase(LogicalNodeType type, const PhysicalScanBase *physical_scan_base, TxnTimeStamp query_ts);

    u64 Hash() const override;

    bool Eq(const CachedNodeBase &other) const override;

    const String &schema_name() const { return *schema_name_; }
    const String &table_name() const { return *table_name_; }

protected:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> table_name_{};
    Vector<ColumnID> column_ids_{};
    TxnTimeStamp query_ts_{};
};

} // namespace infinity