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

export module peer_task;

import stl;
import admin_statement;

namespace infinity {

export enum class PeerTaskType {
    kInvalid,
    kTerminate,
    kRegister,
    kUnregister,
    kHeartBeat,
    kLogSync,
    kChangeRole,
    kNewLeader,
};

export enum class NodeStatus { kAlive, kTimeout, kLostConnection, kRemoved, kInvalid };

export String ToString(NodeStatus);

export struct NodeInfo {
    NodeRole node_role_{NodeRole::kUnInitialized};
    NodeStatus node_status_{NodeStatus::kInvalid};
    String node_name_{};
    String ip_address_{};
    i64 port_{};
    i64 txn_timestamp_{};
    u64 last_update_ts_{};
    i64 leader_term_{};
    i64 heartbeat_interval_{}; // provide by leader and used by follower and learner;
    u64 heartbeat_count_{};    // given by leader to calculate how many heartbeat received on this node.
    // u64 update_interval_{}; // seconds
    // String from_{}; // Which node the information comes from.
};

export class PeerTask {
public:
    explicit PeerTask(PeerTaskType type, bool async = false) : type_(type), complete_(false), async_(async) //complete_ should not be assigned false in wait(), otherwise it might be stuck in wait() forever if complete() is called before wait().
    {}
    virtual ~PeerTask() = default;

    void Wait() {
        if (async_) {
            return;
        }
        std::unique_lock<std::mutex> locker(mutex_);
        cv_.wait(locker, [this] { return complete_; });
    }

    void Complete() {
        if (async_) {
            return;
        }
        std::unique_lock<std::mutex> locker(mutex_);
        complete_ = true;
        cv_.notify_one();
    }

    [[nodiscard]] PeerTaskType Type() const { return type_; }

    virtual String ToString() const = 0;

protected:
    PeerTaskType type_{PeerTaskType::kInvalid};
    mutable std::mutex mutex_{};
    std::condition_variable cv_{};
    bool async_{false};
    bool complete_{false};
};

export class TerminatePeerTask final : public PeerTask {
public:
    TerminatePeerTask(bool force_async) : PeerTask(PeerTaskType::kTerminate, force_async) {}

    String ToString() const final;
};

export class RegisterPeerTask final : public PeerTask {
public:
    RegisterPeerTask(String node_name, NodeRole node_role, String node_ip, i64 node_port, i64 txn_ts)
        : PeerTask(PeerTaskType::kRegister), node_name_(std::move(node_name)), node_role_(node_role), node_ip_(std::move(node_ip)),
          node_port_(node_port), txn_ts_(txn_ts) {}

    String ToString() const final;

    String node_name_{};
    NodeRole node_role_{};
    String node_ip_{};
    i64 node_port_{};
    i64 txn_ts_{};

    // response
    i64 error_code_{};
    String error_message_{};
    String leader_name_{};
    i64 leader_term_{};
    i64 heartbeat_interval_{}; // microseconds
};

export class UnregisterPeerTask final : public PeerTask {
public:
    UnregisterPeerTask(String node_name) : PeerTask(PeerTaskType::kUnregister), node_name_(std::move(node_name)) {}

    String ToString() const final;

    String node_name_{};

    // response
    i64 error_code_{};
    String error_message_{};
};

export class HeartBeatPeerTask final : public PeerTask {
public:
    HeartBeatPeerTask(String node_name, NodeRole node_role, String node_ip, i64 node_port, i64 txn_ts)
        : PeerTask(PeerTaskType::kHeartBeat), node_name_(std::move(node_name)), node_role_(node_role), node_ip_(std::move(node_ip)),
          node_port_(node_port), txn_ts_(txn_ts) {}

    String ToString() const final;

    String node_name_{};
    NodeRole node_role_{};
    String node_ip_{};
    i64 node_port_{};
    i64 txn_ts_{};

    // response
    i64 error_code_{};
    String error_message_{};
    i64 leader_term_{};
    Vector<SharedPtr<NodeInfo>> other_nodes_{};
    NodeStatus sender_status_{NodeStatus::kInvalid};
};

export class SyncLogTask final : public PeerTask {
public:
    SyncLogTask(const String &node_name, const Vector<SharedPtr<String>> &log_strings, bool on_register)
        : PeerTask(PeerTaskType::kLogSync), node_name_(node_name), log_strings_(log_strings), on_register_(on_register) {}

    String ToString() const final;

    String node_name_{};
    Vector<SharedPtr<String>> log_strings_;
    bool on_register_{false};

    // response
    i64 error_code_{};
    String error_message_{};
};

export class ChangeRoleTask final : public PeerTask {
public:
    ChangeRoleTask(String node_name, String role_name)
        : PeerTask(PeerTaskType::kChangeRole), node_name_(node_name), role_name_(std::move(role_name)) {}

    String ToString() const final;

    String node_name_{};
    String role_name_{};

    // response
    i64 error_code_{};
    String error_message_{};
};

} // namespace infinity
