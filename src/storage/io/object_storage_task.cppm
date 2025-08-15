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

export module infinity_core:object_storage_task;

import :stl;

namespace infinity {

export enum class ObjectStorageTaskType {
    kInvalid,
    kDownload,
    kUpload,
    kCopy,
    kRemove,
    kStopProcessor,
    kLocalDrop,
};

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

    virtual std::string ToString() const = 0;
};

export struct DownloadTask final : public BaseObjectStorageTask {
    DownloadTask(const std::string &_file_dir, const std::string &_object_name)
        : BaseObjectStorageTask(ObjectStorageTaskType::kDownload), file_dir(_file_dir), object_name(_object_name) {}

    ~DownloadTask() = default;

    std::string ToString() const final { return "Download Task"; }
    std::string file_dir;
    std::string object_name;
};

export struct UploadTask final : public BaseObjectStorageTask {
    UploadTask(const std::string &_file_dir, const std::string &_object_name)
        : BaseObjectStorageTask(ObjectStorageTaskType::kUpload), file_dir(_file_dir), object_name(_object_name) {}

    ~UploadTask() = default;

    std::string ToString() const final { return "Download Task"; }
    std::string file_dir;
    std::string object_name;
};

export struct CopyTask final : public BaseObjectStorageTask {
    CopyTask(const std::string &_src_object_name, const std::string &_dst_object_name)
        : BaseObjectStorageTask(ObjectStorageTaskType::kCopy), src_object_name(_src_object_name), dst_object_name(_dst_object_name) {}

    ~CopyTask() = default;

    std::string ToString() const final { return "Copy Task"; }
    std::string src_object_name;
    std::string dst_object_name;
};

export struct RemoveTask final : public BaseObjectStorageTask {
    RemoveTask(const std::string &_object_name) : BaseObjectStorageTask(ObjectStorageTaskType::kRemove), object_name(_object_name) {}

    ~RemoveTask() = default;

    std::string ToString() const final { return "Remove Task"; }
    std::string object_name;
};

export struct StopObjectStorageProcessTask final : public BaseObjectStorageTask {
    StopObjectStorageProcessTask() : BaseObjectStorageTask(ObjectStorageTaskType::kStopProcessor) {}

    ~StopObjectStorageProcessTask() = default;

    std::string ToString() const final { return "Stop processor"; }
};

export struct LocalDropTask final : public BaseObjectStorageTask {
    LocalDropTask(std::string drop_path) : BaseObjectStorageTask(ObjectStorageTaskType::kLocalDrop), drop_path_(drop_path) {}

    ~LocalDropTask() = default;

    std::string ToString() const final { return "Local Drop Task"; }
    std::string drop_path_;
};

} // namespace infinity