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

Status ClusterManager::UnInit() {
    switch (current_node_role_) {
        case NodeRole::kUnInitialized: {
            LOG_WARN("Cluster manager is already in UnInitialized role");
            return Status::OK();
        }
        case NodeRole::kAdmin: {
            return UnInitFromAdmin();
        }
        case NodeRole::kStandalone: {
            return UnInitFromStandalone();
        }
        case NodeRole::kLeader: {
            return UnInitFromLeader();
        }
        case NodeRole::kFollower: {
            return UnInitFromFollower();
        }
        case NodeRole::kLearner: {
            return UnInitFromLearner();
        }
    }
    return Status::OK();
}

Status ClusterManager::UnInit(bool not_unregister) {
    if (hb_periodic_thread_.get() != nullptr) {
        {
            std::lock_guard hb_lock(hb_mutex_);
            hb_running_ = false;
            hb_cv_.notify_all();
        }
        hb_periodic_thread_->join();
        hb_periodic_thread_.reset();
    }

    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
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

Tuple<SharedPtr<PeerClient>, Status> ClusterManager::ConnectToServerNoLock(const String &from_node_name, const String &server_ip, i64 server_port) {
    SharedPtr<PeerClient> client = MakeShared<PeerClient>(from_node_name, server_ip, server_port);
    Status client_status = client->Init();
    if (!client_status.ok()) {
        return {nullptr, client_status};
    }
    return {client, client_status};
}

Vector<SharedPtr<NodeInfo>> ClusterManager::ListNodes() const {
    // ADMIN SHOW NODES;
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
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
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);

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
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    return this_node_;
}

} // namespace infinity
