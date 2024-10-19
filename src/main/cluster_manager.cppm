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

export enum class UpdateNodeOp { kRemove, kLostConnection };

export class ClusterManager {
public:
    explicit ClusterManager(TxnManager *txn_manager) : txn_manager_(txn_manager) {}
    ~ClusterManager();

public:
    Status InitAsLeader(const String &node_name);
    Status InitAsFollower(const String &node_name, const String &leader_ip, i64 leader_port);
    Status InitAsLearner(const String &node_name, const String &leader_ip, i64 leader_port);
    Status UnInit();

public:
    Status RegisterToLeader();
    void HeartBeatToLeader();
    void CheckHeartBeat();

private:
    void CheckHeartBeatInner();
    Status RegisterToLeaderNoLock();
    Status UnregisterFromLeaderNoLock();
    Tuple<SharedPtr<PeerClient>, Status> ConnectToServerNoLock(const String &sending_node_name, const String &server_ip, i64 server_port);
    Status SendLogs(const String &node_name, const SharedPtr<PeerClient> &peer_client, const Vector<SharedPtr<String>> &logs, bool synchronize);

    Status GetReadersInfo(Vector<SharedPtr<NodeInfo>> &followers,
                          Vector<SharedPtr<PeerClient>> &follower_clients,
                          Vector<SharedPtr<NodeInfo>> &learners,
                          Vector<SharedPtr<PeerClient>> &learner_clients);

public:
    // Used by leader to add non-leader node in register phase
    Status AddNodeInfo(const SharedPtr<NodeInfo> &new_node);

    // Used by leader to remove unregister node
    Status UpdateNodeByLeader(const String &node_name, UpdateNodeOp update_node_op);

    // Used by leader when get HB request
    Status
    UpdateNodeInfoByHeartBeat(const SharedPtr<NodeInfo> &non_leader_node, Vector<infinity_peer_server::NodeInfo> &other_nodes, i64 &leader_term);

    // Used by leader to notify leader to synchronize logs to the follower and learner, during registration
    Status SyncLogsOnRegistration(const SharedPtr<NodeInfo> &non_leader_node, const SharedPtr<PeerClient> &peer_client);

    void PrepareLogs(const SharedPtr<String> &log_string);
    Status SyncLogs();

    // Used by leader to control the number of follower
    Status SetFollowerNumber(SizeT new_follower_number);
    SizeT GetFollowerNumber() const;

    // Use by follower / learner to update all node info when get HB response from leader
    Status UpdateNodeInfoNoLock(const Vector<SharedPtr<NodeInfo>> &info_of_nodes);

    Status ApplySyncedLogNolock(const Vector<String> &synced_logs);

    // Used by all nodes ADMIN SHOW NODES
    Vector<SharedPtr<NodeInfo>> ListNodes() const;

    // Used by all nodes ADMIN SHOW NODE node_name;
    Tuple<Status, SharedPtr<NodeInfo>> GetNodeInfoPtrByName(const String &node_name) const;

    // Used by all nodes / all mode ADMIN SHOW NODE;
    SharedPtr<NodeInfo> ThisNode() const;

private:
    TxnManager *txn_manager_{};
    mutable std::mutex mutex_;

    SharedPtr<NodeInfo> leader_node_; // Used by follower / learner
    SharedPtr<NodeInfo> this_node_;   // Used by leader and follower/learner

    Map<String, SharedPtr<NodeInfo>> other_node_map_; // Used by leader and follower/learner
                                                      // Vector<SharedPtr<NodeInfo>> other_nodes_; // Used by leader and follower/learner

    // Leader clients to followers and learners
    Map<String, SharedPtr<PeerClient>> reader_client_map_{}; // Used by leader;
    Vector<SharedPtr<String>> logs_to_sync_{};

    SharedPtr<PeerClient> client_to_leader_{}; // Used by follower and learner to connect leader server;

    SharedPtr<Thread> hb_periodic_thread_{};
    std::mutex hb_mutex_;
    std::condition_variable hb_cv_;
    Atomic<bool> hb_running_{false};

    Atomic<SizeT> follower_count_{1};
};

} // namespace infinity
