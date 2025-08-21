// Copyright(C) 2025 InfiniFlow, Inc. All rights reserved.
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

module infinity_core:bg_task_type.impl;

import :bg_task_type;
import :infinity_exception;
import :bg_task_type;

import std;

namespace infinity {

std::string ToString(BGTaskType type) {
    switch (type) {
        case BGTaskType::kInvalid: {
            return "Invalid";
            break;
        }
        case BGTaskType::kStopProcessor: {
            return "StopProcessor";
            break;
        }
        case BGTaskType::kNewCheckpoint: {
            return "NewCheckpoint";
            break;
        }
        case BGTaskType::kNotifyCompact: {
            return "NotifyCompact";
            break;
        }
        case BGTaskType::kNewCompact: {
            return "NewCompact";
            break;
        }
        case BGTaskType::kNotifyOptimize: {
            return "NotifyOptimize";
            break;
        }
        case BGTaskType::kNewCleanup: {
            return "NewCleanup";
            break;
        }
        case BGTaskType::kUpdateSegmentBloomFilterData: {
            return "UpdateSegmentBloomFilterData";
            break;
        }
        case BGTaskType::kDumpMemIndex: {
            return "DumpMemIndex";
            break;
        }
        case BGTaskType::kAppendMemIndex: {
            return "AppendMemIndex";
            break;
        }
        case BGTaskType::kTestCommand: {
            return "TestCommand";
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unknown BGTaskType: {}", std::to_string(static_cast<int>(type))));
            break;
        }
    }
    return "";
}

} // namespace infinity