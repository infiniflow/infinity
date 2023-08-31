//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "pg_protocol_handler.h"
#include "storage/table.h"
#include "planner/logical_node_type.h"
#include "main/session.h"

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace infinity {

enum class ConnectionStatus : char {
    kIdle,
    kRunning,
    kSuspend,
    kTerminated
};

class Connection {
public:
    explicit
    Connection(boost::asio::io_service& io_service);

    void
    Run();

    inline SharedPtr<boost::asio::ip::tcp::socket>
    socket() {
        return socket_;
    }

private:
    void
    HandleConnection();

    void
    HandleRequest();

    void
    HandlerSimpleQuery(SharedPtr<QueryContext>& query_context);

    void
    SendTableDescription(const SharedPtr<Table>& result_table);

    void
    SendQueryResponse(const QueryResult& query_result);

private:
    const SharedPtr<boost::asio::ip::tcp::socket> socket_;

    const SharedPtr<PGProtocolHandler> pg_handler_;

    bool terminate_connection_ = false;

    UniquePtr<Session> session_;
};

}


