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
    DownloadTask(const String &_file_dir, const String& _object_name) : BaseObjectStorageTask(ObjectStorageTaskType::kDownload) , file_dir(_file_dir), object_name(_object_name){}

    ~DownloadTask() = default;

    String ToString() const final { return "Download Task"; }
    String file_dir;
    String object_name;
};

export struct UploadTask final : public BaseObjectStorageTask {
    UploadTask(const String &_file_dir, const String& _object_name) : BaseObjectStorageTask(ObjectStorageTaskType::kUpload), file_dir(_file_dir), object_name(_object_name){}

    ~UploadTask() = default;

    String ToString() const final { return "Download Task"; }
    String file_dir;
    String object_name;
};

export struct CopyTask final : public BaseObjectStorageTask {
    CopyTask(const String &_src_object_name, const String &_dst_object_name) : BaseObjectStorageTask(ObjectStorageTaskType::kCopy), src_object_name(_src_object_name), dst_object_name(_dst_object_name) {}

    ~CopyTask() = default;

    String ToString() const final { return "Copy Task"; }
    String src_object_name;
    String dst_object_name;
};

export struct RemoveTask final : public BaseObjectStorageTask {
    RemoveTask(const String &_object_name) : BaseObjectStorageTask(ObjectStorageTaskType::kRemove), object_name(_object_name) {}

    ~RemoveTask() = default;

    String ToString() const final { return "Remove Task"; }
    String object_name;
};

export struct StopObjectStorageProcessTask final : public BaseObjectStorageTask {
    StopObjectStorageProcessTask() : BaseObjectStorageTask(ObjectStorageTaskType::kStopProcessor) {}

    ~StopObjectStorageProcessTask() = default;

    String ToString() const final { return "Stop processor"; }
};

} // namespace infinity