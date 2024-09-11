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

namespace infinity {

ClusterManager::~ClusterManager() {
    other_nodes_.clear();
    this_node_.reset();
    if (peer_client_.get() != nullptr) {
        peer_client_->UnInit();
    }
    peer_client_.reset();
}

Status ClusterManager::InitAsLeader(const String &node_name) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as leader error: already initialized.");
    }
    this_node_ = MakeShared<NodeInfo>();
    this_node_->node_role_ = NodeRole::kLeader;
    this_node_->node_status_ = NodeStatus::kAlive;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    this_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    return Status::OK();
}

Status ClusterManager::InitAsFollower(const String &node_name, const String &leader_ip, i64 leader_port) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as follower error: already initialized.");
    }
    this_node_ = MakeShared<NodeInfo>();
    this_node_->node_role_ = NodeRole::kFollower;
    this_node_->node_status_ = NodeStatus::kAlive;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    this_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    leader_node_ = MakeShared<NodeInfo>();
    leader_node_->node_role_ = NodeRole::kLeader;
    leader_node_->node_status_ = NodeStatus::kInvalid;
    leader_node_->ip_address_ = leader_ip;
    leader_node_->port_ = leader_port;

    peer_client_ = MakeShared<PeerClient>(leader_ip, leader_port);
    Status client_status = peer_client_->Init();
    if (!client_status.ok()) {
        return client_status;
    }

    // Register to leader;
    SharedPtr<RegisterPeerTask> register_peer_task = MakeShared<RegisterPeerTask>(this_node_->node_name_,
                                                                                  this_node_->node_role_,
                                                                                  this_node_->ip_address_,
                                                                                  this_node_->port_,
                                                                                  txn_manager_->CurrentTS());
    peer_client_->Send(register_peer_task);
    register_peer_task->Wait();

    Status status = Status::OK();
    if (register_peer_task->error_code_ != 0) {
        status.code_ = static_cast<ErrorCode>(register_peer_task->error_code_);
        status.msg_ = MakeUnique<String>(register_peer_task->error_message_);
        return status;
    }

    now = std::chrono::system_clock::now();
    time_since_epoch = now.time_since_epoch();
    leader_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
    leader_node_->node_name_ = register_peer_task->leader_name_;
    leader_node_->node_status_ = NodeStatus::kAlive;
    //    peer_client_->SetPeerNode(NodeRole::kLeader, register_peer_task->leader_name_, register_peer_task->update_time_);
    // Start HB thread.
    if (register_peer_task->heartbeat_interval_ == 0) {
        leader_node_->heartbeat_interval_ = 1000; // 1000 ms by default
    } else {
        leader_node_->heartbeat_interval_ = register_peer_task->heartbeat_interval_;
    }

    this->hb_running_ = true;
    hb_periodic_thread_ = MakeShared<Thread>([this] { this->HeartBeatToLeader(); });
    return status;
}
Status ClusterManager::InitAsLearner(const String &node_name, const String &leader_ip, i64 leader_port) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as learner error: already initialized.");
    }
    this_node_ = MakeShared<NodeInfo>();
    this_node_->node_role_ = NodeRole::kLearner;
    this_node_->node_status_ = NodeStatus::kAlive;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    this_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    leader_node_ = MakeShared<NodeInfo>();
    leader_node_->node_role_ = NodeRole::kLeader;
    leader_node_->node_status_ = NodeStatus::kInvalid;
    leader_node_->ip_address_ = leader_ip;
    leader_node_->port_ = leader_port;

    peer_client_ = MakeShared<PeerClient>(leader_ip, leader_port);
    Status client_status = peer_client_->Init();
    if (!client_status.ok()) {
        return client_status;
    }

    // Register to leader;
    SharedPtr<RegisterPeerTask> register_peer_task = MakeShared<RegisterPeerTask>(this_node_->node_name_,
                                                                                  this_node_->node_role_,
                                                                                  this_node_->ip_address_,
                                                                                  this_node_->port_,
                                                                                  txn_manager_->CurrentTS());
    peer_client_->Send(register_peer_task);
    register_peer_task->Wait();

    Status status = Status::OK();
    if (register_peer_task->error_code_ != 0) {
        status.code_ = static_cast<ErrorCode>(register_peer_task->error_code_);
        status.msg_ = MakeUnique<String>(register_peer_task->error_message_);
        return status;
    }
    // Update this node info
    this_node_->node_status_ = NodeStatus::kAlive;

    now = std::chrono::system_clock::now();
    time_since_epoch = now.time_since_epoch();
    leader_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
    leader_node_->node_name_ = register_peer_task->leader_name_;
    leader_node_->node_status_ = NodeStatus::kAlive;

    //    peer_client_->SetPeerNode(NodeRole::kLeader, register_peer_task->leader_name_, register_peer_task->update_time_);
    // Start HB thread.

    if (register_peer_task->heartbeat_interval_ == 0) {
        leader_node_->heartbeat_interval_ = 1000; // 1000 ms by default
    } else {
        leader_node_->heartbeat_interval_ = register_peer_task->heartbeat_interval_;
    }

    this->hb_running_ = true;
    hb_periodic_thread_ = MakeShared<Thread>([this] { this->HeartBeatToLeader(); });
    return status;
}

Status ClusterManager::UnInit() {

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
    if(this_node_->node_role_ == NodeRole::kFollower or this_node_->node_role_ == NodeRole::kLearner) {
        if(leader_node_->node_status_ == NodeStatus::kAlive) {
            // Leader is alive, need to unregister
            SharedPtr<UnregisterPeerTask> unregister_task = MakeShared<UnregisterPeerTask>(this_node_->node_name_);
            peer_client_->Send(unregister_task);
            unregister_task->Wait();
            if(unregister_task->error_code_ != 0) {
                LOG_ERROR(fmt::format("Fail to unregister from leader: {}", unregister_task->error_message_));
            }
        }
    }

    other_nodes_.clear();
    this_node_.reset();
    if (peer_client_.get() != nullptr) {
        peer_client_->UnInit();
    }
    peer_client_.reset();

    return Status::OK();
}

void ClusterManager::HeartBeatToLeader() {
    auto hb_interval = std::chrono::milliseconds(leader_node_->heartbeat_interval_);

    while (true) {
        std::unique_lock hb_lock(this->hb_mutex_);
        this->hb_cv_.wait_for(hb_lock, hb_interval, [&] { return !this->hb_running_; });
        if (!hb_running_) {
            break;
        }

        auto hb_now = std::chrono::system_clock::now();
        auto hb_time_since_epoch = hb_now.time_since_epoch();
        this_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(hb_time_since_epoch).count();

        // Send heartbeat
        SharedPtr<HeartBeatPeerTask> hb_task = MakeShared<HeartBeatPeerTask>(this_node_->node_name_, txn_manager_->CurrentTS());
        peer_client_->Send(hb_task);
        hb_task->Wait();

        if (hb_task->error_code_ != 0) {
            LOG_ERROR(fmt::format("Can't connect to leader: {} ,{}:{}, error: {}",
                                  leader_node_->node_name_,
                                  leader_node_->ip_address_,
                                  leader_node_->port_,
                                  hb_task->error_message_));
            leader_node_->node_status_ = NodeStatus::kTimeout;
            continue;
        }

        hb_now = std::chrono::system_clock::now();
        hb_time_since_epoch = hb_now.time_since_epoch();
        std::unique_lock<std::mutex> cluster_manager_lock(mutex_);
        // Update leader info
        leader_node_->node_status_ = NodeStatus::kAlive;

        leader_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(hb_time_since_epoch).count();
        leader_node_->leader_term_ = hb_task->leader_term_;
        ++ this_node_->heartbeat_count_;

        // Update the non-leader node info from leader
        UpdateNodeInfoNoLock(hb_task->other_nodes_);
    }
    return ;
}

// Status ClusterManager::Register(SharedPtr<NodeInfo> server_node) {
//     std::unique_lock<std::mutex> lock(mutex_);
//     return Status::OK();
// }
// Status ClusterManager::Unregister(const String &node_name) {
//     std::unique_lock<std::mutex> lock(mutex_);
//     return Status::OK();
// }

Status ClusterManager::AddNodeInfo(const SharedPtr<NodeInfo> &node_info) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_->node_role_ == NodeRole::kLeader) {
        // Add by register
        bool found = false;
        for (const SharedPtr<NodeInfo> &other_node : other_nodes_) {
            if (other_node->node_name_ == node_info->node_name_) {
                // Found the node, duplicate node
                found = true;
                break;
            }
        }
        if (found) {
            return Status::DuplicateNode(node_info->node_name_);
        }
        other_nodes_.emplace_back(node_info);
    } else {
        String error_message = "Invalid node role.";
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status ClusterManager::RemoveNode(const String& node_name) {
    std::unique_lock<std::mutex> lock(mutex_);
    u64 found_count = 0;
    std::erase_if(other_nodes_, [&](const SharedPtr<NodeInfo>& node) {
        if(node_name == node->node_name_) {
            ++ found_count;
            return true;
        }
        return false;
    });

    if(found_count == 0) {
        return Status::NotExistNode(fmt::format("Attempt to remove non-exist node: {}", node_name));
    } else if(found_count == 1) {
        return Status::OK();
    } else {
        String error_message = fmt::format("Attemtp to remove multiple({}) node with name: {}", found_count, node_name);
        LOG_ERROR(error_message);
        return Status::DuplicateNode(error_message);
    }

    return Status::OK();
}

Status ClusterManager::UpdateNodeInfoByHeartBeat(const String& node_name, i64 txn_timestamp, Vector<infinity_peer_server::NodeInfo>& other_nodes, i64& leader_term) {
    // Used by leader
    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_->node_role_ == NodeRole::kLeader) {
        other_nodes.reserve(other_nodes_.size());
        leader_term = this_node_->leader_term_;
        for (const SharedPtr<NodeInfo> &other_node : other_nodes_) {
            if (other_node->node_name_ == node_name) {
                // Found the node, just update the timestamp
                other_node->txn_timestamp_ = txn_timestamp;
                auto now = std::chrono::system_clock::now();
                auto time_since_epoch = now.time_since_epoch();
                other_node->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();
                ++ other_node->heartbeat_count_;
            } else {
                infinity_peer_server::NodeInfo thrift_node_info;
                thrift_node_info.node_name = other_node->node_name_;
                thrift_node_info.node_ip = other_node->ip_address_;
                thrift_node_info.node_port = other_node->port_;
                thrift_node_info.txn_timestamp = other_node->txn_timestamp_;
                thrift_node_info.hb_count = other_node->heartbeat_count_;
                switch(other_node->node_role_) {
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
                        String error_message = fmt::format("Invalid node role of node: {}", other_node->node_name_);
                        return Status::InvalidNodeRole(ToString(other_node->node_role_));
                    }
                }
                switch(other_node->node_status_) {
                    case NodeStatus::kAlive: {
                        thrift_node_info.node_status = infinity_peer_server::NodeStatus::kAlive;
                        break;
                    }
                    case NodeStatus::kTimeout: {
                        thrift_node_info.node_status = infinity_peer_server::NodeStatus::kTimeout;
                        break;
                    }
                    default: {
                        String error_message = fmt::format("Invalid node status of node: {}", other_node->node_name_);
                        return Status::InvalidNodeStatus(error_message);
                    }
                }

                other_nodes.emplace_back(thrift_node_info);
            }
        }
    } else {
        String error_message = "Invalid node role.";
        UnrecoverableError(error_message);
    }
    return Status::OK();
}

Status ClusterManager::UpdateNodeInfoNoLock(const Vector<SharedPtr<NodeInfo>> &info_of_nodes) {
    for(const SharedPtr<NodeInfo>& node_info: info_of_nodes) {
        bool found = false;
        for(const SharedPtr<NodeInfo>& other_node_ptr: other_nodes_) {
            if(node_info->node_name_ == other_node_ptr->node_name_) {
                found = true;
                other_node_ptr->txn_timestamp_ = node_info->txn_timestamp_;
                other_node_ptr->ip_address_ = node_info->ip_address_;
                other_node_ptr->port_ = node_info->port_;
                other_node_ptr->node_role_ = node_info->node_role_;
                other_node_ptr->node_status_ = node_info->node_status_;
            }
        }
        if(!found) {
            other_nodes_.emplace_back(node_info);
        }
    }
    return Status::OK();
}
Vector<SharedPtr<NodeInfo>> ClusterManager::ListNodes() const {
    std::unique_lock<std::mutex> lock(mutex_);
    Vector<SharedPtr<NodeInfo>> result = other_nodes_;
    result.emplace_back(this_node_);
    if (this_node_->node_role_ == NodeRole::kFollower or this_node_->node_role_ == NodeRole::kLearner) {
        result.emplace_back(leader_node_);
    }

    return result;
}
SharedPtr<NodeInfo> ClusterManager::GetNodeInfoPtrByName(const String &node_name) const {
    std::unique_lock<std::mutex> lock(mutex_);
    return nullptr;
}
SharedPtr<NodeInfo> ClusterManager::ThisNode() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return this->this_node_;
}

} // namespace infinity