module;
#if __has_include(<version>)
#include <version>
#else
#include <cassert>
#endif
#if defined(__GLIBCXX__)
#if __GLIBCXX__ >= 20220728
#include <coroutine>
#else
#include "my_coroutine"
#endif
#elif __has_include(<coroutine>)
#include <coroutine>
#endif
# 16 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std_lib:coroutine;

/**
    coroutine synopsis

namespace std_lib {
// [coroutine.traits]
template <class R, class... ArgTypes>
  struct coroutine_traits;
// [coroutine.handle]
template <class Promise = void>
  struct coroutine_handle;
// [coroutine.handle.compare]
constexpr bool operator==(coroutine_handle<> x, coroutine_handle<> y) noexcept;
constexpr strong_ordering operator<=>(coroutine_handle<> x, coroutine_handle<> y) noexcept;
// [coroutine.handle.hash]
template <class T> struct hash;
template <class P> struct hash<coroutine_handle<P>>;
// [coroutine.noop]
struct noop_coroutine_promise;
template<> struct coroutine_handle<noop_coroutine_promise>;
using noop_coroutine_handle = coroutine_handle<noop_coroutine_promise>;
noop_coroutine_handle noop_coroutine() noexcept;
// [coroutine.trivial.awaitables]
struct suspend_never;
struct suspend_always;
} // namespace std_lib

 */

export namespace std_lib {
    using std::coroutine_traits;
    using std::coroutine_handle;
    using std::noop_coroutine;
    using std::suspend_never;
    using std::suspend_always;
}
