//
// Created by JinHai on 2022/7/20.
//

#include "pg_protocol_handler.h"

#include <utility>
#include "pg_message.h"

namespace infinity {

PGProtocolHandler::PGProtocolHandler(const SharedPtr<boost::asio::ip::tcp::socket>& socket)
    : buffer_reader_(socket), buffer_writer_(socket) {}

u32
PGProtocolHandler::read_startup_header() {
    constexpr u32 SSL_MESSAGE_VERSION = 80877103u;
    const auto length = buffer_reader_.read_value<u32>();
    const auto version = buffer_reader_.read_value<u32>();
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
PGProtocolHandler::read_startup_body(const u32 body_size) {
    // TODO: Need to check the startup message which contains information from the cmd by user.
    buffer_reader_.read_string(body_size, NullTerminator::kNo);
}

void
PGProtocolHandler::send_authentication() {
    buffer_writer_.send_value(PGMessageType::kAuthentication);

    // length = LENGTH FIELD + Authentication response code
    constexpr u32 AUTHENTICATION_ERROR_CODE = 0;
    buffer_writer_.send_value<u32>(LENGTH_FIELD_SIZE + sizeof(AUTHENTICATION_ERROR_CODE));

    // Always successful
    // TODO: Add real authentication workflow.
    buffer_writer_.send_value<u32>(AUTHENTICATION_ERROR_CODE);
}

void
PGProtocolHandler::send_parameter(const String &key, const String &value) {
    buffer_writer_.send_value(PGMessageType::kParameterStatus);
    // length field size + key size + 1 null terminator + value size + 1 null terminator
    buffer_writer_.send_value<u32>(static_cast<u32>(LENGTH_FIELD_SIZE + key.size() + value.size() + 2u));
    buffer_writer_.send_string(key, NullTerminator::kYes);
    buffer_writer_.send_string(value, NullTerminator::kYes);
}

void
PGProtocolHandler::send_ready_for_query() {
    buffer_writer_.send_value(static_cast<char>(PGMessageType::kReadyForQuery));
    buffer_writer_.send_value<u32>(LENGTH_FIELD_SIZE + sizeof(TransactionStateType::kIDLE));
    buffer_writer_.send_value(static_cast<char>(TransactionStateType::kIDLE));
    buffer_writer_.flush();
}

PGMessageType
PGProtocolHandler::read_command_type() {
    return static_cast<PGMessageType>(buffer_reader_.read_value<char>());
}

String
PGProtocolHandler::read_command_body() {
    const auto command_length = buffer_reader_.read_value<u32>() - LENGTH_FIELD_SIZE;
    return buffer_reader_.read_string(command_length);
}

void
PGProtocolHandler::send_error_response(const HashMap<PGMessageType, String>& error_response_map) {
    // message header
    buffer_writer_.send_value(PGMessageType::kError);

    u32 message_size = 0;
    for(const auto& error: error_response_map) {
        message_size += error.second.size() + 1u + sizeof(PGMessageType); // Error message string + null terminator
    }

    message_size += LENGTH_FIELD_SIZE + 1; // Length field and last null terminator

    // message length
    buffer_writer_.send_value<u32>(message_size);

    // message body
    for(const auto& error: error_response_map) {
        buffer_writer_.send_value(error.first);
        buffer_writer_.send_string(error.second);
    }

    // message ending terminator
    buffer_writer_.send_value(NULL_END);
    buffer_writer_.flush();
}

void
PGProtocolHandler::SendDescriptionHeader(u32 total_column_name_length, u32 column_count) {
    // https://www.postgresql.org/docs/14/static/protocol-message-formats.html
    buffer_writer_.send_value(PGMessageType::kRowDescription);

    // Length + column count + values for each columns
    u32 message_size = LENGTH_FIELD_SIZE + sizeof(u16)
                            + column_count * (sizeof('\0') + 3 * sizeof(u32) + 3 * sizeof(u16))
                            + total_column_name_length;
    buffer_writer_.send_value<u32>(message_size);
    buffer_writer_.send_value<u16>(column_count);
}

void
PGProtocolHandler::SendDescription(const String& column_name, u32 object_id, u16 width) {
    buffer_writer_.send_string(column_name);

    buffer_writer_.send_value<u32>(0); // No OID for the table;
    buffer_writer_.send_value<u16>(0); // No attribute number;

    buffer_writer_.send_value<u32>(object_id);   // OID of the type
    buffer_writer_.send_value<u16>(width);       // Type width
    buffer_writer_.send_value<i32>(-1); // No modifier
    buffer_writer_.send_value<i16>(0);  // Text format
}

void
PGProtocolHandler::SendData(const Vector<std::optional<String>>& values_as_strings, uint64_t string_length_sum) {
    // https://www.postgresql.org/docs/14/static/protocol-message-formats.html
    buffer_writer_.send_value(PGMessageType::kData);

    u32 message_size =
            LENGTH_FIELD_SIZE + sizeof(u16) + values_as_strings.size() * LENGTH_FIELD_SIZE + string_length_sum;

    // Message length field
    buffer_writer_.send_value<u32>(message_size);

    // Number of columns in row
    buffer_writer_.send_value<u16>(values_as_strings.size());

    for (const std::optional<String> &value_string: values_as_strings) {
        if (value_string.has_value()) {
            const String &value_ref = value_string.value();

            // Value string size
            buffer_writer_.send_value<u32>(value_ref.size());

            // Value without terminator
            buffer_writer_.send_string(value_ref, NullTerminator::kNo);
        } else {
            // Null value
            buffer_writer_.send_value<i32>(-1);
        }
    }
}

void
PGProtocolHandler::SendComplete(const String& complete_message) {
    // Field length + message size + null terminator
    u32 message_size = LENGTH_FIELD_SIZE + complete_message.size() + 1;
    buffer_writer_.send_value(PGMessageType::kComplete);
    buffer_writer_.send_value<u32>(message_size);
    buffer_writer_.send_string(complete_message);
}

}
