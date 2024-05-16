module;

import stl;
import int_encoder;
import fastpfor;
import byte_slice_reader;
import byte_slice_writer;
import no_compress_encoder;
import vbyte_compress_encoder;

export module posting_field;

namespace infinity {

export struct PostingField {
    enum ValueType {
        UNKNOWN,
        U16,
        U32,
    };
    virtual ~PostingField() {}

    virtual SizeT GetSize() const = 0;

    virtual u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const = 0;

    virtual u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const = 0;

    u32 location_;
    u32 offset_;
};

template <typename T>
struct ValueTypeTraits {
    static const PostingField::ValueType TYPE = PostingField::UNKNOWN;
};
template <>
struct ValueTypeTraits<u16> {
    static const PostingField::ValueType TYPE = PostingField::U16;
};
template <>
struct ValueTypeTraits<u32> {
    static const PostingField::ValueType TYPE = PostingField::U32;
};

// export typedef IntEncoder<u32, NewPForDeltaCompressor> Int32Encoder;
export typedef IntEncoder<u32, SIMDBitPacking> Int32Encoder;
export typedef IntEncoder<u16, NewPForDeltaCompressor> Int16Encoder;
export typedef NoCompressIntEncoder<u32> NoCompressEncoder;
export typedef VByteIntEncoder<u32> VByteCompressEncoder;

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
    // typedef IntEncoder<u32, NewPForDeltaCompressor> Encoder;
    typedef IntEncoder<u32, SIMDBitPacking> Encoder;
};

export const Int32Encoder *GetDocIDEncoder();

export const Int32Encoder *GetTFEncoder();

export const Int16Encoder *GetDocPayloadEncoder();

export const Int16Encoder *GetTermPercentageEncoder();

export const Int32Encoder *GetSkipListEncoder();

export const Int32Encoder *GetPosListEncoder();

export template <typename T>
struct TypedPostingField : public PostingField {
    typedef typename EncoderTypeTraits<T>::Encoder Encoder;

    SizeT GetSize() const override { return sizeof(T); }

    u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const override {
        return encoder_->Encode(slice_writer, (const T *)src, len / sizeof(T));
    }

    u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const override {
        return encoder_->Decode((T *)dest, dest_len / sizeof(T), slice_reader);
    }

    const Encoder *encoder_{nullptr};
};

export template <typename T>
struct NoCompressPostingField : public PostingField {

    SizeT GetSize() const override { return sizeof(T); }

    u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const override {
        return encoder_->Encode(slice_writer, (const T *)src, len / sizeof(T));
    }

    u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const override {
        return encoder_->Decode((T *)dest, dest_len / sizeof(T), slice_reader);
    }

    const NoCompressEncoder *encoder_{nullptr};
};

export template <typename T>
struct VByteCompressPostingField : public PostingField {

    SizeT GetSize() const override { return sizeof(T); }

    u32 Encode(ByteSliceWriter &slice_writer, const u8 *src, u32 len) const override {
        return encoder_->Encode(slice_writer, (const T *)src, len / sizeof(T));
    }

    u32 Decode(u8 *dest, u32 dest_len, ByteSliceReader &slice_reader) const override {
        return encoder_->Decode((T *)dest, dest_len / sizeof(T), slice_reader);
    }

    const VByteCompressEncoder *encoder_{nullptr};
};

export struct PostingFields {
    virtual ~PostingFields() {
        for (SizeT i = 0; i < values_.size(); ++i) {
            delete values_[i];
        }
    }

    PostingField *GetValue(SizeT index) const { return values_[index]; }

    SizeT GetSize() const { return values_.size(); }

    SizeT GetTotalSize() const {
        return std::accumulate(values_.begin(), values_.end(), 0, [](SizeT sum, PostingField *field) { return sum + field->GetSize(); });
    }

    void AddValue(PostingField *value) { values_.push_back(value); }

    Vector<PostingField *> values_;
};

} // namespace infinity
