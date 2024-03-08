module;

export module pool_allocator;
import stl;
import memory_pool;

namespace infinity {

export template <typename T>
class PoolAllocator {
public:
    typedef SizeT size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T *pointer;
    typedef const T *const_pointer;
    typedef T &reference;
    typedef const T &const_reference;
    typedef T value_type;

    template <typename T1>
    struct rebind {
        typedef PoolAllocator<T1> other;
    };

    PoolAllocator(MemoryPool *pool) throw() : pool_(pool) {}

    PoolAllocator(const PoolAllocator &other) throw() : pool_(other.pool_) {}

    template <typename T1>
    PoolAllocator(const PoolAllocator<T1> &other) throw() : pool_(other.pool_) {}

    ~PoolAllocator() throw() {}

    pointer address(reference __x) const { return &__x; }

    const_pointer address(const_reference __x) const { return &__x; }

    // NB: __n is permitted to be 0.  The C++ standard says nothing
    // about what the return value is when __n == 0.
    pointer allocate(size_type __n, const void * = 0) { return static_cast<T *>(pool_->Allocate(__n * sizeof(T))); }

    void deallocate(pointer __p, size_type __n) {
        if (pool_) {
            pool_->Deallocate((void *)__p, __n * sizeof(T));
        }
    }

    size_type max_size() const throw() { return SizeT(-1) / sizeof(T); }

    void construct(pointer __p, const T &__val) { ::new (__p) T(__val); }

    void construct(pointer __p, T &&__val) { ::new (__p) T(std::move(__val)); }

    void destroy(pointer __p) { __p->~T(); }

public:
    MemoryPool *pool_{nullptr};
};

template <typename T>
inline bool operator==(const PoolAllocator<T> &lhs, const PoolAllocator<T> &rhs) {
    return lhs.pool_ == rhs.pool_;
}

template <typename T>
inline bool operator!=(const PoolAllocator<T> &lhs, const PoolAllocator<T> &rhs) {
    return !(lhs == rhs);
}

} // namespace infinity
