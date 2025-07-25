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

#include <cassert>

export module infinity_core:fst.writer;
import :stl;
import :crc;

export namespace infinity {

class Writer {
public:
    virtual ~Writer() = default;
    virtual void Write(const u8 *buf, SizeT size) = 0;
    virtual void Flush() = 0;
};

class FstBufferWriter : public Writer {
public:
    Vector<u8> &buffer_;

public:
    FstBufferWriter(Vector<u8> &buffer) : buffer_(buffer) {}

    void Write(const u8 *data_ptr, SizeT data_size) override { buffer_.insert(buffer_.end(), data_ptr, data_ptr + data_size); }

    void Flush() override {}
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
