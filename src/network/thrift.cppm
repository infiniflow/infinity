//
// Created by jinhai on 24-1-9.
//

module;

#include <thrift/concurrency/ThreadManager.h>
// #include <thrift/concurrency/ThreadFactory.h>
// #include <thrift/protocol/TBinaryProtocol.h>
// #include <thrift/server/TSimpleServer.h>
#include <thrift/server/TThreadPoolServer.h>
#include <thrift/server/TThreadedServer.h>
// #include <thrift/transport/TServerSocket.h>
#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>
// #include <thrift/transport/TTransportUtils.h>
// #include <thrift/TToString.h>

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
            using apache::thrift::transport::TServerSocket;
            using apache::thrift::transport::TBufferedTransportFactory;

        }

        namespace protocol {
            using apache::thrift::protocol::TBinaryProtocolFactory;
            using apache::thrift::protocol::TProtocolFactory;
        }
    }
}
