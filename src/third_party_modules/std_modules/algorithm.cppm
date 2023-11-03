module;
#include <algorithm>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:algorithm;
export namespace std {
using std::copy;
using std::count;
using std::find;
using std::find_if;
using std::find_if_not;
using std::max;
using std::min;
using std::reverse_copy;
using std::sort;
using std::reverse;
} // namespace std
