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

#include <unistd.h>

module infinity_core:version_file_worker.impl;

import :version_file_worker;
import :file_worker;
import :block_version;
import :infinity_exception;
import :logger;
import :persistence_manager;
import :infinity_context;
import :fileworker_manager;

import third_party;

namespace infinity {

VersionFileWorker::VersionFileWorker(std::shared_ptr<std::string> file_path, size_t capacity)
    : FileWorker(std::move(file_path)), capacity_(capacity) {}

void VersionFileWorker::Read(BlockVersion *&data, std::unique_ptr<LocalFileHandle> &file_handle, size_t file_size) {
    // std::unique_lock l(mutex_);
    auto &path = *rel_file_path_;
    auto tmp_path = GetWorkingPath();
    if (!inited_) {
        segment_ = boost::interprocess::managed_mapped_file(boost::interprocess::open_or_create_infinity, tmp_path.c_str(), 1145141);
        auto *sm = segment_.get_segment_manager();
        data = segment_.find_or_construct<BlockVersion>(path.c_str())(8192, sm);
        inited_ = true;
        mmap_ = (void *)(-1); // for test
        return;
    }
    auto result = segment_.find<BlockVersion>(path.c_str());
    data = result.first;
}

} // namespace infinity