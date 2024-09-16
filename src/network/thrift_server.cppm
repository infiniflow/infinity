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

#include <sstream>
#include <stdexcept>

export module thrift_server;

import stl;
import infinity;
import infinity_thrift_service;
import query_options;
import thrift;

using namespace std;
// using namespace apache::thrift;
// using namespace apache::thrift::concurrency;
// using namespace apache::thrift::protocol;
// using namespace apache::thrift::transport;
// using namespace apache::thrift::server;

namespace infinity {

export class ThreadedThriftServer {
public:
    void Init(const String& server_address, i32 port_no);
    void Start();
    void Shutdown();

private:
    UniquePtr<apache::thrift::server::TThreadedServer> server{nullptr};

    atomic_bool started_{false};
};

export class PoolThriftServer {
public:
    void Init(const String& server_address, i32 port_no, i32 pool_size);
    void Start();
    void Shutdown();

private:
    UniquePtr<apache::thrift::server::TServer> server{nullptr};

    atomic_bool started_{false};
};

export class NonBlockPoolThriftServer {
public:
    void Init(const String& server_address, i32 port_no, i32 pool_size);
    void Start();
    void Shutdown();

private:
    //    UniquePtr<TServer> server{nullptr};
    SharedPtr<InfinityThriftService> service_handler_{};
    SharedPtr<apache::thrift::concurrency::Thread> server_thread_{};

    atomic_bool started_{false};
};

} // namespace infinity
