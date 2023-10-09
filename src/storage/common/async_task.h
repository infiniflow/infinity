//
// Created by jinhai on 23-6-2.
//

#pragma once

#include "common/types/alias/smart_ptr.h"
#include "common/types/alias/strings.h"
#include <condition_variable>

namespace infinity {

enum class AsyncTaskType {
    kTerminate,
    kBufferRead,
    kBufferWrite,
    kDummy,
    kCommit,
    kInvalid,
};

class AsyncTask {
public:
    explicit AsyncTask(AsyncTaskType task_type) : task_type_(task_type) {}

    virtual ~AsyncTask() = default;

    [[nodiscard]] inline bool IsTerminated() const { return task_type_ == AsyncTaskType::kTerminate; }

    virtual String ToString() = 0;

    virtual void Prepare() = 0;

    virtual void Commit() = 0;

    [[nodiscard]] inline AsyncTaskType TaskType() const { return task_type_; }

    inline void Wait() {
        std::unique_lock<std::mutex> lk(locker_);
        //        completed_ = false;
        cv_.wait(lk, [&] { return completed_; });
        //        flag_.wait(true);
    }

    inline void Notify() {
        std::unique_lock<std::mutex> lk(locker_);
        completed_ = true;
        cv_.notify_all();
        //        flag_.notify_one();
    }

    inline void SetError(UniquePtr<String> err_msg) { err_msg_ = std::move(err_msg); }

    [[nodiscard]] inline bool IsError() const { return !(err_msg_ == nullptr); }

    [[nodiscard]] inline const String &GetError() const { return *err_msg_; }

private:
    AsyncTaskType task_type_{AsyncTaskType::kInvalid};
    bool completed_{false};
    std::mutex locker_{};
    std::condition_variable cv_;
    UniquePtr<String> err_msg_{};
    //    std::atomic_bool flag_{false};
};

} // namespace infinity
