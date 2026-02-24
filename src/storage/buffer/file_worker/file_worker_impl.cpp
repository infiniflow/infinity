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

module infinity_core:file_worker.impl;

import :file_worker;
import :infinity_context;
import :fileworker_manager;
import :persist_result_handler;

import std.compat;
import third_party;

import global_resource_usage;

namespace infinity {

FileWorker::FileWorker(std::shared_ptr<std::string> rel_file_path) : rel_file_path_(std::move(rel_file_path)) {
    mmap_ = nullptr;
    persistence_manager_ = InfinityContext::instance().storage()->persistence_manager();
    file_worker_manager_ = InfinityContext::instance().storage()->fileworker_manager();
}

// Get absolute file path. As key of buffer handle.
std::string FileWorker::GetPath() const { return fmt::format("{}/{}", InfinityContext::instance().config()->DataDir(), *rel_file_path_); }

std::string FileWorker::GetWorkingPath() const { return fmt::format("{}/{}", InfinityContext::instance().config()->TempDir(), *rel_file_path_); }

} // namespace infinity