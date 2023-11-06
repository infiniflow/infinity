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
import file_system;
import file_system_type;

export module file_writer;

namespace infinity {

export class FileWriter {
public:
    explicit FileWriter(FileSystem &fs, const String &path, SizeT buffer_size, u8 file_flags = FileFlags::WRITE_FLAG | FileFlags::CREATE_FLAG)
        : fs_(fs), path_(path), buffer_size_(buffer_size), data_(MakeUnique<char_t[]>(buffer_size)), offset_(0), total_written_(0) {
        // Fixme: This function might throw exception
        file_handler_ = fs.OpenFile(path, file_flags, FileLockType::kWriteLock);
    }

    FileSystem &fs_;
    String path_{};
    UniquePtr<char_t[]> data_{};
    i64 offset_{};
    SizeT total_written_{};
    SizeT buffer_size_{};
    UniquePtr<FileHandler> file_handler_{};

public:
    void WriteByte(const u8 b);

    void WriteShort(const i16 i);

    void WriteInt(const i32 i);

    void WriteVInt(const i32 i);

    void WriteLong(const i64 i);

    void WriteVLong(const i64 i);

    void Write(const char_t *buffer, SizeT bytes_count);

    void Sync();

    void Flush();

    i64 GetFileSize();

    SizeT TotalWrittenBytes() const;
};

} // namespace infinity
