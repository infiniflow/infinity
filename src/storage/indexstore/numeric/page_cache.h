#pragma once

#include "page_handle.h"
#include "concurrentqueue.h"
#include <memory>

namespace infinity{

class PageCache{
public:
    PageCache(size_t num_elements) {
        fifo_queue_ = std::make_unique<moodycamel::ConcurrentQueue<PageHandle *>>(num_elements);
    }

    ~PageCache() = default;

    void Insert(PageHandle* handle) {
        fifo_queue_->enqueue(handle);
    }

    void Delete(PageHandle* handle) {

    }

    PageHandle* Evict() {
        PageHandle* dequeued_buffer_handle = nullptr;
        if(fifo_queue_->try_dequeue(dequeued_buffer_handle)) {
            dequeued_buffer_handle->UnsetEviction();
        }
        return dequeued_buffer_handle;
    }

private:
    std::unique_ptr<moodycamel::ConcurrentQueue<PageHandle *>> fifo_queue_;
};
}