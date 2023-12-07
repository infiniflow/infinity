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
import txn;
import query_context;
import table_def;
import data_table;
import parser;
import physical_operator_type;
import operator_state;
import load_meta;

module physical_create_collection;

namespace infinity {

PhysicalCreateCollection::PhysicalCreateCollection(SharedPtr<String> schema_name,
                                                   SharedPtr<String> collection_name,
                                                   ConflictType conflict_type,
                                                   SharedPtr<Vector<String>> output_names,
                                                   SharedPtr<Vector<SharedPtr<DataType>>> output_types,
                                                   u64 table_index,
                                                   u64 id,
                                                   SharedPtr<Vector<LoadMeta>> load_metas)
    : PhysicalOperator(PhysicalOperatorType::kCreateCollection, nullptr, nullptr, id, load_metas), schema_name_(Move(schema_name)),
      collection_name_(Move(collection_name)), output_names_(Move(output_names)), output_types_(Move(output_types)), conflict_type_(conflict_type),
      table_index_(table_index) {}

void PhysicalCreateCollection::Init() {}

bool PhysicalCreateCollection::Execute(QueryContext *query_context, OperatorState *output_state) {
    output_state->SetComplete();
    return true;
}

} // namespace infinity
