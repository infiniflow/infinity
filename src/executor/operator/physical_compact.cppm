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

export module physical_compact;

import stl;
import physical_operator;
import physical_operator_type;
import base_table_ref;
import query_context;
import operator_state;
import load_meta;
import data_type;

namespace infinity {

struct SegmentEntry;

export class PhysicalCompact : public PhysicalOperator {
public:
    PhysicalCompact(u64 id,
                    SharedPtr<BaseTableRef> base_table_ref,
                    SharedPtr<Vector<String>> output_names,
                    SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                    SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCompact, nullptr, nullptr, id, load_metas), base_table_ref_(base_table_ref),
          output_names_(output_names), output_types_(output_types) {}

    ~PhysicalCompact() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) final;

    SizeT TaskletCount() override { return compactible_segments_group_.size(); }

    Vector<Vector<Vector<SegmentEntry *>>> PlanCompact(SizeT parallel_count);

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

private:
    SharedPtr<BaseTableRef> base_table_ref_;
    Vector<Vector<SegmentEntry *>> compactible_segments_group_;

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity