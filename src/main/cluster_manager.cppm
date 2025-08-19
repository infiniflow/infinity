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

export module infinity_core:cluster_manager;

import :status;
import :node_info;
import :peer_server_thrift_service;
import :peer_server_thrift_types;

import std;

import admin_statement;

namespace infinity {
class PeerClient;

export enum class UpdateNodeOp { kRemove, kLostConnection };

export class ClusterManager {
    // Common
public:
    ClusterManager();
    ~ClusterManager();

    Status UnInitFromAdminNoLock();
    Status UnInitFromStandaloneNoLock();
    Status UnInitAsLeader();
    Status UnInitAsFollower();
    Status UnInitAsLearner();
    Status UnInit(bool not_unregister);
    // Used by all nodes ADMIN SHOW NODES
    std::vector<std::shared_ptr<NodeInfo>> ListNodes() const;
    // Used by all nodes ADMIN SHOW NODE node_name;
    std::tuple<Status, std::shared_ptr<NodeInfo>> GetNodeInfoByName(const std::string &node_name) const;
    // Used by all nodes / all mode ADMIN SHOW NODE;
    std::shared_ptr<NodeInfo> ThisNode() const;
    // Used by all nodes
    NodeRole GetNodeRole() const { return current_node_role_; }

private:
    std::tuple<std::shared_ptr<PeerClient>, Status>
    ConnectToServerNoLock(const std::string &from_node_name, const std::string &server_ip, i64 server_port);
    std::shared_ptr<NodeInfo> this_node_; // Used by leader and follower/learner
    std::map<std::string, std::shared_ptr<NodeInfo>>
        other_node_map_; // Used by leader and follower/learner
                         // std::vector<std::shared_ptr<NodeInfo>> other_nodes_; // Used by leader and follower/learner
    std::atomic<NodeRole> current_node_role_{NodeRole::kUnInitialized};
    mutable std::mutex cluster_mutex_;

    // Admin
public:
    void InitAsAdmin();

    // Standalone
public:
    void InitAsStandalone();

    // Leader
public:
    Status InitAsLeader(const std::string &node_name);
    void CheckHeartBeat();
    // Used by leader to add non-leader node in register phase
    Status AddNodeInfo(const std::shared_ptr<NodeInfo> &new_node);
    Status RemoveNodeInfo(const std::string &node_name);
    // Used by leader to remove unregister node
    Status UpdateNodeByLeader(const std::string &node_name, UpdateNodeOp update_node_op);
    // Used by leader when get HB request
    Status UpdateNodeInfoByHeartBeat(const std::shared_ptr<NodeInfo> &non_leader_node,
                                     std::vector<infinity_peer_server::NodeInfo> &other_nodes,
                                     i64 &leader_term,
                                     infinity_peer_server::NodeStatus::type &sender_status);

    // Used by leader to notify leader to synchronize logs to the follower and learner, during registration
    Status SyncLogsOnRegistration(const std::shared_ptr<NodeInfo> &non_leader_node, const std::shared_ptr<PeerClient> &peer_client);
    void PrepareLogs(const std::shared_ptr<std::string> &log_string);
    Status SyncLogs();
    // Used by leader to control the number of follower
    Status SetFollowerNumber(size_t new_follower_number);
    size_t GetFollowerLimit() const;

private:
    void CheckHeartBeatThread();
    Status SendLogs(const std::string &node_name,
                    const std::shared_ptr<PeerClient> &peer_client,
                    const std::vector<std::shared_ptr<std::string>> &logs,
                    bool synchronize,
                    bool on_register);
    Status GetReadersInfo(std::vector<std::shared_ptr<NodeInfo>> &followers,
                          std::vector<std::shared_ptr<PeerClient>> &follower_clients,
                          std::vector<std::shared_ptr<NodeInfo>> &learners,
                          std::vector<std::shared_ptr<PeerClient>> &learner_clients);
    // Leader clients to followers and learners
    std::map<std::string, std::shared_ptr<PeerClient>> reader_client_map_{}; // Used by leader;
    std::vector<std::shared_ptr<std::string>> logs_to_sync_{};
    std::atomic<u8> follower_limit_{4};
    std::vector<std::shared_ptr<PeerClient>> clients_for_cleanup_;

    // Follower and Learner
public:
    Status InitAsFollower(const std::string &node_name, const std::string &leader_ip, i64 leader_port);
    Status InitAsLearner(const std::string &node_name, const std::string &leader_ip, i64 leader_port);
    Status RegisterToLeader();
    // Use by follower / learner to update all node info when get HB response from leader
    Status UpdateNodeInfoNoLock(const std::vector<std::shared_ptr<NodeInfo>> &info_of_nodes);
    Status ContinueStartup(const std::vector<std::string> &synced_logs);
    Status ApplySyncedLogNolock(const std::vector<std::string> &synced_logs);

private:
    void HeartBeatToLeaderThread();
    Status RegisterToLeaderNoLock();
    Status UnregisterToLeaderNoLock();

    std::shared_ptr<NodeInfo> leader_node_;          // Used by follower / learner
    std::shared_ptr<PeerClient> client_to_leader_{}; // Used by follower and learner to connect leader server;

    std::shared_ptr<std::thread> hb_periodic_thread_{};
    std::mutex hb_mutex_;
    std::condition_variable hb_cv_;
    std::atomic<bool> hb_running_{false};
};

} // namespace infinity
