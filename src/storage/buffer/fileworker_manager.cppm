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

import :bmp_index_file_worker;
import :data_file_worker;
import :emvb_index_file_worker;
import :hnsw_file_worker;
import :ivf_index_file_worker;
import :raw_file_worker;
import :secondary_index_file_worker;
import :var_file_worker;
import :version_file_worker;

import std;

namespace infinity {

export template <typename FileWorkerT>
struct FileWorkerMap {
    FileWorkerT *EmplaceFileWorker(std::unique_ptr<FileWorkerT> file_worker);

    void RemoveImport(TransactionID txn_id);

    FileWorkerT *GetFileWorker(const std::string &rel_file_path);

    void AddToCleanList(FileWorkerT *file_worker);

    void ClearCleans();

    void MoveFiles();

    mutable std::shared_mutex rw_mtx_;
    std::unordered_map<std::string, std::unique_ptr<FileWorkerT>> map_;

    mutable std::shared_mutex rw_clean_mtx_;
    std::vector<FileWorkerT *> cleans_;
};

export class FileWorkerManager {
public:
    explicit FileWorkerManager(std::shared_ptr<std::string> data_dir, std::shared_ptr<std::string> temp_dir);

    void Start();
    void Stop();

    // auto &fileworker_map() { return fileworker_map_; }

    std::shared_ptr<std::string> GetFullDataDir() const { return data_dir_; }

    std::shared_ptr<std::string> GetTempDir() const { return temp_dir_; }

    size_t FileWorkerCount();

    Status RemoveCleanList();

    void RemoveImport(TransactionID txn_id);

    inline PersistenceManager *persistence_manager() const { return persistence_manager_; }

    std::mutex mutex_;
    void MoveFiles();

    FileWorkerMap<BMPIndexFileWorker> bmp_map_;
    FileWorkerMap<DataFileWorker> data_map_;
    FileWorkerMap<EMVBIndexFileWorker> emvb_map_;
    FileWorkerMap<HnswFileWorker> hnsw_map_;
    FileWorkerMap<IVFIndexFileWorker> ivf_map_;
    FileWorkerMap<RawFileWorker> raw_map_;
    FileWorkerMap<SecondaryIndexFileWorker> secondary_map_;
    FileWorkerMap<VarFileWorker> var_map_;
    FileWorkerMap<VersionFileWorker> version_map_;

    // private:
    std::shared_ptr<std::string> data_dir_;
    std::shared_ptr<std::string> temp_dir_;
    PersistenceManager *persistence_manager_{};

    // std::shared_mutex w_locker_;
    // std::unordered_map<std::string, std::unique_ptr<FileWorker>> fileworker_map_;
};

} // namespace infinity
