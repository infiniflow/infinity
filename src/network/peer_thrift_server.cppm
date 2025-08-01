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

export module infinity_core:peer_thrift_server;

import :stl;
import :thrift;

namespace infinity {

enum class PeerThriftServerStatus : u8 {
    kStopped,
    kRunning,
    kStopping,
};

export class PoolPeerThriftServer {
public:
    void Init(const String &server_address, i32 port_no, i32 pool_size);
    Thread Start();
    void Shutdown();

private:
    UniquePtr<apache::thrift::server::TServer> server{nullptr};

    bool initialized_{false};
    Atomic<PeerThriftServerStatus> status_{PeerThriftServerStatus::kStopped};
};

} // namespace infinity
