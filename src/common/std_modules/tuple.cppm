module;
#include <tuple>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:tuple;
export namespace std_lib {
    using std::tuple;
    using std::get;
    using std::tie;
    using std::make_tuple;
    using std::ignore;
}
