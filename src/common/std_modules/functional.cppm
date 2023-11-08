module;
#include <functional>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:functional;
export namespace std_lib {
    using std::function;
    using std::invoke;
    using std::bind;
    
    using std::ref;
    using std::cref;

    using std::mem_fn;

#if defined(__GLIBCXX__) || defined(__GLIBCPP__)
    // We can't using operator== directly. Otherwise it would find the friend declaration in
    // unordered_map, which is disallowed.
    template<typename T>
    inline bool
    operator==(const std::function<T>& __f, std::nullptr_t) noexcept {
        return __f == nullptr;
    }
#endif
}

// FIXME: We couldn't export noninline-const variables.
namespace std_lib::placeholders {
    export inline constexpr decltype(std::placeholders::_1) _1_placeholder = std::placeholders::_1;
    export inline constexpr decltype(std::placeholders::_2) _2_placeholder = std::placeholders::_2;
    export inline constexpr decltype(std::placeholders::_3) _3_placeholder = std::placeholders::_3;
}
