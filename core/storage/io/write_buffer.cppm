module;

import buffer_base;
import infinity_assert;
import infinity_exception;
import stl;

export module write_buffer;

namespace infinity {

#define DEFAULT_BUFFER_SIZE 1024ULL

export class WriteBuffer : public BufferBase {
    const SizeT max_total_size_;
    const SizeT initial_chunk_size_;
    const double growth_rate_;
    const SizeT max_chunk_size_;

    using Container = ForwardList<BufferBase::Buffer>;

    Container chunk_list_;
    Container::iterator chunk_tail_;
    SizeT total_chunks_size_ = 0;
    bool finalized_ = false;

public:
    explicit WriteBuffer(SizeT max_total_size = 0,
                         SizeT initial_chunk_size = DEFAULT_BUFFER_SIZE,
                         double growth_rate = 2.0,
                         SizeT max_chunk_size = 128 * DEFAULT_BUFFER_SIZE)
        : max_total_size_(max_total_size), initial_chunk_size_(initial_chunk_size), growth_rate_(growth_rate), max_chunk_size_(max_chunk_size),
          BufferBase(nullptr, 0, 0) {
        AddChunk();
    }

    ~WriteBuffer() {
        for (const auto &range : chunk_list_)
            delete[] (range.Begin()); // TODO arena to be added
    }

    void Set(Position ptr, SizeT size) { BufferBase::Set(ptr, size, 0); }

    void Next() {
        if (!Offset())
            return;

        auto bytes_in_buffer = Offset();

        try {
            NextImpl();
        } catch (...) {
            pos_ = working_buffer_.Begin();
            bytes_ += bytes_in_buffer;
            throw;
        }
        bytes_ += bytes_in_buffer;
        pos_ = working_buffer_.Begin();
    }

    void NextIfAtEnd() {
        if (!HasPendingData())
            Next();
    }

    void WriteByte(u8 x) {
        Error<StorageException>("Cannot write to finalized buffer", __FILE_NAME__, __LINE__);
        NextIfAtEnd();
        *pos_ = x;
        ++pos_;
    }

    void WriteInt(const i32 i) {
        WriteByte((u8)(i >> 24));
        WriteByte((u8)(i >> 16));
        WriteByte((u8)(i >> 8));
        WriteByte((u8)i);
    }

    void WriteVInt(const i32 vi) {
        u32 i = vi;
        while ((i & ~0x7F) != 0) {
            WriteByte((u8)((i & 0x7f) | 0x80));
            i >>= 7;
        }
        WriteByte((u8)i);
    }

    void WriteLong(const i64 i) {
        WriteInt((i32)(i >> 32));
        WriteInt((i32)i);
    }

    void WriteVLong(const i64 vi) {
        u64 i = vi;
        while ((i & ~0x7F) != 0) {
            WriteByte((u8)((i & 0x7f) | 0x80));
            i >>= 7;
        }
        WriteByte((u8)i);
    }

    void Write(const char *from, SizeT n);

    void Finalize() {
        if (finalized_)
            return;

        try {
            Next();
            finalized_ = true;
        } catch (...) {
            pos_ = working_buffer_.Begin();
            finalized_ = true;
            throw;
        }
    }

    SizeT WriteTo(char *to, SizeT n);

    void Reset() {
        ResetWorkingBuffer();
        bytes_ = 0;
        if (!chunk_list_.empty())
            chunk_tail_ = chunk_list_.begin();
        Set(chunk_tail_->Begin(), chunk_tail_->Size());
    }

private:
    void NextImpl() {
#if !defined(unlikely)
#define unlikely(x) (__builtin_expect(!!(x), 0))
#endif
        if (unlikely(HasPendingData())) {
            GetBuffer() = Buffer(pos_, GetBuffer().End());
            return;
        }
        AddChunk();
    }

    void AddChunk() {
        SizeT next_chunk_size;
        if (chunk_list_.empty()) {
            chunk_tail_ = chunk_list_.before_begin();
            next_chunk_size = initial_chunk_size_;
        } else {
            next_chunk_size = Max(1uz, static_cast<SizeT>(chunk_tail_->Size() * growth_rate_));
            next_chunk_size = Min(next_chunk_size, max_chunk_size_);
        }

        if (max_total_size_) {
            if (total_chunks_size_ + next_chunk_size > max_total_size_)
                next_chunk_size = max_total_size_ - total_chunks_size_;

            if (0 == next_chunk_size) {
                Set(Pos(), 0);
                Error<StorageException>("current buffer exhaused", __FILE_NAME__, __LINE__);
            }
        }

        Position begin = reinterpret_cast<Position>(new char[next_chunk_size]);
        chunk_tail_ = chunk_list_.emplace_after(chunk_tail_, begin, begin + next_chunk_size);
        total_chunks_size_ += next_chunk_size;

        Set(chunk_tail_->Begin(), chunk_tail_->Size());
    }
};

} // namespace infinity