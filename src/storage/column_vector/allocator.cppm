//
// Created by jinhai on 23-10-17.
//
module;

import stl;
import global_resource_usage;

export module allocator;


namespace infinity {

export class Allocator {
public:
    static ptr_t allocate(SizeT bytes) {
        GlobalResourceUsage::IncrRawMemCount();
        return new char[bytes]{};
    }

    static void deallocate(ptr_t ptr) {
        delete[] ptr;
        GlobalResourceUsage::DecrRawMemCount();
    }
};

} // namespace infinity