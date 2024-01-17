// Copyright Vespa.ai. Licensed under the terms of the Apache 2.0 license. See LICENSE in the project root.

#include "memoryusage.h"
#include <sstream>

namespace vespalib {

std::string MemoryUsage::toString() const {
    std::stringstream os;
    os << *this;
    return os.str();
}

std::stringstream &operator<<(std::stringstream &os, const MemoryUsage &usage) {
    os << "allocated: " << usage.allocatedBytes();
    os << ", used: " << usage.usedBytes();
    os << ", dead: " << usage.deadBytes();
    os << ", onhold: " << usage.allocatedBytesOnHold();
    return os;
}

} // namespace vespalib
