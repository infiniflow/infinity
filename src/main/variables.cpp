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

module variables;

import stl;

namespace infinity {

void SystemVariables::InitVariablesMap() {
    name_map_["query_count"] = SysVariable::kQueryCount;
    name_map_["session_count"] = SysVariable::kSessionCount;
    name_map_["buffer_pool_usage"] = SysVariable::kBufferPoolUsage;
    name_map_["schedule_policy"] = SysVariable::kSchedulePolicy;
    name_map_["delta_log_count"] = SysVariable::kDeltaLogCount;
    name_map_["next_transaction_id"] = SysVariable::kNextTxnID;
    name_map_["buffered_object_count"] = SysVariable::kBufferedObjectCount;
    name_map_["object_count_for_gc"] = SysVariable::kGCListSizeOfBufferManager;
    name_map_["active_transaction_count"] = SysVariable::kActiveTxnCount;
    name_map_["current_ts"] = SysVariable::kCurrentTs;
    name_map_["total_commit_count"] = SysVariable::kTotalCommitCount;
    name_map_["connected_time"] = SysVariable::kConnectedTime;
    name_map_["catalog_version"] = SysVariable::kCatalogVersion;
    name_map_["active_wal_filename"] = SysVariable::kActiveWALFilename;
}

HashMap<String, SysVariable> SystemVariables::name_map_;

SysVariable SystemVariables::GetSysVarEnumByName(const String &variable_name) {
    auto it = name_map_.find(variable_name);
    if (it != name_map_.end()) {
        return it->second;
    }
    return SysVariable::kInvalid;
}

}
