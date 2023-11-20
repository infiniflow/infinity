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

#include "thrift_server.h"

#include <thrift/TToString.h>
#include <thrift/concurrency/ThreadFactory.h>
#include <thrift/concurrency/ThreadManager.h>
#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TTransportUtils.h>

#include <iostream>
#include <memory>
#include <stdexcept>

#include "infinity_thrift/InfinityService.h"
#include "infinity_thrift/infinity_types.h"

import infinity;
import stl;
import infinity_exception;
import logger;
import query_result;
import third_party;

using namespace ::infinity_thrift_rpc;

namespace infinity {

class InfinityServiceHandler : virtual public InfinityServiceIf {
public:
    InfinityServiceHandler() = default;

    void Connect(CommonResponse &response) {
        // Your implementation goes here
        printf("Connect\n");
    }

    void Disconnect(CommonResponse &response, const CommonRequest &request) {
        // Your implementation goes here
        printf("Disconnect\n");
    }

    void CreateDatabase(CommonResponse &response, const CreateDatabaseRequest &request) {
        // Your implementation goes here
        printf("CreateDatabase\n");
    }

    void DropDatabase(CommonResponse &response, const DropDatabaseRequest &request) {
        // Your implementation goes here
        printf("DropDatabase\n");
    }

    void CreateTable(CommonResponse &response, const CreateTableRequest &request) {
        // Your implementation goes here
        printf("CreateTable\n");
    }

    void DropTable(CommonResponse &response, const DropTableRequest &request) {
        // Your implementation goes here
        printf("DropTable\n");
    }

    void Insert(CommonResponse &response, const InsertRequest &request) {
        // Your implementation goes here
        printf("Insert\n");
    }

    void Import(CommonResponse &response, const ImportRequest &request) {
        // Your implementation goes here
        printf("Import\n");
    }

    void Select(SelectResponse &response, const SelectRequest &request) {
        // Your implementation goes here
        printf("Select\n");
    }

    void ListDatabase(ListDatabaseResponse &response, const ListDatabaseRequest &request) {
        // Your implementation goes here
        printf("ListDatabase\n");
    }

    void ListTable(ListTableResponse &response, const ListTableRequest &request) {
        // Your implementation goes here
        printf("ListTable\n");
    }

    void DescribeDatabase(DescribeDatabaseResponse &response, const DescribeDatabaseRequest &request) {
        // Your implementation goes here
        printf("DescribeDatabase\n");
    }

    void DescribeTable(DescribeTableResponse &response, const DescribeTableRequest &request) {
        // Your implementation goes here
        printf("DescribeTable\n");
    }

    void GetDatabase(CommonResponse &response, const GetDatabaseRequest &request) {
        // Your implementation goes here
        printf("GetDatabase\n");
    }

    void GetTable(CommonResponse &response, const GetTableRequest &request) {
        // Your implementation goes here
        printf("GetTable\n");
    }

    void CreateIndex(CommonResponse &response, const CreateIndexRequest &request) {
        auto infinity = GetInfinityBySessionID(request.session_id);
        auto database = infinity->GetDatabase(request.db_name);
        auto table = database->GetTable(request.table_name);
        auto column_names = new Vector<String>();

        column_names->reserve(request.column_names.size());
        for (auto &column_name : request.column_names) {
            column_names->emplace_back(column_name);
        }

        String method_type = request.method_type;
        auto *index_para_list = new Vector<InitParameter *>();

        for (auto &index_para : request.index_para_list) {
            auto init_parameter = new InitParameter();
            init_parameter->para_name_ = index_para.para_name;
            init_parameter->para_value_ = index_para.para_value;
            index_para_list->emplace_back(init_parameter);
        }

        auto result = table->CreateIndex(request.index_name, column_names, method_type, index_para_list, (CreateIndexOptions &)request.option);
        if (result.IsOk()) {
            response.__set_success(true);
        } else {
            response.__set_success(false);
            response.__set_error_msg(result.ErrorStr());
        }
    }

    void DropIndex(CommonResponse &response, const DropIndexRequest &request) {
        auto infinity = GetInfinityBySessionID(request.session_id);
        auto database = infinity->GetDatabase(request.db_name);
        auto table = database->GetTable(request.table_name);
        QueryResult result = table->DropIndex(request.index_name);

        if (result.IsOk()) {
            response.__set_success(true);
        } else {
            response.__set_success(false);
            response.__set_error_msg(result.ErrorStr());
        }
    }

private:
    Mutex infinity_session_map_mutex_{};
    HashMap<u64, SharedPtr<Infinity>> infinity_session_map_{};

    SharedPtr<Infinity> GetInfinityBySessionID(i64 session_id) {
        auto it = infinity_session_map_.find(session_id);
        if (it == infinity_session_map_.end()) {
            Error<NetworkException>("session id not found", __FILE_NAME__, __LINE__);
        }
        return it->second;
    }
};

class InfinityServiceCloneFactory : virtual public InfinityServiceIfFactory {
public:
    ~InfinityServiceCloneFactory() override = default;
    InfinityServiceIf *getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
        SharedPtr<TSocket> sock = std::dynamic_pointer_cast<TSocket>(connInfo.transport);
        LOG_TRACE(Format("Incoming connection, SocketInfo: {}, PeerHost: {}, PeerAddress: {}, PeerPort: {}",
                         sock->getSocketInfo(),
                         sock->getPeerHost(),
                         sock->getPeerAddress(),
                         sock->getPeerPort()));
        return new InfinityServiceHandler;
    }
    void releaseHandler(InfinityServiceIf *handler) override { delete handler; }
};

// Thrift server

void ThreadedThriftServer::Init(i32 port_no) {

    server = MakeUnique<TThreadedServer>(MakeShared<InfinityServiceProcessorFactory>(MakeShared<InfinityServiceCloneFactory>()),
                                         MakeShared<TServerSocket>(port_no), // port
                                         MakeShared<TBufferedTransportFactory>(),
                                         MakeShared<TBinaryProtocolFactory>());
}

void ThreadedThriftServer::Start() { server->serve(); }

void ThreadedThriftServer::Shutdown() { server->stop(); }

void PoolThriftServer::Init(i32 port_no, i32 pool_size) {

    SharedPtr<ThreadFactory> threadFactory = MakeShared<ThreadFactory>();

    SharedPtr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    server = MakeUnique<TThreadPoolServer>(MakeShared<InfinityServiceProcessorFactory>(MakeShared<InfinityServiceCloneFactory>()),
                                           MakeShared<TServerSocket>(port_no),
                                           MakeShared<TBufferedTransportFactory>(),
                                           MakeShared<TBinaryProtocolFactory>(),
                                           threadManager);
}

void PoolThriftServer::Start() { server->serve(); }

void PoolThriftServer::Shutdown() { server->stop(); }

} // namespace infinity