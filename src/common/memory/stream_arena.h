#pragma once

#include "tlsf.h"

#include <cstdlib>
#include <cassert>
#include <memory_resource>
#include <iostream>

/*
* Based on Two Level Segregated Fit memory allocator to provide
* fast allocation with less memory fragmentation for allcoations 
* with variable lengths. This is used for stream allocation used
* during aggregation. It's not thread safe.
*/
class StreamArena {
private:
    tlsf_t tlsf_;
    size_t runtime_size_;
    size_t used_size_;
    size_t DefaultPoolSize = 2 * 1024 * 1024;
public:
    StreamArena();
    ~StreamArena();

    void SetDefaultPoolSize(size_t size) { DefaultPoolSize = size;}

    void* Allocate( size_t size );

    void* AllocateAligned( size_t size, size_t alignment );

    void DeAllocate( void* ptr );

    void Reset();
};

class StreamArenaMemoryResource : public std::pmr::memory_resource {
private:
    std::pmr::memory_resource* upstream_;
    StreamArena arena_;
    uint64_t bytes_allocated_;
    uint64_t bytes_freed_;
public:
    explicit StreamArenaMemoryResource(std::pmr::memory_resource* upstream = std::pmr::get_default_resource() )
        : upstream_(upstream) {
    }

    void SetDefaultPoolSize(size_t size) {
    	arena_.SetDefaultPoolSize(size);
    }

private:
    virtual void* do_allocate( size_t size, size_t alignment ) override {
        //auto mem = arena_.AllocateAligned(size, alignment);
        auto mem = arena_.Allocate(size);
        bytes_allocated_ += size;
        return mem;
    }

    virtual void do_deallocate(void* ptr, size_t size, size_t alignment) override {
        arena_.DeAllocate(ptr);
        bytes_freed_ += size;
    }
    //Two memory_resources compare equal if and only if memory allocated from
    //one memory_resource can be deallocated from the other and vice versa.
    virtual bool do_is_equal(const memory_resource& other) const noexcept override {
        if( &other == this ) {
            return true;
        }
        return false;
    }
};