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

import stl;
import parser;
import query_context;
import operator_state;
import physical_operator;
import physical_operator_type;

export module physical_drop_view;

namespace infinity {
export class PhysicalDropView final : public PhysicalOperator {
public:
    explicit PhysicalDropView(SharedPtr<String> schema_name,
                              SharedPtr<String> view_name,
                              ConflictType conflict_type,
                              SharedPtr<Vector<String>> output_names,
                              SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                              u64 id)
        : PhysicalOperator(PhysicalOperatorType::kDropView, nullptr, nullptr, id), schema_name_(Move(schema_name)),
          view_name_(Move(view_name)), output_names_(Move(output_names)), output_types_(Move(output_types)),
          conflict_type_(conflict_type) {}

    ~PhysicalDropView() override = default;

    void Init() override;

    void Execute(QueryContext *query_context, OperatorState *operator_state) final;

    inline SharedPtr<Vector<String>> GetOutputNames() const final { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const final { return output_types_; }

    inline SharedPtr<String> schema_name() const { return schema_name_; }

    inline SharedPtr<String> view_name() const { return view_name_; }

    inline ConflictType conflict_type() const { return conflict_type_; }

private:
    SharedPtr<String> schema_name_{};
    SharedPtr<String> view_name_{};
    ConflictType conflict_type_{ConflictType::kInvalid};

    SharedPtr<Vector<String>> output_names_{};
    SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};

} // namespace infinity
