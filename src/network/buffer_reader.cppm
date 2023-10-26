//
// Created by JinHai on 2022/7/20.
//

module;

import boost;
import pg_message;
import ring_buffer_iterator;
import default_values;
import stl;

export module buffer_reader;

namespace infinity {

export class BufferReader {
public:
    explicit BufferReader(const SharedPtr<AsioSocket> &socket) : socket_(socket){};

    [[nodiscard]] SizeT size() const;

    [[nodiscard]] static inline SizeT max_capacity() { return PG_MSG_BUFFER_SIZE - 1; }

    [[nodiscard]] inline bool full() const { return size() == max_capacity(); }

    i8 read_value_i8();

    u8 read_value_u8();

    i16 read_value_i16();

    u16 read_value_u16();

    i32 read_value_i32();

    u32 read_value_u32();

    String read_string(const SizeT string_length, NullTerminator null_terminator = NullTerminator::kYes);

    String read_string();

private:
    void receive_more(SizeT more_bytes = 1);

    Array<char, PG_MSG_BUFFER_SIZE> data_{};
    RingBufferIterator start_pos_{data_};
    RingBufferIterator current_pos_{data_};

    SharedPtr<AsioSocket> socket_;
};

} // namespace infinity
