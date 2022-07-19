//
// Created by JinHai on 2022/7/20.
//

#include "session.h"

Session::Session(const hv::SocketChannelPtr &channel) : channel_(channel) {

}

void
Session::run(hv::Buffer* buffer) {
    handle_connection(buffer);
//    while(!terminated_) {
//        ;
//    }
}

void
Session::handle_connection(hv::Buffer* buffer) {

}

void
Session::handle_request() {

}
