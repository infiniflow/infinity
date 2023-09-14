#pragma once

// Refers to
// https://github.com/ClickHouse/ClickHouse/blob/master/src/Functions/CRC.cpp
// https://www.boost.org/doc/libs/1_83_0/doc/html/crc/crc_samples.html

namespace infinity {
#include <cstddef>
#include <cstdint>

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
    using ReturnType = T;

    static T makeCRC(const unsigned char *buf, size_t size) {
        static CRCBase<ReturnType> base(polynomial);

        T crc = initial_remainder;
        for (size_t i = 0; i < size; ++i)
            crc = base.tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
        return crc ^ final_xor_value;
    }
};

// CRC32IEEE is equivalent to boost::crc_32_type on little-endian machine.
constexpr uint32_t CRC32_IEEE = 0xEDB88320;
using CRC32IEEE = CRCImpl<uint32_t, CRC32_IEEE, 0xFFFFFFFF, 0xFFFFFFFF>;

}  // namespace infinity
