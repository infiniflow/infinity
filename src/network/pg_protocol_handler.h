//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include <utility>

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
//
//    std::string read_query_packet();

//    void send_description_header(uint32_t total_column_name_length, uint32_t column_count);
//    void send_description(const std::string& column_name, const uint64_t object_id, const int64_t type_width);
//    void send_data_row(const std::vector<std::optional<std::string>>& values_as_strings,
//                       const uint64_t string_length_sum);
//    void send_command_complete(const std::string& complete_message);
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