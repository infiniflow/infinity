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

module physical_memindex_insert;

import stl;
import memory_indexer;

namespace infinity {

bool PhysicalMemIndexInsert::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *memindex_insert_operator_state = static_cast<MemIndexInsertOperatorState *>(operator_state);

    memory_indexer_->InsertExecute(inverter_, column_vector_, row_offset_, row_count_, start_doc_id_, task_seq_, offline_);

    memindex_insert_operator_state->SetComplete();
    return true;
}

}