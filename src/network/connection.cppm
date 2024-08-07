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

export module connection;

import boost;
import stl;
import session;
import pg_protocol_handler;
import query_context;
import data_table;
import query_result;

namespace infinity {

export class Connection {
public:
    explicit Connection(boost::asio::io_service &io_service);

    ~Connection();

    void Run();

    inline SharedPtr<boost::asio::ip::tcp::socket> socket() { return socket_; }

    inline void GetClientInfo(String &ip_address, u16 &port) {
        if (session_.get() != nullptr) {
            session_->GetClientInfo(ip_address, port);
        } else {
            ip_address = "";
            port = 0;
        }
    }

private:
    void HandleConnection();

    void HandleRequest();

    void HandlerSimpleQuery(QueryContext *query_context);

    void SendTableDescription(const SharedPtr<DataTable> &result_table);

    void SendQueryResponse(const QueryResult &query_result);

    void HandleError(const char* error_message);

private:
    const SharedPtr<boost::asio::ip::tcp::socket> socket_{};

    const SharedPtr<PGProtocolHandler> pg_handler_{};

    bool terminate_connection_ = false;

    SharedPtr<RemoteSession> session_{};
};

} // namespace infinity
