module;
#include <initializer_list>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:initializer_list;
export namespace std {
    using std::initializer_list;
}
