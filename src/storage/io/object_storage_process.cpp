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

#include <cassert>

module object_storage_process;

import stl;
import object_storage_task;
import logger;
import blocking_queue;
import infinity_exception;
import third_party;

namespace infinity {

void ObjectStorageProcess::Start() {
    processor_thread_ = Thread([this] { Process(); });
    LOG_INFO("Object storage processor is started.");
}

void ObjectStorageProcess::Stop() {
    LOG_INFO("Object storage processor is stopping.");
    SharedPtr<StopObjectStorageProcessTask> stop_task = MakeShared<StopObjectStorageProcessTask>();
    task_queue_.Enqueue(stop_task);
    stop_task->Wait();
    processor_thread_.join();
    LOG_INFO("Object storage processor is stopped.");
}

void ObjectStorageProcess::Submit(SharedPtr<BaseObjectStorageTask> object_storage_task) {
    ++task_count_;
    task_queue_.Enqueue(std::move(object_storage_task));
}

void ObjectStorageProcess::Process() {
    bool running{true};
    Deque<SharedPtr<BaseObjectStorageTask>> tasks;
    while (running) {
        task_queue_.DequeueBulk(tasks);
        for (const auto &object_storage_task : tasks) {
            switch (object_storage_task->type_) {
                case ObjectStorageTaskType::kStopProcessor: {
                    LOG_INFO("Stop the Object storage processor");
                    {
                        std::unique_lock<std::mutex> locker(task_mutex_);
                        task_text_ = object_storage_task->ToString();
                    }
                    running = false;
                    break;
                }
                case ObjectStorageTaskType::kDownload: {
                    LOG_DEBUG("Download task");
//                    DownloadTask *download_task = static_cast<DownloadTask *>(object_storage_task.get());
//                    assert(download_task != nullptr);
                    LOG_DEBUG("Download task done");
                    break;
                }
                case ObjectStorageTaskType::kUpload: {
                    LOG_DEBUG("Upload task");
//                    UploadTask *upload_task = static_cast<UploadTask *>(object_storage_task.get());
//                    assert(upload_task != nullptr);
                    LOG_DEBUG("Upload task done");
                    break;
                }
                case ObjectStorageTaskType::kCopy: {
                    LOG_DEBUG("Copy task");
//                    CopyTask *copy_task = static_cast<CopyTask *>(object_storage_task.get());
//                    assert(copy_task != nullptr);
                    LOG_DEBUG("Copy task done");
                    break;
                }
                case ObjectStorageTaskType::kRemove: {
                    LOG_DEBUG("Remove task");
//                    RemoveTask *remove_task = static_cast<RemoveTask *>(object_storage_task.get());
//                    assert(remove_task != nullptr);
                    LOG_DEBUG("Remove task done");
                    break;
                }
                default: {
                    String error_message = fmt::format("Invalid object storage: {}", (u8)object_storage_task->type_);
                    UnrecoverableError(error_message);
                    break;
                }
            }
            task_text_.clear();
            object_storage_task->Complete();
        }
        task_count_ -= tasks.size();
        tasks.clear();
    }
}

} // namespace infinity
