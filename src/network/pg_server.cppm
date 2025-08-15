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

export module infinity_core:pg_server;

import :stl;
import :singleton;
import :boost;
import :connection;

namespace infinity {

enum struct PGServerStatus : u8 {
    kStopped = 0,
    kStarting,
    kRunning,
    kStopping,
};

export class PGServer {
public:
    std::thread Run();

    void Shutdown();

private:
    void CreateConnection();

    void StartConnection(std::shared_ptr<Connection> &connection);

    std::atomic<PGServerStatus> status_{PGServerStatus::kStopped};
    atomic_u64 running_connection_count_{0};
    std::unique_ptr<boost::asio::io_context> io_context_ptr_{};
    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr_{};
};

} // namespace infinity
