module;
#include <cstdlib>
#include <ctime>
# 4 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:cstdlib;
export namespace std {
    using std::rand;
    using std::srand;
    using std::time;
    using std::getenv;
    using std::strtoll;
}
