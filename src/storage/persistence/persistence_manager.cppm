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

// A view means a logical plan
namespace infinity {

export struct ObjAddr {
    String obj_key_{};
    SizeT part_offset_{};
    SizeT part_size_{};
    bool Valid() const { return !obj_key_.empty() && part_size_ > 0; }
};

export struct ObjStat {
    SizeT obj_size_{};
    int ref_count_{};
};

export class PersistenceManager {
public:
    // TODO: build cache from existing files under workspace
    PersistenceManager(const String workspace, SizeT object_size_limit) : workspace_(workspace), object_size_limit_(object_size_limit) {}
    ~PersistenceManager() {}

    // Create new object or append to current object, and returns the location.
    ObjAddr Persist(const String &file_path);

    // Create new object or append to current object, and returns the location.
    ObjAddr Persist(const char *data, SizeT len);

    // Download the whole object from object store if it's not in cache. Increase refcount and return the cached object file path.
    String GetObjCache(const ObjAddr &object_addr);

    // Decrease refcount
    void PutObjCache(const ObjAddr &object_addr);

private:
    String ObjCreate();

    // Returns the room (capacity - sum_of_parts_size) of current object. User should check before each ObjAppend operation.
    // capacity is const, for example 100MB.
    int CurrentObjRoom();

    // Append body to the current object. User should have body compressed before each ObjAppend operation.
    // It finalize current object if new size exceeds the capacity. Subsequent ObjAppend on the finalized object is forbidden.
    // There's a 4-byte pad CRC32 checksum of the whole object to detect Silent Data Corruption.
    // PersistenceManager shall upload the finalized object to object store in background.
    void CurrentObjAppend(const String &file_path, SizeT file_size);

    String workspace_;
    SizeT object_size_limit_;

    std::mutex mtx_;
    HashMap<String, ObjStat> objects_; // obj_key -> ObjStat
    // Current unsealed object key
    String current_object_key_;
    SizeT current_object_size_;
};
} // namespace infinity
