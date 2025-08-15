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

module infinity_core:peer_task.impl;

import :peer_task;
import :stl;
import :peer_task;

import third_party;

import admin_statement;

namespace infinity {

std::string ToString(PeerTaskType type) {
    switch (type) {
        case PeerTaskType::kRegister: {
            return "register";
        }
        case PeerTaskType::kHeartBeat: {
            return "heartbeat";
        }
        case PeerTaskType::kUnregister: {
            return "unregister";
        }
        case PeerTaskType::kInvalid: {
            return "invalid";
        }
        case PeerTaskType::kChangeRole: {
            return "change role";
        }
        case PeerTaskType::kLogSync: {
            return "log sync";
        }
        case PeerTaskType::kNewLeader: {
            return "new leader";
        }
        case PeerTaskType::kTerminate: {
            return "terminate";
        }
    }
}

std::string TerminatePeerTask::ToString() const { return fmt::format("{}", infinity::ToString(type_)); }

std::string RegisterPeerTask::ToString() const {
    return fmt::format("{}@{}, {}:{}, {}", infinity::ToString(type_), node_name_, node_ip_, node_port_, infinity::ToString(node_role_));
}

std::string UnregisterPeerTask::ToString() const { return fmt::format("{}@{}", infinity::ToString(type_), node_name_); }

std::string HeartBeatPeerTask::ToString() const { return fmt::format("{}@{}, {}", infinity::ToString(type_), node_name_, txn_ts_); }

std::string SyncLogTask::ToString() const { return fmt::format("{}@{}, {}", infinity::ToString(type_), node_name_, log_strings_.size()); }

std::string ChangeRoleTask::ToString() const { return fmt::format("{} to {}", infinity::ToString(type_), role_name_); }

} // namespace infinity
