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

#include <vector>

module physical_compact;

import stl;
import internal_types;
import operator_state;
import base_table_ref;
import block_index;
import column_vector;
import compact_state_data;
import default_values;
import logger;
import infinity_exception;
import third_party;
import status;
import wal_entry;
import wal_manager;
import infinity_context;

import new_txn;
import db_meeta;
import table_meeta;
import segment_meta;
import bg_task;
import compaction_process;

namespace infinity {

void PhysicalCompact::Init(QueryContext *query_context) {
}

bool PhysicalCompact::Execute(QueryContext *query_context, OperatorState *operator_state) {
    auto *compact_operator_state = static_cast<CompactOperatorState *>(operator_state);

    const String &db_name = *base_table_ref_->table_info_->db_name_;
    const String &table_name = *base_table_ref_->table_info_->table_name_;
    NewTxn *new_txn = query_context->GetNewTxn();

    auto compact_task = MakeShared<NewCompactTask>(new_txn, db_name, table_name);
    auto *compaction_processor = InfinityContext::instance().storage()->compaction_processor();
    compaction_processor->Submit(compact_task);
    compact_task->Wait();

    compact_operator_state->status_ = compact_task->result_status_;
    compact_operator_state->SetComplete();
    return true;
}

} // namespace infinity