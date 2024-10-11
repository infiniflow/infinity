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

export module object_storage_task;

import stl;
// import txn;
// import catalog;
// import catalog_delta_entry;
// import buffer_manager;
// import third_party;

namespace infinity {

export enum class ObjectStorageTaskType { kDownload, kUpload, kCopy, kRemove, kStopProcessor, kInvalid };

export struct BaseObjectStorageTask {
    BaseObjectStorageTask(ObjectStorageTaskType type) : type_(type) {}

    virtual ~BaseObjectStorageTask() = default;

    ObjectStorageTaskType type_{ObjectStorageTaskType::kInvalid};

    bool complete_{false};
    std::mutex mutex_{};
    std::condition_variable cv_{};

    void Wait() {
        std::unique_lock<std::mutex> locker(mutex_);
        cv_.wait(locker, [this] { return complete_; });
    }

    void Complete() {
        std::unique_lock<std::mutex> locker(mutex_);
        complete_ = true;
        cv_.notify_one();
    }

    virtual String ToString() const = 0;
};

export struct DownloadTask final : public BaseObjectStorageTask {
    DownloadTask() : BaseObjectStorageTask(ObjectStorageTaskType::kDownload) {}

    ~DownloadTask() = default;

    String ToString() const final { return "Download Task"; }
};

export struct UploadTask final : public BaseObjectStorageTask {
    UploadTask() : BaseObjectStorageTask(ObjectStorageTaskType::kUpload) {}

    ~UploadTask() = default;

    String ToString() const final { return "Download Task"; }
};

export struct CopyTask final : public BaseObjectStorageTask {
    CopyTask() : BaseObjectStorageTask(ObjectStorageTaskType::kCopy) {}

    ~CopyTask() = default;

    String ToString() const final { return "Copy Task"; }
};

export struct RemoveTask final : public BaseObjectStorageTask {
    RemoveTask() : BaseObjectStorageTask(ObjectStorageTaskType::kRemove) {}

    ~RemoveTask() = default;

    String ToString() const final { return "Remove Task"; }
};

export struct StopObjectStorageProcessTask final : public BaseObjectStorageTask {
    StopObjectStorageProcessTask() : BaseObjectStorageTask(ObjectStorageTaskType::kStopProcessor) {}

    ~StopObjectStorageProcessTask() = default;

    String ToString() const final { return "Stop processor"; }
};

} // namespace infinity