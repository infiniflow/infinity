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
#include <bits/stl_algo.h>
module http_server;

import infinity;
import stl;
import third_party;
import defer_op;
import data_block;
import data_table;
import data_type;
import value;
import infinity_exception;
import logger;
import query_result;
import query_options;
import column_vector;
import infinity_context;
import query_context;
import column_def;

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

        // get database name
        String database_name = request->getPathVariable("database_name");

        // get create option
        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);
        String option = body_info_json["create_option"];
        CreateDatabaseOptions create_option;

        // create database
        auto result = infinity->CreateDatabase(database_name, create_option);

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

        // get database name
        String database_name = request->getPathVariable("database_name");

        // get drop option
        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);
        String option = body_info_json["drop_option"];
        DropDatabaseOptions drop_option;

        auto result = infinity->DropDatabase(database_name,drop_option);

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

class ShowDatabaseHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto database_name = request->getPathVariable("database_name");
        auto result = infinity->ShowDatabase(database_name);

        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;
         if(result.IsOk()) {
            SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
            auto row_count = data_block->row_count();
            auto column_cnt = result.result_table_->ColumnCount();

            for (int row = 0; row < row_count; ++row) {
                nlohmann::json json_database;
                for (infinity::SizeT col = 0; col < column_cnt; ++col){
                    Value value = data_block->GetValue(col, row);
                    const String &column_name = result.result_table_->GetColumnNameById(col);
                    const String &column_value = value.ToString();
                    json_database[column_name] = column_value;
                }
                json_res["res"].push_back(json_database);
            }
            for (auto &element : json_res["res"]) {
                String name = element["name"].dump();
                std::replace(name.begin(), name.end(), ' ', '_');
                nlohmann::json json_name = nlohmann::json::parse(name);
                json_response[json_name] = element["value"];
            }
            http_status = HTTPStatus::CODE_200;
        } else {
            json_response["error_code"] = result.ErrorCode();
            json_response["error_message"] = result.ErrorMsg();
            http_status = HTTPStatus::CODE_500;
        }
        return ResponseFactory::createResponse(http_status, json_res.dump());
    }
};

class CreateTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");

        String body_info = request->readBodyToString();
        nlohmann::json body_info_json = nlohmann::json::parse(body_info);

        auto fields = body_info_json["fields"];
        auto properties = body_info_json["properties"];

        Vector<ColumnDef *> column_definitions;
        int id = 0;
        nlohmann::json json_response;

        for (auto &field : fields) {
            for (auto& filed_element : field.items()){
                String column_name = filed_element.key();
                auto values = filed_element.value();
                SharedPtr<DataType> column_type = DataType::StringDeserialize(values["type"]);
                HashSet<ConstraintType> constraints;
                for (auto &constraint_json : values["constraints"]){
                    String constraint = constraint_json;
                    constraints.insert(ColumnDef::StringToConstraintType(constraint));
                }
                ColumnDef *col_def = new ColumnDef(id ++, column_type, column_name, constraints);
                column_definitions.emplace_back(col_def);
            }
        }
        Vector<TableConstraint *> table_constraint;
        CreateTableOptions create_table_opts;

        auto result = infinity->CreateTable(database_name, table_name, column_definitions, table_constraint, create_table_opts);

        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;

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

class DropTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");
        DropTableOptions drop_table_opts;
        auto result = infinity->DropTable(database_name, table_name, drop_table_opts);

        HTTPStatus http_status;
        http_status = HTTPStatus::CODE_200;
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

class ListTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        auto result = infinity->ShowTables(database_name);
        // Map<String, String, int, int, int, int, >
        nlohmann::json json_response;
        HTTPStatus http_status;
        if(result.IsOk()) {
            SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
            auto row_count = data_block->row_count();
            auto column_cnt = result.result_table_->ColumnCount();
            for (int row = 0; row < row_count; ++row) {
                nlohmann::json json_table;
                for (infinity::SizeT col = 0; col < column_cnt; ++col){
                    const String &column_name = result.result_table_->GetColumnNameById(col);
                    Value value = data_block->GetValue(col, row);
                    const String &column_value = value.ToString();
                    json_table[column_name] = column_value;
                }
                json_table.erase("database");
                json_response["tables"].push_back(json_table);
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

class ShowTableHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        String database_name = request->getPathVariable("database_name");
        String table_name = request->getPathVariable("table_name");
        
        auto result = infinity->ShowTable(database_name, table_name);
        nlohmann::json json_response;
        nlohmann::json json_res;
        HTTPStatus http_status;
        if(result.IsOk()) {
            SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
            auto row_count = data_block->row_count();
            auto column_cnt = result.result_table_->ColumnCount();
            for (int row = 0; row < row_count; ++row) {
                nlohmann::json json_table;
                for (infinity::SizeT col = 0; col < column_cnt; ++col){
                    const String &column_name = result.result_table_->GetColumnNameById(col);
                    Value value = data_block->GetValue(col, row);
                    const String &column_value = value.ToString();
                    json_table[column_name] = column_value;
                }
                // json_table.erase("database");
                json_res["tables"].push_back(json_table);
            }
            for (auto &element : json_res["tables"]) {
                String name = element["name"].dump();
                std::replace(name.begin(), name.end(), ' ', '_');
                nlohmann::json json_name = nlohmann::json::parse(name);
                json_response[json_name] = element["value"];
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

class ListTableIndexesHandler final : public HttpRequestHandler {
public:
    SharedPtr<OutgoingResponse> handle(const SharedPtr<IncomingRequest> &request) final {
        auto infinity = Infinity::RemoteConnect();
        DeferFn defer_fn([&]() { infinity->RemoteDisconnect(); });

        auto db_name = request->getPathVariable("db_name");
        auto table_name = request->getPathVariable("table_name");
        auto result = infinity->ListTableIndexes(db_name, table_name);

        HTTPStatus http_status;
        nlohmann::json json_response;
        json_response["db_name"] = db_name;
        json_response["table_name"] = table_name;

        if(result.IsOk()) {
            SharedPtr<DataBlock> data_block = result.result_table_->GetDataBlockById(0);
            auto row_count = data_block->row_count();
            auto column_cnt = result.result_table_->ColumnCount();

            for (int row = 0; row < row_count; ++row) {
                nlohmann::json json_index;
                for (infinity::SizeT col = 0; col < column_cnt; ++col){
                    const String &column_name = result.result_table_->GetColumnNameById(col);
                    Value value = data_block->GetValue(col, row);
                    const String &column_value = value.ToString();
                    json_index[column_name] = column_value;
                }
                json_response["indexes"].push_back(json_index);
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

}

namespace infinity {

void HTTPServer::Start(u16 port) {

    WebEnvironment::init();

    SharedPtr<HttpRouter> router = HttpRouter::createShared();
    router->route("GET", "/hello", MakeShared<HttpHandler>());

    // database
    router->route("GET", "/databases", MakeShared<ListDatabaseHandler>());
    router->route("POST", "/databases/{database_name}", MakeShared<CreateDatabaseHandler>());
    router->route("DELETE", "/databases/{database_name}", MakeShared<DropDatabaseHandler>());
    router->route("GET", "/databases/{database_name}", MakeShared<ShowDatabaseHandler>());

    // tables
    router->route("GET", "/databases/{database_name}/tables", MakeShared<ListTableHandler>());
    router->route("POST", "/databases/{database_name}/tables/{table_name}", MakeShared<CreateTableHandler>());
    router->route("DELETE", "/databases/{database_name}/tables/{table_name}", MakeShared<DropTableHandler>());
    router->route("GET", "/databases/{database_name}/tables/{table_name}", MakeShared<ShowTableHandler>());
    

    // index
    router->route("GET", "/indexes/{db_name}/{table_name}", MakeShared<ListTableIndexesHandler>());

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
