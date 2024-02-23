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
import cleanup_scanner;
import catalog;

namespace infinity {

export class CleanupTask final : public BGTask {
public:
    // Try clean up is async task?
    CleanupTask(NewCatalog *catalog, TxnTimeStamp visible_ts) : BGTask(BGTaskType::kTryCleanup, true), catalog_(catalog), visible_ts_(visible_ts) {}

public:
    ~CleanupTask() override = default;

    String ToString() const override { return "CleanupTask"; }

    void Execute() {
        CleanupScanner scanner(catalog_, visible_ts_);
        scanner.Scan();
        // FIXME(sys): This is a blocking call, should it be async?
        std::move(scanner).Cleanup();
    }

private:
    NewCatalog *const catalog_;

    const TxnTimeStamp visible_ts_;
};

} // namespace infinity