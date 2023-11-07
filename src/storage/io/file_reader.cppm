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

export module file_reader;

namespace infinity {

export class FileReader {
public:
    explicit FileReader(FileSystem &fs, const String &path, SizeT buffer_size);

    FileReader(const FileReader &other);

    FileSystem &fs_;
    String path_{};
    UniquePtr<char_t[]> data_{};
    i64 buffer_offset_{};
    i64 already_read_size_{};
    SizeT buffer_start_{};
    SizeT buffer_size_{};
    SizeT file_size_{};
    UniquePtr<FileHandler> file_handler_{};

public:
    u8 ReadByte();

    i16 ReadShort();

    i32 ReadInt();

    i32 ReadVInt();

    i64 ReadLong();

    i64 ReadVLong();

    void Read(char_t *buffer, SizeT read_size);

    bool Finished() const;

    i64 GetFilePointer() const;

    void Seek(const i64 pos);

    FileReader *Clone();
};

} // namespace infinity
