// Refers to
// https://github.com/ClickHouse/ClickHouse/blob/master/src/Functions/CRC.cpp
// https://www.boost.org/doc/libs/1_83_0/doc/html/crc/crc_samples.html

module;

export module infinity_core:crc;

import :infinity_type;

import std.compat;

export namespace infinity {

template <class T>
struct CRCBase {
    T tab[256];
    explicit CRCBase(T polynomial) {
        for (size_t i = 0; i < 256; ++i) {
            T c = static_cast<T>(i);
            for (size_t j = 0; j < 8; ++j)
                c = c & 1 ? polynomial ^ (c >> 1) : c >> 1;
            tab[i] = c;
        }
    }
};

template <class T, T polynomial, T initial_remainder, T final_xor_value>
struct CRCImpl {
    static CRCBase<T> base;
    static T makeCRC(const unsigned char *buf, size_t size) {
        T crc = initial_remainder;
        for (size_t i = 0; i < size; ++i)
            crc = base.tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
        return crc ^ final_xor_value;
    }
    void update(const unsigned char *buf, size_t size) {
        for (size_t i = 0; i < size; ++i)
            crc = base.tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    }
    T finalize() { return crc ^ final_xor_value; }
    T crc = initial_remainder;
};

template <class T, T polynomial, T initial_remainder, T final_xor_value>
CRCBase<T> CRCImpl<T, polynomial, initial_remainder, final_xor_value>::base{polynomial};

// CRC32IEEE is equivalent to boost::crc_32_type on little-endian machine.
constexpr u32 CRC32_IEEE = 0xEDB88320;
using CRC32IEEE = CRCImpl<u32, CRC32_IEEE, 0xFFFFFFFF, 0xFFFFFFFF>;

} // namespace infinity
