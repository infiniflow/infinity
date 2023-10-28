module;
#include <vector>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:vector;
export namespace std {
    using std::vector;
    
#if defined(__GLIBCXX_) && __GLIBCXX_ < 20200723
    using std::erase;
    using std::erase_if;
#endif
    using std::operator!=;
    using std::operator==;
    using std::operator-;
}

#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
export namespace __gnu_cxx {
    using __gnu_cxx::operator==;
    using __gnu_cxx::operator-;
}
#endif

// FIXME: We should put this in <new>.
export {
    using ::operator new;
}
