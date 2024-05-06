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

export module variables;

import stl;
import command_statement;

namespace infinity {

// Variable has two scopes: global and session
export enum class SysVariable {
    kQueryCount,                // global and session
    kSessionCount,              // global
    kBufferPoolUsage,           // global
    kSchedulePolicy,            // global
    kDeltaLogCount,             // global
    kNextTxnID,                 // global
    kBufferedObjectCount,       // global
    kGCListSizeOfBufferManager, // global
    kActiveTxnCount,            // global
    kCurrentTs,                 // global
    kTotalCommitCount,          // global and session
    kConnectedTime,             // session
    kCatalogVersion,            // global
    kActiveWALFilename,         // global
    kEnableProfile,             // session
    kProfileRecordCapacity,     // session

    kInvalid,
};



export struct SystemVariables {
    static HashMap<String, SysVariable> name_map_;

    static void InitVariablesMap();
    static SysVariable GetSysVarEnumByName(const String &variable_name);
};

}
