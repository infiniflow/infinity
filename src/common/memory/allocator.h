//
// Created by JinHai on 2022/11/29.
//

#pragma once

#include "main/stats/global_resource_usage.h"

namespace infinity {

class Allocator {
public:
    static ptr_t allocate(size_t bytes) {
        GlobalResourceUsage::IncrRawMemCount();
        return new char[bytes]{};
    }

    static void deallocate(ptr_t ptr) {
        delete[] ptr;
        GlobalResourceUsage::DecrRawMemCount();
    }
};

} // namespace infinity
