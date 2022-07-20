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
            this->session_set_[channel->id()] = std::make_shared<Session>(channel);
//            printf("%s connected! connfd=%d id=%d tid=%ld\n", peer_addr.c_str(), channel->fd(), channel->id(), hv::currentThreadEventLoop->tid());
        } else {
            printf("%s disconnected! id=%d tid=%ld\n", peer_addr.c_str(), channel->id(), hv::currentThreadEventLoop->tid());
//            printf("%s disconnected! connfd=%d id=%d tid=%ld\n", peer_addr.c_str(), channel->fd(), channel->id(), hv::currentThreadEventLoop->tid());
        }
    };
    hv_server_.onMessage = [&](const hv::SocketChannelPtr& channel, hv::Buffer* buf) {
        std::string peer_addr = channel->peeraddr();
        printf("%s send message! id=%d tid=%ld\n", peer_addr.c_str(), channel->id(), hv::currentThreadEventLoop->tid());
        // printf("< %.*s\n", (int)buf->size(), (char*)buf->data());
        std::shared_ptr<Session> session_ptr = session_set_[channel->id()];
        session_ptr->run(buf);

        channel->write(buf);
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

}
