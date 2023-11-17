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

#pragma once

// #include <thrift/concurrency/ThreadManager.h>
// #include <thrift/concurrency/ThreadFactory.h>
// #include <thrift/protocol/TBinaryProtocol.h>
// #include <thrift/server/TSimpleServer.h>
// #include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
// #include <thrift/transport/TServerSocket.h>
// #include <thrift/transport/TSocket.h>
// #include <thrift/transport/TTransportUtils.h>
// #include <thrift/TToString.h>

#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

namespace infinity {

enum class ThriftServerType {
    kThreaded,
    kThreadPool,
};

class ThriftServer {
public:
    explicit ThriftServer(ThriftServerType type) : server_type_(type) {}
    virtual ~ThriftServer() = default;

    virtual void Init() = 0;
    virtual void Start() = 0;
    virtual void Shutdown() = 0;
    [[nodiscard]] ThriftServerType server_type() const { return server_type_; }

private:
    ThriftServerType server_type_{ThriftServerType::kThreaded};
};

class ThreadedThriftServer final : public ThriftServer {
public:
    ThreadedThriftServer() : ThriftServer(ThriftServerType::kThreaded) {}
    ~ThreadedThriftServer() final = default;

    void Init() final;

    void Start() final;

    void Shutdown() final;

private:
    std::unique_ptr<TThreadedServer> server{nullptr};
    //    std::unique_ptr<TServer> server{nullptr};
    //    std::shared_ptr<apache::thrift::server::TServer> server;
};

class PoolThriftServer final : public ThriftServer {
public:
    PoolThriftServer() : ThriftServer(ThriftServerType::kThreadPool) {}
    ~PoolThriftServer() final = default;

    void Init() final;

    void Start() final;

    void Shutdown() final;

private:
    std::unique_ptr<TServer> server{nullptr};
};

} // namespace infinity
