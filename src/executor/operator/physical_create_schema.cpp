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
import base_entry;
import status;

module physical_create_schema;

namespace infinity {

void PhysicalCreateSchema::Init() {}

bool PhysicalCreateSchema::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto txn = query_context->GetTxn();
    BaseEntry* base_entry{};
    Status status = txn->CreateDatabase(*schema_name_, conflict_type_, base_entry);
    auto create_database_operator_state = (CreateDatabaseOperatorState *)(operator_state);
    create_database_operator_state->error_message_ = Move(status.msg_);
    operator_state->SetComplete();
    return true;
}

} // namespace infinity
