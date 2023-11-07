module;
#include <thread>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:thread;
export namespace std_lib {
    using std::thread;
    namespace this_thread {
        using std::this_thread::sleep_for;
        using std::this_thread::yield;
        using std::this_thread::get_id;
    }
}
