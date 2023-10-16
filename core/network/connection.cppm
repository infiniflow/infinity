//
// Created by JinHai on 2022/7/20.
//

module;

import boost;
import stl;
import session;
import pg_protocol_handler;

export module connection;

namespace infinity {

enum class ConnectionStatus : char { kIdle, kRunning, kSuspend, kTerminated };

//class QueryContext;
//
//class QueryResult;
//
//class Session;
//
//class Table;

export class Connection {
public:
    explicit Connection(AsioIOService &io_service);

    void Run();

    inline SharedPtr<AsioSocket> socket() { return socket_; }

private:
    void HandleConnection();

    void HandleRequest();

//    void HandlerSimpleQuery(QueryContext *query_context);
//
//    void SendTableDescription(const SharedPtr<Table> &result_table);
//
//    void SendQueryResponse(const QueryResult &query_result);

private:
    const SharedPtr<AsioSocket> socket_;

    const SharedPtr<PGProtocolHandler> pg_handler_;

    bool terminate_connection_ = false;

    SharedPtr<Session> session_;
};

} // namespace infinity
