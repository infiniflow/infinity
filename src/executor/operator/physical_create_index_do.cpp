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
import physical_operator_type;
import physical_operator;
import query_context;
import operator_state;
import load_meta;

import index_def;

module physical_create_index_do;

namespace infinity {
PhysicalCreateIndexDo::PhysicalCreateIndexDo(u64 id,
                                             UniquePtr<PhysicalOperator> left,
                                             SharedPtr<String> schema_name,
                                             SharedPtr<String> table_name,
                                             SharedPtr<IndexDef> index_definition,
                                             SharedPtr<Vector<String>> output_names,
                                             SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                             SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateIndexDo, Move(left), nullptr, id, load_metas), schema_name_(schema_name), table_name_(table_name),
      index_def_ptr_(index_definition), output_names_(output_names), output_types_(output_types) {}

void PhysicalCreateIndexDo::Init() {}

bool PhysicalCreateIndexDo::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto create_index_do_state = static_cast<CreateIndexDoOperatorState *>(operator_state);
    //
}

}; // namespace infinity