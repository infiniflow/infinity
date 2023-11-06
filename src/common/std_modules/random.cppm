module;
#include <random>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:random;
export namespace std {
    using std::uniform_int_distribution;
    using std::random_device;
    using std::mt19937;
    using std::default_random_engine;
    using std::uniform_real_distribution;
}
