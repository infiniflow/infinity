// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

module;

import stl;

export module async_task;

namespace infinity {

export enum class AsyncTaskType {
    kTerminate,
    kBufferRead,
    kBufferWrite,
    kDummy,
    kCommit,
    kInvalid,
};

export class AsyncTask {
public:
    explicit AsyncTask(AsyncTaskType task_type) : task_type_(task_type) {}

    virtual ~AsyncTask() = default;

    [[nodiscard]] inline bool IsTerminated() const { return task_type_ == AsyncTaskType::kTerminate; }

    virtual String ToString() = 0;

    virtual void Prepare() = 0;

    virtual void Commit() = 0;

    [[nodiscard]] inline AsyncTaskType TaskType() const { return task_type_; }

    inline void Wait() {
        UniqueLock<Mutex> lk(locker_);
        //        completed_ = false;
        cv_.wait(lk, [&] { return completed_; });
        //        flag_.wait(true);
    }

    inline void Notify() {
        UniqueLock<Mutex> lk(locker_);
        completed_ = true;
        cv_.notify_all();
        //        flag_.notify_one();
    }

    inline void SetError(UniquePtr<String> err_msg) { err_msg_ = Move(err_msg); }

    [[nodiscard]] inline bool IsError() const { return !(err_msg_.get() == nullptr); }

    [[nodiscard]] inline const String &GetError() const { return *err_msg_; }

private:
    AsyncTaskType task_type_{AsyncTaskType::kInvalid};
    bool completed_{false};
    Mutex locker_{};
    CondVar cv_;
    UniquePtr<String> err_msg_{};
    //    std::atomic_bool flag_{false};
};

} // namespace infinity
