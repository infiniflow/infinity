// Refers to
// https://datatracker.ietf.org/doc/rfc9562/, chapter 5.7, UUID Version 7

module;

export module uuid;

import stl;

export namespace infinity {

struct UUID {
    union {
        u8 data[16];
        u64 data64[2];
    };

    UUID() {
        auto now = std::chrono::system_clock::now().time_since_epoch();
        u64 ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
        u64 ns = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
        data64[0] = (ms << 16) | (ns & 0xffff);
        std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<u64> dist;
        data64[1] = dist(rng);
        data[6] = (data[6] & 0x0f) | 0x70; // ver, 4-bit, 0b0111
        data[8] = (data[8] & 0x3f) | 0x80; // var, 2-bit, 0b10
    }

    String to_string() const {
        char buf[37];
        std::sprintf(buf,
                     "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                     data[0],
                     data[1],
                     data[2],
                     data[3],
                     data[4],
                     data[5],
                     data[6],
                     data[7],
                     data[8],
                     data[9],
                     data[10],
                     data[11],
                     data[12],
                     data[13],
                     data[14],
                     data[15]);
        return buf;
    }
};

} // namespace infinity
