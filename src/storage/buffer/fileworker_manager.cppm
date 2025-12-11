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

export template <typename DataT>
struct FileWorkerCacheManager {
    void Pin(std::string path) {
        auto &cnt = ref_cnt_[path];
        ++cnt;
    }

    void UnPin(std::string path) {
        auto &cnt = ref_cnt_[path];
        --cnt;
        if (!cnt) {
            cv_.notify_one();
        }
    }

    void Evict() {
        for (auto iter = li_.rbegin(); iter != li_.rend(); ++iter) {
            auto &data = *iter;
            auto &path = rev_dic_[data];
            if (auto ref_cnt_iter = ref_cnt_.find(path); ref_cnt_iter != ref_cnt_.end()) {
                ref_cnt_.erase(path);
                li_.erase(data);
                map_.erase(path);
                delete data;
                return;
            }
        }
    }

    bool Get(std::string path, DataT &data) {
        {
            std::unique_lock l(rw_mutex_);
            if (auto map_iter = map_.find(path); map_iter != map_.end()) {
                Pin(path);
                li_.splice(li_.begin(), li_, map_iter->second);
                data = map_iter->second;
                return true;
            }
        }
        return false;
    }

    void Set(std::string path, DataT data) {
        std::unique_lock l(rw_mutex_);
        if (auto map_iter = map_.find(path); map_iter != map_.end()) {
            li_.splice(li_.begin(), li_, map_iter->second);
        } else {
            cv_.wait(l, [this] { return ref_cnt_.size() < MAX_CAPACITY_; });
            li_.push_front(data);
            map_.emplace(path, li_.begin());
            rev_dic_[data] = path;
        }
    }

    mutable std::shared_mutex rw_mutex_;
    static constexpr size_t MAX_CAPACITY_ = 2;
    std::list<DataT> li_;
    std::unordered_map<DataT, std::string> rev_dic_;
    std::unordered_map<std::string, decltype(li_.begin())> map_;
    std::unordered_map<std::string, size_t> ref_cnt_;
    std::condition_variable_any cv_;
};

export template <typename FileWorkerT, bool = requires(FileWorkerT t) { t.data_; }>
struct FileWorkerMapInjectHelper {};

export template <typename FileWorkerT>
struct FileWorkerMapInjectHelper<FileWorkerT, true> {
    FileWorkerCacheManager<decltype(std::declval<FileWorkerT>().data_)> cache_manager_;
};

export template <typename FileWorkerT>
struct FileWorkerMap : FileWorkerMapInjectHelper<FileWorkerT> {
    FileWorkerT *EmplaceFileWorker(std::unique_ptr<FileWorkerT> file_worker);

    void RemoveImport(TransactionID txn_id);

    FileWorkerT *GetFileWorker(const std::string &rel_file_path);

    FileWorkerT *GetFileWorkerNoLock(const std::string &rel_file_path);

    void MoveToCleans(FileWorkerT *file_worker);

    void ClearCleans();

    void MoveFiles();

    mutable std::shared_mutex rw_mtx_;
    std::unordered_map<std::string, std::unique_ptr<FileWorkerT>> map_;

    std::unordered_set<std::string> active_dic_;

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
