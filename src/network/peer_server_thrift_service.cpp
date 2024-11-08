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
import infinity_exception;
import cluster_manager;
import admin_statement;
import node_info;

namespace infinity {

void PeerServerThriftService::Register(infinity_peer_server::RegisterResponse &response, const infinity_peer_server::RegisterRequest &request) {

    LOG_TRACE("Get Register request");
    // This must be a leader node.
    NodeRole this_server_role = InfinityContext::instance().GetServerRole();
    if (this_server_role == NodeRole::kLeader) {
        NodeRole register_node_role = NodeRole::kUnInitialized;
        switch (request.node_type) {
            case infinity_peer_server::NodeType::kFollower: {
                register_node_role = NodeRole::kFollower;
                break;
            }
            case infinity_peer_server::NodeType::kLearner: {
                register_node_role = NodeRole::kLearner;
                break;
            }
            default: {
                String error_message = fmt::format("Invalid node type: {}", infinity_peer_server::to_string(request.node_type));
                UnrecoverableError(error_message);
            }
        }
        auto now = std::chrono::system_clock::now();
        auto time_since_epoch = now.time_since_epoch();

        SharedPtr<NodeInfo> register_node_info = MakeShared<NodeInfo>(register_node_role,
                                                                      NodeStatus::kAlive,
                                                                      request.node_name,
                                                                      request.node_ip,
                                                                      request.node_port,
                                                                      request.txn_timestamp,
                                                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());
        Status status = InfinityContext::instance().cluster_manager()->AddNodeInfo(register_node_info);
        if (status.ok()) {
            LOG_INFO(fmt::format("Node: {} registered as {}.", request.node_name, infinity_peer_server::to_string(request.node_type)));
            SharedPtr<NodeInfo> leader_node = InfinityContext::instance().cluster_manager()->ThisNode();
            response.leader_name = leader_node->node_name();
            response.leader_term = leader_node->leader_term();
            response.heart_beat_interval = leader_node->heartbeat_interval();
        } else {
            response.error_code = static_cast<i64>(status.code());
            response.error_message = status.message();
        }
    } else {
        response.error_code = static_cast<i64>(ErrorCode::kInvalidNodeRole);
        response.error_message = "Attempt to register a non-leader node";
    }

    return;
}

void PeerServerThriftService::Unregister(infinity_peer_server::UnregisterResponse &response, const infinity_peer_server::UnregisterRequest &request) {
    LOG_TRACE("Get Unregister request");
    NodeRole this_server_role = InfinityContext::instance().GetServerRole();
    if (this_server_role == NodeRole::kLeader) {
        Status status = InfinityContext::instance().cluster_manager()->UpdateNodeByLeader(request.node_name, UpdateNodeOp::kRemove);
        if (!status.ok()) {
            response.error_code = static_cast<i64>(status.code_);
            response.error_message = status.message();
        }
        LOG_INFO(fmt::format("Node: {} unregistered from leader.", request.node_name));
    } else {
        response.error_code = static_cast<i64>(ErrorCode::kInvalidNodeRole);
        response.error_message = "Attempt to unregister from a non-leader node";
    }
    return;
}

void PeerServerThriftService::HeartBeat(infinity_peer_server::HeartBeatResponse &response, const infinity_peer_server::HeartBeatRequest &request) {
    LOG_DEBUG("Get HeartBeat request");
    NodeRole this_server_role = InfinityContext::instance().GetServerRole();
    if (this_server_role == NodeRole::kLeader) {
        NodeRole non_leader_node_role = NodeRole::kUnInitialized;
        switch (request.node_type) {
            case infinity_peer_server::NodeType::kLeader: {
                non_leader_node_role = NodeRole::kLeader;
                break;
            }
            case infinity_peer_server::NodeType::kFollower: {
                non_leader_node_role = NodeRole::kFollower;
                break;
            }
            case infinity_peer_server::NodeType::kLearner: {
                non_leader_node_role = NodeRole::kLearner;
                break;
            }
            default: {
                String error_message = "Invalid node type";
                UnrecoverableError(error_message);
            }
        }

        auto now = std::chrono::system_clock::now();
        auto time_since_epoch = now.time_since_epoch();
        SharedPtr<NodeInfo> register_node_info = MakeShared<NodeInfo>(non_leader_node_role,
                                                                      NodeStatus::kAlive,
                                                                      request.node_name,
                                                                      request.node_ip,
                                                                      request.node_port,
                                                                      request.txn_timestamp,
                                                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

        Status status = InfinityContext::instance().cluster_manager()->UpdateNodeInfoByHeartBeat(register_node_info,
                                                                                                 response.other_nodes,
                                                                                                 response.leader_term,
                                                                                                 response.sender_status);
        if (!status.ok()) {
            response.error_code = static_cast<i64>(status.code());
            response.error_message = status.message();
        }
    } else {
        response.error_code = static_cast<i64>(ErrorCode::kInvalidNodeRole);
        response.sender_status = infinity_peer_server::NodeStatus::type::kRemoved;
        response.error_message = fmt::format("Attempt to heartbeat from a non-leader node: {}", ToString(this_server_role));
    }
    return;
}

void PeerServerThriftService::SyncLog(infinity_peer_server::SyncLogResponse &response, const infinity_peer_server::SyncLogRequest &request) {
    LOG_INFO("Get SyncLog request");
    if (request.log_entries.size() == 0) {
        UnrecoverableError("No log is synced from leader node");
    }

    InfinityContext::instance().storage()->wal_manager()->FlushLogByReplication(request.log_entries);

    Status status = Status::OK();
    if (request.on_startup) {
        status = InfinityContext::instance().cluster_manager()->ContinueStartup(request.log_entries);
    } else {
        status = InfinityContext::instance().cluster_manager()->ApplySyncedLogNolock(request.log_entries);
    }

    if (!status.ok()) {
        response.error_code = static_cast<i64>(status.code());
        response.error_message = status.message();
    }
    return;
}

void PeerServerThriftService::ChangeRole(infinity_peer_server::ChangeRoleResponse &response, const infinity_peer_server::ChangeRoleRequest &request) {
    Status status = Status::OK();
    switch (request.node_type) {
        case infinity_peer_server::NodeType::kAdmin: {
            status = InfinityContext::instance().ChangeRole(NodeRole::kAdmin, true);
            break;
        }
        default: {
            UnrecoverableError("Not support to change to other type of node");
        }
    }

    if (!status.ok()) {
        response.error_code = static_cast<i64>(status.code());
        response.error_message = status.message();
    }
    return;
}

void PeerServerThriftService::NewLeader(infinity_peer_server::NewLeaderResponse &response, const infinity_peer_server::NewLeaderRequest &request) {
    LOG_INFO("Get NewLeader request");
    return;
}

} // namespace infinity
