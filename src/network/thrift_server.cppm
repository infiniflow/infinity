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

export module infinity_core:thrift_server;

import :infinity_thrift_service;

import third_party;

using namespace std;

namespace infinity {

enum class ThriftServerStatus : u8 {
    kStopped,
    kRunning,
    kStopping,
};

#define THRIFT_SERVER_TYPE 0

#if THRIFT_SERVER_TYPE == 2

export class ThreadedThriftServer {
public:
    void Init(const std::string &server_address, i32 port_no);
    void Start();
    void Shutdown();

private:
    std::unique_ptr<apache::thrift::server::TThreadedServer> server{nullptr};

    atomic_bool started_{false};
};

#elif THRIFT_SERVER_TYPE == 0

export class PoolThriftServer {
public:
    void Init(const std::string &server_address, i32 port_no, i32 pool_size);
    std::thread Start();

    void Shutdown();

private:
    std::unique_ptr<apache::thrift::server::TServer> server{nullptr};

    bool initialized_{false};
    std::atomic<ThriftServerStatus> status_ = ThriftServerStatus::kStopped;
};

#elif THRIFT_SERVER_TYPE == 1

export class NonBlockPoolThriftServer {
public:
    void Init(const std::string &server_address, i32 port_no, i32 pool_size);
    void Start();
    void Shutdown();

private:
    //    std::unique_ptr<TServer> server{nullptr};
    std::shared_ptr<InfinityThriftService> service_handler_{};
    std::shared_ptr<apache::thrift::concurrency::std::thread> server_thread_{};

    atomic_bool started_{false};
};

#endif

} // namespace infinity
