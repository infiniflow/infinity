module;
#include <list>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:list;
export namespace std_lib {
    using std::list;
    using std::size;
}
