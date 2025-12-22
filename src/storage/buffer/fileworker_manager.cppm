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
class FileWorkerCacheManager {
public:
    void Pin(std::string_view path) {
        // std::lock_guard l(ref_cnt_mutex_);
        std::lock_guard l(mutex_);
        auto &cnt = ref_cnt_map_[path.data()];
        ++cnt;
    }

    void UnPin(std::string_view path) {
        // std::lock_guard l(ref_cnt_mutex_);
        std::lock_guard l(mutex_);
        auto &cnt = ref_cnt_map_[path.data()];
        --cnt;
        if (!cnt) {
            ref_cnt_map_.erase(path.data());
        }
    }

    bool Get(std::string_view path, DataT &data) {
        {
            std::lock_guard l(mutex_);
            if (auto map_iter = path_data_map_.find(path.data()); map_iter != path_data_map_.end()) {
                payloads_.splice(payloads_.begin(), payloads_, map_iter->second);
                data = *map_iter->second;
                return true;
            }
        }
        return false;
    }

    void Set(std::string_view path, DataT data, size_t request_space) {
        std::lock_guard l(mutex_);
        if (auto map_iter = path_data_map_.find(path.data()); map_iter != path_data_map_.end()) {
            payloads_.splice(payloads_.begin(), payloads_, map_iter->second);
        } else {
            if (!IsAccomodatable(request_space)) {
                Evict(request_space);
            }
            payloads_.push_front(data);
            path_data_map_.emplace(path, payloads_.begin());
            data_path_map_[data] = path;
            memory_map_[path.data()] = request_space;
            current_mem_usage_ += request_space;
        }
    }

private:
    void Evict(size_t request_space) {
        for (auto iter = payloads_.rbegin(); iter != payloads_.rend(); ++iter) {
            auto data = *iter;
            auto &path = data_path_map_[data];

            if (!ref_cnt_map_.contains(path)) { // not pin
                current_mem_usage_ -= memory_map_[path];
                ref_cnt_map_.erase(path);
                payloads_.erase(std::next(iter.base(), -1));
                path_data_map_.erase(path);
                data_path_map_.erase(data);
                delete data;
                // ClearData();
                if (IsAccomodatable(request_space)) {
                    return;
                }
            }
        }
        UnrecoverableError("Buffer manager's memory size is too small.");
    }

    bool IsAccomodatable(size_t request_space) { return current_mem_usage_ + request_space <= MAX_CAPACITY_; }
    // void ClearData() { // should implement as a hook
    //     munmap(mmap_, mmap_size_);
    //     mmap_ = nullptr;
    //     auto mem_usage = data_->MemUsage();
    //     auto *memindex_tracer = InfinityContext::instance().storage()->memindex_tracer();
    //     if (memindex_tracer != nullptr) {
    //         memindex_tracer->DecreaseMemUsed(mem_usage);
    //     }
    //     delete data_;
    // }

    // mutable std::shared_mutex rw_mutex_;
    mutable std::mutex mutex_;
    // static constexpr size_t MAX_BUCKET_NUM_ = 42;
    size_t MAX_CAPACITY_ = InfinityContext::instance().config()->BufferManagerSize();
    size_t current_mem_usage_{};
    std::list<DataT> payloads_;
    std::unordered_map<DataT, std::string> data_path_map_;
    std::unordered_map<std::string, decltype(payloads_.begin())> path_data_map_;
    std::unordered_map<std::string, size_t> ref_cnt_map_;
    std::unordered_map<std::string, size_t> memory_map_;
};

export template <typename FileWorkerT, bool = requires(FileWorkerT t) { t.has_cache_manager_; }>
struct FileWorkerMapInjectHelper {};

export template <typename FileWorkerT>
struct FileWorkerMapInjectHelper<FileWorkerT, true> {
    using data_t = std::remove_cvref_t<decltype(std::declval<FileWorkerT>().has_cache_manager_)>;
    FileWorkerCacheManager<data_t> cache_manager_;
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
