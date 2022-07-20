//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "hv/TcpServer.h"
#include "buffer_reader.h"
#include "buffer_writer.h"

namespace infinity {

class PGProtocolHandler {
public:
    explicit PGProtocolHandler(const hv::SocketChannelPtr &channel);
    void set_reader_buffer(hv::Buffer* buf) { buffer_reader_.set_buffer(buf); }

    uint32_t read_startup_header();
    void read_startup_body(uint32_t body_size);

    void send_authentication();
    void send_parameter(const std::string& key, const std::string& value);
    void send_ready_for_query();

    inline PGMessageType read_message_type() { return static_cast<PGMessageType>(buffer_reader_.read_value<char>()); }

private:
    BufferReader buffer_reader_;
    BufferWriter buffer_writer_;
};

}