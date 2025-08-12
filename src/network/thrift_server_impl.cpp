// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
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
#include <thrift/transport/TTransportUtils.h>

module infinity_core:thrift_server.impl;

import :thrift_server;
import :infinity_thrift_service;
import :infinity_thrift_types;
import :logger;
import :stl;
import :infinity_exception;

import third_party;

using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

namespace infinity {

class InfinityServiceCloneFactory final : public infinity_thrift_rpc::InfinityServiceIfFactory {
public:
    ~InfinityServiceCloneFactory() final = default;

    infinity_thrift_rpc::InfinityServiceIf *getHandler(const ::apache::thrift::TConnectionInfo &connInfo) final { return new InfinityThriftService; }

    void releaseHandler(infinity_thrift_rpc::InfinityServiceIf *handler) final { delete handler; }
};

// Thrift server
#define THRIFT_SERVER_TYPE 0

#if THRIFT_SERVER_TYPE == 2

void ThreadedThriftServer::Init(const String &server_address, i32 port_no) {

    fmt::print("API server listen on {}: {}\n", server_address, port_no);
    //    std::cout << "API server listen on: " << server_address << ": " << port_no << std::endl;
    SharedPtr<TBinaryProtocolFactory> binary_protocol_factory = MakeShared<TBinaryProtocolFactory>();
    binary_protocol_factory->setStrict(true, true);
    server = MakeUnique<TThreadedServer>(MakeShared<infinity_thrift_rpc::InfinityServiceProcessorFactory>(MakeShared<InfinityServiceCloneFactory>()),
                                         MakeShared<TServerSocket>(server_address, port_no),
                                         MakeShared<TBufferedTransportFactory>(),
                                         binary_protocol_factory);
}

void ThreadedThriftServer::Start() {
    if (started_) {
        return;
    }
    started_ = true;
    server->serve();
}

void ThreadedThriftServer::Shutdown() {
    if (started_) {
        server->stop();
        started_ = false;
    }
}

#elif THRIFT_SERVER_TYPE == 0

void PoolThriftServer::Init(const String &server_address, i32 port_no, i32 pool_size) {

    SharedPtr<TServerSocket> server_socket = MakeShared<TServerSocket>(server_address, port_no);

    SharedPtr<TBinaryProtocolFactory> protocol_factory = MakeShared<TBinaryProtocolFactory>();
    //    SharedPtr<TCompactProtocolFactory> protocol_factory = MakeShared<TCompactProtocolFactory>();

    SharedPtr<ThreadFactory> threadFactory = MakeShared<ThreadFactory>();

    SharedPtr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    fmt::print("API server(for Infinity-SDK) listen on {}: {}, connection limit: {}\n", server_address, port_no, pool_size);
    //    std::cout << "API server listen on: " << server_address << ": " << port_no << ", thread pool: " << pool_size << std::endl;

    server =
        MakeUnique<TThreadPoolServer>(MakeShared<infinity_thrift_rpc::InfinityServiceProcessorFactory>(MakeShared<InfinityServiceCloneFactory>()),
                                      server_socket,
                                      MakeShared<TBufferedTransportFactory>(),
                                      protocol_factory,
                                      threadManager);

    initialized_ = true;
}

Thread PoolThriftServer::Start() {
    if (!initialized_) {
        UnrecoverableError("Thrift server is not initialized");
    }
    {
        auto expect = ThriftServerStatus::kStopped;
        if (!status_.compare_exchange_strong(expect, ThriftServerStatus::kRunning)) {
            UnrecoverableError(fmt::format("Thrift server in unexpected state: {}", u8(expect)));
        }
    }
    return Thread([this] {
        server->serve();

        status_.store(ThriftServerStatus::kStopped);
        status_.notify_one();
    });
}

void PoolThriftServer::Shutdown() {
    {
        auto expected = ThriftServerStatus::kRunning;
        if (!status_.compare_exchange_strong(expected, ThriftServerStatus::kStopping)) {
            if (status_ == ThriftServerStatus::kStopped) {
                return;
            } else {
                UnrecoverableError(fmt::format("Thrift server in unexpected state: {}", u8(expected)));
            }
        }
    }
    server->stop();

    status_.wait(ThriftServerStatus::kStopping);
}

#elif THRIFT_SERVER_TYPE == 1

void NonBlockPoolThriftServer::Init(const String &server_address, i32 port_no, i32 pool_size) {

    SharedPtr<ThreadFactory> thread_factory = MakeShared<ThreadFactory>();
    service_handler_ = MakeShared<InfinityThriftService>();
    SharedPtr<infinity_thrift_rpc::InfinityServiceProcessor> service_processor =
        MakeShared<infinity_thrift_rpc::InfinityServiceProcessor>(service_handler_);
    SharedPtr<TProtocolFactory> protocol_factory = MakeShared<TBinaryProtocolFactory>();

    SharedPtr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
    threadManager->threadFactory(thread_factory);
    threadManager->start();

    fmt::print("Non-block API server listen on {}: {}, connection limit: {}\n", server_address, port_no, pool_size);
    //    std::cout << "Non-block API server listen on: " << server_address << ": " << port_no << ", thread pool: " << pool_size << std::endl;

    SharedPtr<TNonblockingServerSocket> non_block_socket = MakeShared<TNonblockingServerSocket>(server_address, port_no);

    //    server_thread_ = thread_factory->newThread(std::shared_ptr<TServer>(
    //        new TNonblockingServer(serviceProcessor, protocolFactory, nbSocket1, threadManager)));

    server_thread_ = thread_factory->newThread(MakeShared<TNonblockingServer>(service_processor, protocol_factory, non_block_socket, threadManager));

    //    server = MakeUnique<TThreadPoolServer>(MakeShared<InfinityServiceProcessorFactory>(MakeShared<InfinityServiceCloneFactory>()),
    //                                           MakeShared<TServerSocket>(port_no),
    //                                           MakeShared<TBufferedTransportFactory>(),
    //                                           MakeShared<TBinaryProtocolFactory>(),
    //                                           threadManager);
}

void NonBlockPoolThriftServer::Start() {
    if (started_) {
        return;
    }
    started_ = true;
    server_thread_->start();
}

void NonBlockPoolThriftServer::Shutdown() {
    if (started_) {
        server_thread_->join();
        started_ = false;
    }
}

#endif

} // namespace infinity
