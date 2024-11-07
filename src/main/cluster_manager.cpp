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

#include <vector>

module cluster_manager;

import stl;
import infinity_context;
import logger;
import infinity_exception;
import peer_server_thrift_types;
import wal_manager;
import wal_entry;
import admin_statement;
import global_resource_usage;
import node_info;
import config;

namespace infinity {

ClusterManager::ClusterManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("ClusterManager");
#endif
}

ClusterManager::~ClusterManager() {
    other_node_map_.clear();
    this_node_.reset();
    if (client_to_leader_.get() != nullptr) {
        client_to_leader_->UnInit(true);
    }
    client_to_leader_.reset();
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("ClusterManager");
#endif
}

void ClusterManager::InitAsAdmin() { current_node_role_ = NodeRole::kAdmin; }

void ClusterManager::InitAsStandalone() { current_node_role_ = NodeRole::kStandalone; }

Status ClusterManager::InitAsLeader(const String &node_name) {

    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as leader error: already initialized.");
    }

    Config *config_ptr = InfinityContext::instance().config();
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    this_node_ = MakeShared<NodeInfo>(NodeRole::kLeader,
                                      NodeStatus::kAlive,
                                      node_name,
                                      config_ptr->PeerServerIP(),
                                      config_ptr->PeerServerPort(),
                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

    current_node_role_ = NodeRole::kLeader;

    return Status::OK();
}

Status ClusterManager::InitAsFollower(const String &node_name, const String &leader_ip, i64 leader_port) {

    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as follower error: already initialized.");
    }

    Config *config_ptr = InfinityContext::instance().config();
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    this_node_ = MakeShared<NodeInfo>(NodeRole::kFollower,
                                      NodeStatus::kAlive,
                                      node_name,
                                      config_ptr->PeerServerIP(),
                                      config_ptr->PeerServerPort(),
                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

    leader_node_ = MakeShared<NodeInfo>(NodeRole::kLeader, leader_ip, leader_port);
    Status client_status = Status::OK();
    std::tie(client_to_leader_, client_status) = ClusterManager::ConnectToServerNoLock(node_name, leader_ip, leader_port);
    if (!client_status.ok()) {
        return client_status;
    }

    current_node_role_ = NodeRole::kFollower;

    return Status::OK();
}

Status ClusterManager::InitAsLearner(const String &node_name, const String &leader_ip, i64 leader_port) {

    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as learner error: already initialized.");
    }

    Config *config_ptr = InfinityContext::instance().config();
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    this_node_ = MakeShared<NodeInfo>(NodeRole::kLearner,
                                      NodeStatus::kAlive,
                                      node_name,
                                      config_ptr->PeerServerIP(),
                                      config_ptr->PeerServerPort(),
                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

    leader_node_ = MakeShared<NodeInfo>(NodeRole::kLeader, leader_ip, leader_port);

    Status client_status = Status::OK();
    std::tie(client_to_leader_, client_status) = ClusterManager::ConnectToServerNoLock(node_name, leader_ip, leader_port);
    if (!client_status.ok()) {
        return client_status;
    }

    current_node_role_ = NodeRole::kLearner;

    return Status::OK();
}

Status ClusterManager::UnInit(bool not_unregister) {
    if (hb_periodic_thread_.get() != nullptr) {
        {
            std::lock_guard lock(hb_mutex_);
            hb_running_ = false;
            hb_cv_.notify_all();
        }
        hb_periodic_thread_->join();
        hb_periodic_thread_.reset();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    if (!not_unregister) {
        Status status = UnregisterToLeaderNoLock();
    }
    current_node_role_ = NodeRole::kUnInitialized;

    other_node_map_.clear();
    leader_node_.reset();
    this_node_.reset();
    if (client_to_leader_.get() != nullptr) {
        client_to_leader_->UnInit(true);
    }
    client_to_leader_.reset();

    for (const auto &reader_client_pair : reader_client_map_) {
        clients_for_cleanup_.emplace_back(reader_client_pair.second);
    }
    reader_client_map_.clear();
    logs_to_sync_.clear();

    return Status::OK();
}

Status ClusterManager::RegisterToLeader() {
    // Register to leader;
    std::unique_lock<std::mutex> lock(mutex_);
    Status status = RegisterToLeaderNoLock();
    if (!status.ok()) {
        return status;
    }

    this->hb_running_ = true;
    hb_periodic_thread_ = MakeShared<Thread>([this] { this->HeartBeatToLeaderThread(); });
    return status;
}

void ClusterManager::HeartBeatToLeaderThread() {
    // Heartbeat interval
    auto hb_interval = std::chrono::milliseconds(leader_node_->heartbeat_interval());
    Storage *storage_ptr = InfinityContext::instance().storage();
    while (true) {
        std::unique_lock hb_lock(this->hb_mutex_);
        this->hb_cv_.wait_for(hb_lock, hb_interval, [&] { return !this->hb_running_; });
        if (!hb_running_) {
            // UnInit cluster manager will set hb_running_ false;
            break;
        }

        if (!client_to_leader_->ServerConnected()) {
            // If peer_client was disconnected, try to reconnect to server.
            Status connect_status = client_to_leader_->Reconnect();
            if (!connect_status.ok()) {
                LOG_ERROR(connect_status.message());
                continue;
            }
        }

        // Update latest update time
        auto hb_now = std::chrono::system_clock::now();
        auto hb_time_since_epoch = hb_now.time_since_epoch();
        SharedPtr<HeartBeatPeerTask> hb_task = nullptr;
        {
            std::unique_lock<std::mutex> cluster_manager_lock(mutex_);
            this_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(hb_time_since_epoch).count());

            String this_node_name = this_node_->node_name();
            NodeRole this_node_role = this_node_->node_role();
            String this_node_ip = this_node_->node_ip();
            i64 this_node_port = this_node_->node_port();

            // Send heartbeat
            hb_task =
                MakeShared<HeartBeatPeerTask>(this_node_name, this_node_role, this_node_ip, this_node_port, storage_ptr->txn_manager()->CurrentTS());
        }
        client_to_leader_->Send(hb_task);
        hb_task->Wait();

        if (hb_task->error_code_ != 0) {
            if (hb_task->error_code_ == (i64)(ErrorCode::kNotRegistered)) {
                LOG_ERROR(fmt::format("Error to connect to leader: {}, {}:{}, error: {}",
                                      leader_node_->node_name(),
                                      leader_node_->node_ip(),
                                      leader_node_->node_port(),
                                      hb_task->error_message_));
                leader_node_->set_node_status(hb_task->sender_status_);
            } else {
                LOG_ERROR(fmt::format("Can't connect to leader: {}, {}:{}, error: {}",
                                      leader_node_->node_name(),
                                      leader_node_->node_ip(),
                                      leader_node_->node_port(),
                                      hb_task->error_message_));
                leader_node_->set_node_status(NodeStatus::kTimeout);
            }
            continue;
        }

        // Prepare the update time of leader
        hb_now = std::chrono::system_clock::now();
        hb_time_since_epoch = hb_now.time_since_epoch();

        {
            std::unique_lock<std::mutex> cluster_manager_lock(mutex_);
            // Update leader info
            leader_node_->set_node_status(NodeStatus::kAlive);
            leader_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(hb_time_since_epoch).count());
            leader_node_->set_leader_term(hb_task->leader_term_);

            // Update heartbeat sending count
            this_node_->heartbeat_count_increase();

            // Update the non-leader node info from leader
            UpdateNodeInfoNoLock(hb_task->other_nodes_);

            // Check if leader can't send message to this reader node
            this_node_->set_node_status(hb_task->sender_status_);
        }
    }
    return;
}

void ClusterManager::CheckHeartBeat() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (current_node_role_ != NodeRole::kLeader) {
        String error_message = "Invalid node role.";
        UnrecoverableError(error_message);
    }
    hb_periodic_thread_ = MakeShared<Thread>([this] { this->CheckHeartBeatThread(); });
}

void ClusterManager::CheckHeartBeatThread() {
    if (current_node_role_ != NodeRole::kLeader) {
        String error_message = "Invalid node role.";
        UnrecoverableError(error_message);
    }
    // this_node_ is the leader;
    auto hb_interval = std::chrono::milliseconds(this_node_->heartbeat_interval());
    while (true) {
        std::unique_lock hb_lock(this->hb_mutex_);
        this->hb_cv_.wait_for(hb_lock, hb_interval, [&] { return !this->hb_running_; });
        if (!hb_running_) {
            // UnInit cluster manager will set hb_running_ false;
            break;
        }

        auto now = std::chrono::system_clock::now();
        auto time_since_epoch = now.time_since_epoch();
        this_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

        for (auto &[node_name, other_node] : other_node_map_) {
            NodeStatus other_node_status = other_node->node_status();
            if (other_node_status == NodeStatus::kAlive) {
                u64 other_node_update_ts = other_node->update_ts();
                u64 this_node_update_ts = this_node_->update_ts();
                u64 this_node_hb_interval = this_node_->heartbeat_interval();

                if (other_node_update_ts + 2 * this_node_hb_interval < this_node_update_ts) {
                    other_node->set_node_status(NodeStatus::kTimeout);
                    LOG_INFO(fmt::format("Node {} is timeout", node_name));
                }
            }
        }
    }
}

Status ClusterManager::RegisterToLeaderNoLock() {
    // Register to leader, used by follower and learner
    Storage *storage_ptr = InfinityContext::instance().storage();
    SharedPtr<RegisterPeerTask> register_peer_task = nullptr;
    if (storage_ptr->reader_init_phase() == ReaderInitPhase::kPhase2) {
        register_peer_task = MakeShared<RegisterPeerTask>(this_node_->node_name(),
                                                          this_node_->node_role(),
                                                          this_node_->node_ip(),
                                                          this_node_->node_port(),
                                                          storage_ptr->txn_manager()->CurrentTS());
    } else {
        register_peer_task =
            MakeShared<RegisterPeerTask>(this_node_->node_name(), this_node_->node_role(), this_node_->node_ip(), this_node_->node_port(), 0);
    }

    client_to_leader_->Send(register_peer_task);
    register_peer_task->Wait();

    Status status = Status::OK();
    if (register_peer_task->error_code_ != 0) {
        status.code_ = static_cast<ErrorCode>(register_peer_task->error_code_);
        status.msg_ = MakeUnique<String>(register_peer_task->error_message_);
        return status;
    }
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    leader_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());
    leader_node_->set_node_name(register_peer_task->leader_name_);
    leader_node_->set_node_status(NodeStatus::kAlive);
    //    client_to_leader_->SetPeerNode(NodeRole::kLeader, register_peer_task->leader_name_, register_peer_task->update_time_);
    // Start HB thread.
    if (register_peer_task->heartbeat_interval_ == 0) {
        leader_node_->set_heartbeat_interval(1000); // 1000 ms by default
    } else {
        leader_node_->set_heartbeat_interval(register_peer_task->heartbeat_interval_);
    }

    return status;
}

Status ClusterManager::UnregisterToLeaderNoLock() {
    Status status = Status::OK();
    if (current_node_role_ == NodeRole::kFollower or current_node_role_ == NodeRole::kLearner) {
        if (leader_node_->node_status() == NodeStatus::kAlive) {
            // Leader is alive, need to unregister
            SharedPtr<UnregisterPeerTask> unregister_task = MakeShared<UnregisterPeerTask>(this_node_->node_name());
            client_to_leader_->Send(unregister_task);
            unregister_task->Wait();
            if (unregister_task->error_code_ != 0) {
                LOG_ERROR(fmt::format("Fail to unregister from leader: {}", unregister_task->error_message_));
                status.code_ = static_cast<ErrorCode>(unregister_task->error_code_);
                status.msg_ = MakeUnique<String>(unregister_task->error_message_);
            }
        }
    }
    return status;
}

Tuple<SharedPtr<PeerClient>, Status> ClusterManager::ConnectToServerNoLock(const String &from_node_name, const String &server_ip, i64 server_port) {
    SharedPtr<PeerClient> client = MakeShared<PeerClient>(from_node_name, server_ip, server_port);
    Status client_status = client->Init();
    if (!client_status.ok()) {
        return {nullptr, client_status};
    }
    return {client, client_status};
}

Status ClusterManager::SendLogs(const String &node_name,
                                const SharedPtr<PeerClient> &peer_client,
                                const Vector<SharedPtr<String>> &logs,
                                bool synchronize,
                                bool on_register) {
    SharedPtr<SyncLogTask> sync_log_task = MakeShared<SyncLogTask>(node_name, logs, on_register);
    peer_client->Send(sync_log_task);

    Status status = Status::OK();
    if (synchronize) {
        sync_log_task->Wait();
    } else {
        return status;
    }

    if (sync_log_task->error_code_ != 0) {
        LOG_ERROR(fmt::format("Fail to send log follower: {}, error message: {}", node_name, sync_log_task->error_message_));
        status.code_ = static_cast<ErrorCode>(sync_log_task->error_code_);
        status.msg_ = MakeUnique<String>(sync_log_task->error_message_);
    }
    return status;
}

Status ClusterManager::GetReadersInfo(Vector<SharedPtr<NodeInfo>> &followers,
                                      Vector<SharedPtr<PeerClient>> &follower_clients,
                                      Vector<SharedPtr<NodeInfo>> &learners,
                                      Vector<SharedPtr<PeerClient>> &learner_clients) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (current_node_role_ != NodeRole::kLeader) {
        return Status::InvalidNodeRole("Expect leader node");
    }

    for (const auto &node_info_pair : other_node_map_) {
        NodeRole other_node_role = node_info_pair.second->node_role();
        NodeStatus other_node_status = node_info_pair.second->node_status();
        if (other_node_role == NodeRole::kFollower && other_node_status == NodeStatus::kAlive) {
            followers.emplace_back(node_info_pair.second);
            follower_clients.emplace_back(reader_client_map_[node_info_pair.first]);
        }
        if (other_node_role == NodeRole::kLearner && other_node_status == NodeStatus::kAlive) {
            learners.emplace_back(node_info_pair.second);
            learner_clients.emplace_back(reader_client_map_[node_info_pair.first]);
        }
    }

    return Status::OK();
}

Status ClusterManager::AddNodeInfo(const SharedPtr<NodeInfo> &other_node) {
    // Only used by Leader on follower/learner registration.

    String other_node_name = other_node->node_name();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (current_node_role_ != NodeRole::kLeader) {
            String error_message = "Non-leader role can't add other node.";
            UnrecoverableError(error_message);
        }

        // Add by register
        auto iter = other_node_map_.find(other_node_name);
        if (iter != other_node_map_.end()) {
            // Duplicated node
            // TODO: Update node info and not throw error.
            return Status::DuplicateNode(other_node_name);
        }
    }

    // Connect to follower/learner server.
    auto [client_to_follower, client_status] =
        ClusterManager::ConnectToServerNoLock(this_node_->node_name(), other_node->node_ip(), other_node->node_port());
    if (!client_status.ok()) {
        return client_status;
    }

    Status log_sending_status = SyncLogsOnRegistration(other_node, client_to_follower);
    if (log_sending_status.ok()) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (current_node_role_ != NodeRole::kLeader) {
            String error_message = "Non-leader role can't add other node.";
            UnrecoverableError(error_message);
        }

        // Add by register
        auto iter = other_node_map_.find(other_node_name);
        if (iter == other_node_map_.end()) {
            // Not find, so add the node
            other_node_map_.emplace(other_node_name, other_node);
        } else {
            // Duplicated node
            // TODO: If the exist node lost connection, or other malfunction, use new other_node to replace it.
            return Status::DuplicateNode(other_node_name);
        }

        reader_client_map_.emplace(other_node_name, client_to_follower);
    }

    return log_sending_status;
}

Status ClusterManager::RemoveNodeInfo(const String &node_name) {
    // Used by leader to remove node
    if (node_name == this_node_->node_name()) {
        return Status::InvalidNodeRole(fmt::format("Can't remove current node: {}", node_name));
    }

    NodeRole server_role = InfinityContext::instance().GetServerRole();
    if (server_role != NodeRole::kLeader) {
        return Status::InvalidNodeRole(fmt::format("Can't remove node in {} mode", ToString(server_role)));
    }

    SharedPtr<PeerClient> client_{nullptr};
    {
        std::unique_lock<std::mutex> lock(mutex_);
        auto node_iter = other_node_map_.find(node_name);
        if (node_iter != other_node_map_.end()) {
            node_iter->second->set_node_status(NodeStatus::kRemoved);
        } else {
            return Status::NotExistNode(node_name);
        }

        auto client_iter = reader_client_map_.find(node_name);
        if (client_iter != reader_client_map_.end()) {
            client_ = client_iter->second;
        } else {
            UnrecoverableError(fmt::format("Cant' find node {} in client collection.", node_name));
        }
    }

    SharedPtr<ChangeRoleTask> change_role_task = MakeShared<ChangeRoleTask>(node_name, "admin");
    client_->Send(change_role_task);
    change_role_task->Wait();

    {
        std::unique_lock<std::mutex> lock(mutex_);
        other_node_map_.erase(node_name);
        clients_for_cleanup_.emplace_back(reader_client_map_[node_name]);
        reader_client_map_.erase(node_name);
    }

    Status status = Status::OK();
    if (change_role_task->error_code_ != 0) {
        LOG_ERROR(fmt::format("Fail to change {} to the role of ADMIN, error: ", node_name, change_role_task->error_message_));
        status.code_ = static_cast<ErrorCode>(change_role_task->error_code_);
        status.msg_ = MakeUnique<String>(change_role_task->error_message_);
    }
    return status;
}

Status ClusterManager::UpdateNodeByLeader(const String &node_name, UpdateNodeOp update_node_op) {
    // Only used in leader mode.
    std::unique_lock<std::mutex> lock(mutex_);

    auto iter = other_node_map_.find(node_name);
    if (iter == other_node_map_.end()) {
        return Status::NotExistNode(fmt::format("Attempt to remove non-exist node: {}", node_name));
    } else {
        switch (update_node_op) {
            case UpdateNodeOp::kRemove: {
                other_node_map_.erase(node_name);
                break;
            }
            case UpdateNodeOp::kLostConnection: {
                // Can't connect to the node
                iter->second->set_node_status(NodeStatus::kLostConnection);
                break;
            }
        }
    }

    auto client_iter = reader_client_map_.find(node_name);
    if (client_iter == reader_client_map_.end()) {
        return Status::NotExistNode(fmt::format("Attempt to disconnect from non-exist node: {}", node_name));
    } else {
        switch (update_node_op) {
            case UpdateNodeOp::kRemove: {
                client_iter->second->UnInit(true);
                clients_for_cleanup_.emplace_back(reader_client_map_[node_name]);
                reader_client_map_.erase(node_name);
                break;
            }
            case UpdateNodeOp::kLostConnection: {
                client_iter->second->UnInit(false);
                clients_for_cleanup_.emplace_back(reader_client_map_[node_name]);
                reader_client_map_.erase(node_name);
                break;
            }
        }
    }

    return Status::OK();
}

Status ClusterManager::UpdateNodeInfoByHeartBeat(const SharedPtr<NodeInfo> &non_leader_node,
                                                 Vector<infinity_peer_server::NodeInfo> &other_nodes,
                                                 i64 &leader_term,
                                                 infinity_peer_server::NodeStatus::type &sender_status) {
    // Used by leader
    std::unique_lock<std::mutex> lock(mutex_);
    if (current_node_role_ != NodeRole::kLeader) {
        sender_status = infinity_peer_server::NodeStatus::type::kRemoved;
        return Status::InvalidNodeRole(fmt::format("Leader node is already switch to {}", ToString(current_node_role_)));
    }

    other_nodes.reserve(other_node_map_.size());
    leader_term = this_node_->leader_term();
    bool non_leader_node_found{false};
    for (const auto &node_info_pair : other_node_map_) {
        NodeInfo *other_node = node_info_pair.second.get();
        String other_node_name = other_node->node_name();
        String other_node_ip = other_node->node_ip();
        i64 other_node_port = other_node->node_port();
        if (other_node_name == non_leader_node->node_name()) {
            if (other_node_ip != non_leader_node->node_ip() or other_node_port != non_leader_node->node_port()) {
                String error_message =
                    fmt::format("Node {}, IP address: {} or port: {} is changed.", other_node_name, other_node_ip, other_node->node_port());
                sender_status = infinity_peer_server::NodeStatus::type::kTimeout;
                return Status::NodeInfoUpdated(ToString(other_node->node_role()));
            }
            // Found the node
            switch (other_node->node_status()) {
                case NodeStatus::kAlive:
                case NodeStatus::kTimeout: {
                    // just update the timestamp
                    other_node->set_txn_ts(non_leader_node->txn_ts());
                    auto now = std::chrono::system_clock::now();
                    auto time_since_epoch = now.time_since_epoch();
                    other_node->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());
                    other_node->heartbeat_count_increase();
                    sender_status = infinity_peer_server::NodeStatus::type::kAlive;
                    break;
                }
                case NodeStatus::kRemoved: {
                    sender_status = infinity_peer_server::NodeStatus::type::kRemoved;
                    break;
                }
                case NodeStatus::kLostConnection: {
                    LOG_ERROR(fmt::format("Node {} from {}:{} can't connected, but still can receive heartbeat.",
                                          other_node_name,
                                          other_node_ip,
                                          other_node_port));
                    sender_status = infinity_peer_server::NodeStatus::type::kLostConnection;
                    break;
                }
                case NodeStatus::kInvalid: {
                    UnrecoverableError("Invalid node status");
                }
            }

            non_leader_node_found = true;
        } else {
            infinity_peer_server::NodeInfo thrift_node_info;
            thrift_node_info.node_name = other_node_name;
            thrift_node_info.node_ip = other_node_ip;
            thrift_node_info.node_port = other_node_port;
            thrift_node_info.txn_timestamp = other_node->txn_ts();
            thrift_node_info.hb_count = other_node->heartbeat_count();
            NodeRole other_node_role = other_node->node_role();
            switch (other_node_role) {
                case NodeRole::kLeader: {
                    thrift_node_info.node_type = infinity_peer_server::NodeType::kLeader;
                    break;
                }
                case NodeRole::kFollower: {
                    thrift_node_info.node_type = infinity_peer_server::NodeType::kFollower;
                    break;
                }
                case NodeRole::kLearner: {
                    thrift_node_info.node_type = infinity_peer_server::NodeType::kLearner;
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid node role of node: {}", other_node_name);
                    return Status::InvalidNodeRole(ToString(other_node_role));
                }
            }
            NodeStatus other_node_status = other_node->node_status();
            switch (other_node_status) {
                case NodeStatus::kAlive: {
                    thrift_node_info.node_status = infinity_peer_server::NodeStatus::kAlive;
                    break;
                }
                case NodeStatus::kTimeout: {
                    thrift_node_info.node_status = infinity_peer_server::NodeStatus::kTimeout;
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid node status : {} of node: {}", ToString(other_node_status), other_node_name);
                    return Status::InvalidNodeStatus(error_message);
                }
            }

            other_nodes.emplace_back(thrift_node_info);
        }
    }

    if (!non_leader_node_found) {
        sender_status = infinity_peer_server::NodeStatus::type::kLostConnection;
        return Status::NotRegistered(non_leader_node->node_name());
    }

    return Status::OK();
}

Status ClusterManager::SyncLogsOnRegistration(const SharedPtr<NodeInfo> &non_leader_node, const SharedPtr<PeerClient> &peer_client) {
    // Used by leader on registration phase

    // Check leader WAL and get the diff log
    LOG_TRACE("Leader will get the log diff");
    Storage *storage_ptr = InfinityContext::instance().storage();
    WalManager *wal_manager = storage_ptr->wal_manager();
    Vector<SharedPtr<String>> wal_strings = wal_manager->GetDiffWalEntryString(non_leader_node->txn_ts());

    // Leader will send the WALs
    String non_leader_node_name = non_leader_node->node_name();
    LOG_TRACE(fmt::format("Leader will send the diff logs count: {} to {} synchronously", wal_strings.size(), non_leader_node_name));
    return SendLogs(non_leader_node_name, peer_client, wal_strings, true, true);
}

void ClusterManager::PrepareLogs(const SharedPtr<String> &log_string) { logs_to_sync_.emplace_back(log_string); }

Status ClusterManager::SyncLogs() {
    LOG_TRACE("Sync logs to follower and async logs to learner");
    Set<String> sent_nodes;
    while (true) {
        // Get follower and learner node
        Vector<SharedPtr<NodeInfo>> followers;
        Vector<SharedPtr<PeerClient>> follower_clients;
        Vector<SharedPtr<NodeInfo>> learners;
        Vector<SharedPtr<PeerClient>> learner_clients;

        Status status = GetReadersInfo(followers, follower_clients, learners, learner_clients);
        if (!status.ok()) {
            return status;
        }

        SizeT follower_count = followers.size();
        SizeT learner_count = learners.size();

        if (follower_count != follower_clients.size() && learner_count != learner_clients.size()) {
            return Status::UnexpectedError("Node info and node client count isn't match");
        }

        // Replicate logs to follower
        for (SizeT idx = 0; idx < follower_count; ++idx) {
            const String &follower_name = followers[idx]->node_name();
            if (!sent_nodes.contains(follower_name)) {
                status = SendLogs(follower_name, follower_clients[idx], logs_to_sync_, true, false);
                if (status.ok()) {
                    sent_nodes.insert(follower_name);
                }
            }
        }

        // Replicate logs to learner
        for (SizeT idx = 0; idx < learner_count; ++idx) {
            const String &learner_name = learners[idx]->node_name();
            if (!sent_nodes.contains(learner_name)) {
                status = SendLogs(learner_name, learner_clients[idx], logs_to_sync_, false, false);
                if (status.ok()) {
                    sent_nodes.insert(learner_name);
                }
            }
        }

        if (sent_nodes.size() == follower_count + learner_count) {
            logs_to_sync_.clear();
            break;
        }
    }
    return Status::OK();
}

Status ClusterManager::SetFollowerNumber(SizeT new_follower_number) {
    if (new_follower_number > 5) {
        return Status::NotSupport("Attempt to set follower count larger than 5.");
    }

    // Check current follower count, if new count is less, leader will downgrade some followers to learner
    follower_count_ = new_follower_number;
    return Status::OK();
}

SizeT ClusterManager::GetFollowerNumber() const { return follower_count_; }

Status ClusterManager::UpdateNodeInfoNoLock(const Vector<SharedPtr<NodeInfo>> &info_of_nodes) {
    // Only follower and learner will use this function.
    HashMap<String, bool> exist_node_name_map;
    exist_node_name_map.reserve(other_node_map_.size());
    for (const auto &node_pair : other_node_map_) {
        exist_node_name_map.emplace(node_pair.first, false);
    }

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    for (const SharedPtr<NodeInfo> &node_info : info_of_nodes) {
        auto iter = other_node_map_.find(node_info->node_name());
        if (iter == other_node_map_.end()) {
            node_info->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());
            other_node_map_.emplace(node_info->node_name(), node_info);
        } else {
            iter->second->set_txn_ts(node_info->txn_ts());
            iter->second->set_node_ip(node_info->node_ip());
            iter->second->set_node_port(node_info->node_port());
            iter->second->set_node_role(node_info->node_role());
            iter->second->set_node_status(node_info->node_status());
            iter->second->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

            exist_node_name_map[node_info->node_name()] = true;
        }
    }

    for (const auto &exist_node_pair : exist_node_name_map) {
        if (!exist_node_pair.second) {
            other_node_map_.erase(exist_node_pair.first);
        }
    }

    return Status::OK();
}

Status ClusterManager::ApplySyncedLogNolock(const Vector<String> &synced_logs) {
    Storage *storage_ptr = InfinityContext::instance().storage();
    WalManager *wal_manager = storage_ptr->wal_manager();
    TransactionID last_txn_id = 0;
    TxnTimeStamp last_commit_ts = 0;
    for (auto &log_str : synced_logs) {
        const i32 entry_size = log_str.size();
        const char *ptr = log_str.data();
        SharedPtr<WalEntry> entry = WalEntry::ReadAdv(ptr, entry_size);
        LOG_DEBUG(fmt::format("WAL Entry: {}", entry->ToString()));
        last_txn_id = entry->txn_id_;
        last_commit_ts = entry->commit_ts_;
        wal_manager->ReplayWalEntry(*entry, false, false);
    }

    LOG_INFO(fmt::format("Replicated from leader: latest txn commit_ts: {}, latest txn id: {}", last_commit_ts, last_txn_id));
    storage_ptr->catalog()->next_txn_id_ = last_txn_id;
    storage_ptr->wal_manager()->UpdateCommitState(last_commit_ts, 0);
    storage_ptr->txn_manager()->SetStartTS(last_commit_ts);
    return Status::OK();
}

Status ClusterManager::ContinueStartup(const Vector<String> &synced_logs) {
    Storage *storage_ptr = InfinityContext::instance().storage();
    WalManager *wal_manager = storage_ptr->wal_manager();
    bool is_checkpoint = true;
    TxnTimeStamp last_commit_ts;
    for (auto &log_str : synced_logs) {
        const i32 entry_size = log_str.size();
        const char *ptr = log_str.data();
        SharedPtr<WalEntry> entry = WalEntry::ReadAdv(ptr, entry_size);
        for (const auto &cmd : entry->cmds_) {
            if (is_checkpoint) {
                if (cmd->GetType() != WalCommandType::CHECKPOINT) {
                    is_checkpoint = false;
                }
            } else {
                if (cmd->GetType() == WalCommandType::CHECKPOINT) {
                    UnrecoverableError("Expect non-checkpoint log");
                }
            }
        }
        LOG_DEBUG(fmt::format("WAL Entry: {}", entry->ToString()));
        wal_manager->ReplayWalEntry(*entry, true, true);
        last_commit_ts = entry->commit_ts_;
    }

    storage_ptr->SetReaderStorageContinue(last_commit_ts + 1);
    return Status::OK();
}

Vector<SharedPtr<NodeInfo>> ClusterManager::ListNodes() const {
    // ADMIN SHOW NODES;
    std::unique_lock<std::mutex> lock(mutex_);
    Vector<SharedPtr<NodeInfo>> result;
    result.reserve(other_node_map_.size() + 2);
    result.emplace_back(this_node_);
    if (current_node_role_ == NodeRole::kFollower or current_node_role_ == NodeRole::kLearner) {
        result.emplace_back(leader_node_);
    }
    for (const auto &node_info_pair : other_node_map_) {
        result.emplace_back(node_info_pair.second);
    }

    return result;
}

Tuple<Status, SharedPtr<NodeInfo>> ClusterManager::GetNodeInfoByName(const String &node_name) const {
    std::unique_lock<std::mutex> lock(mutex_);

    if (current_node_role_ == NodeRole::kAdmin or current_node_role_ == NodeRole::kStandalone or current_node_role_ == NodeRole::kUnInitialized) {
        String error_message = "Error node role type";
        UnrecoverableError(error_message);
    }

    if (current_node_role_ == NodeRole::kFollower or current_node_role_ == NodeRole::kLearner) {
        if (node_name == leader_node_->node_name()) {
            // Show leader
            return {Status::OK(), leader_node_};
        }
    }

    if (node_name == this_node_->node_name()) {
        // Show this node
        return {Status::OK(), this_node_};
    }

    auto iter = other_node_map_.find(node_name);
    if (iter == other_node_map_.end()) {
        return {Status::NotExistNode(node_name), nullptr};
    }
    return {Status::OK(), iter->second};
}

SharedPtr<NodeInfo> ClusterManager::ThisNode() const {
    // ADMIN SHOW NODE;
    std::unique_lock<std::mutex> lock(mutex_);
    return this_node_;
}

} // namespace infinity
