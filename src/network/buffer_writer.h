//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include <array>
#include "hv/TcpServer.h"
#include "pg_message.h"
#include "common/utility/asserter.h"

namespace infinity {
class Session;
class BufferWriter {
public:
    explicit BufferWriter(const hv::SocketChannelPtr &channel);

    void set_session(std::shared_ptr<Session> session_ptr) { session_ptr_ = std::move(session_ptr); }

    void send_string(const std::string& value, NullTerminator null_terminator);
    [[nodiscard]] uint64_t available_capacity() const { return WRITE_BUFFER_SIZE - current_pos_ - 1; }

    template<typename T>
    void send_value(T value) {
        while(sizeof(T) + current_pos_ >= WRITE_BUFFER_SIZE) {
            flush();
        }
        T network_value;
//        if constexpr(std::is_same_v<T, char> || std::is_same_v<T, u_char>) {
//            network_value = value;
//        } else
        if constexpr(std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) {
            network_value = htons(value);
        } else if constexpr(std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) {
            network_value = htonl(value);
        } else if constexpr(std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            network_value = htonll(value);
        } else {
            // char
            network_value = value;
//            Assert(false, "Try to write invalid type of data into the buffer.");
        }
        memcpy(data_ + current_pos_, (char*)(&network_value), sizeof(T));
        current_pos_ += sizeof(T);
    }

    void flush();

private:
    char data_[WRITE_BUFFER_SIZE]{};
    uint64_t current_pos_ = 0;

    const hv::SocketChannelPtr &channel_;
    std::shared_ptr<Session> session_ptr_;
};

}
