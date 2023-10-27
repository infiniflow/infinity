//
// Created by jinhai on 23-9-7.
//

module;

import stl;

export module file_system_type;

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
