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

import :stl;
import serialize;
import :third_party;
import :obj_status;
import :status;

// A view means a logical plan
namespace infinity {

class KVStore;
class KVInstance;
class ObjectStatAccessorBase;

export struct ObjAddr {
    String obj_key_{};
    SizeT part_offset_{};
    SizeT part_size_{};

    bool Valid() const { return !obj_key_.empty(); }

    nlohmann::json Serialize() const;

    void Deserialize(std::string_view obj_str);

    SizeT GetSizeInBytes() const;

    void WriteBufAdv(char *&buf) const;

    static ObjAddr ReadBufAdv(const char *&buf);
    static constexpr String KeyEmpty = "KEY_EMPTY";
};

export struct PersistWriteResult {
    ObjAddr obj_addr_;                     // where data is persisted, only returned by Persist
    Vector<String> persist_keys_;          // object that should be persisted to local disk. because of cleanup current_object
    Vector<String> drop_keys_;             // object that should be removed from local disk. because of 1. disk used over limit
    Vector<String> drop_from_remote_keys_; // object that should be removed from remote storage. because of object's all parts are deleted
};

export struct PersistReadResult {
    ObjAddr obj_addr_;                     // where data should read from
    Vector<String> drop_keys_;             // object that should be removed from local disk. because of 1. disk used over limit
    Vector<String> drop_from_remote_keys_; // object that should be removed from remote storage. because of object's all parts are deleted
    ObjStat *obj_stat_{nullptr};           // object stat
};

export class PersistenceManager {
public:
    constexpr static SizeT ObjAlignment = 8;

    // TODO: build cache from existing files under workspace
    PersistenceManager(const String &workspace, const String &data_dir, SizeT object_size_limit, bool local_storage = true);

    ~PersistenceManager();

    // Create new object or append to current object, and returns the location.
    // file_path is the key of local_path_obj_ and may not exist. tmp_file_path is the file which contains the data to be persisted.
    // tmp_file_path will be deleted after its data be persisted.
    [[nodiscard]] PersistWriteResult Persist(const String &file_path, const String &tmp_file_path, bool try_compose = true);

    // Force finalize current object. Subsequent append on the finalized object is forbidden.
    // IMPORT / COMPACT / OPTIMIZE / DUMP MEM INDEX operations should call this method to finalize the current object.
    [[nodiscard]] PersistWriteResult CurrentObjFinalize(bool validate = false);

    // Download the whole object from object store if it's not in cache. Increase refcount and return the cached object file path.
    [[nodiscard]] PersistReadResult GetObjCache(const String &local_path);

    Tuple<SizeT, Status> GetFileSize(const String &file_path);

    Tuple<SizeT, Status> GetDirectorySize(const String &path_str);

    ObjAddr GetObjCacheWithoutCnt(const String &local_path);

    [[nodiscard]] PersistWriteResult PutObjCache(const String &file_path);

    [[nodiscard]] PersistWriteResult Cleanup(const String &file_path);

    /**
     * Utils
     */
    String GetObjPath(const String &obj_key) const { return std::filesystem::path(workspace_).append(obj_key).string(); }

    void SetKvStore(KVStore *kv_store);

    HashMap<String, ObjStat> GetAllObjects() const;
    HashMap<String, ObjAddr> GetAllFiles() const;

private:
    String ObjCreate();

    // Returns the room (size limit - sum_of_parts_size) of current object. User should check before each ObjAppend operation.
    int CurrentObjRoomNoLock();

    // Append file to the current object.
    // It finalizes current object if new size exceeds the size limit.
    void CurrentObjAppendNoLock(const String &tmp_file_path, SizeT file_size);

    // Finalize current object.
    void CurrentObjFinalizeNoLock(Vector<String> &persist_keys, Vector<String> &drop_keys);

    // Cleanup
    void CleanupNoLock(const ObjAddr &object_addr,
                       Vector<String> &persist_keys,
                       Vector<String> &drop_keys,
                       Vector<String> &drop_from_remote_keys,
                       bool check_ref_count = false);

    String RemovePrefix(const String &path);

    ObjStat GetObjStatByObjAddr(const ObjAddr &obj_addr);

    void CheckValid();

public: // for unit test
    void SaveLocalPath(const String &local_path, const ObjAddr &object_addr);

private:
    void SaveObjStat(const String &obj_key, const ObjStat &obj_stat);

    void AddObjAddrToKVStore(const String &path, const ObjAddr &obj_addr);

    KVStore *kv_store_{nullptr};
    String workspace_;
    String local_data_dir_;
    SizeT object_size_limit_;

    mutable std::mutex mtx_;
    // HashMap<String, ObjStat> objects_;        // obj_key -> ObjStat
    UniquePtr<ObjectStatAccessorBase> objects_; // obj_key -> ObjStat
    // Current unsealed object key
    String current_object_key_;
    SizeT current_object_size_ = 0;
    SizeT current_object_parts_ = 0;
    SizeT current_object_ref_count_ = 0;
    friend struct AddrSerializer;
};

export struct AddrSerializer {
    void Initialize(PersistenceManager *persistence_manager, const Vector<String> &path);

    void InitializeValid(PersistenceManager *persistence_manager);

    SizeT GetSizeInBytes() const;

    void WriteBufAdv(char *&buf) const;

    Vector<String> ReadBufAdv(const char *&buf);

    void AddToPersistenceManager(PersistenceManager *persistence_manager) const;

    Vector<String> paths_;
    Vector<ObjAddr> obj_addrs_; // set mutable to minimize refactor
    Vector<ObjStat> obj_stats_;
};

} // namespace infinity
