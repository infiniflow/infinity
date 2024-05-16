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

import stl;

module file_system;

namespace infinity {

i64 FileHandler::Read(void *data, u64 nbytes) { return file_system_.Read(*this, data, nbytes); }

i64 FileHandler::Write(const void *data, u64 nbytes) { return file_system_.Write(*this, data, nbytes); }

void FileHandler::Rename(const String &old_name, const String &new_name) { return file_system_.Rename(old_name, new_name); }

void FileHandler::Sync() { return file_system_.SyncFile(*this); }

void FileHandler::Close() { return file_system_.Close(*this); }

} // namespace infinity
