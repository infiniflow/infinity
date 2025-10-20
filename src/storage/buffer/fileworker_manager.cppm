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

export module infinity_core:fileworker_manager;

import :file_worker;
import :default_values;

import std;

namespace infinity {

class KVInstance;
class PersistenceManager;
struct ObjAddr;
class Status;

export class FileWorkerManager {
public:
    explicit FileWorkerManager(std::shared_ptr<std::string> data_dir, std::shared_ptr<std::string> temp_dir);

public:
    void Start();
    void Stop();

    auto &fileworker_map() { return fileworker_map_; }

    // Create a new BufferHandle, or in replay process. (read data block from wal)
    FileWorker *EmplaceFileWorker(std::unique_ptr<FileWorker> file_worker);

    FileWorker *GetFileWorker(const std::string &rel_file_path);

    std::shared_ptr<std::string> GetFullDataDir() const { return data_dir_; }

    std::shared_ptr<std::string> GetTempDir() const { return temp_dir_; }

    size_t BufferedObjectCount();

    Status RemoveCleanList(KVInstance *kv_instance);

    inline PersistenceManager *persistence_manager() const { return persistence_manager_; }

    void AddToCleanList(FileWorker *fileworker);

private:
    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;
    PersistenceManager *persistence_manager_{};

    std::mutex w_locker_;
    std::unordered_map<std::string, std::unique_ptr<FileWorker>> fileworker_map_;

    std::mutex clean_locker_;
    std::vector<FileWorker *> clean_list_{};
};

} // namespace infinity
