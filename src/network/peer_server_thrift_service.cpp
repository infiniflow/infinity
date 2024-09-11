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

module peer_server_thrift_service;

import stl;
import third_party;
import logger;
import peer_server_thrift_types;
import infinity_context;
import peer_task;
import status;

namespace infinity {

void PeerServerThriftService::Register(infinity_peer_server::RegisterResponse &response, const infinity_peer_server::RegisterRequest &request) {

    LOG_TRACE("Get Register request");
    // This must be a leader node.
    NodeInfo* leader_node = InfinityContext::instance().cluster_manager()->ThisNode().get();
    if(leader_node->node_role_ == NodeRole::kLeader) {
        SharedPtr<NodeInfo> non_leader_node_info = MakeShared<NodeInfo>();
        non_leader_node_info->node_name_ = request.node_name;
        switch(request.node_type) {
            case infinity_peer_server::NodeType::kLeader: {
                non_leader_node_info->node_role_ = NodeRole::kLeader;
                break;
            }
            case infinity_peer_server::NodeType::kFollower: {
                non_leader_node_info->node_role_ = NodeRole::kFollower;
                break;
            }
            case infinity_peer_server::NodeType::kLearner: {
                non_leader_node_info->node_role_ = NodeRole::kLearner;
                break;
            }
        }
        non_leader_node_info->ip_address_ = request.node_ip;
        non_leader_node_info->port_ = request.node_port;
        non_leader_node_info->node_status_ = NodeStatus::kAlive;
        non_leader_node_info->txn_timestamp_ = request.txn_timestamp;

        auto now = std::chrono::system_clock::now();
        auto time_since_epoch = now.time_since_epoch();
        non_leader_node_info->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
        InfinityContext::instance().cluster_manager()->AddNodeInfo(non_leader_node_info);

        response.leader_name = leader_node->node_name_;
        response.leader_term = leader_node->leader_term_;
        response.heart_beat_interval = leader_node->heartbeat_interval_;
    } else {
        response.error_code = static_cast<i64>(ErrorCode::kInvalidServerRole);
        response.error_msg = fmt::format("Attempt to register a non-leader node: {}", ToString(leader_node->node_role_));
    }

    return ;
}

void PeerServerThriftService::UnRegister(infinity_peer_server::UnRegisterResponse &response, const infinity_peer_server::UnRegisterRequest &request) {
    LOG_INFO("Get UnRegister request");
    return ;
}
void PeerServerThriftService::HeartBeat(infinity_peer_server::HeartBeatResponse &response, const infinity_peer_server::HeartBeatRequest &request) {
    LOG_INFO("Get HeartBeat request");
    return ;
}
void PeerServerThriftService::SyncLog(infinity_peer_server::SyncLogResponse &response, const infinity_peer_server::SyncLogRequest &request) {
    LOG_INFO("Get SyncLog request");
    return ;
}
void PeerServerThriftService::ChangeRole(infinity_peer_server::ChangeRoleResponse &response, const infinity_peer_server::ChangeRoleRequest &request) {
    LOG_INFO("Get ChangeRole request");
    return ;
}
void PeerServerThriftService::NewLeader(infinity_peer_server::NewLeaderResponse &response, const infinity_peer_server::NewLeaderRequest &request) {
    LOG_INFO("Get NewLeader request");
    return ;
}

} // namespace infinity
