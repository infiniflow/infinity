module;

#include <concepts>

import stl;
import std;
import specific_concurrent_queue;

export module hnsw_mem_pool;

namespace infinity {

// use concept for zero cost polymorphism
template <typename F, typename... Args>
concept PooledTypeFunctor = requires(F t) {
    typename F::DataT;
    { F(std::declval<Args>()...) };
    { t.Alloc(std::declval<Args>()...) } -> std::same_as<typename F::DataT>;
    { t.Release(std::declval<typename F::DataT &>()) };
};

template <typename F, typename... Args>
    requires PooledTypeFunctor<F, Args...>
class MemPool;

template <typename F, typename... Args>
    requires PooledTypeFunctor<F, Args...>
class PooledType {
    using MemPoolT = MemPool<F, Args...>;

    MemPoolT *pool_;
    F functor_;
    F::DataT data_;

public:
    PooledType(MemPoolT *pool, Args... args) : pool_(pool), functor_(F(args...)) {}

    PooledType(PooledType &&other) : pool_(other.pool_), data_(Move(other.data_)), functor_(Move(other.functor_)) { other.pool_ = nullptr; }

    PooledType(const PooledType &) = delete;
    PooledType &operator=(const PooledType &) = delete;
    PooledType &operator=(PooledType &&) = delete;

    ~PooledType() {
        if (pool_) {
            functor_.Release(data_);
            pool_->Release(Move(data_));
        }
    }

    F::DataT &operator*() { return data_; }

private:
    friend class MemPool<F, Args...>;
    F::DataT FunctorAlloc(Args... args) { return functor_.Alloc(args...); }
};

template <typename F, typename... Args>
    requires PooledTypeFunctor<F, Args...>
class MemPool {
    SpecificConcurrentQueue<typename F::DataT> pool_;

public:
    using PooledT = PooledType<F, Args...>;

    PooledT Get(Args... args) {
        PooledT result(this, args...);
        if (!pool_.TryDequeue(result.data_)) [[unlikely]] {
            result.data_ = result.FunctorAlloc(args...);
        }
        return result;
    }

private:
    friend class PooledType<F, Args...>;
    void Release(typename F::DataT data) { pool_.Enqueue(Move(data)); }
};

// -------------- implement concept below ----------------

struct PooledVectorBoolFunctor {
    using DataT = Vector<bool>;

    const SizeT used_size_;

    PooledVectorBoolFunctor([[maybe_unused]] SizeT alloc_size, SizeT used_size) : used_size_(used_size) {}

    DataT Alloc(SizeT alloc_size, [[maybe_unused]] SizeT used_size) { return Vector<bool>(alloc_size, false); }

    void Release(DataT &data) {
        std::fill(data.begin(), data.begin() + used_size_, false); // TODO:: memset
    }
};

template <typename T, typename C>
struct PooledMaxHeapFunctor {
    using DataT = Heap<T, C>;

    PooledMaxHeapFunctor() = default;

    DataT Alloc() { return Heap<T, C>(); }

    void Release(DataT &data) {
        while (!data.empty()) {
            data.pop();
        }
    }
};

export using VisitedMemPool = MemPool<PooledVectorBoolFunctor, SizeT, SizeT>;

export template <typename T, typename C>
using MaxHeapMemPool = MemPool<PooledMaxHeapFunctor<T, C>>;

} // namespace infinity