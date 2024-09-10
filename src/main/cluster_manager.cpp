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

module cluster_manager;

import stl;
import infinity_context;
import logger;

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
    this_node_->node_status_ = NodeStatus::kReady;
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
    this_node_->node_status_ = NodeStatus::kReady;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::high_resolution_clock::now();
    auto duration_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    this_node_->last_update_ts_ = duration_in_seconds.count();

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
                                                                                  txn_manager_->CurrentTS(),
                                                                                  this_node_->last_update_ts_);
    peer_client_->Send(register_peer_task);
    register_peer_task->Wait();

    Status status = Status::OK();
    if (register_peer_task->error_code_ != 0) {
        status.code_ = static_cast<ErrorCode>(register_peer_task->error_code_);
        status.msg_ = MakeUnique<String>(register_peer_task->error_message_);
        return status;
    }

    leader_node_->last_update_ts_ = register_peer_task->leader_update_time_;
    leader_node_->node_name_ = register_peer_task->leader_name_;
    //    peer_client_->SetPeerNode(NodeRole::kLeader, register_peer_task->leader_name_, register_peer_task->update_time_);
    // Start HB thread.
    if(register_peer_task->heartbeat_interval_ == 0) {
        leader_node_->heartbeat_interval_ = 1000; // 1000 ms by default
    } else {
        leader_node_->heartbeat_interval_ = register_peer_task->heartbeat_interval_;
    }

    hb_periodic_thread_ = MakeShared<Thread>([&] {
        auto hb_interval = std::chrono::milliseconds(leader_node_->heartbeat_interval_);
        this->hb_running_ = true;
        while (true) {
            std::unique_lock lock(this->hb_mutex_);
            this->hb_cv_.wait_for(lock, hb_interval, [&] { return !this->hb_running_; });

            auto hb_now = std::chrono::high_resolution_clock::now();
            auto hb_duration_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(hb_now.time_since_epoch());
            this_node_->last_update_ts_ = hb_duration_in_seconds.count();

            // Send heartbeat
            SharedPtr<HeartBeatPeerTask> hb_task =
                MakeShared<HeartBeatPeerTask>(this_node_->node_name_, txn_manager_->CurrentTS(), this_node_->last_update_ts_);
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

            // Update leader info
            leader_node_->node_status_ = NodeStatus::kConnected;
            leader_node_->last_update_ts_ = hb_task->leader_update_time_;
            leader_node_->leader_term_ = hb_task->leader_term_;

            if (!hb_running_) {
                break;
            }
        }
    });
    hb_periodic_thread_->detach();
    return status;
}
Status ClusterManager::InitAsLearner(const String &node_name, const String &leader_ip, i64 leader_port) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as learner error: already initialized.");
    }
    this_node_ = MakeShared<NodeInfo>();
    this_node_->node_role_ = NodeRole::kLearner;
    this_node_->node_status_ = NodeStatus::kReady;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::high_resolution_clock::now();
    auto duration_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    this_node_->last_update_ts_ = duration_in_seconds.count();

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
                                                                                  txn_manager_->CurrentTS(),
                                                                                  this_node_->last_update_ts_);
    peer_client_->Send(register_peer_task);
    register_peer_task->Wait();

    Status status = Status::OK();
    if (register_peer_task->error_code_ != 0) {
        status.code_ = static_cast<ErrorCode>(register_peer_task->error_code_);
        status.msg_ = MakeUnique<String>(register_peer_task->error_message_);
        return status;
    }

    leader_node_->last_update_ts_ = register_peer_task->leader_update_time_;
    leader_node_->node_name_ = register_peer_task->leader_name_;
    //    peer_client_->SetPeerNode(NodeRole::kLeader, register_peer_task->leader_name_, register_peer_task->update_time_);
    // Start HB thread.

    if(register_peer_task->heartbeat_interval_ == 0) {
        leader_node_->heartbeat_interval_ = 1000; // 1000 ms by default
    } else {
        leader_node_->heartbeat_interval_ = register_peer_task->heartbeat_interval_;
    }

    this->hb_running_ = true;
    hb_periodic_thread_ = MakeShared<Thread>([this] {
        auto hb_interval = std::chrono::milliseconds(leader_node_->heartbeat_interval_);

        while (true) {
            std::unique_lock lock(this->hb_mutex_);
            this->hb_cv_.wait_for(lock, hb_interval, [&] { return !this->hb_running_; });
            if (!hb_running_) {
                break;
            }

            auto hb_now = std::chrono::high_resolution_clock::now();
            auto hb_duration_in_seconds = std::chrono::duration_cast<std::chrono::seconds>(hb_now.time_since_epoch());
            this_node_->last_update_ts_ = hb_duration_in_seconds.count();

            // Send heartbeat
            SharedPtr<HeartBeatPeerTask> hb_task =
                MakeShared<HeartBeatPeerTask>(this_node_->node_name_, txn_manager_->CurrentTS(), this_node_->last_update_ts_);
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

            // Update leader info
            leader_node_->node_status_ = NodeStatus::kConnected;
            leader_node_->last_update_ts_ = hb_task->leader_update_time_;
            leader_node_->leader_term_ = hb_task->leader_term_;
        }
    });
    return status;
}

Status ClusterManager::UnInit() {
    if(hb_periodic_thread_.get() != nullptr) {
        {
            std::lock_guard lock(hb_mutex_);
            hb_running_ = false;
            hb_cv_.notify_all();
        }
        hb_periodic_thread_->join();
        hb_periodic_thread_.reset();
    }

    std::unique_lock<std::mutex> lock(mutex_);
    other_nodes_.clear();
    this_node_.reset();
    if (peer_client_.get() != nullptr) {
        peer_client_->UnInit();
    }
    peer_client_.reset();

    return Status::OK();
}

// Status ClusterManager::Register(SharedPtr<NodeInfo> server_node) {
//     std::unique_lock<std::mutex> lock(mutex_);
//     return Status::OK();
// }
// Status ClusterManager::Unregister(const String &node_name) {
//     std::unique_lock<std::mutex> lock(mutex_);
//     return Status::OK();
// }

Status ClusterManager::UpdateNodeInfo(const SharedPtr<NodeInfo> &server_node) {
    std::unique_lock<std::mutex> lock(mutex_);
    return Status::OK();
}
Status ClusterManager::UpdateNonLeaderNodeInfo(const Vector<SharedPtr<NodeInfo>> &info_of_nodes) {
    std::unique_lock<std::mutex> lock(mutex_);
    return Status::OK();
}
Vector<SharedPtr<NodeInfo>> ClusterManager::ListNodes() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return {};
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