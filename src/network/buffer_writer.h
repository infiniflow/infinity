//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include <array>
#include "pg_message.h"
#include "common/utility/infinity_assert.h"
#include "ring_buffer_iterator.h"
#include <boost/asio/ip/tcp.hpp>

namespace infinity {

class BufferWriter {
public:
    explicit BufferWriter(const std::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket_(socket) {}

    [[nodiscard]] size_t size() const;

    inline static size_t max_capacity() { return PG_MSG_BUFFER_SIZE - 1; }

    inline bool full() const { return size() == max_capacity(); }

    template<typename T>
    void send_value(T host_value) {
        try_flush(sizeof(T));

        T network_value;
        if constexpr(std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>) {
            network_value = htons(host_value);
        } else if constexpr(std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>) {
            network_value = htonl(host_value);
        } else if constexpr(std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>) {
            network_value = htonll(host_value);
        } else {
            network_value = host_value;
        }
        std::copy_n(reinterpret_cast<const char*>(&network_value), sizeof(T), current_pos_);
        std::advance(current_pos_, sizeof(T));
    }

    void send_string(const std::string& value, NullTerminator null_terminator = NullTerminator::kYes);

    // 0 means flush whole buffer.
    void flush(size_t bytes = 0);

private:
    void try_flush(size_t bytes);

    std::array<char, PG_MSG_BUFFER_SIZE> data_{};
    RingBufferIterator start_pos_{data_};
    RingBufferIterator current_pos_{data_};
    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};

}
