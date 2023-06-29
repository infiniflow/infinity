#pragma once

#include "buffer_base.h"
#include "common/utility/infinity_assert.h"

#include <forward_list>
#include <iostream>
namespace infinity{

#define DEFAULT_BUFFER_SIZE 1024ULL

class WriteBuffer : public BufferBase {
    const size_t max_total_size_;
    const size_t initial_chunk_size_;
    const double growth_rate_;
    const size_t max_chunk_size_;

    using Container = std::forward_list<BufferBase::Buffer>;

    Container chunk_list_;
    Container::iterator chunk_tail_;
    size_t total_chunks_size_ = 0;
    bool finalized_ = false;
public:
    explicit WriteBuffer(
    	size_t max_total_size = 0,
    	size_t initial_chunk_size = DEFAULT_BUFFER_SIZE,
    	double growth_rate = 2.0,
    	size_t max_chunk_size = 128 * DEFAULT_BUFFER_SIZE)
    	:max_total_size_(max_total_size),
    	initial_chunk_size_(initial_chunk_size),
    	growth_rate_(growth_rate),
    	max_chunk_size_(max_chunk_size),
    	BufferBase(nullptr, 0, 0){
    	AddChunk();
    }

	~WriteBuffer() {
	    for (const auto & range : chunk_list_)
	        delete [](range.Begin()); //TODO arena to be added
	}

    void Set(Position ptr, size_t size) { 
    	BufferBase::Set(ptr, size, 0); 
    }

    void Next() {
        if (!Offset())
            return;

        auto bytes_in_buffer = Offset();

        try {
            NextImpl();
        }
        catch (...) {
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

	void WriteByte(uint8_t x) {
        StorageAssert(!finalized_, "Cannot write to finalized buffer");
        NextIfAtEnd();
        *pos_ = x;
        ++pos_;
    }

    void WriteInt(const int32_t i) {
	    WriteByte((uint8_t)(i >> 24));
	    WriteByte((uint8_t)(i >> 16));
	    WriteByte((uint8_t)(i >>  8));
	    WriteByte((uint8_t) i);
    }

    void WriteVInt(const int32_t vi) {
	    uint32_t i = vi;
	    while ((i & ~0x7F) != 0) {
	        WriteByte((uint8_t)((i & 0x7f) | 0x80));
	        i >>= 7;
	    }
	    WriteByte((uint8_t)i);
    }

	void WriteLong(const int64_t i) {
	    WriteInt((int32_t) (i >> 32));
	    WriteInt((int32_t) i);
	}

	void WriteVLong(const int64_t vi) {
	    uint64_t i = vi;
	    while ((i & ~0x7F) != 0) {
	      WriteByte((u8)((i & 0x7f) | 0x80));
	      i >>= 7;
	    }
	    WriteByte((u8)i);
	}

    void Write(const char * from, size_t n) {
    	StorageAssert(!finalized_, "Cannot write to finalized buffer");
        size_t bytes_copied = 0;

        assert(!working_buffer_.Empty());
        while (bytes_copied < n) {
            NextIfAtEnd();
            size_t bytes_to_copy = std::min(static_cast<size_t>(working_buffer_.End() - pos_), n - bytes_copied);
            memcpy(pos_, from + bytes_copied, bytes_to_copy);
            pos_ += bytes_to_copy;
            bytes_copied += bytes_to_copy;
        }
    }

	void Finalize() {
        if (finalized_)
            return;

        try {
            Next();
            finalized_ = true;
        }
        catch (...) {
            pos_ = working_buffer_.Begin();
            finalized_ = true;
            throw;
        }
    }

	size_t WriteTo(char * to, size_t n) {
		n = std::min(n, Count());
        size_t bytes_copied = 0;
        Buffer working_buffer = Buffer(0,0);
        Container::iterator chunk = chunk_list_.begin();
        while (bytes_copied < n) {
        	working_buffer = *chunk;
            size_t bytes_to_copy = std::min(static_cast<size_t>(working_buffer.Size()), n - bytes_copied);
            ::memcpy(to + bytes_copied, working_buffer.Begin(), bytes_to_copy);
            bytes_copied += bytes_to_copy;
            chunk = std::next(chunk);
        }
        return bytes_copied;
    } 

    void Reset() {
        ResetWorkingBuffer();
        bytes_ = 0;
        if(!chunk_list_.empty())
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
	    size_t next_chunk_size;
	    if (chunk_list_.empty()) {
	        chunk_tail_ = chunk_list_.before_begin();
	        next_chunk_size = initial_chunk_size_;
	    } else {
	        next_chunk_size = std::max(1uz, static_cast<size_t>(chunk_tail_->Size() * growth_rate_));
	        next_chunk_size = std::min(next_chunk_size, max_chunk_size_);
	    }

	    if (max_total_size_) {
	        if (total_chunks_size_ + next_chunk_size > max_total_size_)
	            next_chunk_size = max_total_size_ - total_chunks_size_;

	        if (0 == next_chunk_size) {
	            Set(Pos(), 0);
	            throw StorageException("current buffer exhaused"); 
	        }
	    }

	    Position begin = reinterpret_cast<Position>(new char[next_chunk_size]);
	    chunk_tail_ = chunk_list_.emplace_after(chunk_tail_, begin, begin + next_chunk_size);
	    total_chunks_size_ += next_chunk_size;

	    Set(chunk_tail_->Begin(), chunk_tail_->Size());
	}

};

}