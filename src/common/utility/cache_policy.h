#pragma once

#include <functional>
#include <memory>
#include <mutex>

namespace infinity {

template <typename TKey, typename TMapped, typename HashFunction = std::hash<TKey>>
class ICachePolicy {
public:
    using Key = TKey;
    using Mapped = TMapped;
    using MappedPtr = std::shared_ptr<Mapped>;

    virtual size_t Count(std::lock_guard<std::mutex> & /* cache_lock */) const = 0;
    virtual size_t MaxSize() const = 0;

    virtual void Reset() = 0;
    virtual void Remove(const Key & key) = 0;
    virtual MappedPtr Get(const Key & key) = 0;
    virtual void Set(const Key & key, const MappedPtr & mapped) = 0;

    virtual ~ICachePolicy() = default;
};

}
