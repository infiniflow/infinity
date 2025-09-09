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

export module infinity_core:buffer_reader;

import :boost;
import :pg_message;
import :ring_buffer_iterator;
import :default_values;

import std.compat;

import global_resource_usage;

namespace infinity {

export class BufferReader {
public:
    explicit BufferReader(const std::shared_ptr<boost::asio::ip::tcp::socket> &socket) : socket_(socket) {}

    ~BufferReader() = default;

    [[nodiscard]] size_t size() const;

    [[nodiscard]] static inline size_t max_capacity() { return PG_MSG_BUFFER_SIZE - 1; }

    [[nodiscard]] inline bool full() const { return size() == max_capacity(); }

    i8 read_value_i8();

    u8 read_value_u8();

    i16 read_value_i16();

    u16 read_value_u16();

    i32 read_value_i32();

    u32 read_value_u32();

    std::string read_string(const size_t string_length, NullTerminator null_terminator = NullTerminator::kYes);

    std::string read_string();

private:
    void receive_more(size_t more_bytes = 1);

    std::array<char, PG_MSG_BUFFER_SIZE> data_{};
    RingBufferIterator start_pos_{data_};
    RingBufferIterator current_pos_{data_};

    std::shared_ptr<boost::asio::ip::tcp::socket> socket_;
};

} // namespace infinity
