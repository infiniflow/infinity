//
// Created by JinHai on 2022/7/20.
//

module;

import boost;
import stl;
import pg_message;

export module buffer_writer;

namespace infinity {

export class BufferWriter {
public:
    explicit BufferWriter(const SharedPtr<AsioSocket>& socket) : socket_(socket) {}

    [[nodiscard]] SizeT size() const;

    inline static SizeT max_capacity() { return PG_MSG_BUFFER_SIZE - 1; }

    inline bool full() const { return size() == max_capacity(); }

    template <typename T>
    void send_value(T host_value) {
        try_flush(sizeof(T));

        T network_value;
//        if constexpr (std::is_same_v<T, i16> || std::is_same_v<T, u16>) {
//            network_value = htons(host_value);
//        } else if constexpr (std::is_same_v<T, i32> || std::is_same_v<T, u32>) {
//            network_value = htonl(host_value);
//        } else if constexpr (std::is_same_v<T, i64> || std::is_same_v<T, u64>) {
//            network_value = htonll(host_value);
//        } else {
//            network_value = host_value;
//        }
//        std::copy_n(reinterpret_cast<const char *>(&network_value), sizeof(T), current_pos_);
//        std::advance(current_pos_, sizeof(T));

    }

    void send_string(const String &value, NullTerminator null_terminator = NullTerminator::kYes);

    // 0 means flush whole buffer.
    void flush(SizeT bytes = 0);

private:
    void try_flush(SizeT bytes);

    Array<char, PG_MSG_BUFFER_SIZE> data_{};
//    RingBufferIterator start_pos_{data_};
//    RingBufferIterator current_pos_{data_};
    SharedPtr<AsioSocket> socket_{};
};

} // namespace infinity
