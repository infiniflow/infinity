#pragma once

#include "storage/compression/int_encoder.h"

#include <cstdint>
#include <vector>

namespace infinity {

struct PostingValue {
    enum ValueType {
        UNKNOWN,
        U16,
        U32,
    };
    virtual ~PostingValue() {}

    virtual size_t
    GetSize() const = 0;

    virtual uint32_t
    Encode(ByteSliceWriter& slice_writer, const uint8_t* src, uint32_t len) const = 0;

    virtual uint32_t
    Decode(uint8_t* dest, uint32_t dest_len, ByteSliceReader& slice_reader) const = 0;

    uint32_t location_;
    uint32_t offset_;
};

template<typename T>
struct ValueTypeTraits {
    static const PostingValue::ValueType TYPE = PostingValue::UNKNOWN;
};
template<>
struct ValueTypeTraits<uint16_t> {
    static const PostingValue::ValueType TYPE = PostingValue::U16;
};
template<>
struct ValueTypeTraits<uint32_t> {
    static const PostingValue::ValueType TYPE = PostingValue::U32;
};

typedef IntEncoder<uint32_t, NewPForDeltaCompressor> Int32Encoder;
typedef IntEncoder<uint16_t, NewPForDeltaCompressor> Int16Encoder;

template<typename T>
struct EncoderTypeTraits {
    struct UnknownType {};
    typedef UnknownType Encoder;
};
template<>
struct EncoderTypeTraits<uint16_t> {
    typedef IntEncoder<uint16_t, NewPForDeltaCompressor> Encoder;
};
template<>
struct EncoderTypeTraits<uint32_t> {
    typedef IntEncoder<uint32_t, NewPForDeltaCompressor> Encoder;
};

extern const Int32Encoder*
GetDocIDEncoder();

extern const Int32Encoder*
GetTFEncoder();

extern const Int16Encoder*
GetDocPayloadEncoder();

extern const Int32Encoder*
GetSkipListEncoder();

extern const Int32Encoder*
GetPosListEncoder();


template<typename T>
struct TypedPostingValue : public PostingValue {
    typedef typename EncoderTypeTraits<T>::Encoder Encoder;

    size_t
    GetSize() const override {
        return sizeof(T);
    }

    uint32_t
    Encode(ByteSliceWriter& slice_writer, const uint8_t* src, uint32_t len) const override {
        return encoder_->Encode(slice_writer, (const T*)src, len / sizeof(T));
    }

    uint32_t
    Decode(uint8_t* dest, uint32_t dest_len, ByteSliceReader& slice_reader) const override {
        return encoder_->Decode((T*)dest, dest_len / sizeof(T), slice_reader);
    }

    const Encoder* encoder_;
};

struct PostingValues {
    ~PostingValues() {
        for(size_t i = 0; i < values_.size(); ++i) {
            delete values_[i];
        }
    }

    PostingValue*
    GetValue(size_t index) const {
        return values_[index];
    }

    size_t
    GetSize() const {
        return values_.size();
    }

    size_t
    GetTotalSize() const {
        return values_.size() * values_[0]->GetSize();
    }

    void
    AddValue(PostingValue* value) {
        values_.push_back(value);
    }

    std::vector<PostingValue*> values_;
};

}// namespace infinity
