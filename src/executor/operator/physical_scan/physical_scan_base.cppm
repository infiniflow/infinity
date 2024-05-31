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

export module physical_scan_base;

import stl;
import physical_operator;
import physical_operator_type;
import global_block_id;
import base_table_ref;
import block_index;
import load_meta;

namespace infinity {

export class PhysicalScanBase : public PhysicalOperator {
public:
    PhysicalScanBase(u64 id,
                     PhysicalOperatorType type,
                     UniquePtr<PhysicalOperator> left,
                     UniquePtr<PhysicalOperator> right,
                     SharedPtr<BaseTableRef> base_table_ref,
                     SharedPtr<Vector<LoadMeta>> load_metas);

    virtual Vector<SharedPtr<Vector<GlobalBlockID>>> PlanBlockEntries(i64 parallel_count) const;

    SizeT TaskletCount() override;

protected:
    SharedPtr<BaseTableRef> base_table_ref_{};
};

} // namespace infinity
