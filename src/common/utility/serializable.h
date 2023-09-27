#include <concepts>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>
#include <vector>

namespace infinity {
using String = std::string;

template <typename T>
concept POD = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

template <typename T> inline int32_t GetSizeInBytes(const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    return sizeof(T);
}

template <> inline int32_t GetSizeInBytes(const String &value) {
    return sizeof(int32_t) + value.length();
}

template <typename T> inline T ReadBuf(char *const buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    return value;
}

template <typename T> inline T ReadBufAdv(char *&buf) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    T value = *ptr;
    buf += sizeof(T);
    return value;
}

template <> inline String ReadBuf<String>(char *const buf) {
    int32_t size = ReadBuf<int32_t>(buf);
    String str(buf + sizeof(int32_t), size);
    return str;
}

template <> inline String ReadBufAdv<String>(char *&buf) {
    int32_t size = ReadBufAdv<int32_t>(buf);
    String str(buf, size);
    buf += size;
    return str;
}

template <typename T> inline void WriteBuf(char *const buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
}

template <typename T> inline void WriteBufAdv(char *&buf, const T &value) {
    static_assert(std::is_standard_layout_v<T>, "T must be POD");
    T *ptr = (T *)buf;
    *ptr = value;
    buf += sizeof(T);
}

template <> inline void WriteBuf<String>(char *const buf, const String &value) {
    int32_t len = value.length();
    WriteBuf(buf, len);
    memcpy(buf + len, value.c_str(), len);
}

template <> inline void WriteBufAdv<String>(char *&buf, const String &value) {
    int32_t len = value.length();
    WriteBufAdv(buf, len);
    memcpy(buf, value.c_str(), len);
    buf += len;
}

} // namespace infinity