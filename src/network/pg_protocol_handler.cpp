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

import boost;
import stl;
import pg_message;
module pg_protocol_handler;
import global_resource_usage;

namespace infinity {

PGProtocolHandler::PGProtocolHandler(const SharedPtr<boost::asio::ip::tcp::socket> &socket) : buffer_reader_(socket), buffer_writer_(socket) {}

PGProtocolHandler::~PGProtocolHandler() = default;

u32 PGProtocolHandler::read_startup_header() {
    constexpr u32 SSL_MESSAGE_VERSION = 80877103u;
    const auto length = buffer_reader_.read_value_u32();
    const auto version = buffer_reader_.read_value_u32();
    if (version == SSL_MESSAGE_VERSION) {
        // TODO: support SSL
        // Now we said not support ssl
        buffer_writer_.send_value_u8(static_cast<unsigned char>(PGMessageType::kSSLNo));
        buffer_writer_.flush();
        return read_startup_header();
    } else {
        return length - 2 * LENGTH_FIELD_SIZE;
    }
}

void PGProtocolHandler::read_startup_body(const u32 body_size) {
    // TODO: Need to check the startup message which contains information from the cmd by user.
    buffer_reader_.read_string(body_size, NullTerminator::kNo);
}

void PGProtocolHandler::send_authentication() {
    buffer_writer_.send_value_u8(static_cast<u8>(PGMessageType::kAuthentication));

    // length = LENGTH FIELD + Authentication response code
    constexpr u32 AUTHENTICATION_ERROR_CODE = 0;
    buffer_writer_.send_value_u32(LENGTH_FIELD_SIZE + sizeof(AUTHENTICATION_ERROR_CODE));

    // Always successful
    // TODO: Add real authentication workflow.
    buffer_writer_.send_value_u32(AUTHENTICATION_ERROR_CODE);
}

void PGProtocolHandler::send_parameter(const String &key, const String &value) {
    buffer_writer_.send_value_u8(static_cast<u8>(PGMessageType::kParameterStatus));
    // length field size + key size + 1 null terminator + value size + 1 null terminator
    buffer_writer_.send_value_u32(static_cast<u32>(LENGTH_FIELD_SIZE + key.size() + value.size() + 2u));
    buffer_writer_.send_string(key, NullTerminator::kYes);
    buffer_writer_.send_string(value, NullTerminator::kYes);
}

void PGProtocolHandler::send_ready_for_query() {
    buffer_writer_.send_value_i8(static_cast<char>(PGMessageType::kReadyForQuery));
    buffer_writer_.send_value_u32(LENGTH_FIELD_SIZE + sizeof(TransactionStateType::kIDLE));
    buffer_writer_.send_value_i8(static_cast<char>(TransactionStateType::kIDLE));
    buffer_writer_.flush();
}

PGMessageType PGProtocolHandler::read_command_type() { return static_cast<PGMessageType>(buffer_reader_.read_value_i8()); }

String PGProtocolHandler::read_command_body() {
    const auto command_length = buffer_reader_.read_value_u32() - LENGTH_FIELD_SIZE;
    return buffer_reader_.read_string(command_length);
}

void PGProtocolHandler::send_error_response(const HashMap<PGMessageType, String> &error_response_map) {
    // message header
    buffer_writer_.send_value_u8(static_cast<u8>(PGMessageType::kError));

    u32 message_size = 0;
    for (const auto &error : error_response_map) {
        message_size += error.second.size() + 1u + sizeof(PGMessageType); // Error message string + null terminator
    }

    message_size += LENGTH_FIELD_SIZE + 1; // Length field and last null terminator

    // message length
    buffer_writer_.send_value_u32(message_size);

    // message body
    for (const auto &error : error_response_map) {
        buffer_writer_.send_value_u8(static_cast<u8>(error.first));
        buffer_writer_.send_string(error.second);
    }

    // message ending terminator
    buffer_writer_.send_value_u8(NULL_END);
    buffer_writer_.flush();
}

void PGProtocolHandler::SendDescriptionHeader(u32 total_column_name_length, u32 column_count) {
    // https://www.postgresql.org/docs/14/static/protocol-message-formats.html
    buffer_writer_.send_value_u8(static_cast<u8>(PGMessageType::kRowDescription));

    // Length + column count + values for each columns
    u32 message_size = LENGTH_FIELD_SIZE + sizeof(u16) + column_count * (sizeof('\0') + 3 * sizeof(u32) + 3 * sizeof(u16)) + total_column_name_length;
    buffer_writer_.send_value_u32(message_size);
    buffer_writer_.send_value_u16(column_count);
}

void PGProtocolHandler::SendDescription(const String &column_name, u32 object_id, u16 width) {
    buffer_writer_.send_string(column_name);

    buffer_writer_.send_value_u32(0); // No OID for the table;
    buffer_writer_.send_value_u16(0); // No attribute number;

    buffer_writer_.send_value_u32(object_id); // OID of the type
    buffer_writer_.send_value_u16(width);     // Type width
    buffer_writer_.send_value_i32(-1);        // No modifier
    buffer_writer_.send_value_i16(0);         // Text format
}

void PGProtocolHandler::SendData(const Vector<Optional<String>> &values_as_strings, u64 string_length_sum) {
    // https://www.postgresql.org/docs/14/static/protocol-message-formats.html
    buffer_writer_.send_value_u8(static_cast<u8>(PGMessageType::kData));

    u32 message_size = LENGTH_FIELD_SIZE + sizeof(u16) + values_as_strings.size() * LENGTH_FIELD_SIZE + string_length_sum;

    // Message length field
    buffer_writer_.send_value_u32(message_size);

    u16 column_count = values_as_strings.size();

    // Number of columns in row
    buffer_writer_.send_value_u16(column_count);

    for (u16 idx = 0; idx < column_count; ++idx) {
        const Optional<String> &value_string = values_as_strings[idx];
        if (value_string.has_value()) {
            const String &value_ref = value_string.value();

            // Value string size
            buffer_writer_.send_value_u32(value_ref.size());

            // Value without terminator
            buffer_writer_.send_string(value_ref, NullTerminator::kNo);
        } else {
            // Null value
            buffer_writer_.send_value_i32(-1);
        }
    }
}

void PGProtocolHandler::SendComplete(const String &complete_message) {
    // Field length + message size + null terminator
    u32 message_size = LENGTH_FIELD_SIZE + complete_message.size() + 1;
    buffer_writer_.send_value_u8(static_cast<u8>(PGMessageType::kComplete));
    buffer_writer_.send_value_u32(message_size);
    buffer_writer_.send_string(complete_message);
}

} // namespace infinity
