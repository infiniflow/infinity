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

import physical_operator;
import physical_operator_type;
import query_context;
import parser;
import operator_state;
import load_meta;
import infinity_exception;

export module physical_drop_index;

namespace infinity {

export class PhysicalDropIndex final : public PhysicalOperator {
public:
    explicit PhysicalDropIndex(SharedPtr<String> schema_name,
                               SharedPtr<String> table_name,
                               SharedPtr<String> index_name,
                               ConflictType conflict_type,
                               SharedPtr<Vector<String>> output_names,
                               SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                               u64 id,
                               SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kDropIndex, nullptr, nullptr, id, load_metas), schema_name_(Move(schema_name)),
          table_name_(Move(table_name)), index_name_(Move(index_name)), conflict_type_(conflict_type),
          output_names_(Move(output_names)), output_types_(Move(output_types)) {}

    ~PhysicalDropIndex() override = default;

    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SizeT TaskletCount() override {
        Error<NotImplementException>("TaskletCount not Implement");
        return 0;
    }

    inline SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    inline SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
    const SharedPtr<String> index_name_{};
    const ConflictType conflict_type_{ConflictType::kInvalid};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
} // namespace infinity