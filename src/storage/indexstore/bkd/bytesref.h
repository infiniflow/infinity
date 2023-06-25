#pragma once

#include <memory>
#include <string>
#include <vector>

namespace infinity {

class BytesRef final : public std::enable_shared_from_this<BytesRef> {
public:
    static std::vector<uint8_t> EMPTY_BYTES_;

    std::vector<uint8_t> bytes_;

    int offset_ = 0;

    int length_ = 0;

    BytesRef();

    BytesRef(std::vector<uint8_t> &bytes, int offset, int length);

    explicit BytesRef(std::vector<uint8_t> &bytes);

    explicit BytesRef(int capacity);

    bool IsValid();
};

class BytesRefBuilder : public std::enable_shared_from_this<BytesRefBuilder> {

private:
    const std::shared_ptr<BytesRef> ref_;

public:
    BytesRefBuilder() = default;

    std::vector<uint8_t> Bytes() {
        return ref_->bytes_;
    }

    int Length() {
        return ref_->length_;
    }

    void SetLength(int length) {
        ref_->length_ = length;
    }

    uint8_t ByteAt(int offset) {
        return ref_->bytes_.at(offset);
    }

    void SetByteAt(int offset, uint8_t b) {
        ref_->bytes_.at(offset) = b;
    }

    void Grow(int capacity) {
        ref_->bytes_.resize(capacity);
    }

    void Append(uint8_t b) {
        Grow(ref_->length_ + 1);
        ref_->bytes_.at(ref_->length_++) = b;
    }

    void Append(std::vector<uint8_t> &b, int off, int len) {
        Grow(ref_->length_ + len);
        std::copy(b.begin()+off, b.begin()+off+len, ref_->bytes_.begin()+ref_->length_);
        ref_->length_ += len;
    }

    void Append(const std::shared_ptr<BytesRef>& ref) {
        Append(ref->bytes_, ref->offset_, ref->length_);
    }

    void Clear() {
        SetLength(0);
    }

    std::shared_ptr<BytesRef> Get() {
        return ref_;
    }

};

}