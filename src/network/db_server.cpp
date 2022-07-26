//
// Created by JinHai on 2022/7/19.
//

#include <boost/bind/bind.hpp>
#include <thread>
#include "db_server.h"

namespace infinity {

DBServer::DBServer(const StartupParameter &parameter)
    : acceptor_(io_service_, boost::asio::ip::tcp::endpoint(parameter.address, parameter.port)) { }

void DBServer::Run() {
    initialized = true;

    CreateSession();
    io_service_.run();
}

void DBServer::Shutdown() {
    while(running_session_count_ > 0) {
        // Running session exists.
        std::this_thread::yield();
    }
    io_service_.stop();

    acceptor_.close();
}

void
DBServer::CreateSession() {
    std::shared_ptr<Session> session_ptr = std::make_shared<Session>(io_service_);
    acceptor_.async_accept(*(session_ptr->socket()), boost::bind(&DBServer::StartSession, this, session_ptr));
}


void
DBServer::StartSession(std::shared_ptr<Session>& session) {
    std::thread session_thread ([session = session, &num_running_sessions = this->running_session_count_]() mutable {
        ++ num_running_sessions;
        session->Run();

        // User disconnected
        session.reset();
        -- num_running_sessions;
    });

    session_thread.detach();
    CreateSession();
}

}
