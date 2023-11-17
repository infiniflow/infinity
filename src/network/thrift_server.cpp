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
#include <sstream>
#include <stdexcept>

#include "thrift_cpp/Calculator.h"

using namespace tutorial;
using namespace shared;

class CalculatorHandler : public CalculatorIf {
public:
    CalculatorHandler() = default;

    void ping() override { cout << "ping()" << endl; }

    int32_t add(const int32_t n1, const int32_t n2) override {
        cout << "add(" << n1 << ", " << n2 << ")" << endl;
        return n1 + n2;
    }

    int32_t calculate(const int32_t logid, const Work &work) override {
        cout << "calculate(" << logid << ", " << work << ")" << endl;
        int32_t val;

        switch (work.op) {
            case Operation::ADD:
                val = work.num1 + work.num2;
                break;
            case Operation::SUBTRACT:
                val = work.num1 - work.num2;
                break;
            case Operation::MULTIPLY:
                val = work.num1 * work.num2;
                break;
            case Operation::DIVIDE:
                if (work.num2 == 0) {
                    InvalidOperation io;
                    io.whatOp = work.op;
                    io.why = "Cannot divide by 0";
                    throw io;
                }
                val = work.num1 / work.num2;
                break;
            default:
                InvalidOperation io;
                io.whatOp = work.op;
                io.why = "Invalid Operation";
                throw io;
        }

        SharedStruct ss;
        ss.key = logid;
        ss.value = to_string(val);

        log[logid] = ss;

        return val;
    }

    void getStruct(SharedStruct &ret, const int32_t logid) override {
        cout << "getStruct(" << logid << ")" << endl;
        ret = log[logid];
    }

    void zip() override { cout << "zip()" << endl; }

    void CreateDatabase(tutorial::CommonResponse &_return, const tutorial::CreateDatabaseRequest &req) override {

    }

    void Connect(tutorial::CommonResponse &_return) override {
        auto infinity = infinity::Infinity::RemoteConnect();
        if (infinity == nullptr) {
            _return.success = false;
            _return.error_msg = "error";
        } else {
            infinity_session_map_mutex_.lock();
            infinity_session_map_.emplace(infinity->GetSessionId(), infinity);
            infinity_session_map_mutex_.unlock();
            _return.success = true;
            _return.error_msg = "success";
        }
    }

    void Disconnect(CommonResponse& _return, const CommonRequest& request) {
        // Your implementation goes here
        printf("Disconnect\n");
    }



    void DropDatabase(CommonResponse& _return, const DropDatabaseRequest& request) {
        // Your implementation goes here
        printf("DropDatabase\n");
    }

//    void Disconnect(CommonResponse& _return, const CommonRequest& request) {
//        // Your implementation goes here
//        printf("Disconnect\n");
//    }


//
//    void DropDatabase(CommonResponse& _return, const DropDatabaseRequest& request) {
//        // Your implementation goes here
//        printf("DropDatabase\n");
//    }
//
    void CreateTable(CommonResponse& _return, const CreateTableRequest& request) {
        // Your implementation goes here
        printf("CreateTable\n");
    }
//
//    void DropTable(CommonResponse& _return, const DropTableRequest& request) {
//        // Your implementation goes here
//        printf("DropTable\n");
//    }
//
//    void Insert(CommonResponse& _return, const InsertRequest& request) {
//        // Your implementation goes here
//        printf("Insert\n");
//    }
//
//    void Import(CommonResponse& _return, const ImportRequest& request) {
//        // Your implementation goes here
//        printf("Import\n");
//    }
//
//    void Select(SelectResponse& _return, const SelectRequest& request) {
//        // Your implementation goes here
//        printf("Select\n");
//    }
//
//    void ListDatabase(ListDatabaseResponse& _return, const ListDatabaseRequest& request) {
//        // Your implementation goes here
//        printf("ListDatabase\n");
//    }
//
//    void ListTable(ListTableResponse& _return, const ListTableRequest& request) {
//        // Your implementation goes here
//        printf("ListTable\n");
//    }
//
//    void DescribeDatabase(DescribeDatabaseResponse& _return, const DescribeDatabaseRequest& request) {
//        // Your implementation goes here
//        printf("DescribeDatabase\n");
//    }
//
//    void DescribeTable(DescribeTableResponse& _return, const DescribeTableRequest& request) {
//        // Your implementation goes here
//        printf("DescribeTable\n");
//    }
//
//    void GetDatabase(CommonResponse& _return, const GetDatabaseRequest& request) {
//        // Your implementation goes here
//        printf("GetDatabase\n");
//    }
//
//    void GetTable(CommonResponse& _return, const GetTableRequest& request) {
//        // Your implementation goes here
//        printf("GetTable\n");
//    }
//
//    void CreateIndex(CommonResponse& _return, const CreateIndexRequest& request) {
//        // Your implementation goes here
//        printf("CreateIndex\n");
//    }
//
//    void DropIndex(CommonResponse& _return, const DropIndexRequest& request) {
//        // Your implementation goes here
//        printf("DropIndex\n");
//    }
protected:
    map<int32_t, SharedStruct> log;
    std::mutex infinity_session_map_mutex_{};
    std::unordered_map<infinity::u64, infinity::SharedPtr<infinity::Infinity>> infinity_session_map_{};
};

/*
  CalculatorIfFactory is code generated.
  CalculatorCloneFactory is useful for getting access to the server side of the
  transport.  It is also useful for making per-connection state.  Without this
  CloneFactory, all connections will end up sharing the same handler instance.
*/
class CalculatorCloneFactory : virtual public CalculatorIfFactory {
public:
    ~CalculatorCloneFactory() override = default;
    CalculatorIf *getHandler(const ::apache::thrift::TConnectionInfo &connInfo) override {
        std::shared_ptr<TSocket> sock = std::dynamic_pointer_cast<TSocket>(connInfo.transport);
        cout << "Incoming connection\n";
        cout << "\tSocketInfo: " << sock->getSocketInfo() << "\n";
        cout << "\tPeerHost: " << sock->getPeerHost() << "\n";
        cout << "\tPeerAddress: " << sock->getPeerAddress() << "\n";
        cout << "\tPeerPort: " << sock->getPeerPort() << "\n";
        return new CalculatorHandler;
    }
    void releaseHandler(::shared::SharedServiceIf *handler) override { delete handler; }
};

import logger;

namespace infinity {

void ThreadedThriftServer::Init(int32_t port_no) {

    server = std::make_unique<TThreadedServer>(std::make_shared<CalculatorProcessorFactory>(std::make_shared<CalculatorCloneFactory>()),
                                               std::make_shared<TServerSocket>(port_no), // port
                                               std::make_shared<TBufferedTransportFactory>(),
                                               std::make_shared<TBinaryProtocolFactory>());
}

void ThreadedThriftServer::Start() { server->serve(); }

void ThreadedThriftServer::Shutdown() { server->stop(); }

void PoolThriftServer::Init(int32_t port_no, int32_t pool_size) {

    std::shared_ptr<ThreadFactory> threadFactory = std::make_shared<ThreadFactory>();

    std::shared_ptr<ThreadManager> threadManager = ThreadManager::newSimpleThreadManager(pool_size);
    threadManager->threadFactory(threadFactory);
    threadManager->start();

    server = std::make_unique<TThreadPoolServer>(std::make_shared<CalculatorProcessorFactory>(std::make_shared<CalculatorCloneFactory>()),
                                                 std::make_shared<TServerSocket>(port_no),
                                                 std::make_shared<TBufferedTransportFactory>(),
                                                 std::make_shared<TBinaryProtocolFactory>(),
                                                 threadManager);
}

void PoolThriftServer::Start() { server->serve(); }

void PoolThriftServer::Shutdown() { server->stop(); }

} // namespace infinity