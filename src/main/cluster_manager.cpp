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

namespace infinity {

ClusterManager::~ClusterManager() {
    other_nodes_.clear();
    this_node_.reset();
    if(peer_client_.get() != nullptr) {
        peer_client_->UnInit();
    }
    peer_client_.reset();
}

Status ClusterManager::InitAsLeader(const String& node_name) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(this_node_.get()!= nullptr) {
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

Status ClusterManager::InitAsFollower(const String& node_name, const String& leader_ip, i64 leader_port) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as follower error: already initialized.");
    }
    this_node_ = MakeShared<NodeInfo>();
    this_node_->node_role_ = NodeRole::kFollower;
    this_node_->node_status_ = NodeStatus::kReady;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    this_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    peer_client_ = PeerClient::Connect(leader_ip, leader_port);
    return Status::OK();
}
Status ClusterManager::InitAsLearner(const String& node_name, const String& leader_ip, i64 leader_port) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as learner error: already initialized.");
    }
    this_node_ = MakeShared<NodeInfo>();
    this_node_->node_role_ = NodeRole::kLearner;
    this_node_->node_status_ = NodeStatus::kReady;
    this_node_->node_name_ = node_name;
    this_node_->ip_address_ = InfinityContext::instance().config()->PeerServerIP();
    this_node_->port_ = InfinityContext::instance().config()->PeerServerPort();

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    this_node_->last_update_ts_ = std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count();

    peer_client_ = PeerClient::Connect(leader_ip, leader_port);
    return Status::OK();
}

Status ClusterManager::UnInit() {
    std::unique_lock<std::mutex> lock(mutex_);

    return Status::OK();
}

Status ClusterManager::Register(SharedPtr<NodeInfo> server_node) {
    std::unique_lock<std::mutex> lock(mutex_);
    return Status::OK();
}
Status ClusterManager::Unregister(const String &node_name) {
    std::unique_lock<std::mutex> lock(mutex_);
    return Status::OK();
}

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