#pragma once

#include <algorithm>
#include <cstdint>

namespace infinity {

// Base class for ReadBuffer and WriteBuffer.
class BufferBase {
public:
    using Position = char *;

    struct Buffer {
        Buffer(Position begin_pos, Position end_pos) : begin_pos_(begin_pos), end_pos_(end_pos) {}

        Position Begin() const { return begin_pos_; }
        Position End() const { return end_pos_; }
        size_t Size() const { return size_t(end_pos_ - begin_pos_); }
        void Resize(size_t size) { end_pos_ = begin_pos_ + size; }
        bool Empty() const { return Size() == 0; }

    private:
        Position begin_pos_;
        Position end_pos_;
    };

    BufferBase(Position ptr, size_t size, size_t offset) : pos_(ptr + offset), working_buffer_(ptr, ptr + size), internal_buffer_(ptr, ptr + size) {}

    void Set(Position ptr, size_t size, size_t offset) {
        internal_buffer_ = Buffer(ptr, ptr + size);
        working_buffer_ = Buffer(ptr, ptr + size);
        pos_ = ptr + offset;
    }

    Buffer &InternalBuffer() { return internal_buffer_; }

    Buffer &GetBuffer() { return working_buffer_; }

    Position &Pos() { return pos_; }

    size_t Offset() const { return size_t(pos_ - working_buffer_.Begin()); }

    /// How many bytes are available for read/write
    size_t Available() const { return size_t(working_buffer_.End() - pos_); }

    /// How many bytes have been read/written
    size_t Count() const { return bytes_ + Offset(); }

    bool HasPendingData() const { return Available() > 0; }

protected:
    void ResetWorkingBuffer() {
        working_buffer_.Resize(0);
        pos_ = working_buffer_.End();
    }

    /// Read/write position.
    Position pos_;

    /// How many bytes have been read/written, not counting those that are now in the buffer.
    size_t bytes_ = 0;

    /// A piece of memory that you can use.
    Buffer working_buffer_;

    /// A reference to a piece of memory you are reading.
    Buffer internal_buffer_;
};

} // namespace infinity
