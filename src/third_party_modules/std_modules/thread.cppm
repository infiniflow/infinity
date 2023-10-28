module;
#include <thread>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:thread;
export namespace std {
    using std::thread;
    namespace this_thread {
        using std::this_thread::sleep_for;
        using std::this_thread::yield;
        using std::this_thread::get_id;
    }
}
