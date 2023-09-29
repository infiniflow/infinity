//
// Created by JinHai on 2022/7/20.
//

#pragma once

#include "common/utility/str.h"
#include "common/utility/infinity_assert.h"
#include "pg_message.h"
#include "ring_buffer_iterator.h"

#include <iostream>
#include <utility>

#include <boost/asio/ip/tcp.hpp>

namespace infinity {

class BufferReader {
public:
    explicit BufferReader(std::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket_(std::move(socket)) {};

    [[nodiscard]] size_t
    size() const;

    [[nodiscard]] static inline size_t
    max_capacity() { return PG_MSG_BUFFER_SIZE - 1; }

    [[nodiscard]] inline bool
    full() const { return size() == max_capacity(); }

    template<typename T>
    T
    read_value() {
        receive_more(sizeof(T));
        T network_value{0};
        std::copy_n(start_pos_, sizeof(T), reinterpret_cast<char*>(&network_value));
        std::advance(start_pos_, sizeof(T));

        if constexpr(std::is_same_v<T, char> || std::is_same_v<T, u_char>) {
            return network_value;
        }
        if constexpr(std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) {
            return ntohs(network_value);
        }
        if constexpr(std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) {
            return ntohl(network_value);
        }
        if constexpr(std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            return ntohll(network_value);
        }
        NetworkAssert(false, "Try to read invalid type of data from the buffer.");
    }

    std::string
    read_string(const size_t string_length, NullTerminator null_terminator = NullTerminator::kYes);

    std::string
    read_string();

private:
    void
    receive_more(size_t more_bytes = 1);

    std::array<char, PG_MSG_BUFFER_SIZE> data_{};
    RingBufferIterator start_pos_{data_};
    RingBufferIterator current_pos_{data_};

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};

}

