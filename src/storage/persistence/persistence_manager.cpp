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
import logger;

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

void ObjAddr::WriteBufAdv(char *&buf) const {
    ::infinity::WriteBufAdv(buf, obj_key_);
    ::infinity::WriteBufAdv(buf, part_offset_);
    ::infinity::WriteBufAdv(buf, part_size_);
}

void ObjAddr::ReadBufAdv(char *&buf) {
    obj_key_ = ::infinity::ReadBufAdv<String>(buf);
    part_offset_ = ::infinity::ReadBufAdv<SizeT>(buf);
    part_size_ = ::infinity::ReadBufAdv<SizeT>(buf);
}

nlohmann::json ObjStat::Serialize() const {
    nlohmann::json obj;
    obj["obj_size"] = obj_size_;
    obj["deleted_size"] = deleted_size_;
    obj["deleted_ranges"] = nlohmann::json::array();
    for (auto &range : deleted_ranges_) {
        nlohmann::json range_obj;
        range_obj["start"] = range.start_;
        range_obj["end"] = range.end_;
        obj["deleted_ranges"].emplace_back(range_obj);
    }
    return obj;
}

void ObjStat::Deserialize(const nlohmann::json &obj) {
    obj_size_ = 0;
    ref_count_ = 0;
    deleted_size_ = 0;
    if (obj.contains("obj_size")) {
        obj_size_ = 0;
    }
    if (obj.contains("deleted_size")) {
        deleted_size_ = 0;
    }
    if (obj.contains("deleted_ranges")) {
        SizeT start = 0;
        SizeT end = 0;
        for (auto &range_obj : obj["deleted_ranges"]) {
            if (range_obj.contains("start")) {
                start = range_obj["start"];
            }
            if (range_obj.contains("end")) {
                end = range_obj["end"];
            }
            deleted_ranges_.emplace(Range{.start_ = start, .end_ = end});
        }
    }
}

void ObjStat::WriteBufAdv(char *&buf) const {
    ::infinity::WriteBufAdv(buf, obj_size_);
    ::infinity::WriteBufAdv(buf, deleted_size_);
    ::infinity::WriteBufAdv(buf, deleted_ranges_.size());
    for (auto &range : deleted_ranges_) {
        ::infinity::WriteBufAdv(buf, range.start_);
        ::infinity::WriteBufAdv(buf, range.end_);
    }
}

void ObjStat::ReadBufAdv(char *&buf) {
    obj_size_ = ::infinity::ReadBufAdv<SizeT>(buf);
    ref_count_ = 0;
    deleted_size_ = ::infinity::ReadBufAdv<SizeT>(buf);

    SizeT start, end;
    SizeT len = ::infinity::ReadBufAdv<SizeT>(buf);
    for (SizeT i = 0; i < len; ++i) {
        start = ::infinity::ReadBufAdv<SizeT>(buf);
        end = ::infinity::ReadBufAdv<SizeT>(buf);
        deleted_ranges_.emplace(Range{.start_ = start, .end_ = end});
    }
}

PersistenceManager::PersistenceManager(const String &workspace, const String &data_dir, SizeT object_size_limit)
    : workspace_(workspace), local_data_dir_(data_dir), object_size_limit_(object_size_limit) {
    current_object_key_ = ObjCreate();
    current_object_size_ = 0;

    if (local_data_dir_.empty() || local_data_dir_.back() != '/') {
        local_data_dir_ += '/';
    }
}

PersistenceManager::~PersistenceManager() {
    SizeT sum_ref_count = 0;
    for (auto& [key, obj_stat] : objects_) {
        if (obj_stat.ref_count_ > 0) {
            LOG_ERROR(fmt::format("Object {} still has ref count {}", key, obj_stat.ref_count_));
        }
        sum_ref_count += obj_stat.ref_count_;
    }
    assert(sum_ref_count == 0);
}


ObjAddr PersistenceManager::Persist(const String &file_path, bool allow_compose) {
    std::error_code ec;
    fs::path src_fp = file_path;
    SizeT src_size = fs::file_size(src_fp, ec);
    if (ec) {
        String error_message = fmt::format("Failed to get file size of {}.", file_path);
        UnrecoverableError(error_message);
    }
    if (src_size >= object_size_limit_ || !allow_compose) {
        String obj_key = ObjCreate();
        fs::path dst_fp = workspace_;
        dst_fp.append(obj_key);
        bool ok = fs::copy_file(src_fp, dst_fp, fs::copy_options::overwrite_existing, ec);
        if (!ok) {
            String error_message = fmt::format("Failed to copy file {}.", file_path);
            UnrecoverableError(error_message);
        }
        ObjAddr obj_addr(obj_key, 0, src_size);
        std::lock_guard<std::mutex> lock(mtx_);
        objects_.emplace(obj_key, ObjStat(src_size, 0, 0));

        String local_path = RemovePrefix(file_path);
        if (local_path.empty()) {
            String error_message = fmt::format("Failed to find local path of {}", local_path);
            UnrecoverableError(error_message);
        }
        local_path_obj_[local_path] = obj_addr;
        fmt::print("Persist {} to {}\n", file_path, obj_key);
        return obj_addr;
    } else {
        std::lock_guard<std::mutex> lock(mtx_);
        if (int(src_size) > CurrentObjRoomNoLock()) {
            CurrentObjFinalizeNoLock();
        }
        ObjAddr obj_addr(current_object_key_, current_object_size_, src_size);
        CurrentObjAppendNoLock(file_path, src_size);

        String local_path = RemovePrefix(file_path);
        if (local_path.empty()) {
            String error_message = fmt::format("Failed to find local path of {}", local_path);
            UnrecoverableError(error_message);
        }
        local_path_obj_[local_path] = obj_addr;
        fmt::print("Append {} to {}\n", file_path, current_object_key_);
        return obj_addr;
    }
}

ObjAddr PersistenceManager::Persist(const char *data, SizeT src_size, bool allow_compose) {
    fs::path dst_fp = workspace_;
    if (src_size >= object_size_limit_ || !allow_compose) {
        String obj_key = ObjCreate();
        dst_fp.append(obj_key);
        std::ofstream outFile(dst_fp, std::ios::app);
        if (!outFile.is_open()) {
            String error_message = fmt::format("Failed to open file {}.", dst_fp.string());
            UnrecoverableError(error_message);
        }
        outFile.write(data, src_size);
        outFile.close();
        ObjAddr obj_addr(obj_key, 0, src_size);
        std::lock_guard<std::mutex> lock(mtx_);
        objects_.emplace(obj_key, ObjStat(src_size, 0, 0));
        return obj_addr;
    } else {
        dst_fp.append(current_object_key_);
        std::lock_guard<std::mutex> lock(mtx_);
        if (int(src_size) > CurrentObjRoomNoLock()) {
            CurrentObjFinalizeNoLock();
        }
        ObjAddr obj_addr(current_object_key_, current_object_size_, src_size);
        std::ofstream outFile(dst_fp, std::ios::app);
        if (!outFile.is_open()) {
            String error_message = fmt::format("Failed to open file {}.", dst_fp.string());
            UnrecoverableError(error_message);
        }
        outFile.write(data, src_size);
        outFile.close();
        current_object_size_ += src_size;
        if (current_object_size_ >= object_size_limit_) {
            objects_.emplace(current_object_key_, ObjStat(src_size, 0, 0));
            current_object_key_ = ObjCreate();
            current_object_size_ = 0;
        }
        return obj_addr;
    }
}

// TODO:
// - Add a 4-byte pad CRC32 checksum of the whole object to detect Silent Data Corruption.
// - Upload the finalized object to object store in background.
void PersistenceManager::CurrentObjFinalize() {
    std::lock_guard<std::mutex> lock(mtx_);
    CurrentObjFinalizeNoLock();
}

void PersistenceManager::CurrentObjFinalizeNoLock() {
    if (current_object_size_ > 0) {
        objects_.emplace(current_object_key_, ObjStat(current_object_size_, 0, 0));
        current_object_key_ = ObjCreate();
        current_object_size_ = 0;
    }
}

String PersistenceManager::GetObjCache(const String &file_path) {
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("Failed to find object for local path {}", local_path);
        UnrecoverableError(error_message);
    }
    auto oit = objects_.find(it->second.obj_key_);
    if (oit != objects_.end()) {
        oit->second.ref_count_++;
        fmt::print("get obj cache {}, key = {}, ref_count = {}\n", file_path, it->second.obj_key_, oit->second.ref_count_);
    }
    return fs::path(workspace_).append(it->second.obj_key_).string();
}

ObjAddr PersistenceManager::GetObjFromLocalPath(const String &file_path) {
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        return ObjAddr();
    }
    return it->second;
}

void PersistenceManager::PutObjCache(const String &file_path, bool is_linked_file) {
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("Failed to find file_path: {} stored object", local_path);
        UnrecoverableError(error_message);
    }
    auto oit = objects_.find(it->second.obj_key_);
    if (oit == objects_.end()) {
        return;
    }

    oit->second.ref_count_--;
    // For large files linked, fill in the file size when putting to ensure obj valid
    if (is_linked_file) {
        String obj_full_path = fs::path(workspace_).append(it->second.obj_key_).string();
        oit->second.obj_size_ = fs::file_size(obj_full_path);
        it->second.part_size_ = oit->second.obj_size_;
        fmt::print("put link file {}, key = {}, size = {}\n", file_path, it->second.obj_key_, oit->second.obj_size_);
    }
    fmt::print("PutObjCache {}, obj = {}, ref_count {}\n", file_path, it->second.obj_key_, oit->second.ref_count_);
    auto it1 = objects_.find(it->second.obj_key_);
    fmt::print("after put ref_count {}, size = {}, key = {}\n", it1->second.ref_count_, it1->second.obj_size_, it->second.obj_key_);
}

String PersistenceManager::ObjCreate() { return UUID().to_string(); }

ObjAddr PersistenceManager::ObjCreateRefCount(const String &file_path) {
    String obj_key = ObjCreate();
    ObjAddr obj_addr = ObjAddr(obj_key, 0, 0);
    {
        std::lock_guard<std::mutex> lock(mtx_);
        objects_.emplace(obj_key, ObjStat(0, 1, 0));
    }

    fs::path src_fp = workspace_;
    fs::path dst_fp = file_path;
    src_fp.append(obj_key);
    try {
        if (fs::exists(dst_fp)) {
            fs::remove(dst_fp);
        }
        fs::create_symlink(src_fp, dst_fp);
    } catch (const fs::filesystem_error &e) {
        String error_message = fmt::format("Failed to link file {}.", file_path);
        UnrecoverableError(error_message);
    }
    std::lock_guard<std::mutex> lock(mtx_);
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }
    local_path_obj_[local_path] = obj_addr;
    return obj_addr;
}

int PersistenceManager::CurrentObjRoomNoLock() { return int(object_size_limit_) - int(current_object_size_); }

void PersistenceManager::CurrentObjAppendNoLock(const String &file_path, SizeT file_size) {
    fs::path src_fp = file_path;
    fs::path dst_fp = workspace_;
    dst_fp.append(current_object_key_);
    std::ifstream srcFile(src_fp, std::ios::binary);
    if (!srcFile.is_open()) {
        String error_message = fmt::format("Failed to open source file {}", file_path);
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
    dstFile.close();
    current_object_size_ += file_size;
    if (current_object_size_ >= object_size_limit_) {
        objects_.emplace(current_object_key_, ObjStat(current_object_size_, 0, 0));
        current_object_key_ = ObjCreate();
        current_object_size_ = 0;
    }
}

void PersistenceManager::CleanupNoLock(const ObjAddr &object_addr) {
    auto it = objects_.find(object_addr.obj_key_);
    if (it == objects_.end()) {
        String error_message = fmt::format("Failed to find object {}", object_addr.obj_key_);
        UnrecoverableError(error_message);
    }
    Range range(object_addr.part_offset_, object_addr.part_offset_ + object_addr.part_size_);
    auto target_range = it->second.deleted_ranges_.lower_bound(range);

    if (target_range != it->second.deleted_ranges_.end() && target_range->HasIntersection(range)) {
        String error_message =
            fmt::format("ObjAddr {} offset {} size {} already been deleted.", object_addr.obj_key_, object_addr.part_offset_, object_addr.part_size_);
        UnrecoverableError(error_message);
    }

    if (target_range != it->second.deleted_ranges_.begin() && std::prev(target_range)->HasIntersection(range)) {
        String error_message =
            fmt::format("ObjAddr {} offset {} size {} already been deleted.", object_addr.obj_key_, object_addr.part_offset_, object_addr.part_size_);
        UnrecoverableError(error_message);
    }

    it->second.deleted_ranges_.insert(range);
    it->second.deleted_size_ += object_addr.part_size_;
    assert(it->second.deleted_size_ <= it->second.obj_size_);
    if (it->second.deleted_size_ == it->second.obj_size_ && object_addr.obj_key_ != current_object_key_) {
        fs::path fp = workspace_;
        fp.append(object_addr.obj_key_);
        fs::remove(fp);
        objects_.erase(it);
    }
}

ObjStat PersistenceManager::GetObjStatByObjAddr(const ObjAddr &obj_addr) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = objects_.find(obj_addr.obj_key_);
    if (it == objects_.end()) {
        return ObjStat();
    }
    return it->second;
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
    } else {
        local_path_obj_.emplace(local_path, object_addr);
    }
}

void PersistenceManager::SaveObjStat(const ObjAddr &obj_addr, const ObjStat &obj_stat) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = objects_.find(obj_addr.obj_key_);
    if (it != objects_.end()) {
        it->second = obj_stat;
    } else {
        objects_.emplace(obj_addr.obj_key_, obj_stat);
    }
}

String PersistenceManager::RemovePrefix(const String &path) {
    if (path.starts_with(local_data_dir_)) {
        return path.substr(local_data_dir_.length());
    }
    return "";
}

SizeT PersistenceManager::SumRefCounts() {
    SizeT ref_counts = 0;
    for (auto& [key, obj_stat] : objects_) {
        fmt::print("key = {}, ref count = {}\n", key, obj_stat.ref_count_);
        ref_counts += obj_stat.ref_count_;
    }
    return ref_counts;
}

void PersistenceManager::Cleanup(const String &file_path) {
    String local_path = RemovePrefix(file_path);
    if (local_path.empty()) {
        String error_message = fmt::format("Failed to find local path of {}", local_path);
        UnrecoverableError(error_message);
    }

    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("Failed to find object {}", local_path);
        UnrecoverableError(error_message);
    }
    CleanupNoLock(it->second);
    local_path_obj_.erase(it);
}

nlohmann::json PersistenceManager::Serialize() {
    std::lock_guard<std::mutex> lock(mtx_);
    nlohmann::json json_obj;
    json_obj["obj_stat_size"] = objects_.size();
    json_obj["obj_stat_array"] = nlohmann::json::array();
    for (auto &[path, obj_stat] : objects_) {
        nlohmann::json pair;
        pair["obj_path"] = path;
        pair["obj_stat"] = obj_stat.Serialize();
        json_obj["obj_stat_array"].emplace_back(pair);
    }
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
    SizeT len = 0;
    if (obj.contains("obj_stat_size")) {
        len = obj["obj_stat_size"];
    }
    for (SizeT i = 0; i < len; ++i) {
        auto &json_pair = obj["obj_stat_array"][i];
        String path = json_pair["obj_path"];
        ObjStat obj_stat;
        obj_stat.Deserialize(json_pair["obj_stat"]);
        objects_.emplace(path, obj_stat);
    }
    len = 0;
    if (obj.contains("obj_addr_size")) {
        len = obj["obj_addr_size"];
    }
    for (SizeT i = 0; i < len; ++i) {
        auto &json_pair = obj["obj_addr_array"][i];
        String path = json_pair["local_path"];
        ObjAddr obj_addr;
        obj_addr.Deserialize(json_pair["obj_addr"]);
        local_path_obj_.emplace(path, obj_addr);
    }
}

void PersistenceManager::WriteBufAdv(char *&buf, const Vector<String> &local_paths) {
    ::infinity::WriteBufAdv(buf, local_paths.size());
    for (auto &path : local_paths) {
        ObjAddr obj_addr = GetObjFromLocalPath(path);
        ::infinity::WriteBufAdv(buf, path);
        obj_addr.WriteBufAdv(buf);
        if (!obj_addr.Valid()) {
            ObjStat invalid_stat;
            invalid_stat.WriteBufAdv(buf);
        } else {
            ObjStat obj_stat = GetObjStatByObjAddr(obj_addr);
            obj_stat.WriteBufAdv(buf);
        }
    }
}

void PersistenceManager::ReadBufAdv(char *&buf) {
    SizeT size = ::infinity::ReadBufAdv<SizeT>(buf);
    for (SizeT i = 0; i < size; ++i) {
        String path = ::infinity::ReadBufAdv<String>(buf);
        ObjAddr obj_addr;
        ObjStat obj_stat;
        obj_addr.ReadBufAdv(buf);
        obj_stat.ReadBufAdv(buf);
        SaveLocalPath(path, obj_addr);
        SaveObjStat(obj_addr, obj_stat);
    }
}

SizeT PersistenceManager::GetSizeInBytes(const Vector<String> &local_paths) {
    SizeT size = sizeof(SizeT);
    for (auto &path : local_paths) {
        ObjAddr obj_addr = GetObjFromLocalPath(path);
        ObjStat obj_stat = GetObjStatByObjAddr(obj_addr);
        size += sizeof(i32) + path.size();
        size += sizeof(i32) + obj_addr.obj_key_.size() + sizeof(SizeT) + sizeof(SizeT);
        size += sizeof(SizeT) + sizeof(SizeT) + sizeof(SizeT) + obj_stat.deleted_ranges_.size() * (2 * sizeof(SizeT));
    }
    return size;
}

} // namespace infinity