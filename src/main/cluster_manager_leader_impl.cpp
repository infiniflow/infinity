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

module infinity_core:cluster_manager_leader.impl;

import :cluster_manager;
import :config;
import :infinity_context;
import :peer_thrift_client;

import std;
import third_party;

import admin_statement;

namespace infinity {

Status ClusterManager::InitAsLeader(const std::string &node_name) {

    Config *config_ptr = InfinityContext::instance().config();
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as leader error: already initialized.");
    }

    this_node_ = std::make_shared<NodeInfo>(NodeRole::kLeader,
                                            NodeStatus::kAlive,
                                            node_name,
                                            config_ptr->PeerServerIP(),
                                            config_ptr->PeerServerPort(),
                                            std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

    current_node_role_ = NodeRole::kLeader;
    return Status::OK();
}

void ClusterManager::CheckHeartBeat() {
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    if (current_node_role_ != NodeRole::kLeader) {
        UnrecoverableError("Invalid node role.");
    }
    hb_running_ = true;
    this_node_->set_heartbeat_interval(1000);
    hb_periodic_thread_ = std::make_shared<std::thread>([this] { this->CheckHeartBeatThread(); });
}

void ClusterManager::CheckHeartBeatThread() {
    if (current_node_role_ != NodeRole::kLeader) {
        UnrecoverableError("Invalid node role.");
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

        std::unique_lock cluster_lock(cluster_mutex_);
        for (auto &[node_name, other_node] : other_node_map_) {
            NodeStatus other_node_status = other_node->node_status();
            if (other_node_status == NodeStatus::kAlive) {
                u64 other_node_update_ts = other_node->update_ts();
                u64 this_node_update_ts = this_node_->update_ts();
                //                u64 this_node_hb_interval = this_node_->heartbeat_interval();

                if (other_node_update_ts + 4 < this_node_update_ts) {
                    other_node->set_node_status(NodeStatus::kTimeout);
                    LOG_INFO(fmt::format("Node {} is timeout", node_name));
                }
            }
        }
    }
}

Status ClusterManager::AddNodeInfo(const std::shared_ptr<NodeInfo> &other_node) {
    // Only used by Leader on follower/learner registration.

    std::string other_node_name = other_node->node_name();
    {
        std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
        if (current_node_role_ != NodeRole::kLeader) {
            UnrecoverableError("Non-leader role can't add other node.");
        }

        if (other_node_name == this_node_->node_name()) {
            return Status::DuplicateNode(other_node_name);
        }

        if (other_node->node_ip() == this_node_->node_ip() and other_node->node_port() == this_node_->node_port()) {
            return Status::InvalidServerAddress(
                fmt::format("Follower or learner peer server address {}: {} are same as leader", this_node_->node_ip(), this_node_->node_port()));
        }

        // Add by register
        auto iter = other_node_map_.find(other_node_name);
        if (iter != other_node_map_.end()) {
            // Duplicated node
            // TODO: Update node info and not throw error.
            return Status::DuplicateNode(other_node_name);
        }

        u32 follower_count = 0;
        u32 learner_count = 0;
        for (auto &other_node_pair : other_node_map_) {
            switch (other_node_pair.second->node_role()) {
                case NodeRole::kFollower: {
                    follower_count += 1;
                    break;
                }
                case NodeRole::kLearner: {
                    learner_count += 1;
                    break;
                }
                default: {
                    UnrecoverableError("Non-follower / learner role should be here.");
                }
            }
        }

        // Add learner and follower limit
        switch (other_node->node_role()) {
            case NodeRole::kFollower: {
                if (follower_count + 1 == follower_limit_) {
                    return Status::TooManyFollower(follower_limit_);
                }
                break;
            }
            case NodeRole::kLearner: {
                if (learner_count + 1 == std::numeric_limits<u8>::max()) {
                    return Status::TooManyLearner();
                }
                break;
            }
            default: {
                return Status::InvalidNodeRole(fmt::format("Invalid node role: {}", ToString(other_node->node_role())));
            }
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
        std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
        if (current_node_role_ != NodeRole::kLeader) {
            UnrecoverableError("Non-leader role can't add other node.");
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

Status ClusterManager::RemoveNodeInfo(const std::string &node_name) {
    // Used by leader to remove node
    if (node_name == this_node_->node_name()) {
        return Status::InvalidNodeRole(fmt::format("Can't remove current node: {}", node_name));
    }

    NodeRole server_role = InfinityContext::instance().GetServerRole();
    if (server_role != NodeRole::kLeader) {
        return Status::InvalidNodeRole(fmt::format("Can't remove node in {} mode", ToString(server_role)));
    }

    std::shared_ptr<PeerClient> client_{nullptr};
    NodeStatus old_status = NodeStatus::kInvalid;
    {
        std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
        auto node_iter = other_node_map_.find(node_name);
        if (node_iter != other_node_map_.end()) {
            NodeInfo &node_info = *node_iter->second;
            old_status = node_info.node_status();
            node_info.set_node_status(NodeStatus::kRemoved);
        } else {
            return Status::NotExistNode(node_name);
        }

        auto client_iter = reader_client_map_.find(node_name);
        if (client_iter != reader_client_map_.end()) {
            client_ = client_iter->second;
        } else if (old_status == NodeStatus::kAlive) {
            UnrecoverableError(fmt::format("Cant' find node {} in client collection.", node_name));
        }
    }

    if (old_status == NodeStatus::kAlive) {
        auto change_role_task = std::make_shared<ChangeRoleTask>(node_name, "admin");
        client_->Send(change_role_task);
        change_role_task->Wait();

        if (change_role_task->error_code_ != 0) {
            Status status = Status::OK();
            LOG_ERROR(fmt::format("Fail to change {} to the role of ADMIN, error: ", node_name, change_role_task->error_message_));
            status.code_ = static_cast<ErrorCode>(change_role_task->error_code_);
            status.msg_ = std::make_unique<std::string>(change_role_task->error_message_);
            return status;
        }
    }
    {
        std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
        other_node_map_.erase(node_name);
        clients_for_cleanup_.emplace_back(reader_client_map_[node_name]);
        reader_client_map_.erase(node_name);
    }
    return Status::OK();
}

Status ClusterManager::UpdateNodeByLeader(const std::string &node_name, UpdateNodeOp update_node_op) {
    // Only used in leader mode.
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);

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

Status ClusterManager::UpdateNodeInfoByHeartBeat(const std::shared_ptr<NodeInfo> &non_leader_node,
                                                 std::vector<infinity_peer_server::NodeInfo> &other_nodes,
                                                 i64 &leader_term,
                                                 infinity_peer_server::NodeStatus::type &sender_status) {
    // Used by leader
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    if (current_node_role_ != NodeRole::kLeader) {
        sender_status = infinity_peer_server::NodeStatus::type::kRemoved;
        return Status::InvalidNodeRole(fmt::format("Leader node is already switch to {}", ToString(current_node_role_)));
    }

    other_nodes.reserve(other_node_map_.size());
    leader_term = this_node_->leader_term();
    bool non_leader_node_found{false};
    for (const auto &node_info_pair : other_node_map_) {
        NodeInfo *other_node = node_info_pair.second.get();
        std::string other_node_name = other_node->node_name();
        std::string other_node_ip = other_node->node_ip();
        i64 other_node_port = other_node->node_port();
        if (other_node_name == non_leader_node->node_name()) {
            if (other_node_ip != non_leader_node->node_ip() or other_node_port != non_leader_node->node_port()) {
                std::string error_msg =
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
                    std::string error_msg = fmt::format("Invalid node role of node: {}", other_node_name);
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
                    std::string error_msg = fmt::format("Invalid node status : {} of node: {}", ToString(other_node_status), other_node_name);
                    return Status::InvalidNodeStatus(error_msg);
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

Status ClusterManager::SyncLogsOnRegistration(const std::shared_ptr<NodeInfo> &non_leader_node, const std::shared_ptr<PeerClient> &peer_client) {
    // Used by leader on registration phase

    // Check leader WAL and get the diff log
    LOG_TRACE("Leader will get the log diff");
    Storage *storage_ptr = InfinityContext::instance().storage();
    WalManager *wal_manager = storage_ptr->wal_manager();
    std::vector<std::shared_ptr<std::string>> wal_strings = wal_manager->GetDiffWalEntryString(non_leader_node->txn_ts());

    // Leader will send the WALs
    std::string non_leader_node_name = non_leader_node->node_name();
    LOG_TRACE(fmt::format("Leader will send the diff logs count: {} to {} synchronously", wal_strings.size(), non_leader_node_name));
    return SendLogs(non_leader_node_name, peer_client, wal_strings, true, true);
}

void ClusterManager::PrepareLogs(const std::shared_ptr<std::string> &log_string) { logs_to_sync_.emplace_back(log_string); }

Status ClusterManager::SyncLogs() {
    LOG_TRACE("Sync logs to follower and async logs to learner");
    std::set<std::string> sent_nodes;
    while (true) {
        // Get follower and learner node
        std::vector<std::shared_ptr<NodeInfo>> followers;
        std::vector<std::shared_ptr<PeerClient>> follower_clients;
        std::vector<std::shared_ptr<NodeInfo>> learners;
        std::vector<std::shared_ptr<PeerClient>> learner_clients;

        Status status = GetReadersInfo(followers, follower_clients, learners, learner_clients);
        if (!status.ok()) {
            return status;
        }

        size_t follower_count = followers.size();
        size_t learner_count = learners.size();

        if (follower_count != follower_clients.size() && learner_count != learner_clients.size()) {
            return Status::UnexpectedError("Node info and node client count isn't match");
        }

        // Replicate logs to follower
        for (size_t idx = 0; idx < follower_count; ++idx) {
            const std::string &follower_name = followers[idx]->node_name();
            if (!sent_nodes.contains(follower_name)) {
                status = SendLogs(follower_name, follower_clients[idx], logs_to_sync_, true, false);
                if (status.ok()) {
                    sent_nodes.insert(follower_name);
                }
            }
        }

        // Replicate logs to learner
        for (size_t idx = 0; idx < learner_count; ++idx) {
            const std::string &learner_name = learners[idx]->node_name();
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

Status ClusterManager::SetFollowerNumber(size_t new_follower_number) {
    if (new_follower_number > 5) {
        return Status::NotSupport("Attempt to set follower count larger than 5.");
    }

    // Check current follower count, if new count is less, leader will downgrade some followers to learner
    follower_limit_ = new_follower_number;
    return Status::OK();
}

size_t ClusterManager::GetFollowerLimit() const { return follower_limit_; }

Status ClusterManager::SendLogs(const std::string &node_name,
                                const std::shared_ptr<PeerClient> &peer_client,
                                const std::vector<std::shared_ptr<std::string>> &logs,
                                bool synchronize,
                                bool on_register) {
    auto sync_log_task = std::make_shared<SyncLogTask>(node_name, logs, on_register);
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
        status.msg_ = std::make_unique<std::string>(sync_log_task->error_message_);
    }
    return status;
}

Status ClusterManager::GetReadersInfo(std::vector<std::shared_ptr<NodeInfo>> &followers,
                                      std::vector<std::shared_ptr<PeerClient>> &follower_clients,
                                      std::vector<std::shared_ptr<NodeInfo>> &learners,
                                      std::vector<std::shared_ptr<PeerClient>> &learner_clients) {
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
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

} // namespace infinity