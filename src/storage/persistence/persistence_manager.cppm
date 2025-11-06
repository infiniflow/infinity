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

export module infinity_core:persistence_manager;

import :obj_status;
import :status;

import std.compat;
import third_party;

import serialize;

// A view means a logical plan
namespace infinity {

class KVStore;
class KVInstance;
class ObjectStatAccessor;
class ObjectStats;
export class Storage;

export struct ObjAddr {
    std::string obj_key_{};
    size_t part_offset_{};
    size_t part_size_{};

    ObjAddr() {}
    ObjAddr(const std::string &obj_key, size_t part_offset, size_t part_size) : obj_key_(obj_key), part_offset_(part_offset), part_size_(part_size) {}

    bool Valid() const { return !obj_key_.empty(); }

    nlohmann::json Serialize() const;

    void Deserialize(std::string_view obj_str);

    // size_t GetSizeInBytes() const;
    // void WriteBufAdv(char *&buf) const;
    // static ObjAddr ReadBufAdv(const char *&buf);
    static constexpr std::string KeyEmpty = "KEY_EMPTY";
};

export struct PersistWriteResult {
    ObjAddr obj_addr_;                               // where data is persisted, only returned by Persist
    std::vector<std::string> persist_keys_;          // object that should be persisted to local disk. because of cleanup current_object
    std::vector<std::string> drop_keys_;             // object that should be removed from local disk. because of 1. disk used over limit
    std::vector<std::string> drop_from_remote_keys_; // object that should be removed from remote storage. because of object's all parts are deleted
};

export struct PersistReadResult {
    ObjAddr obj_addr_;                               // where data should read from
    std::vector<std::string> drop_keys_;             // object that should be removed from local disk. because of 1. disk used over limit
    std::vector<std::string> drop_from_remote_keys_; // object that should be removed from remote storage. because of object's all parts are deleted
    std::shared_ptr<ObjStat> obj_stat_{nullptr};     // object stat
};

export class PersistenceManager {
public:
    constexpr static size_t ObjAlignment = 8;

    // TODO: build cache from existing files under workspace
    PersistenceManager(Storage *storage,
                       const std::string &workspace,
                       const std::string &data_dir,
                       size_t object_size_limit,
                       bool local_storage = true);

    ~PersistenceManager();

    // Create new object or append to current object, and returns the location.
    // file_path is the key of local_path_obj_ and may not exist. tmp_file_path is the file which contains the data to be persisted.
    // tmp_file_path will be deleted after its data be persisted.
    [[nodiscard]] PersistWriteResult Persist(const std::string &file_path, const std::string &tmp_file_path, bool try_compose = true);

    // Force finalize current object. Subsequent append on the finalized object is forbidden.
    // IMPORT / COMPACT / OPTIMIZE / DUMP MEM INDEX operations should call this method to finalize the current object.
    [[nodiscard]] PersistWriteResult CurrentObjFinalize(bool validate = false);

    // Download the whole object from object store if it's not in cache. Increase refcount and return the cached object file path.
    [[nodiscard]] PersistReadResult GetObjCache(const std::string &local_path);

    std::tuple<size_t, Status> GetFileSize(const std::string &file_path);

    // std::tuple<size_t, Status> GetDirectorySize(const std::string &path_str);
    // ObjAddr GetObjCacheWithoutCnt(const std::string &local_path);

    [[nodiscard]] PersistWriteResult PutObjCache(const std::string &file_path);

    [[nodiscard]] PersistWriteResult Cleanup(const std::string &file_path);

    // Clean up stale object data that has no corresponding file path
    PersistWriteResult CleanupStaleObjectData();

    /**
     * Utils
     */
    KVStore *kv_store() const { return kv_store_; }
    void SetKvStore(KVStore *kv_store);

    std::string GetObjPath(const std::string &obj_key) const { return std::filesystem::path(workspace_).append(obj_key).string(); }

    std::unordered_map<std::string, std::shared_ptr<ObjStat>> GetAllObjects() const;
    std::unordered_map<std::string, ObjAddr> GetAllFiles() const;

private:
    std::string ObjCreate();

    // Returns the room (size limit - sum_of_parts_size) of current object. User should check before each ObjAppend operation.
    int CurrentObjRoomNoLock();

    // Append file to the current object.
    // It finalizes current object if new size exceeds the size limit.
    void CurrentObjAppendNoLock(const std::string &tmp_file_path, size_t file_size);

    // Finalize current object.
    void CurrentObjFinalizeNoLock(std::vector<std::string> &persist_keys);

    // Cleanup
    void CleanupNoLock(const ObjAddr &object_addr,
                       std::vector<std::string> &persist_keys,
                       std::vector<std::string> &drop_from_remote_keys,
                       bool check_ref_count = false);

    std::string RemovePrefix(const std::string &path);

    // ObjStat GetObjStatByObjAddr(const ObjAddr &obj_addr);

    void CheckValid();

public: // for unit test
        // void SaveLocalPath(const std::string &local_path, const ObjAddr &object_addr);
private:
    // void SaveObjStat(const std::string &obj_key, const std::shared_ptr<ObjStat> &obj_stat);

    // void AddObjAddrToKVStore(const std::string &path, const ObjAddr &obj_addr);

    Storage *storage_;
    KVStore *kv_store_{nullptr};
    std::string workspace_;
    std::string local_data_dir_;
    size_t object_size_limit_;

    mutable std::mutex mtx_;
    // std::unordered_map<std::string, ObjStat> object_stats_;        // obj_key -> ObjStat
    // std::shared_ptr<ObjectStatAccessor> object_stats_; // obj_key -> ObjStat
    std::shared_ptr<ObjectStats> object_stats_{};
    // Current unsealed object key
    std::string current_object_key_;
    size_t current_object_size_ = 0;
    size_t current_object_parts_ = 0;
    size_t current_object_ref_count_ = 0;
    friend struct AddrSerializer;
};

} // namespace infinity
