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

export module buffer_base;

import stl;

namespace infinity {

// Base class for ReadBuffer and WriteBuffer.
export class BufferBase {
public:
    using Position = char *;

    struct Buffer {
        Buffer(Position begin_pos, Position end_pos) : begin_pos_(begin_pos), end_pos_(end_pos) {}

        Position Begin() const { return begin_pos_; }
        Position End() const { return end_pos_; }
        SizeT Size() const { return SizeT(end_pos_ - begin_pos_); }
        void Resize(SizeT size) { end_pos_ = begin_pos_ + size; }
        bool Empty() const { return Size() == 0; }

    private:
        Position begin_pos_;
        Position end_pos_;
    };

    BufferBase(Position ptr, SizeT size, SizeT offset) : pos_(ptr + offset), working_buffer_(ptr, ptr + size), internal_buffer_(ptr, ptr + size) {}

    void Set(Position ptr, SizeT size, SizeT offset) {
        internal_buffer_ = Buffer(ptr, ptr + size);
        working_buffer_ = Buffer(ptr, ptr + size);
        pos_ = ptr + offset;
    }

    Buffer &InternalBuffer() { return internal_buffer_; }

    Buffer &GetBuffer() { return working_buffer_; }

    Position &Pos() { return pos_; }

    SizeT Offset() const { return SizeT(pos_ - working_buffer_.Begin()); }

    /// How many bytes are available for read/write
    SizeT Available() const { return SizeT(working_buffer_.End() - pos_); }

    /// How many bytes have been read/written
    SizeT Count() const { return bytes_ + Offset(); }

    bool HasPendingData() const { return Available() > 0; }

protected:
    void ResetWorkingBuffer() {
        working_buffer_.Resize(0);
        pos_ = working_buffer_.End();
    }

    /// Read/write position.
    Position pos_;

    /// How many bytes have been read/written, not counting those that are now in the buffer.
    SizeT bytes_ = 0;

    /// A piece of memory that you can use.
    Buffer working_buffer_;

    /// A reference to a piece of memory you are reading.
    Buffer internal_buffer_;
};

} // namespace infinity
