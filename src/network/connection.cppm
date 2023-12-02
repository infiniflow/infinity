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

import boost;
import stl;
import session;
import pg_protocol_handler;
import query_context;
import data_table;
import query_result;

export module connection;

namespace infinity {

enum class ConnectionStatus : char { kIdle, kRunning, kSuspend, kTerminated };

export class Connection {
public:
    explicit Connection(AsioIOService &io_service);

    ~Connection();

    void Run();

    inline SharedPtr<AsioSocket> socket() { return socket_; }

private:
    void HandleConnection();

    void HandleRequest();

    void HandlerSimpleQuery(QueryContext *query_context);

    void SendTableDescription(const SharedPtr<DataTable> &result_table);

    void SendQueryResponse(const QueryResult &query_result);

private:
    const SharedPtr<AsioSocket> socket_{};

    const SharedPtr<PGProtocolHandler> pg_handler_{};

    bool terminate_connection_ = false;

    SharedPtr<RemoteSession> session_{};
};

} // namespace infinity
