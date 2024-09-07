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

export module cluster_manager;

import stl;
import third_party;
import status;

namespace infinity {

export enum class NodeRole {
    kUnInitialized,
    kAdmin,
    kStandalone,
    kLeader,
    kFollower,
    kLearner,
};


export String ToString(NodeRole);

export enum class NodeStatus {
    kReady,
    kConnected,
    kTimeout,
    kInvalid
};

export String ToString(NodeStatus);

export struct ServerNode {
    NodeRole node_role_{NodeRole::kUnInitialized};
    NodeStatus node_status_{NodeStatus::kInvalid};
    String node_name_{};
    String ip_address_{};
    i16 port_{};
    u64 last_update_ts_{};
    // u64 update_interval_{}; // seconds
    // String from_{}; // Which node the information comes from.
};

export class ClusterManager {
public:
    ClusterManager() = default;
    ~ClusterManager() = default;

    Status InitAsLeader(const String& node_name);
    Status InitAsFollower(const String& node_name, const String& leader_ip, i16 leader_port);
    Status InitAsLearner(const String& node_name, const String& leader_ip, i16 leader_port);
    Status UnInit();


    Status Register(SharedPtr<ServerNode> server_node);
    Status Unregister(const String& node_name); // Used by manual or disconnect from follower/learner

    Status UpdateNodeInfo(const SharedPtr<ServerNode>& server_node); // Used by leader;
    Status UpdateNonLeaderNodeInfo(const Vector<SharedPtr<ServerNode>>& info_of_nodes); // Use by follower / learner to update all node info.
    Vector<SharedPtr<ServerNode>> ListNodes() const;
    SharedPtr<ServerNode> GetServerNodePtrByName(const String& node_name) const;
    SharedPtr<ServerNode> ThisNode() const;
private:
    mutable std::mutex mutex_;
    Vector<SharedPtr<ServerNode>> other_nodes_;
    SharedPtr<ServerNode> this_node_;
};

}
