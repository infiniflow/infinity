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

export module persistence_manager;

import stl;
import serialize;
import third_party;

// A view means a logical plan
namespace infinity {

export struct ObjAddr {
    String obj_key_{};
    SizeT part_offset_{};
    SizeT part_size_{};

    bool Valid() const { return !obj_key_.empty() && part_size_ > 0; }

    nlohmann::json Serialize() const;

    void Deserialize(const nlohmann::json &obj);

    SizeT GetSizeInBytes() const;

    void WriteBufAdv(char *&buf) const;

    static ObjAddr ReadBufAdv(char *&buf);
};

export struct Range {
    SizeT start_{}; // inclusive
    SizeT end_{};   // exclusive
    bool operator<(const Range &rhs) const { return start_ < rhs.start_; }
    bool operator==(const Range &rhs) const { return start_ == rhs.start_ && end_ == rhs.end_; }
    bool Cover(const Range &rhs) const { return start_ <= rhs.start_ && rhs.end_ <= end_; }
    bool Intersect(const Range &rhs) const { return start_ < rhs.end_ && rhs.start_ < end_; }
};

export struct ObjStat {
    SizeT obj_size_{}; // footer (if present) is excluded
    SizeT parts_{};    // an object attribute
    SizeT ref_count_{}; // the number of user (R and W) of some part of this object
    Set<Range> deleted_ranges_{};

    ObjStat() = default;

    ObjStat(SizeT obj_size, SizeT parts, SizeT ref_count) : obj_size_(obj_size), parts_(parts), ref_count_(ref_count) {}

    nlohmann::json Serialize() const;

    void Deserialize(const nlohmann::json &obj);

    SizeT GetSizeInBytes() const;

    void WriteBufAdv(char *&buf) const;

    static ObjStat ReadBufAdv(char *&buf);
};

export class PersistenceManager {
public:
    // TODO: build cache from existing files under workspace
    PersistenceManager(const String &workspace, const String &data_dir, SizeT object_size_limit);

    ~PersistenceManager();

    /**
     * For composed objects
     */
    // Create new object or append to current object, and returns the location.
    ObjAddr Persist(const String &file_path);

    // Create new object or append to current object, and returns the location.
    ObjAddr Persist(const char *data, SizeT len);

    // Force finalize current object. Subsequent append on the finalized object is forbidden.
    void CurrentObjFinalize();

    /**
     * For dedicated objects
     */
    ObjAddr ObjCreateRefCount(const String &file_path);

    /**
     * For composed and dedicated objects
     */
    // Download the whole object from object store if it's not in cache. Increase refcount and return the cached object file path.
    String GetObjCache(const String &local_path);

    ObjAddr GetObjFromLocalPath(const String &file_path);

    void PutObjCache(const String &file_path);

    void Cleanup(const String &file_path);

    /**
     * Utils
     */
    nlohmann::json Serialize();

    void Deserialize(const nlohmann::json &obj);

    HashMap<String, ObjStat> GetAllObjects() const;
    HashMap<String, ObjAddr> GetAllFiles() const;

private:
    String ObjCreate();

    // Returns the room (size limit - sum_of_parts_size) of current object. User should check before each ObjAppend operation.
    int CurrentObjRoomNoLock();

    // Append file to the current object.
    // It finalize current object if new size exceeds the size limit.
    void CurrentObjAppendNoLock(const String &file_path, SizeT file_size);

    // Finalize current object.
    void CurrentObjFinalizeNoLock();

    // Cleanup
    void CleanupNoLock(const ObjAddr &object_addr);

    String RemovePrefix(const String &path);

    ObjStat GetObjStatByObjAddr(const ObjAddr &obj_addr);

public: // for unit test
    void SaveLocalPath(const String &local_path, const ObjAddr &object_addr);

private:
    void SaveObjStat(const ObjAddr &obj_addr, const ObjStat &obj_stat);

    String workspace_;
    String local_data_dir_;
    SizeT object_size_limit_;

    mutable std::mutex mtx_;
    HashMap<String, ObjStat> objects_;        // obj_key -> ObjStat
    HashMap<String, ObjAddr> local_path_obj_; // local_file_path -> ObjAddr
    // Current unsealed object key
    String current_object_key_;
    SizeT current_object_size_;
    SizeT current_object_parts_;

    friend struct AddrSerializer;
};

export struct AddrSerializer {
    void Initialize(PersistenceManager *pm, const Vector<String> &path);

    SizeT GetSizeInBytes() const;
 
    void WriteBufAdv(char *&buf) const;

    Vector<String> ReadBufAdv(char *&buf);

    void AddToPersistenceManager(PersistenceManager *pm) const;

    Vector<String> paths_;
    Vector<ObjAddr> obj_addrs_; // set mutable to minimize refactor
    Vector<ObjStat> obj_stats_;
};

} // namespace infinity
