module;
#include <limits>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:limits;
export namespace std_lib {
    using std::numeric_limits;
}