//
// Created by jinhai on 23-7-20.
//

#pragma once

#include <utility>

#include "common/types/internal_types.h"
#include "common/types/data_type.h"
#include "storage/common/async_batch_processor.h"
#include "buffer_handle.h"

namespace infinity {

class BufferIO {
public:
    static SharedPtr<AsyncTask>
    OnPrepare(List<SharedPtr<AsyncTask>>& async_tasks);

    static void
    OnCommit(const SharedPtr<AsyncTask>& async_task);
};

class BufferReadTask final : public AsyncTask {
public:
    explicit
    BufferReadTask(void* buffer_handle) : AsyncTask(AsyncTaskType::kBufferRead), buffer_handle_(buffer_handle) {}

    inline String
    ToString() override {
        return "BufferReadTask";
    }

    void
    Prepare() final;

    void
    Commit() final;

private:
    void* buffer_handle_{};
};


class BufferWriteTask : public AsyncTask {
public:
    explicit
    BufferWriteTask(void* buffer_handle) : AsyncTask(AsyncTaskType::kBufferWrite), buffer_handle_(buffer_handle) {}

    inline String
    ToString() override {
        return "BufferWriteTask";
    }

    void
    Prepare() final;

    void
    Commit() final;

private:
    void* buffer_handle_{};
};

}
