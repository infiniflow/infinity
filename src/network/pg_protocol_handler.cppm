// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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
    explicit PGProtocolHandler(const SharedPtr<boost::asio::ip::tcp::socket> &socket);
    ~PGProtocolHandler();

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
    //    pair<String, String> read_parse_packet();
    //    void read_sync_packet();
    //
    //    void send_status_message(const PGMessageType message_type);

    //    void force_flush() { buffer_writer_.flush(); }

private:
    BufferReader buffer_reader_;
    BufferWriter buffer_writer_;
};

} // namespace infinity