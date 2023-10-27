//
// Created by jinhai on 23-10-17.
//

module;

import stl;

export module serialize;

namespace infinity {

//template <typename T>
//concept POD = IsTrivial<T> && IsStandLayout<T>;

export template <typename T>
inline i32 GetSizeInBytes(const T &value) {
    static_assert(IsStandLayout<T>, "T must be POD");
    return sizeof(T);
}

export template <>
inline i32 GetSizeInBytes(const String &value) {
    return sizeof(i32) + value.length();
}

export template <typename T>
inline T ReadBuf(char *const buf) {
    static_assert(IsStandLayout<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    return value;
}

export template <typename T>
inline T ReadBufAdv(char *&buf) {
    static_assert(IsStandLayout<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    buf += sizeof(T);
    return value;
}

export template <>
inline String ReadBuf<String>(char *const buf) {
    i32 size = ReadBuf<i32>(buf);
    String str(buf + sizeof(i32), size);
    return str;
}

export template <>
inline String ReadBufAdv<String>(char *&buf) {
    i32 size = ReadBufAdv<i32>(buf);
    String str(buf, size);
    buf += size;
    return str;
}

export template <typename T>
inline void WriteBuf(char *const buf, const T &value) {
    static_assert(IsStandLayout<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
}

export template <typename T>
inline void WriteBufAdv(char *&buf, const T &value) {
    static_assert(IsStandLayout<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
    buf += sizeof(T);
}

export template <>
inline void WriteBuf<String>(char *const buf, const String &value) {
    i32 len = value.length();
    WriteBuf(buf, len);
    Memcpy(buf + len, value.c_str(), len);
}

export template <>
inline void WriteBufAdv<String>(char *&buf, const String &value) {
    i32 len = value.length();
    WriteBufAdv(buf, len);
    Memcpy(buf, value.c_str(), len);
    buf += len;
}

} // namespace infinity