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
import pg_message;
import ring_buffer_iterator;
import default_values;
import stl;
import global_resource_usage;

export module buffer_writer;

namespace infinity {

export class BufferWriter {
public:
    explicit BufferWriter(const SharedPtr<boost::asio::ip::tcp::socket> &socket) : socket_(socket) {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::IncrObjectCount("BufferWriter");
#endif
    }
    ~BufferWriter() {
#ifdef INFINITY_DEBUG
        GlobalResourceUsage::DecrObjectCount("BufferWriter");
#endif
    }
    [[nodiscard]] SizeT size() const;

    inline static SizeT max_capacity() { return PG_MSG_BUFFER_SIZE - 1; }

    inline bool full() const { return size() == max_capacity(); }

    void send_value_i8(i8 host_value);

    void send_value_u8(u8 host_value);

    void send_value_i16(i16 host_value);

    void send_value_u16(u16 host_value);

    void send_value_i32(i32 host_value);

    void send_value_u32(u32 host_value);

    void send_string(const String &value, NullTerminator null_terminator = NullTerminator::kYes);

    // 0 means flush whole buffer.
    void flush(SizeT bytes = 0);

private:
    void try_flush(SizeT bytes);

    Array<char, PG_MSG_BUFFER_SIZE> data_{};
    RingBufferIterator start_pos_{data_};
    RingBufferIterator current_pos_{data_};
    SharedPtr<boost::asio::ip::tcp::socket> socket_{};
};

} // namespace infinity
