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

export module infinity_core:node_info;

import :infinity_type;

import std;

import admin_statement;

namespace infinity {

export enum class NodeStatus { kAlive, kTimeout, kLostConnection, kRemoved, kInvalid };

export std::string ToString(NodeStatus);

export class NodeInfo {
public:
    NodeInfo(NodeRole role,
             NodeStatus status,
             const std::string &name,
             const std::string &ip_addr,
             i64 port,
             i64 txn_ts,
             u64 update_ts,
             u64 heartbeat_count);
    NodeInfo(NodeRole role, NodeStatus status, const std::string &name, const std::string &ip_addr, i64 port, i64 txn_ts, u64 update_ts);
    NodeInfo(NodeRole role, NodeStatus status, const std::string &name, const std::string &ip_addr, i64 port, u64 update_ts);
    NodeInfo(NodeRole role, const std::string &ip_addr, i64 port);

    void set_heartbeat_interval(i64 heartbeat_interval) { heartbeat_interval_ = heartbeat_interval; }
    i64 heartbeat_interval() const { return heartbeat_interval_; }

    void set_update_ts(u64 new_update_ts) { update_ts_ = new_update_ts; }
    u64 update_ts() const { return update_ts_; }

    void set_node_status(NodeStatus new_status) { node_status_ = new_status; }
    NodeStatus node_status() const { return node_status_; }

    void set_node_role(NodeRole new_node_role) { node_role_ = new_node_role; }
    NodeRole node_role() const { return node_role_; }

    void set_node_name(const std::string &new_node_name);
    std::string node_name() const;

    void set_node_ip(const std::string &new_node_ip);
    std::string node_ip() const;

    void set_node_port(i64 new_port) { port_ = new_port; }
    i64 node_port() const { return port_; }

    void set_txn_ts(i64 new_txn_ts) { txn_ts_ = new_txn_ts; }
    i64 txn_ts() const { return txn_ts_; }

    void set_leader_term(i64 new_leader_term) { leader_term_ = new_leader_term; }
    i64 leader_term() const { return leader_term_; }

    void heartbeat_count_increase() { ++heartbeat_count_; }
    u64 heartbeat_count() const { return heartbeat_count_; }

    bool IsSameNode(const NodeInfo &other);

private:
    mutable std::mutex node_mutex_{};
    std::atomic<NodeRole> node_role_{NodeRole::kUnInitialized};
    std::atomic<NodeStatus> node_status_{NodeStatus::kInvalid};
    std::string node_name_{};
    std::string ip_address_{};
    std::atomic<i64> port_{};
    i64 txn_ts_{};
    std::atomic<u64> update_ts_{};
    std::atomic<i64> leader_term_{};
    std::atomic<i64> heartbeat_interval_{}; // provide by leader and used by follower and learner;
    std::atomic<u64> heartbeat_count_{};    // given by leader to calculate how many heartbeat received on this node.
    // u64 update_interval_{}; // seconds
    // std::string from_{}; // Which node the information comes from.
};

} // namespace infinity
