module;

// This is a known LLVM bug: https://github.com/llvm/llvm-project/issues/195661
#define PHMAP_HAVE_SSE2 0
#define PHMAP_HAVE_SSSE3 0

#include <parallel_hashmap/phmap.h>

export module infinity_core:parallel_hashmap;

export namespace phmap {
namespace priv {
namespace flat_hash_map {
using ::phmap::priv::raw_hash_set;
using ::phmap::priv::hashtable_debug_internal::HashtableDebugAccess;
} // namespace flat_hash_map
} // namespace priv
}; // namespace phmap

namespace infinity {
export template <class T>
using HashDefaultHash = phmap::priv::hash_default_hash<T>;
export template <class T>
using HashDefaultEQ = phmap::priv::hash_default_eq<T>;
export template <typename K, typename V>
using PHPair = phmap::priv::Pair<const K, V>;
export template <class T>
using PHAlloc = phmap::priv::Allocator<T>;
export template <class K,
                 class V,
                 class Hash = HashDefaultHash<K>,
                 class Eq = HashDefaultEQ<K>,
                 class Alloc = PHAlloc<PHPair<const K, V>>> // alias for std::allocator
using FlatHashMap = phmap::flat_hash_map<K, V, Hash, Eq, Alloc>;

export template <class T,
                 class Hash = HashDefaultHash<T>,
                 class Eq = HashDefaultEQ<T>,
                 class Alloc = PHAlloc<T>> // alias for std::allocator
using FlatHashSet = phmap::flat_hash_set<T, Hash, Eq, Alloc>;
} // namespace infinity