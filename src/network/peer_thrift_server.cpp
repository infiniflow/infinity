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

#include <memory>
#include <thrift/TToString.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/protocol/TCompactProtocol.h>
#include <thrift/server/TNonblockingServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TNonblockingServerSocket.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

module peer_thrift_server;

import peer_server_thrift_service;
import peer_server_thrift_types;
import logger;
import third_party;
import stl;

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

namespace infinity {

class PeerServiceCloneFactory final : public infinity_peer_server::PeerServiceIfFactory {
public:
    ~PeerServiceCloneFactory() final = default;

    infinity_peer_server::PeerServiceIf *getHandler(const ::apache::thrift::TConnectionInfo &connInfo) final { return new PeerServerThriftService; }

    void releaseHandler(infinity_peer_server::PeerServiceIf *handler) final { delete handler; }
};

void PoolPeerThriftServer::Init(const String &server_address, i32 port_no, i32 pool_size) {

    SharedPtr<TServerSocket> server_socket = MakeShared<TServerSocket>(server_address, port_no);

    SharedPtr<TBinaryProtocolFactory> protocol_factory = MakeShared<TBinaryProtocolFactory>();

    SharedPtr<ThreadFactory> threadFactory = MakeShared<ThreadFactory>();

    SharedPtr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    fmt::print("Peer server listen on {}: {}, connection limit: {}\n", server_address, port_no, pool_size);

    server = MakeUnique<TThreadPoolServer>(MakeShared<infinity_peer_server::PeerServiceProcessorFactory>(MakeShared<PeerServiceCloneFactory>()),
                                           server_socket,
                                           MakeShared<TBufferedTransportFactory>(),
                                           protocol_factory,
                                           threadManager);
}

void PoolPeerThriftServer::Start() {
    if (started_) {
        return;
    }
    started_ = true;
    server->serve();
}

void PoolPeerThriftServer::Shutdown() {
    if (started_) {
        server->stop();
        started_ = false;
    }
}

} // namespace infinity
