//
// Created by JinHai on 2022/7/20.
//

#include "pg_protocol_handler.h"
#include "pg_message.h"

namespace infinity {

PGProtocolHandler::PGProtocolHandler(const hv::SocketChannelPtr &channel)
    : buffer_reader_(channel), buffer_writer_(channel) {}

uint32_t PGProtocolHandler::read_startup_header() {
    constexpr uint32_t SSL_MESSAGE_VERSION = 80877103;
    const auto length = buffer_reader_.read_value<uint32_t>();
    const auto version = buffer_reader_.read_value<uint32_t>();
    if(version == SSL_MESSAGE_VERSION)  {
        // TODO: support SSL
        // Now we said not support ssl
        buffer_writer_.send_value(static_cast<unsigned char>(PGMessageType::kSSLNo));
        buffer_writer_.flush();
        return read_startup_header();
    } else {
        return length - 2 * infinity::LENGTH_FIELD_SIZE;
    }
}

void
PGProtocolHandler::read_startup_body(const uint32_t body_size) {
    // TODO: Need to check the startup message which contains information from the cmd by user.
    buffer_reader_.read_by_size(body_size, NullTerminator::kNo);
}

void
PGProtocolHandler::send_authentication() {
    buffer_writer_.send_value(PGMessageType::kAuthentication);

    // length = LENGTH FIELD + Authentication response code
    buffer_writer_.send_value(2 * LENGTH_FIELD_SIZE);

    // Always successful
    // TODO: Add real authentication workflow.
    buffer_writer_.send_value(0);
}

void
PGProtocolHandler::send_parameter(const std::string &key, const std::string &value) {
    buffer_writer_.send_value(PGMessageType::kParameterStatus);
    // length field size + key size + 1 null terminator + value size + 1 null terminator
    buffer_writer_.send_value<uint32_t>(static_cast<uint32_t>(LENGTH_FIELD_SIZE + key.size() + value.size() + 2));
    buffer_writer_.send_string(key, NullTerminator::kYes);
    buffer_writer_.send_string(value, NullTerminator::kYes);
}

void
PGProtocolHandler::send_ready_for_query() {
    buffer_writer_.send_value(PGMessageType::kReadyForQuery);
    buffer_writer_.send_value<uint32_t>(LENGTH_FIELD_SIZE + 1);
    buffer_writer_.send_value(TransactionStateType::kIDLE);
    buffer_writer_.flush();
}

}
