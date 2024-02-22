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

export module cleanup_task;

import stl;
import bg_task;
import txn;
import catalog;
import table_entry;

namespace infinity {

export class CleanupTask final : public BGTask {
public:
    CleanupTask(NewCatalog *catalog, Txn *txn);

public:
    ~CleanupTask() override = default;

    String ToString() const override { return "CleanupTask"; }

    void Execute();

private:
    void CleanupTable(TableEntry *table_entry);

private:
    NewCatalog *const catalog_;

    Txn *const txn_;
};

} // namespace infinity