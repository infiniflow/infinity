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
import infinity_context;
import cluster_manager;
import admin_statement;
import node_info;

namespace infinity {

PeerClient::~PeerClient() {
    if (running_) {
        UnInit(false);
    }
    server_connected_ = false;
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("PeerClient");
#endif
}

Status PeerClient::Init() {

    Status status = Reconnect();
    if (status.ok()) {
        processor_thread_ = MakeShared<Thread>([this] { this->Process(); });
    }

    return status;
}

/// TODO: comment
Status PeerClient::UnInit(bool sync) {
    LOG_INFO(fmt::format("Peer client: {} is stopping.", from_node_name_));

    if (processor_thread_.get() != nullptr) {
        SharedPtr<TerminatePeerTask> terminate_task = MakeShared<TerminatePeerTask>(!sync);
        peer_task_queue_.Enqueue(terminate_task);
        terminate_task->Wait();
        if (sync) {
            processor_thread_->join();
        } else {
            processor_thread_->detach();
        }
    }

    LOG_INFO(fmt::format("Peer client: {} is stopped.", from_node_name_));
    return Disconnect();
}

Status PeerClient::Reconnect() {
    Status status = Status::OK();
    if (server_connected_ == true) {
        return status;
    }

    try {
        socket_ = MakeShared<TSocket>(ip_address_, port_);

        TSocket *socket = static_cast<TSocket *>(socket_.get());
        socket->setConnTimeout(2000); // 2s to timeout
                                      //        socket->setRecvTimeout(2000);
                                      //        socket->setSendTimeout(2000);
        transport_ = MakeShared<TBufferedTransport>(socket_);
        protocol_ = MakeShared<TBinaryProtocol>(transport_);
        client_ = MakeUnique<PeerServiceClient>(protocol_);
        transport_->open();
        server_connected_ = true;
    } catch (const std::exception &e) {
        status = Status::CantConnectServer(ip_address_, port_, e.what());
    }
    return status;
}

Status PeerClient::Disconnect() {
    Status status = Status::OK();
    if (server_connected_ == false) {
        return status;
    }

    try {
        socket_->close();
        transport_->close();
        protocol_.reset();
        client_.reset();
        transport_->close();
        server_connected_ = false;
    } catch (const std::exception &e) {
        status = Status::CantConnectServer(ip_address_, port_, e.what());
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
                    SyncLogTask *sync_log_task = static_cast<SyncLogTask *>(peer_task.get());
                    SyncLogs(sync_log_task);
                    break;
                }
                case PeerTaskType::kChangeRole: {
                    LOG_TRACE(peer_task->ToString());
                    ChangeRoleTask *change_role_task = static_cast<ChangeRoleTask *>(peer_task.get());
                    ChangeRole(change_role_task);
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

    try {
        LOG_INFO(fmt::format("Register to leader {}:{}", request.node_ip, request.node_port));
        client_->Register(response, request);
    } catch (apache::thrift::transport::TTransportException &thrift_exception) {
        server_connected_ = false;
        switch (thrift_exception.getType()) {
            case TTransportExceptionType::END_OF_FILE: {
                peer_task->error_message_ = thrift_exception.what();
                peer_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectLeader);
                return;
            }
            default: {
                String error_message = "Register to the leader";
                UnrecoverableError(error_message);
            }
        }
    }

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

    try {
        client_->Unregister(response, request);
    } catch (apache::thrift::transport::TTransportException &thrift_exception) {
        server_connected_ = false;
        switch (thrift_exception.getType()) {
            case TTransportExceptionType::END_OF_FILE: {
                peer_task->error_message_ = thrift_exception.what();
                peer_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectLeader);
                return;
            }
            default: {
                String error_message = "Unregister from the leader";
                UnrecoverableError(error_message);
            }
        }
    }

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
                UnrecoverableError(error_message);
            }
        }
    }

    if (response.error_code != 0) {
        // Error
        peer_task->error_code_ = response.error_code;
        peer_task->error_message_ = response.error_message;
        if (response.sender_status == infinity_peer_server::NodeStatus::type::kLostConnection) {
            peer_task->leader_term_ = response.leader_term;
            peer_task->sender_status_ = NodeStatus::kLostConnection;
        }
    } else {
        switch (response.sender_status) {
            case infinity_peer_server::NodeStatus::type::kAlive: {
                peer_task->sender_status_ = NodeStatus::kAlive;
                break;
            }
            case infinity_peer_server::NodeStatus::type::kTimeout: {
                peer_task->sender_status_ = NodeStatus::kTimeout;
                break;
            }
            case infinity_peer_server::NodeStatus::type::kLostConnection: {
                peer_task->sender_status_ = NodeStatus::kLostConnection;
                break;
            }
            case infinity_peer_server::NodeStatus::type::kRemoved: {
                peer_task->sender_status_ = NodeStatus::kRemoved;
                break;
            }
            default: {
                String error_message = "Invalid sender status";
                UnrecoverableError(error_message);
            }
        }

        peer_task->leader_term_ = response.leader_term;
        SizeT node_count = response.other_nodes.size();
        peer_task->other_nodes_.reserve(node_count);
        for (SizeT idx = 0; idx < node_count; ++idx) {

            auto &other_node = response.other_nodes[idx];
            if (from_node_name_ == other_node.node_name) {
                continue;
            }
            NodeRole node_role = NodeRole::kUnInitialized;
            switch (other_node.node_type) {
                case infinity_peer_server::NodeType::type::kLeader: {
                    node_role = NodeRole::kLeader;
                    break;
                }
                case infinity_peer_server::NodeType::type::kFollower: {
                    node_role = NodeRole::kFollower;
                    break;
                }
                case infinity_peer_server::NodeType::type::kLearner: {
                    node_role = NodeRole::kLearner;
                    break;
                }
                default: {
                    String error_message = "Invalid role type";
                    UnrecoverableError(error_message);
                }
            }

            NodeStatus node_status = NodeStatus::kInvalid;
            switch (other_node.node_status) {
                case infinity_peer_server::NodeStatus::type::kAlive: {
                    node_status = NodeStatus::kAlive;
                    break;
                }
                case infinity_peer_server::NodeStatus::type::kTimeout: {
                    node_status = NodeStatus::kTimeout;
                    break;
                }
                default: {
                    String error_message = "Invalid node status";
                    UnrecoverableError(error_message);
                }
            }

            SharedPtr<NodeInfo> node_info = MakeShared<NodeInfo>(node_role,
                                                                 node_status,
                                                                 other_node.node_name,
                                                                 other_node.node_ip,
                                                                 other_node.node_port,
                                                                 other_node.txn_timestamp,
                                                                 0,
                                                                 other_node.hb_count);
            peer_task->other_nodes_.emplace_back(node_info);
        }
    }
}

void PeerClient::SyncLogs(SyncLogTask *peer_task) {
    SyncLogRequest request;
    SyncLogResponse response;
    request.node_name = peer_task->node_name_;
    SizeT log_count = peer_task->log_strings_.size();
    request.log_entries.reserve(log_count);
    request.on_startup = peer_task->on_register_;
    for (SizeT i = 0; i < log_count; ++i) {
        request.log_entries.emplace_back(*peer_task->log_strings_[i]);
    }

    try {
        client_->SyncLog(response, request);
        if (response.error_code != 0) {
            // Error
            peer_task->error_code_ = response.error_code;
            peer_task->error_message_ = response.error_message;
            LOG_ERROR(fmt::format("Sync log to node: {}, error: {}", peer_task->node_name_, peer_task->error_message_));
        }
    } catch (apache::thrift::transport::TTransportException &thrift_exception) {
        peer_task->error_message_ = thrift_exception.what();
        peer_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectServer);
        LOG_ERROR(fmt::format("Sync log to node, transport error: {}, error: {}", peer_task->node_name_, peer_task->error_message_));
        Status status = InfinityContext::instance().cluster_manager()->UpdateNodeByLeader(peer_task->node_name_, UpdateNodeOp::kLostConnection);
        if (!status.ok()) {
            LOG_ERROR(status.message());
        }
    } catch (apache::thrift::TApplicationException &application_exception) {
        peer_task->error_message_ = application_exception.what();
        peer_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectServer);
        LOG_ERROR(fmt::format("Sync log to node, application: {}, error: {}", peer_task->node_name_, peer_task->error_message_));
        Status status = InfinityContext::instance().cluster_manager()->UpdateNodeByLeader(peer_task->node_name_, UpdateNodeOp::kLostConnection);
        if (!status.ok()) {
            LOG_ERROR(status.message());
        }
    } catch (const std::exception &e) {
        LOG_ERROR(e.what());
    }
}

void PeerClient::ChangeRole(ChangeRoleTask *change_role_task) {
    ChangeRoleRequest request;
    ChangeRoleResponse response;
    request.node_name = change_role_task->node_name_;
    request.node_type = infinity_peer_server::NodeType::kInvalid;
    ToLower(change_role_task->role_name_);
    if (IsEqual(change_role_task->role_name_, "admin")) {
        request.node_type = infinity_peer_server::NodeType::kAdmin;
    }
    try {
        client_->ChangeRole(response, request);
        if (response.error_code != 0) {
            // Error
            change_role_task->error_code_ = response.error_code;
            change_role_task->error_message_ = response.error_message;
            LOG_ERROR(fmt::format("Sync log to node: {}, error: {}", change_role_task->node_name_, change_role_task->error_message_));
        }
    } catch (apache::thrift::transport::TTransportException &thrift_exception) {
        change_role_task->error_message_ = thrift_exception.what();
        change_role_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectServer);
        LOG_ERROR(fmt::format("Sync log to node, transport error: {}, error: {}", change_role_task->node_name_, change_role_task->error_message_));
        Status status =
            InfinityContext::instance().cluster_manager()->UpdateNodeByLeader(change_role_task->node_name_, UpdateNodeOp::kLostConnection);
        if (!status.ok()) {
            LOG_ERROR(status.message());
        }
    } catch (apache::thrift::TApplicationException &application_exception) {
        change_role_task->error_message_ = application_exception.what();
        change_role_task->error_code_ = static_cast<i64>(ErrorCode::kCantConnectServer);
        LOG_ERROR(fmt::format("Sync log to node, application: {}, error: {}", change_role_task->node_name_, change_role_task->error_message_));
        Status status =
            InfinityContext::instance().cluster_manager()->UpdateNodeByLeader(change_role_task->node_name_, UpdateNodeOp::kLostConnection);
        if (!status.ok()) {
            LOG_ERROR(status.message());
        }
    } catch (const std::exception &e) {
        LOG_ERROR(e.what());
    }
}

} // namespace infinity
