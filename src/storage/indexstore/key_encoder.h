#pragma once

#include "common/types/data_type.h"

#include <string>
#include <type_traits>

namespace infinity {

template<typename T>
concept NotString = !std::is_same_v<std::decay_t<T>, std::string>;

template<typename T>
requires NotString<T> inline static size_t
AppendToBuf(char* buf, const T& t, size_t start) {
    memcpy((void*)&buf[start], &t, sizeof(T));
    start += sizeof(T);
    return start;
}

template<typename T>
requires NotString<T> inline static size_t
LoadFromBuf(const char* buf, T& t, size_t start) {
    memcpy(&t, &buf[start], sizeof(T));
    start += sizeof(T);
    return start;
}

template<class T>
requires NotString<T> inline static T
LoadFromBuf(const void* p) {
    T x;
    memcpy(&x, p, sizeof(T));
    return x;
}

// Extracts an 8-byte order-preserving prefix of a given key. Little endian is assumed
template<class Slice>
static uint64_t
ExtractHead(const Slice& key) {
    switch(key.size()) {
        case 0:
            return 0;
        case 1:
            return static_cast<uint64_t>(key.data()[0]) << 56;
        case 2:
            return static_cast<uint64_t>(
                    __builtin_bswap16(LoadFromBuf<uint16_t>(key.data())))
                    << 48;
        case 3:
            return (static_cast<uint64_t>(
                    __builtin_bswap16(LoadFromBuf<uint16_t>(key.data())))
                    << 48) |
                   (static_cast<uint64_t>(key.data()[2]) << 40);
        case 4:
            return static_cast<uint64_t>(
                    __builtin_bswap32(LoadFromBuf<uint32_t>(key.data())))
                    << 32;

        case 5:
            return (static_cast<uint64_t>(
                    __builtin_bswap32(LoadFromBuf<uint32_t>(key.data())))
                    << 32) |
                   (static_cast<uint64_t>(key.data()[4]) << 24);

        case 6:
            return (static_cast<uint64_t>(
                    __builtin_bswap32(LoadFromBuf<uint32_t>(key.data())))
                    << 32) |
                   (static_cast<uint64_t>(
                           __builtin_bswap16(LoadFromBuf<uint16_t>(key.data() + 4)))
                           << 16);
        case 7:
            return (static_cast<uint64_t>(
                    __builtin_bswap32(LoadFromBuf<uint32_t>(key.data())))
                    << 32) |
                   (static_cast<uint64_t>(
                           __builtin_bswap16(LoadFromBuf<uint16_t>(key.data() + 4)))
                           << 16) |
                   (static_cast<uint64_t>(key.data()[6]) << 8);
        default:
            return __builtin_bswap64(LoadFromBuf<uint64_t>(key.data()));
    }
}

// Order-preserving binary encoding for a particular type so that
// the values could be compared by memcpy their encoded bytes.
// Big-endian not supported right now

typedef void (* EncodeFunc)(const void* value, std::string& buf);

typedef u32 (* SizeFunc)();

class KeyEncoder {
public:
    template<typename KeyEncoderTraits>
    KeyEncoder(KeyEncoderTraits traits);

    u32
    Size() const {
        return size_func_();
    }

    void
    Encode(const void* value, std::string& buf) const {
        encode_func_(value, buf);
    }
private:
    EncodeFunc encode_func_;
    SizeFunc size_func_;
};

extern const KeyEncoder*
GetKeyEncoder(LogicalType type);

template<auto T>
struct IsIntegral : public std::false_type {};
template<>
struct IsIntegral<LogicalType::kBoolean> : public std::true_type {};
template<>
struct IsIntegral<LogicalType::kTinyInt> : public std::true_type {};
template<>
struct IsIntegral<LogicalType::kInteger> : public std::true_type {};
template<>
struct IsIntegral<LogicalType::kBigInt> : public std::true_type {};
template<>
struct IsIntegral<LogicalType::kHugeInt> : public std::true_type {};


template<LogicalType data_type, typename Enable = void>
class KeyEncoderTraits {};

/*
template<LogicalType data_type>
class KeyEncoderTraits<data_type, typename std::enable_if<IsIntegral<data_type>::value>::type> {
public:
    static void Encode(const void* value, std::string& buf) {

    }
};
*/
template<>
class KeyEncoderTraits<LogicalType::kBoolean> {
public:
    static u32
    Size() {
        return sizeof(bool);
    }

    static void
    Encode(const void* value, std::string& buf) {
        bool val;
        memcpy(&val, value, sizeof(bool));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kTinyInt> {
public:
    static u32
    Size() {
        return sizeof(i8);
    }

    static void
    Encode(const void* value, std::string& buf) {
        i8 val;
        memcpy(&val, value, sizeof(i8));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kSmallInt> {
public:
    static u32
    Size() {
        return sizeof(i16);
    }

    static void
    Encode(const void* value, std::string& buf) {
        i16 val;
        memcpy(&val, value, sizeof(i16));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kInteger> {
public:
    static u32
    Size() {
        return sizeof(i32);
    }

    static void
    Encode(const void* value, std::string& buf) {
        i32 val;
        memcpy(&val, value, sizeof(i32));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kBigInt> {
public:
    static u32
    Size() {
        return sizeof(i64);
    }

    static void
    Encode(const void* value, std::string& buf) {
        i64 val;
        memcpy(&val, value, sizeof(i64));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kHugeInt> {
public:
    static u32
    Size() {
        return sizeof(HugeInt);
    }

    static void
    Encode(const void* value, std::string& buf) {
        HugeInt val;
        memcpy(&val, value, sizeof(HugeInt));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kDecimal> {
public:
    static u32
    Size() {
        return sizeof(DecimalType);
    }

    static void
    Encode(const void* value, std::string& buf) {
        DecimalType decimal_val;
        memcpy(&decimal_val, value, sizeof(DecimalType));
        buf.append((char*)&decimal_val, sizeof(decimal_val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kFloat> {
public:
    static u32
    Size() {
        return sizeof(FloatT);
    }

    static void
    Encode(const void* value, std::string& buf) {
        FloatT val;
        memcpy(&val, value, sizeof(FloatT));
        buf.append((char*)&val, sizeof(FloatT));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kDouble> {
public:
    static u32
    Size() {
        return sizeof(DoubleT);
    }

    static void
    Encode(const void* value, std::string& buf) {
        DoubleT val;
        memcpy(&val, value, sizeof(DoubleT));
        buf.append((char*)&val, sizeof(DoubleT));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kDate> {
public:
    static u32
    Size() {
        return sizeof(DateType);
    }

    static void
    Encode(const void* value, std::string& buf) {
        DateType val;
        memcpy(&val, value, sizeof(DateType));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kTime> {
public:
    static u32
    Size() {
        return sizeof(TimeType);
    }

    static void
    Encode(const void* value, std::string& buf) {
        TimeType val;
        memcpy(&val, value, sizeof(TimeType));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kDateTime> {
public:
    static u32
    Size() {
        return sizeof(DateTimeType);
    }

    static void
    Encode(const void* value, std::string& buf) {
        DateTimeType val;
        memcpy(&val, value, sizeof(DateTimeType));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kTimestamp> {
public:
    static u32
    Size() {
        return sizeof(TimestampType);
    }

    static void
    Encode(const void* value, std::string& buf) {
        TimestampType val;
        memcpy(&val, value, sizeof(TimestampType));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kInterval> {
public:
    static u32
    Size() {
        return sizeof(IntervalT);
    }

    static void
    Encode(const void* value, std::string& buf) {
        IntervalT val;
        memcpy(&val, value, sizeof(IntervalT));
        buf.append((char*)&val, sizeof(val));
    }
};

template<>
class KeyEncoderTraits<LogicalType::kVarchar> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
        auto slice = reinterpret_cast<const std::string*>(value);
        buf.append(slice->data(), slice->size());
    }
};

template<>
class KeyEncoderTraits<LogicalType::kArray> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kTuple> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kPoint> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kLine> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kLineSeg> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kBox> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kPath> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kPolygon> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kCircle> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kBitmap> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kUuid> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kBlob> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kEmbedding> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kRowID> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kMixed> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kNull> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kMissing> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

template<>
class KeyEncoderTraits<LogicalType::kInvalid> {
public:
    static u32
    Size() {
        return -1;
    }

    static void
    Encode(const void* value, std::string& buf) {
    }
};

}