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

export module peer_thrift_client;

import peer_server_thrift_types;
import stl;
import thrift;
import status;
import blocking_queue;
import peer_task;

namespace infinity {

using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace infinity_peer_server;

export class PeerClient {
public:
    PeerClient(const String &ip_addr, i64 port, const String &node_name = {}) : this_node_name_(node_name) {
        node_info_.ip_address_ = ip_addr;
        node_info_.port_ = port;
    }
    ~PeerClient();

    //    void SetPeerNode(NodeRole role, const String& node_name, i64 update_ts) {
    //        node_info_.node_name_ = node_name;
    //        node_info_.last_update_ts_ = update_ts;
    //        node_info_.node_status_ = NodeStatus::kAlive;
    //        node_info_.node_role_ = role;
    //    }
    //

    Status Init();
    Status UnInit();

    Status Reconnect();
    void Send(SharedPtr<PeerTask> task);

    bool ServerConnected() const { return server_connected_; }

private:
    void Process();
    void Register(RegisterPeerTask *peer_task);
    void Unregister(UnregisterPeerTask *peer_task);
    void HeartBeat(HeartBeatPeerTask *peer_task);

private:
    String this_node_name_;
    NodeInfo node_info_;

    // For message transportation
    SharedPtr<TTransport> socket_{};
    SharedPtr<TTransport> transport_{};
    SharedPtr<TProtocol> protocol_{};
    UniquePtr<PeerServiceClient> client_{};
    Atomic<bool> server_connected_{false};

    Atomic<bool> running_{false};
    BlockingQueue<SharedPtr<PeerTask>> peer_task_queue_{};
    SharedPtr<Thread> processor_thread_{};
    Atomic<u64> peer_task_count_{};
};

} // namespace infinity
