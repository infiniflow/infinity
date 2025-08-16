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

export module infinity_core:variables;

import std;

namespace infinity {

// Variable has two scopes: global and session
export enum class GlobalVariable {
    kQueryCount,              // global
    kSessionCount,            // global
    kBufferPoolUsage,         // global
    kSchedulePolicy,          // global
    kNextTxnID,               // global
    kBufferedObjectCount,     // global
    kUnusedBufferObjectCount, // global
    kActiveTxnCount,          // global
    kCurrentTs,               // global
    kTotalCommitCount,        // global
    kTotalRollbackCount,      // global
    kActiveWALFilename,       // global
    kProfileRecordCapacity,   // global
    kBackgroundTaskCount,     // global
    kRunningBGTask,           // global
    kRunningCompactTask,      // global
    kSystemMemoryUsage,       // global
    kOpenFileCount,           // global
    kCPUUsage,                // global
    kJeProf,                  // global
    kFollowerNum,             // global
    kResultCache,             // global
    kCacheResultCapacity,     // global
    kCacheResultNum,          // global
    kMemoryCacheMiss,         // global
    kDiskCacheMiss,           // global
    kEnableProfile,           // global
    kInvalid,
};

export enum class SessionVariable {
    kQueryCount,         // session
    kTotalCommitCount,   // session
    kTotalRollbackCount, // session
    kConnectedTime,      // session

    kInvalid,
};

export struct VarUtil {
    static std::unordered_map<std::string, GlobalVariable> global_name_map_;
    static std::unordered_map<std::string, SessionVariable> session_name_map_;

    static void InitVariablesMap();
    static GlobalVariable GetGlobalVarByName(const std::string &variable_name);
    static SessionVariable GetSessionVarByName(const std::string &variable_name);
};

} // namespace infinity
