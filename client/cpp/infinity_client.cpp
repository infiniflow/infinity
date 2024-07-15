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

#include "infinity_client.h"

namespace infinity::client {

/// TODO: comment
Client Client::Connect(const std::string &ip_address, uint16_t port) {
    std::shared_ptr<TSocket> socket = std::make_shared<TSocket>(ip_address, port);
    std::shared_ptr<TBufferedTransport> transport = std::make_shared<TBufferedTransport>(socket);
    std::shared_ptr<TBinaryProtocol> protocol = std::make_shared<TBinaryProtocol>(transport);
    std::unique_ptr<InfinityServiceClient> client = std::make_unique<InfinityServiceClient>(protocol);
    transport->open();
    CommonResponse response;
    ConnectRequest request;
    request.__set_client_version(10); // 0.2.1.dev6
    client->Connect(response, request);
    return {socket, transport, protocol, std::move(client), response.session_id};
}

/// TODO: comment
ClientStatus Client::Disconnect() {
    CommonRequest request;
    CommonResponse response;

    request.session_id = session_id_;
    client_->Disconnect(response, request);
    transport_->close();
    return ClientStatus::OK();
}

/// TODO: comment
ClientStatus Client::CreateDatabase(const std::string &db_name, CreateOption create_option) {
    CreateDatabaseRequest request;
    CommonResponse response;

    switch (create_option) {
        case CreateOption::kErrorIfExists: {
            request.create_option.conflict_type = infinity_thrift_rpc::CreateConflict::type::Error;
            break;
        }
        case CreateOption::kIgnoreIfExists: {
            request.create_option.conflict_type = infinity_thrift_rpc::CreateConflict::type::Ignore;
            break;
        }
        case CreateOption::kReplaceIfExists: {
            request.create_option.conflict_type = infinity_thrift_rpc::CreateConflict::type::Replace;
            break;
        }
        default: {
            return ClientStatus{ClientErrorCode::kInvalidConflictType, "invalid conflict type"};
        }
    }

    request.db_name = db_name;
    request.session_id = session_id_;

    client_->CreateDatabase(response, request);
    return ClientStatus{static_cast<ClientErrorCode>(response.error_code), response.error_msg};
}

/// TODO: comment
ClientStatus Client::DropDatabase(const std::string &db_name, DropOption drop_option) {
    DropDatabaseRequest request;
    CommonResponse response;

    switch (drop_option) {
        case DropOption::kErrorIfNotExists: {
            request.drop_option.conflict_type = infinity_thrift_rpc::DropConflict::type::Error;
            break;
        }
        case DropOption::kIgnoreIfNotExists: {
            request.drop_option.conflict_type = infinity_thrift_rpc::DropConflict::type::Ignore;
            break;
        }
        default: {
            return ClientStatus{ClientErrorCode::kInvalidConflictType, "invalid conflict type"};
        }
    }

    request.db_name = db_name;
    request.session_id = session_id_;

    client_->DropDatabase(response, request);
    return ClientStatus{static_cast<ClientErrorCode>(response.error_code), response.error_msg};
}

/// TODO: comment
std::vector<std::string> Client::ListDatabases() const {
    ListDatabaseRequest request;
    ListDatabaseResponse response;

    request.session_id = session_id_;

    client_->ListDatabase(response, request);
    return response.db_names;
}

/// Describe database
DatabaseInfo Client::ShowDatabase(const std::string &db_name) const { return DatabaseInfo{}; }

/// Create table
ClientStatus Client::CreateTable(const std::string &table_name,
                                 std::vector<ColumnDefinition> &columns,
                                 const TableProperties &table_properties,
                                 CreateOption create_option) {
    return ClientStatus::OK();
}

/// Drop table
ClientStatus Client::DropTable(const std::string &table_name, DropOption drop_option) { return ClientStatus::OK(); }

/// List tables
[[nodiscard]] std::vector<std::string> Client::ListTables(const std::string &database_name) const {
    ListTableRequest request;
    ListTableResponse response;


    return std::vector<std::string>();
}

/// Show table
TableInfo Client::ShowTable(const std::string &db_name, const std::string &table_name) const {
    return {};
}

} // namespace infinity::client
