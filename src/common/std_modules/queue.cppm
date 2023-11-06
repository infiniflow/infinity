module;
#include <queue>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:queue;
export namespace std {
    using std::queue;
    using std::size;
}
