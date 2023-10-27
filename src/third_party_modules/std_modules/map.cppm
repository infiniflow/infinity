module;
#include <map>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:map;
export namespace std {
    using std::map;
#if defined(__GLIBCXX_) && __GLIBCXX_ < 20200723
    using std::erase_if;
#endif
}
