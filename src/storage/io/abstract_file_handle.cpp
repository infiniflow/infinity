// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module abstract_file_handle;

import stl;
import virtual_storage_system_type;
import virtual_storage_system;

namespace infinity {

AbstractFileHandle::~AbstractFileHandle() = default;

AbstractFileHandle::AbstractFileHandle(VirtualStorageSystem *storage_system, StorageType storage_type, const String &path, FileAccessMode access_mode)
    : storage_system_(storage_system), storage_type_(storage_type), path_(path), access_mode_(access_mode) {}

Status AbstractFileHandle::Open(const String &path, FileAccessMode access_mode) { return Status::NotSupport("In abstract class"); };

} // namespace infinity
