module;
#include <exception>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:exception;
export namespace std {
    using std::exception_ptr;
    using std::exception;
}

#if defined(__GLIBCXX__) &&  __GLIBCXX__ < 20220506
export namespace std::__exception_ptr {
    using std::__exception_ptr::operator==;
}
#endif
