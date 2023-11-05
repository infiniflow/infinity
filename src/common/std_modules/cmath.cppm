module;
#include <cmath>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:cmath;
export namespace std {
using std::log;
}