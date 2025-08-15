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

export module infinity_core:peer_thrift_client;

import :peer_server_thrift_types;
import :stl;
import :thrift;
import :status;
import :blocking_queue;
import :peer_task;

import global_resource_usage;

namespace infinity {

using namespace ::apache::thrift;
using namespace ::apache::thrift::protocol;
using namespace ::apache::thrift::transport;
using namespace infinity_peer_server;

export class PeerClient {
public:
    PeerClient(const std::string &from_node_name, const std::string &ip_addr, i64 port) : from_node_name_(from_node_name), ip_address_(ip_addr), port_(port) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("PeerClient");
#endif
    }

    ~PeerClient();
    //    void SetPeerNode(NodeRole role, const std::string& node_name, i64 update_ts) {
    //        node_info_.node_name_ = node_name;
    //        node_info_.last_update_ts_ = update_ts;
    //        node_info_.node_status_ = NodeStatus::kAlive;
    //        node_info_.node_role_ = role;
    //    }
    //

    Status Init();
    Status UnInit(bool sync);

    Status Reconnect();
    Status Disconnect();
    void Send(std::shared_ptr<PeerTask> task);

    bool ServerConnected() const { return server_connected_; }

private:
    void Process();
    void Call(std::function<void()> call_func);

    void Register(RegisterPeerTask *peer_task);
    void Unregister(UnregisterPeerTask *peer_task);
    void HeartBeat(HeartBeatPeerTask *peer_task);
    void SyncLogs(SyncLogTask *peer_task);
    void ChangeRole(ChangeRoleTask *change_role_task);

private:
    std::string from_node_name_{};
    std::string ip_address_{};
    i64 port_{};

    // For message transportation
    std::shared_ptr<TTransport> socket_{};
    std::shared_ptr<TTransport> transport_{};
    std::shared_ptr<TProtocol> protocol_{};
    std::unique_ptr<PeerServiceClient> client_{};
    std::atomic<bool> server_connected_{false};

    BlockingQueue<std::shared_ptr<PeerTask>> peer_task_queue_{"PeerClient"};
    std::shared_ptr<std::thread> processor_thread_{};
    std::atomic<u64> peer_task_count_{};
};

} // namespace infinity
