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

import infinity_exception;

module physical_create_index;

namespace infinity {

void PhysicalCreateIndex::Init() {}

void PhysicalCreateIndex::Execute(QueryContext *query_context, InputState *input_state, OutputState *output_state) {
    auto create_index_input_state = static_cast<CreateIndexInputState *>(input_state);
    auto create_index_output_state = static_cast<CreateIndexOutputState *>(output_state);

    auto *txn = query_context->GetTxn();
    auto result = txn->CreateIndex(*schema_name_, *table_name_, index_def_ptr_, conflict_type_);
    if (result.err_.get() != nullptr) {
        output_state->error_message_ = Move(result.err_);
    }
    output_state->SetComplete();
}

} // namespace infinity
