module;
#include <memory>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:memory;
export namespace std {
    using std::unique_ptr;
    using std::make_unique;
    using std::shared_ptr;
    // We can't using it in libstdc++12.
    // using std::make_shared;
    using std::addressof;
    using std::allocator;
    using std::allocator_traits;

    using std::operator!=;
    using std::operator==;
}
