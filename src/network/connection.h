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
    explicit Connection(boost::asio::io_service& io_service);
    void Run();

    std::shared_ptr<boost::asio::ip::tcp::socket> socket() { return socket_; }
private:
    void HandleConnection();

    void HandleRequest();

    void HandlerSimpleQuery(std::shared_ptr<QueryContext>& query_context);

    const std::shared_ptr<boost::asio::ip::tcp::socket> socket_;

    const std::shared_ptr<PGProtocolHandler> pg_handler_;

    bool terminate_connection_ = false;

    void SendTableDescription(const std::shared_ptr<Table>& result_table);
    void SendQueryResponse(const std::shared_ptr<Table>& result_table);
    void SendComplete(LogicalNodeType operator_type, uint64_t row_count);

private:
    std::shared_ptr<Session> session_ptr_;
};

}


