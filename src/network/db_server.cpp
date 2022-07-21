//
// Created by JinHai on 2022/7/19.
//

#include "db_server.h"

namespace infinity {

DBServer::DBServer(const StartupParameter &parameter)
    : address_(parameter.address), port_(parameter.port) { }

void DBServer::run() {
    int listen_fd = hv_server_.createsocket(port_, address_.c_str());
    if (listen_fd < 0) {
        return ;
    }
    hv_server_.onConnection = [&](const hv::SocketChannelPtr& channel) {
        std::string peer_addr = channel->peeraddr();
        if (channel->isConnected()) {
            printf("%s connected! id=%d tid=%ld\n", peer_addr.c_str(), channel->id(), hv::currentThreadEventLoop->tid());
            create_session(channel);
//            printf("%s connected! connfd=%d id=%d tid=%ld\n", peer_addr.c_str(), channel->fd(), channel->id(), hv::currentThreadEventLoop->tid());
        } else {
            printf("%s disconnected! id=%d tid=%ld\n", peer_addr.c_str(), channel->id(), hv::currentThreadEventLoop->tid());
            auto session_ptr = get_session_by_id(channel->id());
            session_ptr->stop();
//            printf("%s disconnected! connfd=%d id=%d tid=%ld\n", peer_addr.c_str(), channel->fd(), channel->id(), hv::currentThreadEventLoop->tid());
        }
    };
    hv_server_.onMessage = [&](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
        std::string peer_addr = channel->peeraddr();
        printf("%s send message! id=%d tid=%ld\n", peer_addr.c_str(), channel->id(), hv::currentThreadEventLoop->tid());
        // printf("< %.*s\n", (int)buf->size(), (char*)buf->data());

        auto session_ptr = get_session_by_id(channel->id());
        if(session_ptr->status() == SessionStatus::kIdle) {
            this->start_session(session_ptr, buf);
        } else if(session_ptr->status() == SessionStatus::kSuspend) {
            session_ptr->resume();
        } else {
            Assert(false, "Message for terminated or running session, panic!");
        }
    };
    // 1 acceptor thread + N worker = set thread number;
    // This parameter indicates the max connection number.
    hv_server_.setThreadNum(1);
    hv_server_.setLoadBalance(LB_LeastConnections);

    hv_server_.start();

    // press Enter to stop
    while (getchar() != '\n');
}

void DBServer::shutdown() {

}

void
DBServer::start_session(std::shared_ptr<Session> session_ptr, hv::Buffer* buf) {
    std::thread session_thread([&] {
        ++ running_session_count;

        session_ptr->run(buf);

        -- running_session_count;
        session_ptr.reset();
        destroy_session(session_ptr->id());
    });

    session_thread.detach();
}

void
DBServer::create_session(const hv::SocketChannelPtr& channel) {
    std::unique_lock<std::mutex> lock(session_mutex_);
    session_set_[channel->id()] = std::make_shared<Session>(channel);
}

std::shared_ptr<Session>
DBServer::get_session_by_id(uint32_t id) {
    std::unique_lock<std::mutex> lock(session_mutex_);
    auto session_ptr = session_set_[id];
    return session_ptr;
}

void
DBServer::destroy_session(uint32_t id) {
    std::unique_lock<std::mutex> lock(session_mutex_);
    session_set_.erase(id);
}


}
