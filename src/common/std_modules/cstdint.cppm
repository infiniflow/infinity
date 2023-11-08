module;
#include <cstddef>
#include <cstdint>
# 4 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:cstdint;
export namespace std_lib {
    using std::int8_t;
    using std::int16_t;
    using std::int32_t;
    using std::int64_t;
    using std::uint8_t;
    using std::uint16_t;
    using std::uint32_t;
    using std::uint64_t;
    using std::size_t;

    using std::nullptr_t;
}
