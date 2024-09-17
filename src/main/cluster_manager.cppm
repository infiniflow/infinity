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
import peer_thrift_client;
import peer_server_thrift_types;
import peer_task;
import txn_manager;

namespace infinity {

export class ClusterManager {
public:
    explicit
    ClusterManager(TxnManager* txn_manager): txn_manager_(txn_manager) {}
    ~ClusterManager();

public:
    Status InitAsLeader(const String& node_name);
    Status InitAsFollower(const String& node_name, const String& leader_ip, i64 leader_port);
    Status InitAsLearner(const String& node_name, const String& leader_ip, i64 leader_port);
    Status UnInit();


public:
    void HeartBeatToLeader();

private:
    Status RegisterToLeaderNoLock();
    Status UnregisterFromLeaderNoLock();
    Tuple<SharedPtr<PeerClient>, Status> ConnectToServerNoLock(const String &server_ip, i64 server_port);

public:
    Status AddNodeInfo(const SharedPtr<NodeInfo>& new_node);
    Status RemoveNode(const String& node_name);

    Status UpdateNodeInfoByHeartBeat(const String& node_name, i64 txn_timestamp, Vector<infinity_peer_server::NodeInfo>& other_nodes, i64& leader_term); // Used by leader;
    Status UpdateNodeInfoNoLock(const Vector<SharedPtr<NodeInfo>>& info_of_nodes); // Use by follower / learner to update all node info.
    Vector<SharedPtr<NodeInfo>> ListNodes() const;
    SharedPtr<NodeInfo> GetNodeInfoPtrByName(const String& node_name) const;
    SharedPtr<NodeInfo> ThisNode() const;
private:
    TxnManager* txn_manager_{};
    mutable std::mutex mutex_;

    SharedPtr<NodeInfo> leader_node_; // Used by follower / learner
    Vector<SharedPtr<NodeInfo>> other_nodes_; // Used by leader and follower/learner
    SharedPtr<NodeInfo> this_node_; // Used by leader and follower/learner

    SharedPtr<PeerClient> peer_client_{}; // Used by follower and learner;

    Map<String, SharedPtr<PeerClient>> follower_clients_{}; // Used by leader;

    SharedPtr<Thread> hb_periodic_thread_{};
    std::mutex hb_mutex_;
    std::condition_variable hb_cv_;
    Atomic<bool> hb_running_{false};
};

}
