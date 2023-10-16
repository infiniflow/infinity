//
// Created by JinHai on 2022/7/20.
//
module;

import stl;
import boost;
import pg_message;
import buffer_reader;
import buffer_writer;

export module pg_protocol_handler;

namespace infinity {

export class PGProtocolHandler {
public:
    explicit PGProtocolHandler(const SharedPtr<AsioSocket> &socket);

    u32 read_startup_header();

    void read_startup_body(u32 body_size);

    void send_authentication();

    void send_parameter(const String &key, const String &value);

    void send_ready_for_query();

    PGMessageType read_command_type();

    String read_command_body();

    void send_error_response(const HashMap<PGMessageType, String> &error_response_map);
    //
    //    String read_query_packet();

    void SendDescriptionHeader(u32 total_column_name_length, u32 column_count);

    void SendDescription(const String &column_name, u32 object_id, u16 width);

    void SendData(const Vector<Optional<String>> &values_as_strings, u64 string_length_sum);

    void SendComplete(const String &complete_message);
    //
    //    std::pair<String, String> read_parse_packet();
    //    void read_sync_packet();
    //
    //    void send_status_message(const PGMessageType message_type);

    //    void force_flush() { buffer_writer_.flush(); }

private:
    BufferReader buffer_reader_;
    BufferWriter buffer_writer_;
};

} // namespace infinity