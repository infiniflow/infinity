module;
#include <array>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:array;
export namespace std {
    using std::array;
}
