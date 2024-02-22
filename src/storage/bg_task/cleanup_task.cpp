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

module cleanup_task;

import txn;
import catalog;
import db_entry;
import table_entry;
import block_entry;

namespace infinity {

// Clean up is async task ?
CleanupTask::CleanupTask(NewCatalog *catalog, Txn *txn) : BGTask(BGTaskType::kCleanup, true), catalog_(catalog), txn_(txn) {}

void CleanupTask::Execute() {
    TxnTimeStamp cleanup_ts = txn_->BeginTS();
    //
}

} // namespace infinity
