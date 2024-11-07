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

module node_info;

import stl;
// import third_party;

namespace infinity {

NodeInfo::NodeInfo(NodeRole role,
                   NodeStatus status,
                   const String &name,
                   const String &ip_addr,
                   i64 port,
                   i64 txn_ts,
                   u64 update_ts,
                   u64 heartbeat_count)
    : node_role_(role), node_status_(status), node_name_(name), ip_address_(ip_addr), port_(port), txn_ts_(txn_ts), update_ts_(update_ts),
      heartbeat_count_(heartbeat_count) {}

NodeInfo::NodeInfo(NodeRole role, NodeStatus status, const String &name, const String &ip_addr, i64 port, i64 txn_ts, u64 update_ts)
    : node_role_(role), node_status_(status), node_name_(name), ip_address_(ip_addr), port_(port), txn_ts_(txn_ts), update_ts_(update_ts) {}

NodeInfo::NodeInfo(NodeRole role, NodeStatus status, const String &name, const String &ip_addr, i64 port, u64 update_ts)
    : node_role_(role), node_status_(status), node_name_(name), ip_address_(ip_addr), port_(port), update_ts_(update_ts) {}

NodeInfo::NodeInfo(NodeRole role, const String &ip_addr, i64 port) : node_role_(role), ip_address_(ip_addr), port_(port) {}

NodeInfo::NodeInfo(NodeRole role, NodeStatus status) : node_role(role), node_status_(status) {}

void NodeInfo::set_node_name(const String &new_node_name) {
    std::unique_lock locker(node_mutex_);
    node_name_ = new_node_name;
}

String NodeInfo::node_name() const {
    std::unique_lock locker(node_mutex_);
    return node_name_;
}

void NodeInfo::set_node_ip(const String &new_node_ip) {
    std::unique_lock locker(node_mutex_);
    ip_address_ = new_node_ip;
}

String NodeInfo::node_ip() const {
    std::unique_lock locker(node_mutex_);
    return ip_address_;
}

String ToString(NodeStatus status) {
    switch (status) {
        case NodeStatus::kAlive:
            return "alive";
        case NodeStatus::kTimeout:
            return "timeout";
        case NodeStatus::kInvalid:
            return "invalid";
        case NodeStatus::kLostConnection:
            return "lost connection";
        case NodeStatus::kRemoved:
            return "removed";
    }
}

} // namespace infinity