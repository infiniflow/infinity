// Refers to
// https://github.com/ClickHouse/ClickHouse/blob/master/src/Functions/CRC.cpp
// https://www.boost.org/doc/libs/1_83_0/doc/html/crc/crc_samples.html

module;

import stl;

export module crc;

export namespace infinity {

template <class T>
struct CRCBase {
    T tab[256];
    explicit CRCBase(T polynomial) {
        for (SizeT i = 0; i < 256; ++i) {
            T c = static_cast<T>(i);
            for (SizeT j = 0; j < 8; ++j)
                c = c & 1 ? polynomial ^ (c >> 1) : c >> 1;
            tab[i] = c;
        }
    }
};

template <class T, T polynomial, T initial_remainder, T final_xor_value>
struct CRCImpl {
    using ReturnType = T;

    static T makeCRC(const unsigned char *buf, SizeT size) {
        static CRCBase<ReturnType> base(polynomial);

        T crc = initial_remainder;
        for (SizeT i = 0; i < size; ++i)
            crc = base.tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
        return crc ^ final_xor_value;
    }
};

// CRC32IEEE is equivalent to boost::crc_32_type on little-endian machine.
constexpr u32 CRC32_IEEE = 0xEDB88320;
using CRC32IEEE = CRCImpl<u32, CRC32_IEEE, 0xFFFFFFFF, 0xFFFFFFFF>;

} // namespace infinity
