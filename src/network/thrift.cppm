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

#include <thrift/concurrency/ThreadManager.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TCompactProtocol.h>

export module thrift;

export namespace apache {
    namespace thrift {
        namespace concurrency {
            using apache::thrift::concurrency::Thread;
            using apache::thrift::concurrency::ThreadFactory;
            using apache::thrift::concurrency::ThreadManager;
        }

        namespace server {
            using apache::thrift::server::TThreadedServer;
            using apache::thrift::server::TServer;
            using apache::thrift::server::TThreadPoolServer;
        }

        namespace transport {
            using apache::thrift::transport::TSocket;
            using apache::thrift::transport::TTransport;
            using apache::thrift::transport::TServerSocket;
            using apache::thrift::transport::TBufferedTransport;
            using apache::thrift::transport::TBufferedTransportFactory;
            using apache::thrift::transport::TTransportException;
            using TTransportExceptionType = apache::thrift::transport::TTransportException::TTransportExceptionType;
        }

        namespace protocol {
            using apache::thrift::protocol::TProtocol;
            using apache::thrift::protocol::TBinaryProtocol;
            using apache::thrift::protocol::TBinaryProtocolFactory;
            using apache::thrift::protocol::TCompactProtocolFactory;
            using apache::thrift::protocol::TProtocolFactory;
        }
    }
}
