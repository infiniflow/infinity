// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

export module snapshot_brief;

import stl;
import command_statement;
import status;

namespace infinity {

export struct SnapshotBrief {
    String snapshot_name_; // snapshot_name_
    SnapshotScope scope_{SnapshotScope::kInvalid};  // system / db / table snapshot
    String create_time_;      // when create the snapshot
    u64 commit_ts_;        // txn ts the snapshot created.
    u64 size_;             // total snapshot size

    static Vector<SnapshotBrief> GetSnapshots(const String& dir);
};

} // namespace infinity
