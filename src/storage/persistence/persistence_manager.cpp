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

void ObjAddr::WriteBuf(char *buf) const {
    WriteBufAdv<String>(buf, obj_key_);
    WriteBufAdv<SizeT>(buf, part_offset_);
    WriteBufAdv<SizeT>(buf, part_size_);
}

void ObjAddr::ReadBuf(const char *buf) {
    obj_key_ = ReadBufAdv<String>(buf);
    part_offset_ = ReadBufAdv<SizeT>(buf);
    part_size_ = ReadBufAdv<SizeT>(buf);
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
        assert(local_path_obj_.count(local_path) == 0);
        local_path_obj_[local_path] = obj_addr;
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
        assert(local_path_obj_.count(local_path) == 0);
        local_path_obj_[local_path] = obj_addr;
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

String PersistenceManager::GetObjCache(const String &local_path) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it == local_path_obj_.end()) {
        String error_message = fmt::format("Failed to find object for local path {}", local_path);
        UnrecoverableError(error_message);
    }
    auto oit = objects_.find(it->second.obj_key_);
    if (oit == objects_.end()) {
        String error_message = fmt::format("Failed to find object {}", it->second.obj_key_);
        UnrecoverableError(error_message);
    }
    oit->second.ref_count_++;
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
        String error_message = fmt::format("Failed to find file_path: {} stored object", local_path);
        UnrecoverableError(error_message);
    }
    return it->second;
}

void PersistenceManager::PutObjCache(const String &file_path) {
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
        String error_message = fmt::format("Failed to find object {}", it->second.obj_key_);
        UnrecoverableError(error_message);
    }
    oit->second.ref_count_--;
}

void PersistenceManager::SaveLocalPath(const String &local_path, const ObjAddr &object_addr) {
    std::lock_guard<std::mutex> lock(mtx_);
    auto it = local_path_obj_.find(local_path);
    if (it != local_path_obj_.end()) {
        it->second = object_addr;
    } else {
        local_path_obj_.emplace(local_path, object_addr);
    }
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
        std::error_code ec;
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
    assert(local_path_obj_.count(local_path) == 0);
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
        String error_message = fmt::format("Failed to open destination file {}", dst_fp.string());
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

String PersistenceManager::RemovePrefix(const String &path) {
    if (path.starts_with(local_data_dir_)) {
        return path.substr(local_data_dir_.length());
    }
    return "";
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
    json_obj["len"] = local_path_obj_.size();
    json_obj["array"] = nlohmann::json::array();
    for (auto &[path, obj_addr] : local_path_obj_) {
        nlohmann::json pair;
        pair["local_path"] = path;
        pair["obj_addr"] = obj_addr.Serialize();
        json_obj["array"].emplace_back(pair);
    }
    return json_obj;
}

void PersistenceManager::Deserialize(const nlohmann::json &obj) {
    std::lock_guard<std::mutex> lock(mtx_);
    SizeT len = obj["len"];
    for (SizeT i = 0; i < len; ++i) {
        auto &json_pair = obj["array"][0];
        String path = json_pair["local_path"];
        ObjAddr obj_addr;
        obj_addr.Deserialize(json_pair["obj_addr"]);
        local_path_obj_.emplace(path, obj_addr);
    }
}

} // namespace infinity