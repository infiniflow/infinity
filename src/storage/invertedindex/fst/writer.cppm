module;
#include <cassert>
import stl;
import crc;
export module fst:writer;

namespace infinity {

class Writer {
public:
    virtual ~Writer() = default;
    virtual void Write(const u8 *buf, SizeT size) = 0;
    virtual void Flush() = 0;
};

class BufferWriter : public Writer {
private:
    u8 *buffer_;
    SizeT bufferSize_;
    SizeT currentPos_;

public:
    BufferWriter(u8 *buf, SizeT size) : buffer_(buf), bufferSize_(size), currentPos_(0) {}

    void Write(const u8 *buf, SizeT size) override {
        SizeT remainingSpace = bufferSize_ - currentPos_;
        assert(size <= remainingSpace);
        SizeT bytesToCopy = std::min(remainingSpace, size);
        std::memcpy(buffer_ + currentPos_, buf, bytesToCopy);
        currentPos_ += bytesToCopy;
    }

    void Flush() override {}

    // Additional method to get the content of the buffer
    const u8 *GetBuffer() const { return buffer_; }

    // Additional method to get the current position in the buffer
    SizeT GetCurrentPosition() const { return currentPos_; }
};

class OstreamWriter : public Writer {
private:
    std::ostream &ostream_;

public:
    explicit OstreamWriter(std::ostream &os) : ostream_(os) {}

    void Write(const u8 *buf, SizeT size) override { ostream_.write((const char *)buf, size); }

    void Flush() override { ostream_.flush(); }
};

/// Wraps any writer and counts the number of bytes written.
class CountingWriter : public Writer {
private:
    Writer &wtr_;
    u64 count_ = 0;
    CRC32IEEE summer_ = {};

public:
    explicit CountingWriter(Writer &wtr) : wtr_(wtr) {}

    void Write(const u8 *buf, SizeT size) override {
        wtr_.Write(buf, size);
        count_ += size;
        summer_.update(buf, size);
    }

    void Flush() override { wtr_.Flush(); }

    /// Return the total number of bytes written to the underlying writer.
    ///
    /// The count returned is the sum of all counts resulting from a call
    /// to `write`.
    u64 Count() const { return count_; }

    /// Returns the "masked" CRC32 checksum of the data so far using the
    /// Castagnoli polynomial. This "masked" checksum is the same one used
    /// by the Snappy frame format. Masking is supposed to make the checksum
    /// robust with respect to data that contains the checksum itself.
    u32 MaskedChecksum() {
        u32 sum = summer_.finalize();
        return ((sum >> 15) | (sum << 17)) + 0xa282ead8ul;
    }

    /// Gets a reference to the underlying writer.
    Writer &GetRef() { return wtr_; }
};

} // namespace infinity
