//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include <utility>
#include <map>
#include <optional>

#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "buffer_reader.h"
#include "buffer_writer.h"

namespace infinity {
class PGProtocolHandler {
public:
    explicit PGProtocolHandler(std::shared_ptr<boost::asio::ip::tcp::socket> socket);

    uint32_t read_startup_header();
    void read_startup_body(uint32_t body_size);

    void send_authentication();
    void send_parameter(const std::string& key, const std::string& value);
    void send_ready_for_query();

    PGMessageType read_command_type();
    std::string read_command_body();

    void send_error_response(const std::map<PGMessageType, std::string>& error_response_map);
//
//    std::string read_query_packet();

    void SendDescriptionHeader(uint32_t total_column_name_length, uint32_t column_count);
    void SendDescription(const std::string& column_name, uint32_t object_id, uint16_t width);
    void SendData(const std::vector<std::optional<std::string>>& values_as_strings, uint64_t string_length_sum);
    void SendComplete(const std::string& complete_message);
//
//    std::pair<std::string, std::string> read_parse_packet();
//    void read_sync_packet();
//
//    void send_status_message(const PGMessageType message_type);

//    void force_flush() { buffer_writer_.flush(); }

private:
    BufferReader buffer_reader_;
    BufferWriter buffer_writer_;
};

}