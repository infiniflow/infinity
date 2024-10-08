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



export module file_reader;

import stl;
import status;
import infinity_exception;
import local_file_handle;
import virtual_store;

namespace infinity {

export class FileReader {
public:
    explicit FileReader(const String &path, SizeT buffer_size);

    FileReader(const FileReader &other) = delete;

    String path_{};
    UniquePtr<char_t[]> data_{};
    u64 buffer_offset_{};
    u64 already_read_size_{};
    SizeT buffer_start_{};
    SizeT buffer_size_{};
    SizeT buffer_length_{};
    SizeT file_size_{};
    UniquePtr<LocalFileHandle> file_handle_{};

public:
    u8 ReadByte();

    i16 ReadShort();

    i32 ReadInt();

    i32 ReadVInt();

    i64 ReadLong();

    i64 ReadVLong();

    void Read(char_t *buffer, SizeT read_size);

    bool Finished() const;

    u64 GetFilePointer() const;

    void Seek(const u64 pos);

private:
    void ReFill();
};

inline u8 FileReader::ReadByte() {
    if (buffer_offset_ >= buffer_length_) {
        ReFill();
    }
    return data_[buffer_offset_++];
}

inline i16 FileReader::ReadShort() { return static_cast<i16>(((ReadByte() & 0xFF) << 8) | (ReadByte() & 0xFF)); }

inline i32 FileReader::ReadInt() {
    i32 b = (ReadByte() << 24);
    b |= (ReadByte() << 16);
    b |= (ReadByte() << 8);
    return (b | ReadByte());
}

inline i32 FileReader::ReadVInt() {
    u8 b = ReadByte();
    i32 i = b & 0x7F;
    for (i32 shift = 7; (b & 0x80) != 0; shift += 7) {
        b = ReadByte();
        i |= (b & 0x7F) << shift;
    }
    return i;
}

inline i64 FileReader::ReadLong() {
    i64 i = ((i64)ReadInt() << 32);
    return (i | ((i64)ReadInt() & 0xFFFFFFFFL));
}

inline i64 FileReader::ReadVLong() {
    u8 b = ReadByte();
    i64 i = b & 0x7F;
    for (i32 shift = 7; (b & 0x80) != 0; shift += 7) {
        b = ReadByte();
        i |= (((i64)b) & 0x7FL) << shift;
    }
    return i;
}

} // namespace infinity
