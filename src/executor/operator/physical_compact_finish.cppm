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

export module physical_compact_finish;

import stl;
import physical_operator;
import physical_operator_type;
import operator_state;
import query_context;
import load_meta;
import data_type;
import infinity_exception;
import base_table_ref;
import compact_state_data;

namespace infinity {

export class PhysicalCompactFinish : public PhysicalOperator {
public:
    explicit PhysicalCompactFinish(u64 id,
                                   UniquePtr<PhysicalOperator> left,
                                   UniquePtr<PhysicalOperator> right,
                                   SharedPtr<BaseTableRef> base_table_ref,
                                   SharedPtr<Vector<String>> output_names,
                                   SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                   SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCompactFinish, std::move(left), std::move(right), id, load_metas), base_table_ref_(base_table_ref),
          output_names_(output_names), output_types_(output_types) {}

    ~PhysicalCompactFinish() override = default;

    void Init() override {}

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    inline SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

    SizeT TaskletCount() override {
        UnrecoverableError("Not implement: TaskletCount not Implement");
        return 0;
    }

private:
    void SaveSegmentData(QueryContext *query_context, const CompactStateData *compact_state_data);

    void ApplyDeletes(QueryContext *query_context, const CompactStateData *compact_state_data);

private:
    SharedPtr<BaseTableRef> base_table_ref_;

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity