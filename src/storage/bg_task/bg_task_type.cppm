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

export module infinity_core:bg_task_type;

import :stl;
import :third_party;
import global_resource_usage;
import :status;

namespace infinity {

export enum class BGTaskType {
    kStopProcessor,
    kNewCheckpoint,
    kNotifyCompact,
    kNewCompact,
    kNotifyOptimize,
    kNewCleanup,
    kUpdateSegmentBloomFilterData, // Not used
    kDumpMemIndex,
    kAppendMemIndex,
    kTestCommand,
    kInvalid
};

export String ToString(BGTaskType type);

} // namespace infinity