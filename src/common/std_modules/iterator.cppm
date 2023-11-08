module;
#include <iterator>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:iterator;
export namespace std_lib {
    using std::ostream_iterator;
    using std::iterator_traits;
    using std::distance;
    using std::operator-;
    using std::operator==;
    using std::operator!=;

    using std::begin;
    using std::cbegin;
    using std::end;
    using std::cend;
}
