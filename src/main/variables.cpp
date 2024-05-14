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

void VarUtil::InitVariablesMap() {
    global_name_map_["query_count"] = GlobalVariable::kQueryCount;
    global_name_map_["session_count"] = GlobalVariable::kSessionCount;
    global_name_map_["buffer_usage"] = GlobalVariable::kBufferPoolUsage;
    global_name_map_["schedule_policy"] = GlobalVariable::kSchedulePolicy;
    global_name_map_["delta_log_count"] = GlobalVariable::kDeltaLogCount;
    global_name_map_["next_transaction_id"] = GlobalVariable::kNextTxnID;
    global_name_map_["buffer_object_count"] = GlobalVariable::kBufferedObjectCount;
    global_name_map_["unused_buffer_object"] = GlobalVariable::kUnusedBufferObjectCount;
    global_name_map_["active_txn_count"] = GlobalVariable::kActiveTxnCount;
    global_name_map_["current_timestamp"] = GlobalVariable::kCurrentTs;
    global_name_map_["total_commit_count"] = GlobalVariable::kTotalCommitCount;
    global_name_map_["total_rollback_count"] = GlobalVariable::kTotalRollbackCount;
    global_name_map_["active_wal_filename"] = GlobalVariable::kActiveWALFilename;

    session_name_map_["query_count"] = SessionVariable::kQueryCount;
    session_name_map_["total_commit_count"] = SessionVariable::kTotalCommitCount;
    session_name_map_["total_rollback_count"] = SessionVariable::kTotalRollbackCount;
    session_name_map_["connected_timestamp"] = SessionVariable::kConnectedTime;
    session_name_map_["enable_profile"] = SessionVariable::kEnableProfile;
    session_name_map_["profile_record_capacity"] = SessionVariable::kProfileRecordCapacity;
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
