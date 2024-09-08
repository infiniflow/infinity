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
    static SharedPtr<PeerClient> Connect(const String &ip_address, i64 port);

    ~PeerClient();
    PeerClient(SharedPtr<TTransport> socket, SharedPtr<TTransport> transport, SharedPtr<TProtocol> protocol, UniquePtr<PeerServiceClient> client)
        : socket_(socket), transport_(transport), protocol_(protocol), client_(std::move(client)) {}

    Status Init();
    Status UnInit();
    void Send(PeerTask* task);

private:
    SharedPtr<TTransport> socket_;
    SharedPtr<TTransport> transport_;
    SharedPtr<TProtocol> protocol_;
    UniquePtr<PeerServiceClient> client_;
    bool running_;
    BlockingQueue<PeerTask*> task_queue_;
};

} // namespace infinity
