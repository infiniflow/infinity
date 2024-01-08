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

#include <sstream>
#include <stdexcept>

import stl;
import infinity;
import database;
import table;
import parser;
import query_options;

using namespace std;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace apache::thrift::server;

namespace infinity {

class ThreadedThriftServer {
public:
    void Init(int32_t port_no);
    void Start();
    void Shutdown();

private:
    std::unique_ptr<TThreadedServer> server{nullptr};
};

class PoolThriftServer {
public:
    void Init(int32_t port_no, int32_t pool_size);
    void Start();
    void Shutdown();

private:
    std::unique_ptr<TServer> server{nullptr};
};

class InfinityServiceHandler;

class NonBlockPoolThriftServer {
public:
    void Init(int32_t port_no, int32_t pool_size);
    void Start();
    void Shutdown();

private:
//    std::unique_ptr<TServer> server{nullptr};
    std::shared_ptr<InfinityServiceHandler> service_handler_{};
    std::shared_ptr<apache::thrift::concurrency::Thread> server_thread_{};
};

} // namespace infinity
