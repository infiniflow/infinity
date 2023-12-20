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

import stl;
import singleton;
import boost;
import connection;

export module db_server;

namespace infinity {

export struct StartupParameter {
    SharedPtr<String> config_path{};
};

class ThriftServer;

export class DBServer {
public:
    void Run();

    void Shutdown();

private:
    void CreateConnection();

    void StartConnection(SharedPtr<Connection> &connection);

    atomic_bool initialized{false};
    atomic_u64 running_connection_count_{0};
    AsioIOService io_service_{};
    UniquePtr<AsioAcceptor> acceptor_ptr_{};
};

}
