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

#include <cassert>
#include <filesystem>

module persistence_manager;

import stl;
import uuid;
import serialize;
import third_party;
import infinity_exception;
import virtual_store;
import logger;
import wal_manager;

namespace fs = std::filesystem;

namespace infinity {
constexpr SizeT BUFFER_SIZE = 1024 * 1024; // 1 MB

nlohmann::json ObjAddr::Serialize() const {
    nlohmann::json obj;
    obj["obj_key"] = obj_key_;
    obj["part_offset"] = part_offset_;
    obj["part_size"] = part_size_;
    return obj;
}

void ObjAddr::Deserialize(const nlohmann::json &obj) {
    obj_key_ = obj["obj_key"];
    part_offset_ = obj["part_offset"];
    part_size_ = obj["part_size"];
}

SizeT ObjAddr::GetSizeInBytes() const { return sizeof(int32_t) + obj_key_.size() + sizeof(SizeT) + sizeof(SizeT); }

void ObjAddr::WriteBufAdv(char *&buf) const {
    ::infinity::WriteBufAdv(buf, obj_key_);
    ::infinity::WriteBufAdv(buf, part_offset_);
    ::infinity::WriteBufAdv(buf, part_size_);
}

ObjAddr ObjAddr::ReadBufAdv(const char *&buf) {
    ObjAddr ret;
    ret.obj_key_ = ::infinity::ReadBufAdv<String>(buf);
    ret.part_offset_ = ::infinity::ReadBufAdv<SizeT>(buf);
    ret.part_size_ = ::infinity::ReadBufAdv<SizeT>(buf);
    return ret;
}

PersistenceManager::PersistenceManager(const String &workspace,
                                       const String &data_dir,
                                       SizeT object_size_limit,
                                       StorageType storage_type,
                                       StorageMode storage_mode)
    : workspace_(workspace), local_data_dir_(data_dir), object_size_limit_(object_size_limit) {

    switch (storage_type) {
        case StorageType::kLocal: {
            objects_ = MakeUnique<ObjectStatAccessorLocalStorage>();
            break;
        }
        case StorageType::kMinio: {
            objects_ = MakeUnique<ObjectStatAccessorLocalStorage>();
//            objects_ = MakeUnique<ObjectStatAccessorObjectStorage>();
            break;
        }
        default: {
            UnrecoverableError(fmt::format("Unsupported storage type: {}.", ToString(storage_type)));
        }
    }

    current_object_key_ = ObjCreate();
    current_object_size_ = 0;
    current_object_parts_ = 0;
    current_object_ref_count_ = 0;

    if (local_data_dir_.empty() || local_data_dir_.back() != '/') {
        local_data_dir_ += '/';
    }

    if (!VirtualStore::Exists(workspace_)) {
        VirtualStore::MakeDirectory(workspace_);
    }
    String read_path_empty = GetObjPath(ObjAddr::KeyEmpty);
    VirtualStore::Truncate(read_path_empty, 0);
}

PersistenceManager::~PersistenceManager() = default;

PersistWriteResult PersistenceManager::Persist(const String &file_path, const String &tmp_file_path, bool try_compose) {
    PersistWriteResult result;

    std::error_code ec;
    fs::path src_fp = tmp_file_path;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }
    {
        // Cleanup the file if it already exists in the local_path_obj_
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = local_path_obj_.find(local_path);
        if (it != local_path_obj_.end()) {
            CleanupNoLock(it->second, result.persist_keys_, result.drop_keys_, result.drop_from_remote_keys_);
            LOG_TRACE(fmt::format("Persist deleted mapping from local path {} to ObjAddr({}, {}, {})",
                                  local_path,
                                  it->second.obj_key_,
                                  it->second.part_offset_,
                                  it->second.part_size_));
        }
    }

    SizeT src_size = fs::file_size(src_fp, ec);
    if (ec) {
        String error_message = fmt::format("Failed to get file size of {}.", file_path);
        UnrecoverableError(error_message);
    }
    if (src_size == 0) {
        String error_message = fmt::format("Persist empty local path {}.", file_path);
        LOG_WARN(error_message);
        ObjAddr obj_addr(ObjAddr::KeyEmpty, 0, 0);
        fs::remove(tmp_file_path, ec);
        if (ec) {
            String error_message = fmt::format("Failed to remove {}.", tmp_file_path);
            UnrecoverableError(error_message);
        }
        std::lock_guard<std::mutex> lock(mtx_);
        local_path_obj_[local_path] = obj_addr;
        result.persist_keys_.push_back(ObjAddr::KeyEmpty);
        result.obj_addr_ = obj_addr;
        return result;
    } else if (!try_compose || src_size >= object_size_limit_) {
        String obj_key = ObjCreate();
        fs::path dst_fp = workspace_;
        dst_fp.append(obj_key);
        fs::rename(src_fp, dst_fp, ec);
        if (ec) {
            String error_message = fmt::format("Failed to rename {} to {}.", src_fp.string(), dst_fp.string());
            UnrecoverableError(error_message);
        }
        ObjAddr obj_addr(obj_key, 0, src_size);
        std::lock_guard<std::mutex> lock(mtx_);
        objects_->PutNew(obj_key, ObjStat(src_size, 1, 0), result.drop_keys_);
        LOG_TRACE(fmt::format("Persist added dedicated object {}", obj_key));

        local_path_obj_[local_path] = obj_addr;
        LOG_TRACE(fmt::format("Persist local path {} to dedicated ObjAddr ({}, {}, {})",
                              local_path,
                              obj_addr.obj_key_,
                              obj_addr.part_offset_,
                              obj_addr.part_size_));
        result.persist_keys_.push_back(obj_key);
        result.obj_addr_ = obj_addr;
        return result;
    } else {
        std::lock_guard<std::mutex> lock(mtx_);
        if (int(src_size) > CurrentObjRoomNoLock()) {
            CurrentObjFinalizeNoLock(result.persist_keys_, result.drop_keys_);
        }
        ObjAddr obj_addr(current_object_key_, current_object_size_, src_size);
        CurrentObjAppendNoLock(tmp_file_path, src_size);
        fs::remove(tmp_file_path, ec);
        if (ec) {
            String error_message = fmt::format("Failed to remove {}.", tmp_file_path);
            UnrecoverableError(error_message);
        }

        local_path_obj_[local_path] = obj_addr;
        LOG_TRACE(fmt::format("Persist local path {} to composed ObjAddr ({}, {}, {})",
                              local_path,
                              obj_addr.obj_key_,
                              obj_addr.part_offset_,
                              obj_addr.part_size_));
        result.obj_addr_ = obj_addr;
        return result;
    }
}

// TODO:
// - Upload the finalized object to object store in background.
PersistWriteResult PersistenceManager::CurrentObjFinalize(bool validate) {
    PersistWriteResult result;
    std::lock_guard<std::mutex> lock(mtx_);
    CurrentObjFinalizeNoLock(result.persist_keys_, result.drop_keys_);

    if (validate) {
        CheckValid();
    }

    return result;
}

void PersistenceManager::CheckValid() {
    for (auto &[local_path, obj_addr] : local_path_obj_) {
        ObjStat *obj_stat = objects_->GetNoCount(obj_addr.obj_key_);
        if (obj_stat == nullptr) {
            String error_message = fmt::format("CurrentObjFinalize Failed to find object for local path {}", local_path);
            LOG_ERROR(error_message);
        }
    }
    objects_->CheckValid(current_object_size_);
}

void PersistenceManager::CurrentObjFinalizeNoLock(Vector<String> &persist_keys, Vector<String> &drop_keys) {
    persist_keys.push_back(current_object_key_);
    if (current_object_size_ > 0) {
        if (current_object_parts_ > 1) {
            // Add footer to composed object -- format version 1
            fs::path dst_fp = workspace_;
            dst_fp.append(current_object_key_);
            std::ofstream outFile(dst_fp, std::ios::app);
            if (!outFile.is_open()) {
                String error_message = fmt::format("Failed to open file {}.", dst_fp.string());
                UnrecoverableError(error_message);
            }
            const u32 compose_format = 1;
            outFile.write((char *)&compose_format, sizeof(u32));
            outFile.close();
        }

        objects_->PutNew(current_object_key_, ObjStat(current_object_size_, current_object_parts_, current_object_ref_count_), drop_keys);
        LOG_TRACE(fmt::format("CurrentObjFinalizeNoLock added composed object {}", current_object_key_));
        current_object_key_ = ObjCreate();
        current_object_size_ = 0;
        current_object_parts_ = 0;
        current_object_ref_count_ = 0;
    }
}

PersistReadResult PersistenceManager::GetObjCache(const String &file_path) {
    PersistReadResult result;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("GetObjCache Failed to find object for local path {}", local_path);
        LOG_WARN(error_message);
        result.cached_ = true;
        return result;
    }
    result.obj_addr_ = it->second;
    if (it->second.part_size_ == 0) {
        LOG_TRACE(fmt::format("GetObjCache empty object {} for local path {}", it->second.obj_key_, local_path));
        if (it->second.obj_key_ != ObjAddr::KeyEmpty) {
            String error_message = fmt::format("GetObjCache object {} is empty", it->second.obj_key_);
            UnrecoverableError(error_message);
        }
        result.cached_ = true;
    } else if (ObjStat *obj_stat = objects_->Get(it->second.obj_key_); obj_stat != nullptr) {
        LOG_TRACE(fmt::format("GetObjCache object {} ref count {}", it->second.obj_key_, obj_stat->ref_count_));
        if (!obj_stat->cached_) {
            String read_path = GetObjPath(result.obj_addr_.obj_key_);
            VirtualStore::DownloadObject(read_path, read_path);
            if (VirtualStore::Exists(read_path)) {
                LOG_TRACE(fmt::format("GetObjCache download object {}", read_path));
                obj_stat->cached_ = true;
            }
        }
        result.cached_ = obj_stat->cached_;
    } else {
        if (it->second.obj_key_ != current_object_key_) {
            String error_message = fmt::format("GetObjCache object {} not found", it->second.obj_key_);
            UnrecoverableError(error_message);
        }
        current_object_ref_count_++;
        LOG_TRACE(fmt::format("GetObjCache current object {} ref count {}", it->second.obj_key_, current_object_ref_count_));
        result.cached_ = true;
    }
    return result;
}

Tuple<SizeT, Status> PersistenceManager::GetFileSize(const String &file_path) {
    PersistReadResult result;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        return {0, Status::NotFound(fmt::format("Can't find {}", local_path))};
    }

    return {it->second.part_size_, Status::OK()};
}

ObjAddr PersistenceManager::GetObjCacheWithoutCnt(const String &local_path) {
    String lock_path = RemovePrefix(local_path);
    if (lock_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(lock_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("GetObjCacheWithoutCnt Failed to find object for local path {}", lock_path);
        LOG_WARN(error_message);
        return ObjAddr();
    }
    return it->second;
}

PersistWriteResult PersistenceManager::PutObjCache(const String &file_path) {
    PersistWriteResult result;
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find file path of {}", file_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("Failed to find file_path: {} stored object", local_path);
        UnrecoverableError(error_message);
    }
    if (it->second.part_size_ == 0) {
        LOG_TRACE(fmt::format("PutObjCache empty object {} for local path {}", it->second.obj_key_, local_path));
        return result;
    }
    ObjStat *obj_stat = objects_->Release(it->second.obj_key_, result.drop_keys_);
    if (obj_stat == nullptr) {
        if (it->second.obj_key_ != current_object_key_) {
            UnrecoverableError(fmt::format("PutObjCache object {} not found", it->second.obj_key_));
        }
        if (current_object_ref_count_ <= 0) {
            UnrecoverableError(fmt::format("PutObjCache object {} ref count is {}", it->second.obj_key_, current_object_ref_count_));
        }
        current_object_ref_count_--;
        LOG_TRACE(fmt::format("PutObjCache current object {} ref count {}", it->second.obj_key_, current_object_ref_count_));
        return result;
    }
    LOG_TRACE(fmt::format("PutObjCache object {} ref count {}", it->second.obj_key_, obj_stat->ref_count_));
    return result;
}

String PersistenceManager::ObjCreate() { return UUID().to_string(); }

int PersistenceManager::CurrentObjRoomNoLock() { return int(object_size_limit_) - int(current_object_size_); }

void PersistenceManager::CurrentObjAppendNoLock(const String &tmp_file_path, SizeT file_size) {
    fs::path src_fp = tmp_file_path;
    fs::path dst_fp = Path(workspace_) / current_object_key_;
    std::ifstream srcFile(src_fp, std::ios::binary);
    if (!srcFile.is_open()) {
        String error_message = fmt::format("Failed to open source file {}", tmp_file_path);
        UnrecoverableError(error_message);
    }
    std::ofstream dstFile(dst_fp, std::ios::binary | std::ios::app);
    if (!dstFile.is_open()) {
        String error_message = fmt::format("Failed to open destination file {} {}", strerror(errno), dst_fp.string());
        UnrecoverableError(error_message);
    }
    char buffer[BUFFER_SIZE];
    while (srcFile.read(buffer, BUFFER_SIZE)) {
        dstFile.write(buffer, srcFile.gcount());
    }
    dstFile.write(buffer, srcFile.gcount());
    srcFile.close();
    current_object_size_ += file_size;
    current_object_parts_++;
    if (current_object_size_ >= object_size_limit_) {
        UnrecoverableError(
            fmt::format("CurrentObjAppendNoLock object {} size {} exceeds limit {}", current_object_key_, current_object_size_, object_size_limit_));
    }
    dstFile.close();
}

void PersistenceManager::CleanupNoLock(const ObjAddr &object_addr,
                                       Vector<String> &persist_keys,
                                       Vector<String> &drop_keys,
                                       Vector<String> &drop_from_remote_keys,
                                       bool check_ref_count) {
    ObjStat *obj_stat = objects_->GetNoCount(object_addr.obj_key_);
    if (obj_stat == nullptr) {
        if (object_addr.obj_key_ == ObjAddr::KeyEmpty) {
            assert(object_addr.part_size_ == 0);
            return;
        } else if (object_addr.obj_key_ == current_object_key_) {
            CurrentObjFinalizeNoLock(persist_keys, drop_keys);
            obj_stat = objects_->GetNoCount(object_addr.obj_key_);
            assert(obj_stat != nullptr);
        } else {
            String error_message = fmt::format("CleanupNoLock Failed to find object {}", object_addr.obj_key_);
            UnrecoverableError(error_message);
            return;
        }
    }
    if (check_ref_count) {
        if (obj_stat->ref_count_ > 0) {
            String error_message = fmt::format("CleanupNoLock object {} ref count is {}", object_addr.obj_key_, obj_stat->ref_count_);
            UnrecoverableError(error_message);
        }
    }
    Range orig_range(object_addr.part_offset_, object_addr.part_offset_ + object_addr.part_size_);
    Range range(orig_range);
    auto inst_it = obj_stat->deleted_ranges_.lower_bound(range);

    if (inst_it != obj_stat->deleted_ranges_.begin()) {
        auto inst_it_prev = std::prev(inst_it);
        if (inst_it_prev->Cover(orig_range)) {
            // Check duplication. Cleanup could delete a local file multiple times.
            String error_message = fmt::format("CleanupNoLock delete [{}, {}) more than once", range.start_, range.end_);
            LOG_WARN(error_message);
            return;
        } else if (inst_it_prev->Intersect(orig_range)) {
            // Check intersection with prev range
            String error_message = fmt::format("ObjAddr {} range to delete [{}, {}) intersects with prev one [{}, {})",
                                               object_addr.obj_key_,
                                               orig_range.start_,
                                               orig_range.end_,
                                               inst_it_prev->start_,
                                               inst_it_prev->end_);
            UnrecoverableError(error_message);
        } else if (orig_range.start_ == inst_it_prev->end_) {
            // Try merge with prev range
            range.start_ = inst_it_prev->start_;
            inst_it = obj_stat->deleted_ranges_.erase(inst_it_prev);
        }
    }

    if (inst_it != obj_stat->deleted_ranges_.end()) {
        if (inst_it->Cover(orig_range)) {
            // Check duplication. Cleanup could delete a local file multiple times.
            String error_message = fmt::format("CleanupNoLock delete [{}, {}) more than once", orig_range.start_, orig_range.end_);
            LOG_WARN(error_message);
            return;
        } else if (inst_it->Intersect(orig_range)) {
            // Check intersection with next range
            String error_message = fmt::format("ObjAddr {} range to delete [{}, {}) intersects with next one [{}, {})",
                                               object_addr.obj_key_,
                                               orig_range.start_,
                                               orig_range.end_,
                                               inst_it->start_,
                                               inst_it->end_);
            UnrecoverableError(error_message);
        } else if (orig_range.end_ == inst_it->start_) {
            // Try merge with next range
            range.end_ = inst_it->end_;
            obj_stat->deleted_ranges_.erase(inst_it);
        }
    }

    auto [_, ok] = obj_stat->deleted_ranges_.insert(range);
    if (!ok) {
        String error_message = fmt::format("Failed to delete ObjAddr {} range [{}, {})", object_addr.obj_key_, range.start_, range.end_);
        UnrecoverableError(error_message);
    }

    LOG_TRACE(fmt::format("Deleted object {} range [{}, {})", object_addr.obj_key_, orig_range.start_, orig_range.end_));
    if (range.start_ == 0 && range.end_ == obj_stat->obj_size_ && object_addr.obj_key_ != current_object_key_) {
        if (object_addr.obj_key_.empty()) {
            String error_message = fmt::format("Failed to find object key");
            UnrecoverableError(error_message);
        }
        drop_from_remote_keys.emplace_back(object_addr.obj_key_);
        objects_->Invalidate(object_addr.obj_key_);
        LOG_TRACE(fmt::format("Deleted object {}", object_addr.obj_key_));
    }
}

ObjStat PersistenceManager::GetObjStatByObjAddr(const ObjAddr &obj_addr) {
    std::lock_guard<std::mutex> lock(mtx_);
    ObjStat *obj_stat = objects_->GetNoCount(obj_addr.obj_key_);
    if (obj_stat == nullptr) {
        return ObjStat();
    }
    return *obj_stat;
}

void PersistenceManager::SaveLocalPath(const String &file_path, const ObjAddr &object_addr) {
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it != local_path_obj_.end()) {
        it->second = object_addr;
        LOG_TRACE(fmt::format("SaveLocalPath updated local path {} to ObjAddr({}, {}, {})",
                              local_path,
                              object_addr.obj_key_,
                              object_addr.part_offset_,
                              object_addr.part_size_));
    } else {
        local_path_obj_.emplace(local_path, object_addr);
        LOG_TRACE(fmt::format("SaveLocalPath added local path {} to ObjAddr({}, {}, {})",
                              local_path,
                              object_addr.obj_key_,
                              object_addr.part_offset_,
                              object_addr.part_size_));
    }
}

void PersistenceManager::SaveObjStat(const ObjAddr &obj_addr, const ObjStat &obj_stat) {
    std::lock_guard<std::mutex> lock(mtx_);
    objects_->PutNoCount(obj_addr.obj_key_, obj_stat);
}

String PersistenceManager::RemovePrefix(const String &path) {
    if (path.starts_with(local_data_dir_)) {
        return path.substr(local_data_dir_.length());
    } else if (!path.starts_with("/")) {
        return path;
    }
    return "";
}

PersistWriteResult PersistenceManager::Cleanup(const String &file_path) {
    PersistWriteResult result;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("Failed to find object for local path {}", local_path);
        LOG_WARN(error_message);
        return result;
    }
    CleanupNoLock(it->second, result.persist_keys_, result.drop_keys_, result.drop_from_remote_keys_, true);
    LOG_TRACE(fmt::format("Deleted mapping from local path {} to ObjAddr({}, {}, {})",
                          local_path,
                          it->second.obj_key_,
                          it->second.part_offset_,
                          it->second.part_size_));
    local_path_obj_.erase(it);
    return result;
}

nlohmann::json PersistenceManager::Serialize() {
    std::lock_guard<std::mutex> lock(mtx_);
    nlohmann::json json_obj;
    json_obj["objects"] = objects_->Serialize();
    json_obj["obj_addr_size"] = local_path_obj_.size();
    json_obj["obj_addr_array"] = nlohmann::json::array();
    for (auto &[path, obj_addr] : local_path_obj_) {
        nlohmann::json pair;
        pair["local_path"] = path;
        pair["obj_addr"] = obj_addr.Serialize();
        json_obj["obj_addr_array"].emplace_back(pair);
    }
    return json_obj;
}

void PersistenceManager::Deserialize(const nlohmann::json &obj) {
    std::lock_guard<std::mutex> lock(mtx_);
    objects_->Deserialize(obj["objects"]);
    SizeT len = 0;
    if (obj.contains("obj_addr_size")) {
        len = obj["obj_addr_size"];
    }
    for (SizeT i = 0; i < len; ++i) {
        auto &json_pair = obj["obj_addr_array"][i];
        String path = json_pair["local_path"];
        ObjAddr obj_addr;
        obj_addr.Deserialize(json_pair["obj_addr"]);
        local_path_obj_.emplace(path, obj_addr);
        LOG_TRACE(fmt::format("Deserialize added local path {}", path));
    }
}

HashMap<String, ObjStat> PersistenceManager::GetAllObjects() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return objects_->GetAllObjects();
}

HashMap<String, ObjAddr> PersistenceManager::GetAllFiles() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return local_path_obj_;
}

void AddrSerializer::Initialize(PersistenceManager *persistence_manager, const Vector<String> &path) {
    if (persistence_manager == nullptr) {
        return; // not use persistence manager
    }
    if (!paths_.empty()) {
        UnrecoverableError("AddrSerializer has been initialized");
    }
    for (const String &path : path) {
        paths_.push_back(path);
        ObjAddr obj_addr = persistence_manager->GetObjCacheWithoutCnt(path);
        obj_addrs_.push_back(obj_addr);
        if (!obj_addr.Valid()) {
            // In ImportWal, version file is not flushed here, set before write wal
            ObjStat invalid_stat;
            obj_stats_.push_back(invalid_stat);
        } else {
            ObjStat obj_stat = persistence_manager->GetObjStatByObjAddr(obj_addr);
            obj_stats_.push_back(obj_stat);
        }
    }
}

void AddrSerializer::InitializeValid(PersistenceManager *persistence_manager) {
    if (persistence_manager == nullptr) {
        return; // not use persistence manager
    }
    for (SizeT i = 0; i < paths_.size(); ++i) {
        if (obj_addrs_[i].Valid()) {
            continue;
        }

        ObjAddr obj_addr = persistence_manager->GetObjCacheWithoutCnt(paths_[i]);

        obj_addrs_[i] = obj_addr;
        if (!obj_addr.Valid()) {
            UnrecoverableError(fmt::format("Invalid object address for path {}", paths_[i]));
        } else {
            ObjStat obj_stat = persistence_manager->GetObjStatByObjAddr(obj_addr);
            obj_stats_[i] = obj_stat;
        }
    }
}

SizeT AddrSerializer::GetSizeInBytes() const {
    SizeT size = sizeof(SizeT);
    for (SizeT i = 0; i < paths_.size(); ++i) {
        size += sizeof(i32) + paths_[i].size();
        size += obj_addrs_[i].GetSizeInBytes();
        size += obj_stats_[i].GetSizeInBytes();
    }
    return size;
}

void AddrSerializer::WriteBufAdv(char *&buf) const {
    ::infinity::WriteBufAdv(buf, paths_.size());
    for (SizeT i = 0; i < paths_.size(); ++i) {
        ::infinity::WriteBufAdv(buf, paths_[i]);
        if (!obj_addrs_[i].Valid()) {
            UnrecoverableError(fmt::format("Invalid object address for path {}", paths_[i]));
        }
        obj_addrs_[i].WriteBufAdv(buf);
        obj_stats_[i].WriteBufAdv(buf);
    }
}

Vector<String> AddrSerializer::ReadBufAdv(const char *&ptr) {
    SizeT path_count = ::infinity::ReadBufAdv<SizeT>(ptr);
    for (SizeT i = 0; i < path_count; ++i) {
        paths_.push_back(::infinity::ReadBufAdv<String>(ptr));
        obj_addrs_.push_back(ObjAddr::ReadBufAdv(ptr));
        obj_stats_.push_back(ObjStat::ReadBufAdv(ptr));
    }
    return paths_;
}

void AddrSerializer::AddToPersistenceManager(PersistenceManager *persistence_manager) const {
    if (persistence_manager == nullptr) {
        return;
    }
    for (SizeT i = 0; i < paths_.size(); ++i) {
        if (!obj_addrs_[i].Valid()) {
            UnrecoverableError(fmt::format("Invalid object address for path {}", paths_[i]));
        }
        LOG_TRACE(fmt::format("Add path {} to persistence manager", paths_[i]));
        persistence_manager->SaveLocalPath(paths_[i], obj_addrs_[i]);
        persistence_manager->SaveObjStat(obj_addrs_[i], obj_stats_[i]);
    }
}

} // namespace infinity