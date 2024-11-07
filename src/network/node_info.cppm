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

export module node_info;

import stl;
import admin_statement;

namespace infinity {

export enum class NodeStatus { kAlive, kTimeout, kLostConnection, kRemoved, kInvalid };

export String ToString(NodeStatus);

export class NodeInfo {
public:
    NodeInfo(NodeRole role, NodeStatus status, const String &name, const String &ip_addr, i64 port, i64 txn_ts, u64 update_ts, u64 heartbeat_count);
    NodeInfo(NodeRole role, NodeStatus status, const String &name, const String &ip_addr, i64 port, i64 txn_ts, u64 update_ts);
    NodeInfo(NodeRole role, NodeStatus status, const String &name, const String &ip_addr, i64 port, u64 update_ts);
    NodeInfo(NodeRole role, const String &ip_addr, i64 port);

    void set_heartbeat_interval(i64 heartbeat_interval) { heartbeat_interval_ = heartbeat_interval; }
    i64 heartbeat_interval() const { return heartbeat_interval_; }

    void set_update_ts(u64 new_update_ts) { update_ts_ = new_update_ts; }
    u64 update_ts() const { return update_ts_; }

    void set_node_status(NodeStatus new_status) { node_status_ = new_status; }
    NodeStatus node_status() const { return node_status_; }

    void set_node_role(NodeRole new_node_role) { node_role_ = new_node_role; }
    NodeRole node_role() const { return node_role_; }

    void set_node_name(const String &new_node_name);
    String node_name() const;

    void set_node_ip(const String &new_node_ip);
    String node_ip() const;

    void set_node_port(i64 new_port) { port_ = new_port; }
    i64 node_port() const { return port_; }

    void set_txn_ts(i64 new_txn_ts) { txn_ts_ = new_txn_ts; }
    i64 txn_ts() const { return txn_ts_; }

    void set_leader_term(i64 new_leader_term) { leader_term_ = new_leader_term; }
    i64 leader_term() const { return leader_term_; }

    void heartbeat_count_increase() { ++heartbeat_count_; }
    u64 heartbeat_count() const { return heartbeat_count_; }

private:
    mutable std::mutex node_mutex_{};
    Atomic<NodeRole> node_role_{NodeRole::kUnInitialized};
    Atomic<NodeStatus> node_status_{NodeStatus::kInvalid};
    String node_name_{};
    String ip_address_{};
    Atomic<i64> port_{};
    i64 txn_ts_{};
    Atomic<u64> update_ts_{};
    Atomic<i64> leader_term_{};
    Atomic<i64> heartbeat_interval_{}; // provide by leader and used by follower and learner;
    Atomic<u64> heartbeat_count_{};    // given by leader to calculate how many heartbeat received on this node.
    // u64 update_interval_{}; // seconds
    // String from_{}; // Which node the information comes from.
};

} // namespace infinity
