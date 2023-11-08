module;
#include <condition_variable>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:condition_variable;
export namespace std_lib {
    using std::condition_variable;
}
