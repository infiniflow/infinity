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
import global_resource_usage;
import obj_stat_accessor;
import kv_store;
import kv_code;
import infinity_context;

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

void ObjAddr::Deserialize(std::string_view obj_str) {
    simdjson::padded_string obj_json(obj_str);
    simdjson::parser parser;
    simdjson::document doc = parser.iterate(obj_json);
    obj_key_ = doc["obj_key"].get<String>();
    part_offset_ = doc["part_offset"].get<SizeT>();
    part_size_ = doc["part_size"].get<SizeT>();
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

PersistenceManager::PersistenceManager(const String &workspace, const String &data_dir, SizeT object_size_limit, bool local_storage)
    : workspace_(workspace), local_data_dir_(data_dir), object_size_limit_(object_size_limit) {
    if (local_storage) {
        objects_ = MakeUnique<ObjectStatAccessor_LocalStorage>();
    } else {
        UnrecoverableError("Remote storage is not supported yet.");
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

#ifdef INFINITY_DEBUG
    GlobalResourceUsage::IncrObjectCount("PersistenceManager");
#endif
}

PersistenceManager::~PersistenceManager() {
#ifdef INFINITY_DEBUG
    GlobalResourceUsage::DecrObjectCount("PersistenceManager");
#endif
}

PersistWriteResult PersistenceManager::Persist(const String &file_path, const String &tmp_file_path, bool try_compose) {
    PersistWriteResult result;

    std::error_code ec;
    fs::path src_fp = tmp_file_path;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }
    String pm_fp_key = KeyEncode::PMObjectKey(local_path);
    String pm_fp_value;
    Status status = kv_store_->Get(pm_fp_key, pm_fp_value);
    if (status.ok()) {
        // Cleanup the file if it already exists in the KV
        ObjAddr obj_addr;
        obj_addr.Deserialize(pm_fp_value);
        CleanupNoLock(obj_addr, result.persist_keys_, result.drop_keys_, result.drop_from_remote_keys_);
        LOG_TRACE(fmt::format("Persist deleted mapping from local path {} to ObjAddr({}, {}, {})",
                              local_path,
                              obj_addr.obj_key_,
                              obj_addr.part_offset_,
                              obj_addr.part_size_));
    }

    SizeT src_size = fs::file_size(src_fp, ec);
    if (ec) {
        UnrecoverableError(fmt::format("Failed to get file size of {}", file_path));
    }
    if (src_size == 0) {
        LOG_WARN(fmt::format("Persist empty local path {}", file_path));
        ObjAddr obj_addr(ObjAddr::KeyEmpty, 0, 0);
        fs::remove(tmp_file_path, ec); // This may cause the issue
        if (ec) {
            String error_message = fmt::format("Failed to remove {}", tmp_file_path);
            UnrecoverableError(error_message);
        }
        status = kv_store_->Put(pm_fp_key, obj_addr.Serialize().dump());
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        result.persist_keys_.push_back(ObjAddr::KeyEmpty);
        result.obj_addr_ = obj_addr;
        return result;
    }
    if (!try_compose || src_size >= object_size_limit_) {
        String obj_key = ObjCreate();
        fs::path dst_fp = workspace_;
        dst_fp.append(obj_key);
        fs::rename(src_fp, dst_fp, ec);
        if (ec) {
            UnrecoverableError(fmt::format("Failed to rename {} to {}", src_fp.string(), dst_fp.string()));
        }
        ObjAddr obj_addr(obj_key, 0, src_size);
        std::lock_guard<std::mutex> lock(mtx_);
        objects_->PutNew(obj_key, ObjStat(src_size, 1, 0), result.drop_keys_);
        LOG_TRACE(fmt::format("Persist added dedicated object {}", obj_key));

        status = kv_store_->Put(pm_fp_key, obj_addr.Serialize().dump());
        if (!status.ok()) {
            UnrecoverableError(status.message());
        }

        LOG_TRACE(fmt::format("Persist local path {} to dedicated ObjAddr ({}, {}, {})",
                              local_path,
                              obj_addr.obj_key_,
                              obj_addr.part_offset_,
                              obj_addr.part_size_));
        result.persist_keys_.push_back(obj_key);
        result.obj_addr_ = obj_addr;
        return result;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if (int(src_size) >= CurrentObjRoomNoLock()) {
        CurrentObjFinalizeNoLock(result.persist_keys_, result.drop_keys_);
    }
    current_object_size_ = (current_object_size_ + ObjAlignment - 1) & ~(ObjAlignment - 1);
    ObjAddr obj_addr(current_object_key_, current_object_size_, src_size);
    CurrentObjAppendNoLock(tmp_file_path, src_size);
    fs::remove(tmp_file_path, ec);
    if (ec) {
        String error_message = fmt::format("Failed to remove {}", tmp_file_path);
        UnrecoverableError(error_message);
    }

    status = kv_store_->Put(pm_fp_key, obj_addr.Serialize().dump());
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }

    LOG_TRACE(fmt::format("Persist local path {} to composed ObjAddr ({}, {}, {})",
                          local_path,
                          obj_addr.obj_key_,
                          obj_addr.part_offset_,
                          obj_addr.part_size_));
    result.obj_addr_ = obj_addr;
    return result;
}

// TODO:
// - Upload the finalized object to object store in background.
PersistWriteResult PersistenceManager::CurrentObjFinalize(bool validate) {
    PersistWriteResult result;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        CurrentObjFinalizeNoLock(result.persist_keys_, result.drop_keys_);
    }

    if (validate) {
        CheckValid();
    }

    return result;
}

void PersistenceManager::CheckValid() {
    using TimeDurationType = std::chrono::duration<float, std::milli>;
    const auto part1_begin = std::chrono::high_resolution_clock::now();
    String pm_object_prefix = KeyEncode::PMObjectPrefix();
    SizeT prefix_len = pm_object_prefix.size();
    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();
    UniquePtr<KVIterator> iter = kv_instance->GetIterator();
    iter->Seek(pm_object_prefix);
    for (; iter->Valid() && iter->Key().starts_with(pm_object_prefix); iter->Next()) {
        String local_path = iter->Key().ToString().substr(prefix_len);
        ObjAddr obj_addr;
        obj_addr.Deserialize(iter->Value().ToString());
        if (obj_addr.obj_key_ == ObjAddr::KeyEmpty) {
            continue;
        }
        ObjStat *obj_stat = objects_->GetNoCount(obj_addr.obj_key_);
        if (obj_stat == nullptr) {
            String error_message = fmt::format("CheckValid Failed to find object for local path {}", local_path);
            LOG_ERROR(error_message);
        }
    }
    std::lock_guard<std::mutex> lock(mtx_);
    const auto part2_begin = std::chrono::high_resolution_clock::now();
    objects_->CheckValid(current_object_size_);
    const auto part2_end = std::chrono::high_resolution_clock::now();
    LOG_INFO(fmt::format("PersistenceManager::CheckValid part 1: {} ms, part2: {} ms",
                         static_cast<TimeDurationType>(part2_begin - part1_begin).count(),
                         static_cast<TimeDurationType>(part2_end - part2_begin).count()));
}

void PersistenceManager::CurrentObjFinalizeNoLock(Vector<String> &persist_keys, Vector<String> &drop_keys) {
    if (current_object_size_ > 0) {
        persist_keys.push_back(current_object_key_);
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
    } else {
        LOG_TRACE(fmt::format("CurrentObjFinalizeNoLock added empty object {}", current_object_key_));
    }
}

PersistReadResult PersistenceManager::GetObjCache(const String &file_path) {
    PersistReadResult result;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    String pm_fp_key = KeyEncode::PMObjectKey(local_path);
    String value;
    Status status = kv_store_->Get(pm_fp_key, value);
    if (!status.ok()) {
        String error_message = fmt::format("GetObjCache Failed to find object for local path {}: {}", local_path, status.message());
        LOG_WARN(error_message);
        LOG_TRACE(fmt::format("All key-value pairs in kv_store: \n{}", kv_store_->ToString()));
        return result;
    }
    ObjAddr obj_addr;
    obj_addr.Deserialize(value);

    std::lock_guard<std::mutex> lock(mtx_);
    result.obj_addr_ = obj_addr;
    if (obj_addr.part_size_ == 0) {
        LOG_TRACE(fmt::format("GetObjCache empty object {} for local path {}", obj_addr.obj_key_, local_path));
        if (obj_addr.obj_key_ != ObjAddr::KeyEmpty) {
            String error_message = fmt::format("GetObjCache object {} is empty", obj_addr.obj_key_);
            UnrecoverableError(error_message);
        }
    } else if (Optional<ObjStat> obj_stat = objects_->Get(obj_addr.obj_key_); obj_stat) {
        LOG_TRACE(fmt::format("GetObjCache object {}, file_path: {}, ref count {}", obj_addr.obj_key_, file_path, obj_stat->ref_count_));
        String read_path = GetObjPath(result.obj_addr_.obj_key_);
        if (!VirtualStore::Exists(read_path)) {
            auto expect = ObjCached::kCached;
            obj_stat->cached_.compare_exchange_strong(expect, ObjCached::kNotCached);
            result.obj_stat_ = &obj_stat.value();
        }
    } else {
        if (obj_addr.obj_key_ != current_object_key_) {
            String error_message = fmt::format("GetObjCache object {} not found", obj_addr.obj_key_);
            UnrecoverableError(error_message);
        }
        current_object_ref_count_++;
        LOG_TRACE(fmt::format("GetObjCache current object {} ref count {}", obj_addr.obj_key_, current_object_ref_count_));
    }
    return result;
}

Tuple<SizeT, Status> PersistenceManager::GetDirectorySize(const String &path_str) {
    SizeT total_size = 0;

    if (!VirtualStore::Exists(path_str)) {
        return {0, Status::IOError(fmt::format("{} doesn't exist.", path_str))};
    }

    const Path path(path_str);
    for (const auto &entry : fs::recursive_directory_iterator(path)) {
        if (fs::is_regular_file(entry.status())) {
            total_size += fs::file_size(entry.path());
        }
    }

    return {total_size, Status::OK()};
}

Tuple<SizeT, Status> PersistenceManager::GetFileSize(const String &file_path) {
    PersistReadResult result;

    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    String pm_fp_key = KeyEncode::PMObjectKey(local_path);
    String value;
    Status status = kv_store_->Get(pm_fp_key, value);
    if (!status.ok()) {
        String error_message = fmt::format("GetFileSize Failed to find object for local path {}: {}", local_path, status.message());
        LOG_WARN(error_message);
        return {0, Status::NotFound(fmt::format("Can't find {}", local_path))};
    }
    ObjAddr obj_addr;
    obj_addr.Deserialize(value);
    return {obj_addr.part_size_, Status::OK()};
}

ObjAddr PersistenceManager::GetObjCacheWithoutCnt(const String &local_path) {
    String lock_path = RemovePrefix(local_path);
    if (lock_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    String pm_fp_key = KeyEncode::PMObjectKey(local_path);
    String value;
    Status status = kv_store_->Get(pm_fp_key, value);
    if (!status.ok()) {
        String error_message = fmt::format("GetFileSize Failed to find object for local path {}: {}", local_path, status.message());
        LOG_WARN(error_message);
        return ObjAddr();
    }
    ObjAddr obj_addr;
    obj_addr.Deserialize(value);
    return obj_addr;
}

PersistWriteResult PersistenceManager::PutObjCache(const String &file_path) {
    PersistWriteResult result;
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find file path of {}", file_path);
        UnrecoverableError(error_message);
    }
    String pm_fp_key = KeyEncode::PMObjectKey(local_path);
    String value;
    Status status = kv_store_->Get(pm_fp_key, value);
    if (!status.ok()) {
        String error_message = fmt::format("Failed to find file_path: {} stored object", local_path);
        UnrecoverableError(error_message);
    }
    ObjAddr obj_addr;
    obj_addr.Deserialize(value);
    if (obj_addr.part_size_ == 0) {
        LOG_TRACE(fmt::format("PutObjCache empty object {} for local path {}", obj_addr.obj_key_, local_path));
        return result;
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if (obj_addr.obj_key_ == current_object_key_) {
        if (current_object_ref_count_ <= 0) {
            UnrecoverableError(fmt::format("PutObjCache current object {} ref count is {}", obj_addr.obj_key_, current_object_ref_count_));
        }
        current_object_ref_count_--;
        LOG_TRACE(fmt::format("PutObjCache current object {} ref count {}", obj_addr.obj_key_, current_object_ref_count_));
    } else {
        Optional<ObjStat> obj_stat = objects_->Release(obj_addr.obj_key_, result.drop_keys_);
        if (obj_stat) {
            LOG_TRACE(fmt::format("PutObjCache object {} ref count {}", obj_addr.obj_key_, obj_stat->ref_count_));
        } else {
            LOG_WARN(fmt::format("PutObjCache object {} unknown ref count", obj_addr.obj_key_));
        }
    }
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
    {
        dstFile.seekp(0, std::ios::end);
        SizeT current_size = dstFile.tellp();
        if (current_size < current_object_size_) {
            std::vector<char> zero_padding(current_object_size_ - current_size, 0);
            dstFile.write(zero_padding.data(), zero_padding.size());
        }
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
    SizeT range_end = object_addr.part_offset_ + object_addr.part_size_;
    range_end = (range_end + ObjAlignment - 1) & ~(ObjAlignment - 1);
    Range orig_range(object_addr.part_offset_, range_end);
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
    SizeT obj_size = (obj_stat->obj_size_ + ObjAlignment - 1) & ~(ObjAlignment - 1);
    if (range.start_ == 0 && range.end_ == obj_size && object_addr.obj_key_ != current_object_key_) {
        if (object_addr.obj_key_.empty()) {
            String error_message = fmt::format("Failed to find object key");
            UnrecoverableError(error_message);
        }
        if (check_ref_count) {
            if (obj_stat->ref_count_ > 0) {
                String error_message = fmt::format("CleanupNoLock object {} ref count is {}", object_addr.obj_key_, obj_stat->ref_count_);
                UnrecoverableError(error_message);
            }
        }
        drop_from_remote_keys.emplace_back(object_addr.obj_key_);
        objects_->Invalidate(object_addr.obj_key_);
        LOG_TRACE(fmt::format("Deleted object {}", object_addr.obj_key_));
    } else {
        objects_->PutNoCount(object_addr.obj_key_, *obj_stat);
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

void PersistenceManager::SaveLocalPath(const String &file_path, const ObjAddr &object_addr) { AddObjAddrToKVStore(file_path, object_addr); }

void PersistenceManager::SaveObjStat(const String &obj_key, const ObjStat &obj_stat) {
    std::lock_guard<std::mutex> lock(mtx_);
    objects_->PutNoCount(obj_key, obj_stat);
}

void PersistenceManager::AddObjAddrToKVStore(const String &path, const ObjAddr &obj_addr) {
    String key = KeyEncode::PMObjectKey(RemovePrefix(path));
    String value = obj_addr.Serialize().dump();
    Status status = kv_store_->Put(key, value);
    if (!status.ok()) {
        UnrecoverableError(status.message());
    }
}

String PersistenceManager::RemovePrefix(const String &path) {
    if (path.starts_with(local_data_dir_)) {
        return path.substr(local_data_dir_.length());
    }
    if (!path.starts_with("/")) {
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

    String pm_fp_key = KeyEncode::PMObjectKey(local_path);
    String value;
    Status status = kv_store_->Get(pm_fp_key, value);
    if (!status.ok()) {
        String error_message = fmt::format("Failed to find object for local path {}", local_path);
        LOG_WARN(error_message);
        return result;
    }
    ObjAddr obj_addr;
    obj_addr.Deserialize(value);

    CleanupNoLock(obj_addr, result.persist_keys_, result.drop_keys_, result.drop_from_remote_keys_, true);
    LOG_TRACE(fmt::format("Deleted mapping from local path {} to ObjAddr({}, {}, {})",
                          local_path,
                          obj_addr.obj_key_,
                          obj_addr.part_offset_,
                          obj_addr.part_size_));
    return result;
}

void PersistenceManager::SetKvStore(KVStore *kv_store) {
    if (kv_store_) {
        UnrecoverableError("KVStore has been set");
    }
    kv_store_ = kv_store;
    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();
    objects_->Deserialize(kv_instance.get());
}

HashMap<String, ObjStat> PersistenceManager::GetAllObjects() const { return objects_->GetAllObjects(); }

HashMap<String, ObjAddr> PersistenceManager::GetAllFiles() const {
    HashMap<String, ObjAddr> local_path_obj;
    const String &obj_prefix = KeyEncode::PMObjectPrefix();
    SizeT obj_prefix_len = obj_prefix.size();

    UniquePtr<KVInstance> kv_instance = kv_store_->GetInstance();
    auto iter = kv_instance->GetIterator();
    iter->Seek(obj_prefix);
    while (iter->Valid() && iter->Key().starts_with(obj_prefix)) {
        String path = iter->Key().ToString().substr(obj_prefix_len);
        ObjAddr obj_addr;
        obj_addr.Deserialize(iter->Value().ToString());
        local_path_obj.emplace(path, obj_addr);
        iter->Next();
    }
    return local_path_obj;
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
            obj_stats_[i] = std::move(obj_stat);
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
        persistence_manager->SaveObjStat(obj_addrs_[i].obj_key_, obj_stats_[i]);
    }
}

} // namespace infinity