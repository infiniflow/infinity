//
// Created by JinHai on 2022/7/20.
//

#pragma once
#include "hv/TcpServer.h"
#include "common/utility/str.h"
#include "common/utility/asserter.h"
#include "pg_message.h"

#include <iostream>

namespace infinity {

class BufferReader {
public:
    explicit BufferReader(const hv::SocketChannelPtr &channel);
    void set_buffer(hv::Buffer* buf) { buffer_ = buf; };
    inline uint64_t unread_buffer_size();

    std::string read_by_size(uint64_t size, NullTerminator flag);

    inline void reset() { start_pos_ = 0; }

    template<typename T>
    T read_value() {
        while(start_pos_ + sizeof(T) > buffer_->size()) {
            // TODO: need to read more data
            Assert(false, "Try to read the data out of the buffer.");
        }

        T network_value = 0;
        char* data_pos = start_pos_ + (char*)(buffer_->data());
        memcpy((char*)&network_value, data_pos, sizeof(T));
        start_pos_ += sizeof(T);
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
        Assert(false, "Try to read invalid type of data from the buffer.");
    }

private:
    const hv::SocketChannelPtr &channel_;
    hv::Buffer* buffer_ = nullptr;
    uint64_t start_pos_ = 0;
};

}

