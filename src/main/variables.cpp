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
import default_values;

namespace infinity {

void VarUtil::InitVariablesMap() {
    global_name_map_[QUERY_COUNT_VAR_NAME.data()] = GlobalVariable::kQueryCount;
    global_name_map_[SESSION_COUNT_VAR_NAME.data()] = GlobalVariable::kSessionCount;
    global_name_map_[BUFFER_USAGE_VAR_NAME.data()] = GlobalVariable::kBufferPoolUsage;
    global_name_map_[SCHEDULE_POLICY_VAR_NAME.data()] = GlobalVariable::kSchedulePolicy;
    global_name_map_[DELTA_LOG_COUNT_VAR_NAME.data()] = GlobalVariable::kDeltaLogCount;
    global_name_map_[NEXT_TXN_ID_VAR_NAME.data()] = GlobalVariable::kNextTxnID;
    global_name_map_[BUFFER_OBJECT_COUNT_VAR_NAME.data()] = GlobalVariable::kBufferedObjectCount;
    global_name_map_[UNUSED_BUFFER_OBJECT_COUNT_VAR_NAME.data()] = GlobalVariable::kUnusedBufferObjectCount;
    global_name_map_[ACTIVE_TXN_COUNT_VAR_NAME.data()] = GlobalVariable::kActiveTxnCount;
    global_name_map_[CURRENT_TS_VAR_NAME.data()] = GlobalVariable::kCurrentTs;
    global_name_map_[TOTAL_COMMIT_COUNT_VAR_NAME.data()] = GlobalVariable::kTotalCommitCount;
    global_name_map_[TOTAL_ROLLBACK_COUNT_VAR_NAME.data()] = GlobalVariable::kTotalRollbackCount;
    global_name_map_[ACTIVE_WAL_FILENAME_VAR_NAME.data()] = GlobalVariable::kActiveWALFilename;
    global_name_map_[PROFILE_RECORD_CAPACITY_VAR_NAME.data()] = GlobalVariable::kProfileRecordCapacity;
    global_name_map_[BG_TASK_COUNT_VAR_NAME.data()] = GlobalVariable::kBackgroundTaskCount;
    global_name_map_[RUNNING_BG_TASK_VAR_NAME.data()] = GlobalVariable::kRunningBGTask;
    global_name_map_[RUNNING_COMPACT_TASK_VAR_NAME.data()] = GlobalVariable::kRunningCompactTask;
    global_name_map_[SYSTEM_MEMORY_USAGE_VAR_NAME.data()] = GlobalVariable::kSystemMemoryUsage;
    global_name_map_[OPEN_FILE_COUNT_VAR_NAME.data()] = GlobalVariable::kOpenFileCount;
    global_name_map_[CPU_USAGE_VAR_NAME.data()] = GlobalVariable::kCPUUsage;
    global_name_map_["jeprof"] = GlobalVariable::kJeProf;
    global_name_map_["cleanup_trace"] = GlobalVariable::kCleanupTrace;
    global_name_map_[FOLLOWER_NUMBER.data()] = GlobalVariable::kFollowerNum;

    session_name_map_[QUERY_COUNT_VAR_NAME.data()] = SessionVariable::kQueryCount;
    session_name_map_[TOTAL_COMMIT_COUNT_VAR_NAME.data()] = SessionVariable::kTotalCommitCount;
    session_name_map_[TOTAL_ROLLBACK_COUNT_VAR_NAME.data()] = SessionVariable::kTotalRollbackCount;
    session_name_map_[CONNECTED_TS_VAR_NAME.data()] = SessionVariable::kConnectedTime;
    session_name_map_["enable_profile"] = SessionVariable::kEnableProfile;
}

HashMap<String, GlobalVariable> VarUtil::global_name_map_;
HashMap<String, SessionVariable> VarUtil::session_name_map_;

GlobalVariable VarUtil::GetGlobalVarByName(const String &variable_name) {
    auto it = global_name_map_.find(variable_name);
    if (it != global_name_map_.end()) {
        return it->second;
    }
    return GlobalVariable::kInvalid;
}

SessionVariable VarUtil::GetSessionVarByName(const String &variable_name) {
    auto it = session_name_map_.find(variable_name);
    if (it != session_name_map_.end()) {
        return it->second;
    }
    return SessionVariable::kInvalid;
}

}
