module;
#include <algorithm>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:algorithm;
export namespace std {
    using std::sort;
    using std::reverse_copy;
    using std::copy;
    using std::min;
    using std::count;
}
