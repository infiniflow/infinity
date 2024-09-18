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

module peer_thrift_client;

import stl;
import third_party;
import logger;
import peer_server_thrift_types;
import status;
import thrift;
import infinity_exception;
import peer_task;

namespace infinity {

PeerClient::~PeerClient() {
    if (running_) {
        UnInit();
    }
    server_connected_ = false;
}

Status PeerClient::Init() {

    Status status = Reconnect();
    if (status.ok()) {
        processor_thread_ = MakeShared<Thread>([this] { this->Process(); });
    }

    return status;
}

/// TODO: comment
Status PeerClient::UnInit() {
    LOG_INFO(fmt::format("Peer client: {} is stopping.", this_node_name_));
    if (processor_thread_.get() != nullptr) {
        SharedPtr<TerminatePeerTask> terminate_task = MakeShared<TerminatePeerTask>();
        peer_task_queue_.Enqueue(terminate_task);
        terminate_task->Wait();
        processor_thread_->join();
    }
    LOG_INFO(fmt::format("Peer client: {} is stopped.", this_node_name_));
    return Status::OK();
}

Status PeerClient::Reconnect() {
    Status status = Status::OK();
    if (server_connected_ == true) {
        return status;
    }

    try {
        socket_ = MakeShared<TSocket>(node_info_.ip_address_, node_info_.port_);

        TSocket *socket = static_cast<TSocket *>(socket_.get());
        socket->setConnTimeout(2000); // 2s to timeout
        transport_ = MakeShared<TBufferedTransport>(socket_);
        protocol_ = MakeShared<TBinaryProtocol>(transport_);
        client_ = MakeUnique<PeerServiceClient>(protocol_);
        transport_->open();
        server_connected_ = true;
    } catch (const std::exception &e) {
        status = Status::CantConnectServer(node_info_.ip_address_, node_info_.port_, e.what());
    }
    return status;
}

void PeerClient::Send(SharedPtr<PeerTask> peer_task) {
    ++peer_task_count_;
    peer_task_queue_.Enqueue(std::move(peer_task));
}

void PeerClient::Process() {
    Deque<SharedPtr<PeerTask>> peer_tasks;
    running_ = true;
    while (running_) {
        peer_task_queue_.DequeueBulk(peer_tasks);
        for (const auto &peer_task : peer_tasks) {
            switch (peer_task->Type()) {
                case PeerTaskType::kTerminate: {
                    LOG_INFO("Stop the background processor");
                    running_ = false;
                    break;
                }
                case PeerTaskType::kRegister: {
                    LOG_TRACE(peer_task->ToString());
                    RegisterPeerTask *register_peer_task = static_cast<RegisterPeerTask *>(peer_task.get());
                    Register(register_peer_task);
                    break;
                }
                case PeerTaskType::kUnregister: {
                    LOG_TRACE(peer_task->ToString());
                    UnregisterPeerTask *unregister_peer_task = static_cast<UnregisterPeerTask *>(peer_task.get());
                    Unregister(unregister_peer_task);
                    break;
                }
                case PeerTaskType::kHeartBeat: {
                    LOG_TRACE(peer_task->ToString());
                    HeartBeatPeerTask *heartbeat_peer_task = static_cast<HeartBeatPeerTask *>(peer_task.get());
                    HeartBeat(heartbeat_peer_task);
                    break;
                }
                case PeerTaskType::kLogSync: {
                    LOG_TRACE(peer_task->ToString());

                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid peer task type");
                    UnrecoverableError(error_message);
                    break;
                }
            }
            peer_task->Complete();
        }
        peer_task_count_ -= peer_tasks.size();
        peer_tasks.clear();
    }
}

void PeerClient::Register(RegisterPeerTask *peer_task) {

    RegisterRequest request;
    request.node_name = peer_task->node_name_;
    switch (peer_task->node_role_) {
        case NodeRole::kFollower: {
            request.node_type = NodeType::type::kFollower;
            break;
        }
        case NodeRole::kLearner: {
            request.node_type = NodeType::type::kLearner;
            break;
        }
        default: {
            String error_message = fmt::format("Only follower and learner can send register message to leader");
            UnrecoverableError(error_message);
        }
    }
    request.node_ip = peer_task->node_ip_;
    request.node_port = peer_task->node_port_;
    request.txn_timestamp = peer_task->txn_ts_;

    RegisterResponse response;
    client_->Register(response, request);
    if (response.error_code != 0) {
        // Error
        peer_task->error_code_ = response.error_code;
        peer_task->error_message_ = response.error_message;
    } else {
        peer_task->leader_name_ = response.leader_name;
        peer_task->leader_term_ = response.leader_term;
        peer_task->heartbeat_interval_ = response.heart_beat_interval;
    }
}

void PeerClient::Unregister(UnregisterPeerTask *peer_task) {
    UnregisterRequest request;
    UnregisterResponse response;
    request.node_name = peer_task->node_name_;

    client_->Unregister(response, request);
    if (response.error_code != 0) {
        // Error
        peer_task->error_code_ = response.error_code;
        peer_task->error_message_ = response.error_message;
    }
}

void PeerClient::HeartBeat(HeartBeatPeerTask *peer_task) {
    HeartBeatRequest request;
    HeartBeatResponse response;
    request.node_name = peer_task->node_name_;
    switch (peer_task->node_role_) {
        case NodeRole::kFollower: {
            request.node_type = NodeType::type::kFollower;
            break;
        }
        case NodeRole::kLearner: {
            request.node_type = NodeType::type::kLearner;
            break;
        }
        default: {
            String error_message = fmt::format("Only follower and learner can send register message to leader");
            UnrecoverableError(error_message);
        }
    }
    request.node_ip = peer_task->node_ip_;
    request.node_port = peer_task->node_port_;
    request.txn_timestamp = peer_task->txn_ts_;

    try {
        client_->HeartBeat(response, request);
    } catch (apache::thrift::transport::TTransportException &thrift_exception) {
        server_connected_ = false;
        switch (thrift_exception.getType()) {
            case TTransportExceptionType::END_OF_FILE: {
                peer_task->error_message_ = thrift_exception.what();
                peer_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectLeader);
                return;
            }
            default: {
                String error_message = "Heartbeat: error in data transfer to leader";
                LOG_CRITICAL(error_message);
                UnrecoverableError(error_message);
            }
        }
    }

    if (response.error_code != 0) {
        // Error
        peer_task->error_code_ = response.error_code;
        peer_task->error_message_ = response.error_message;
    } else {
        peer_task->leader_term_ = response.leader_term;
        SizeT node_count = response.other_nodes.size();
        peer_task->other_nodes_.reserve(node_count);
        for (SizeT idx = 0; idx < node_count; ++idx) {
            SharedPtr<NodeInfo> node_info = MakeShared<NodeInfo>();
            auto &other_node = response.other_nodes[idx];
            if (this_node_name_ == other_node.node_name) {
                continue;
            }
            node_info->node_name_ = other_node.node_name;
            node_info->ip_address_ = other_node.node_ip;
            node_info->port_ = other_node.node_port;
            node_info->txn_timestamp_ = other_node.txn_timestamp;
            node_info->heartbeat_count_ = other_node.hb_count;
            switch (other_node.node_type) {
                case infinity_peer_server::NodeType::type::kLeader: {
                    node_info->node_role_ = NodeRole::kLeader;
                    break;
                }
                case infinity_peer_server::NodeType::type::kFollower: {
                    node_info->node_role_ = NodeRole::kFollower;
                    break;
                }
                case infinity_peer_server::NodeType::type::kLearner: {
                    node_info->node_role_ = NodeRole::kLearner;
                    break;
                }
                default: {
                    String error_message = "Invalid role type";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
            switch (other_node.node_status) {
                case infinity_peer_server::NodeStatus::type::kAlive: {
                    node_info->node_status_ = NodeStatus::kAlive;
                    break;
                }
                case infinity_peer_server::NodeStatus::type::kTimeout: {
                    node_info->node_status_ = NodeStatus::kTimeout;
                    break;
                }
                default: {
                    String error_message = "Invalid node status";
                    LOG_CRITICAL(error_message);
                    UnrecoverableError(error_message);
                }
            }
            peer_task->other_nodes_.emplace_back(node_info);
        }
    }
}

} // namespace infinity
