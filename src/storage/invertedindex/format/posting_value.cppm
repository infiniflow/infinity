module;

import stl;
import int_encoder;
import byte_slice_reader;
import byte_slice_writer;
import no_compress_encoder;

export module posting_value;

namespace infinity {

export struct PostingValue {
    enum ValueType {
        UNKNOWN,
        U16,
        U32,
    };
    virtual ~PostingValue() {}

    virtual SizeT GetSize() const = 0;

    virtual u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const = 0;

    virtual u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const = 0;

    u32 location_;
    u32 offset_;
};

template <typename T>
struct ValueTypeTraits {
    static const PostingValue::ValueType TYPE = PostingValue::UNKNOWN;
};
template <>
struct ValueTypeTraits<u16> {
    static const PostingValue::ValueType TYPE = PostingValue::U16;
};
template <>
struct ValueTypeTraits<u32> {
    static const PostingValue::ValueType TYPE = PostingValue::U32;
};

export typedef IntEncoder<u32, NewPForDeltaCompressor> Int32Encoder;
export typedef IntEncoder<u16, NewPForDeltaCompressor> Int16Encoder;
export typedef NoCompressIntEncoder<u32> NoCompressEncoder;

template <typename T>
struct EncoderTypeTraits {
    struct UnknownType {};
    typedef UnknownType Encoder;
};
template <>
struct EncoderTypeTraits<u16> {
    typedef IntEncoder<u16, NewPForDeltaCompressor> Encoder;
};
template <>
struct EncoderTypeTraits<u32> {
    typedef IntEncoder<u32, NewPForDeltaCompressor> Encoder;
};

export const Int32Encoder *GetDocIDEncoder();

export const Int32Encoder *GetTFEncoder();

export const Int16Encoder *GetDocPayloadEncoder();

export const Int32Encoder *GetSkipListEncoder();

export const Int32Encoder *GetPosListEncoder();

export template <typename T>
struct TypedPostingValue : public PostingValue {
    typedef typename EncoderTypeTraits<T>::Encoder Encoder;

    SizeT GetSize() const override { return sizeof(T); }

    u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const override {
        return encoder_->Encode(slice_writer, (const T *)src, len / sizeof(T));
    }

    u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const override {
        return encoder_->Decode((T *)dest, dest_len / sizeof(T), slice_reader);
    }

    const Encoder *encoder_;
};

export template <typename T>
struct NoCompressPostingValue : public PostingValue {

    SizeT GetSize() const override { return sizeof(T); }

    u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const override {
        return encoder_->Encode(slice_writer, (const T *)src, len / sizeof(T));
    }

    u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const override {
        return encoder_->Decode((T *)dest, dest_len / sizeof(T), slice_reader);
    }

    const NoCompressEncoder *encoder_;
};

export struct PostingValues {
    ~PostingValues() {
        for (SizeT i = 0; i < values_.size(); ++i) {
            delete values_[i];
        }
    }

    PostingValue *GetValue(SizeT index) const { return values_[index]; }

    SizeT GetSize() const { return values_.size(); }

    SizeT GetTotalSize() const { return values_.size() * values_[0]->GetSize(); }

    void AddValue(PostingValue *value) { values_.push_back(value); }

    Vector<PostingValue *> values_;
};

} // namespace infinity
