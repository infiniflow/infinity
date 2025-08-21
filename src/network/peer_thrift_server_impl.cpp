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

module infinity_core:peer_thrift_server.impl;

import :peer_thrift_server;
import :peer_server_thrift_service;
import :peer_server_thrift_types;
import :infinity_exception;

import std;
import third_party;

namespace infinity {

class PeerServiceCloneFactory final : public infinity_peer_server::PeerServiceIfFactory {
public:
    ~PeerServiceCloneFactory() final = default;

    infinity_peer_server::PeerServiceIf *getHandler(const apache::thrift::TConnectionInfo &connInfo) final { return new PeerServerThriftService; }

    void releaseHandler(infinity_peer_server::PeerServiceIf *handler) final { delete handler; }
};

void PoolPeerThriftServer::Init(const std::string &server_address, i32 port_no, i32 pool_size) {

    std::shared_ptr<apache::thrift::transport::TServerSocket> server_socket =
        std::make_shared<apache::thrift::transport::TServerSocket>(server_address, port_no);

    std::shared_ptr<apache::thrift::protocol::TBinaryProtocolFactory> protocol_factory =
        std::make_shared<apache::thrift::protocol::TBinaryProtocolFactory>();

    std::shared_ptr<apache::thrift::concurrency::ThreadFactory> threadFactory = std::make_shared<apache::thrift::concurrency::ThreadFactory>();

    std::shared_ptr<apache::thrift::concurrency::ThreadManager> threadManager =
        apache::thrift::concurrency::ThreadManager::newSimpleThreadManager(pool_size);
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    fmt::print("Peer server listen on {}: {}, connection limit: {}\n", server_address, port_no, pool_size);

    server = std::make_unique<apache::thrift::server::TThreadPoolServer>(
        std::make_shared<infinity_peer_server::PeerServiceProcessorFactory>(std::make_shared<PeerServiceCloneFactory>()),
        server_socket,
        std::make_shared<apache::thrift::transport::TBufferedTransportFactory>(),
        protocol_factory,
        threadManager);
    initialized_ = true;
}

std::thread PoolPeerThriftServer::Start() {
    if (!initialized_) {
        UnrecoverableError("Peer server is not initialized.");
    }
    {
        auto expect = PeerThriftServerStatus::kStopped;
        if (!status_.compare_exchange_strong(expect, PeerThriftServerStatus::kRunning)) {
            UnrecoverableError(fmt::format("Peer server in unexpected state: {}", u8(expect)));
        }
    }
    return std::thread([this] {
        server->serve();

        status_.store(PeerThriftServerStatus::kStopped);
        status_.notify_one();
    });
}

void PoolPeerThriftServer::Shutdown() {
    {
        auto expected = PeerThriftServerStatus::kRunning;
        if (!status_.compare_exchange_strong(expected, PeerThriftServerStatus::kStopping)) {
            if (status_ == PeerThriftServerStatus::kStopped) {
                return;
            } else {
                UnrecoverableError(fmt::format("Peer server in unexpected state: {}", u8(expected)));
            }
        }
    }
    server->stop();

    status_.wait(PeerThriftServerStatus::kStopping);
}

} // namespace infinity
