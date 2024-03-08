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

export module file_system_type;

import stl;

namespace infinity {

export enum class FileSystemType {
    kPosix,
    kS3,
    kHDFS,
    kNFS,
};

export enum class FileLockType { kNoLock, kReadLock, kWriteLock };

export class FileFlags {
public:
    static constexpr u8 READ_FLAG = 1 << 0;
    static constexpr u8 WRITE_FLAG = 1 << 1;
    static constexpr u8 DIRECT_IO = 1 << 2;
    static constexpr u8 CREATE_FLAG = 1 << 3;
    static constexpr u8 TRUNCATE_CREATE = 1 << 4;
    static constexpr u8 APPEND_FLAG = 1 << 5;
};

} // namespace infinity
