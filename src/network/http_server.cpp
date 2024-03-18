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

// #include "oatpp/core/Types.hpp"
// #include "oatpp/core/macro/codegen.hpp"
// #include "oatpp/parser/json/mapping/ObjectMapper.hpp"

// #include OATPP_CODEGEN_BEGIN(DTO) ///< Begin DTO codegen section

// class DatabaseDto : public oatpp::DTO {

//   DTO_INIT(DatabaseDto, DTO)

//   DTO_FIELD(String, database_name);
//   DTO_FIELD(infinity::CreateDatabaseOptions, options);

// };

// #include OATPP_CODEGEN_END(DTO) ///< End DTO codegen section

module http_server;

import infinity;
import third_party;
import defer_op;
import data_block;
import value;
import query_options;


namespace {

using namespace infinity;

class HttpHandler : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) override {
        return ResponseFactory::createResponse(Status::CODE_200, "Hello World!");
    }
};

class ListDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto result = infinity->ListDatabases();
        nlohmann::json json_response;
        HTTPStatus http_status;
        if(result.IsOk()) {

            SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
            auto row_count = data_block->row_count();
            for (int i = 0; i < row_count; ++i) {
                Value value = data_block->GetValue(0, i);
                const String &db_name = value.GetVarchar();
                json_response["databases"].push_back(db_name);
            }

            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class CreateDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String body_info = request->readBodyToString();
        
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);
        String database_name = body_info_json["database_name"];
        CreateDatabaseOptions options;
        auto result = infinity->CreateDatabase(database_name, options);
        
        HTTPStatus http_status;
        nlohmann::json json_response;

        if(result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class DropDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });
        
        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);
        String database_name = body_info_json["database_name"];
        DropDatabaseOptions options;
        auto result = infinity->DropDatabase(database_name,options);

        nlohmann::json json_response;
        HTTPStatus http_status;

         if(result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

class RetrieveDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto db_name = request->getPathVariable("db_name");
        auto result = infinity->GetDatabase(db_name);

        nlohmann::json json_response;
        HTTPStatus http_status;

         if(result.IsOk()) {
            json_response["error_code"] = 0;
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_response.dump());
    }
};

}

namespace infinity {

void HTTPServer::Start(u16 port) {

    WebEnvironment::init();

    SharedPtr<HttpRouter> router = HttpRouter::createShared();
    router->route("GET", "/hello", MakeShared<HttpHandler>());
    router->route("GET", "/databases", MakeShared<ListDatabaseHandler>());
    router->route("POST", "/databases", MakeShared<CreateDatabaseHandler>());
    router->route("DELETE", "/databases", MakeShared<DropDatabaseHandler>());
    router->route("GET", "/databases/{db_name}", MakeShared<RetrieveDatabaseHandler>());

    SharedPtr<HttpConnectionProvider> connection_provider = HttpConnectionProvider::createShared({"localhost", port, WebAddress::IP_4});
    SharedPtr<HttpConnectionHandler> connection_handler =  HttpConnectionHandler::createShared(router);

    server_ = MakeShared<WebServer>(connection_provider, connection_handler);

    fmt::print("HTTP server listen on port: {}\n", port);

    server_->run();

}

void HTTPServer::Shutdown() {

    server_->stop();
    WebEnvironment::destroy();

}

} // namespace infinity
