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
import status;
import config;
import infinity_context;
import storage;
import logger;
import wal_manager;
import wal_entry;
import infinity_exception;

namespace infinity {

Status ClusterManager::InitAsFollower(const String &node_name, const String &leader_ip, i64 leader_port) {

    Config *config_ptr = InfinityContext::instance().config();
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as follower error: already initialized.");
    }

    this_node_ = MakeShared<NodeInfo>(NodeRole::kFollower,
                                      NodeStatus::kAlive,
                                      node_name,
                                      config_ptr->PeerServerIP(),
                                      config_ptr->PeerServerPort(),
                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

    leader_node_ = MakeShared<NodeInfo>(NodeRole::kLeader, leader_ip, leader_port);
    Status client_status = Status::OK();
    std::tie(client_to_leader_, client_status) = ClusterManager::ConnectToServerNoLock(node_name, leader_ip, leader_port);
    if (!client_status.ok()) {
        return client_status;
    }

    current_node_role_ = NodeRole::kFollower;

    return Status::OK();
}

Status ClusterManager::InitAsLearner(const String &node_name, const String &leader_ip, i64 leader_port) {

    Config *config_ptr = InfinityContext::instance().config();
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    if (this_node_.get() != nullptr) {
        return Status::ErrorInit("Init node as learner error: already initialized.");
    }

    this_node_ = MakeShared<NodeInfo>(NodeRole::kLearner,
                                      NodeStatus::kAlive,
                                      node_name,
                                      config_ptr->PeerServerIP(),
                                      config_ptr->PeerServerPort(),
                                      std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

    leader_node_ = MakeShared<NodeInfo>(NodeRole::kLeader, leader_ip, leader_port);

    Status client_status = Status::OK();
    std::tie(client_to_leader_, client_status) = ClusterManager::ConnectToServerNoLock(node_name, leader_ip, leader_port);
    if (!client_status.ok()) {
        return client_status;
    }

    current_node_role_ = NodeRole::kLearner;

    return Status::OK();
}

Status ClusterManager::RegisterToLeader() {
    // Register to leader;
    std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
    Status status = RegisterToLeaderNoLock();
    if (!status.ok()) {
        return status;
    }

    this->hb_running_ = true;
    hb_periodic_thread_ = MakeShared<Thread>([this] { this->HeartBeatToLeaderThread(); });
    return status;
}

Status ClusterManager::RegisterToLeaderNoLock() {
    // Register to leader, used by follower and learner
    Storage *storage_ptr = InfinityContext::instance().storage();
    SharedPtr<RegisterPeerTask> register_peer_task = nullptr;
    if (storage_ptr->reader_init_phase() == ReaderInitPhase::kPhase2) {
        register_peer_task = MakeShared<RegisterPeerTask>(this_node_->node_name(),
                                                          this_node_->node_role(),
                                                          this_node_->node_ip(),
                                                          this_node_->node_port(),
                                                          storage_ptr->txn_manager()->CurrentTS());
    } else {
        register_peer_task =
            MakeShared<RegisterPeerTask>(this_node_->node_name(), this_node_->node_role(), this_node_->node_ip(), this_node_->node_port(), 0);
    }

    client_to_leader_->Send(register_peer_task);
    register_peer_task->Wait();

    Status status = Status::OK();
    if (register_peer_task->error_code_ != 0) {
        status.code_ = static_cast<ErrorCode>(register_peer_task->error_code_);
        status.msg_ = MakeUnique<String>(fmt::format("From leader: {}", register_peer_task->error_message_));
        return status;
    }
    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();
    leader_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());
    leader_node_->set_node_name(register_peer_task->leader_name_);
    leader_node_->set_node_status(NodeStatus::kAlive);
    //    client_to_leader_->SetPeerNode(NodeRole::kLeader, register_peer_task->leader_name_, register_peer_task->update_time_);
    // Start HB thread.
    if (register_peer_task->heartbeat_interval_ == 0) {
        leader_node_->set_heartbeat_interval(1000); // 1000 ms by default
    } else {
        leader_node_->set_heartbeat_interval(register_peer_task->heartbeat_interval_);
    }

    return status;
}

Status ClusterManager::UnregisterToLeaderNoLock() {
    Status status = Status::OK();
    if (current_node_role_ == NodeRole::kFollower or current_node_role_ == NodeRole::kLearner) {
        if (leader_node_->node_status() == NodeStatus::kAlive) {
            // Leader is alive, need to unregister
            SharedPtr<UnregisterPeerTask> unregister_task = MakeShared<UnregisterPeerTask>(this_node_->node_name());
            client_to_leader_->Send(unregister_task);
            unregister_task->Wait();
            if (unregister_task->error_code_ != 0) {
                LOG_ERROR(fmt::format("Fail to unregister from leader: {}", unregister_task->error_message_));
                status.code_ = static_cast<ErrorCode>(unregister_task->error_code_);
                status.msg_ = MakeUnique<String>(unregister_task->error_message_);
            }
        }
    }
    return status;
}

void ClusterManager::HeartBeatToLeaderThread() {
    // Heartbeat interval
    auto hb_interval = std::chrono::milliseconds(leader_node_->heartbeat_interval());
    Storage *storage_ptr = InfinityContext::instance().storage();
    while (true) {
        std::unique_lock hb_lock(this->hb_mutex_);
        this->hb_cv_.wait_for(hb_lock, hb_interval, [&] { return !this->hb_running_; });
        if (!hb_running_) {
            // UnInit cluster manager will set hb_running_ false;
            break;
        }

        if (!client_to_leader_->ServerConnected()) {
            // If peer_client was disconnected, try to reconnect to server.
            Status connect_status = client_to_leader_->Reconnect();
            if (!connect_status.ok()) {
                LOG_ERROR(connect_status.message());
                continue;
            }
        }

        // Update latest update time
        auto hb_now = std::chrono::system_clock::now();
        auto hb_time_since_epoch = hb_now.time_since_epoch();
        SharedPtr<HeartBeatPeerTask> hb_task = nullptr;
        {
            std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
            this_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(hb_time_since_epoch).count());

            String this_node_name = this_node_->node_name();
            NodeRole this_node_role = this_node_->node_role();
            String this_node_ip = this_node_->node_ip();
            i64 this_node_port = this_node_->node_port();

            // Send heartbeat
            hb_task =
                MakeShared<HeartBeatPeerTask>(this_node_name, this_node_role, this_node_ip, this_node_port, storage_ptr->txn_manager()->CurrentTS());
        }
        client_to_leader_->Send(hb_task);
        hb_task->Wait();

        if (hb_task->error_code_ != 0) {
            if (hb_task->error_code_ == (i64)(ErrorCode::kNotRegistered)) {
                LOG_ERROR(fmt::format("Error to connect to leader: {}, {}:{}, error: {}",
                                      leader_node_->node_name(),
                                      leader_node_->node_ip(),
                                      leader_node_->node_port(),
                                      hb_task->error_message_));
                leader_node_->set_node_status(hb_task->sender_status_);
            } else {
                LOG_ERROR(fmt::format("Can't connect to leader: {}, {}:{}, error: {}",
                                      leader_node_->node_name(),
                                      leader_node_->node_ip(),
                                      leader_node_->node_port(),
                                      hb_task->error_message_));
                leader_node_->set_node_status(NodeStatus::kTimeout);
            }
            continue;
        }

        // Prepare the update time of leader
        hb_now = std::chrono::system_clock::now();
        hb_time_since_epoch = hb_now.time_since_epoch();

        {
            std::unique_lock<std::mutex> cluster_lock(cluster_mutex_);
            // Update leader info
            leader_node_->set_node_status(NodeStatus::kAlive);
            leader_node_->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(hb_time_since_epoch).count());
            leader_node_->set_leader_term(hb_task->leader_term_);

            // Update heartbeat sending count
            this_node_->heartbeat_count_increase();

            // Update the non-leader node info from leader
            UpdateNodeInfoNoLock(hb_task->other_nodes_);

            // Check if leader can't send message to this reader node
            this_node_->set_node_status(hb_task->sender_status_);
        }
    }
    return;
}

Status ClusterManager::UpdateNodeInfoNoLock(const Vector<SharedPtr<NodeInfo>> &info_of_nodes) {
    // Only follower and learner will use this function.
    HashMap<String, bool> exist_node_name_map;
    exist_node_name_map.reserve(other_node_map_.size());
    for (const auto &node_pair : other_node_map_) {
        exist_node_name_map.emplace(node_pair.first, false);
    }

    auto now = std::chrono::system_clock::now();
    auto time_since_epoch = now.time_since_epoch();

    for (const SharedPtr<NodeInfo> &node_info : info_of_nodes) {
        auto iter = other_node_map_.find(node_info->node_name());
        if (iter == other_node_map_.end()) {
            node_info->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());
            other_node_map_.emplace(node_info->node_name(), node_info);
        } else {
            iter->second->set_txn_ts(node_info->txn_ts());
            iter->second->set_node_ip(node_info->node_ip());
            iter->second->set_node_port(node_info->node_port());
            iter->second->set_node_role(node_info->node_role());
            iter->second->set_node_status(node_info->node_status());
            iter->second->set_update_ts(std::chrono::duration_cast<std::chrono::seconds>(time_since_epoch).count());

            exist_node_name_map[node_info->node_name()] = true;
        }
    }

    for (const auto &exist_node_pair : exist_node_name_map) {
        if (!exist_node_pair.second) {
            other_node_map_.erase(exist_node_pair.first);
        }
    }

    return Status::OK();
}

Status ClusterManager::ApplySyncedLogNolock(const Vector<String> &synced_logs) {
    Storage *storage_ptr = InfinityContext::instance().storage();
    WalManager *wal_manager = storage_ptr->wal_manager();
    TransactionID last_txn_id = 0;
    TxnTimeStamp last_commit_ts = 0;
    for (auto &log_str : synced_logs) {
        const i32 entry_size = log_str.size();
        const char *ptr = log_str.data();
        SharedPtr<WalEntry> entry = WalEntry::ReadAdv(ptr, entry_size);
        LOG_DEBUG(fmt::format("WAL Entry: {}", entry->ToString()));
        last_txn_id = entry->txn_id_;
        last_commit_ts = entry->commit_ts_;
        ReplayWalOptions options{.on_startup_ = false, .is_replay_ = false, .sync_from_leader_ = true};
        wal_manager->ReplayWalEntry(*entry, options);
    }

    LOG_INFO(fmt::format("Replicated from leader: latest txn commit_ts: {}, latest txn id: {}", last_commit_ts, last_txn_id));
    storage_ptr->catalog()->next_txn_id_ = last_txn_id;
    storage_ptr->wal_manager()->UpdateCommitState(last_commit_ts, 0);
    storage_ptr->txn_manager()->SetStartTS(last_commit_ts);
    return Status::OK();
}

Status ClusterManager::ContinueStartup(const Vector<String> &synced_logs) {
    Storage *storage_ptr = InfinityContext::instance().storage();
    WalManager *wal_manager = storage_ptr->wal_manager();
    bool is_checkpoint = true;
    TxnTimeStamp last_commit_ts;
    for (auto &log_str : synced_logs) {
        const i32 entry_size = log_str.size();
        const char *ptr = log_str.data();
        SharedPtr<WalEntry> entry = WalEntry::ReadAdv(ptr, entry_size);
        for (const auto &cmd : entry->cmds_) {
            if (is_checkpoint) {
                if (cmd->GetType() != WalCommandType::CHECKPOINT) {
                    is_checkpoint = false;
                }
            } else {
                if (cmd->GetType() == WalCommandType::CHECKPOINT) {
                    UnrecoverableError("Expect non-checkpoint log");
                }
            }
        }
        LOG_DEBUG(fmt::format("WAL Entry: {}", entry->ToString()));
        ReplayWalOptions options{.on_startup_ = true, .is_replay_ = false, .sync_from_leader_ = true};
        wal_manager->ReplayWalEntry(*entry, options);
        last_commit_ts = entry->commit_ts_;
    }

    storage_ptr->AdminToReaderBottom(last_commit_ts + 1);
    return Status::OK();
}

} // namespace infinity