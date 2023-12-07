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
import index_def;
import load_meta;

export module physical_create_index;

namespace infinity {
export class PhysicalCreateIndex : public PhysicalOperator {
public:
    void Init() override;

    bool Execute(QueryContext *query_context, OperatorState *operator_state) override;

    SharedPtr<Vector<String>> GetOutputNames() const override { return output_names_; }

    SharedPtr<Vector<SharedPtr<DataType>>> GetOutputTypes() const override { return output_types_; }

public:
    static inline UniquePtr<PhysicalCreateIndex> Make(SharedPtr<String> schema_name,
                                                      SharedPtr<String> table_name,
                                                      SharedPtr<IndexDef> index_definition,
                                                      ConflictType conflict_type,
                                                      SharedPtr<Vector<String>> output_names,
                                                      SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                      u64 id,
                                                      SharedPtr<Vector<LoadMeta>> load_metas) {
        return MakeUnique<PhysicalCreateIndex>(schema_name, table_name, index_definition, conflict_type, output_names, output_types, id, load_metas);
    }

    PhysicalCreateIndex(SharedPtr<String> schema_name,
                        SharedPtr<String> table_name,
                        SharedPtr<IndexDef> index_definition,
                        ConflictType conflict_type,
                        SharedPtr<Vector<String>> output_names,
                        SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                        u64 id,
                        SharedPtr<Vector<LoadMeta>> load_metas)
        : PhysicalOperator(PhysicalOperatorType::kCreateIndex, nullptr, nullptr, id, load_metas), schema_name_(schema_name), table_name_(table_name),
          index_def_ptr_(index_definition), conflict_type_(conflict_type), output_names_(output_names), output_types_(output_types) {}

public:
    const SharedPtr<String> schema_name_{};
    const SharedPtr<String> table_name_{};
    const SharedPtr<IndexDef> index_def_ptr_{};
    const ConflictType conflict_type_{};

    const SharedPtr<Vector<String>> output_names_{};
    const SharedPtr<Vector<SharedPtr<DataType>>> output_types_{};
};
} // namespace infinity
