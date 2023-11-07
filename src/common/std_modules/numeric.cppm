module;
#include <numeric>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:numeric;
export namespace std {
    using std::accumulate;
}
