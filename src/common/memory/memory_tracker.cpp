#include "memory_tracker.h"

#include <mutex>

namespace infinity {

SharedPtr<MemoryTracker>
MemoryTracker::Create(
    const SharedPtr<MemoryTracker>& parent,
    i64 max_memory) {
    auto* tracker = new MemoryTracker(parent, max_memory);
    return SharedPtr<MemoryTracker>(tracker);
}

}