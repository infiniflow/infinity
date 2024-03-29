module;

import stl;
import int_encoder;
import no_compress_encoder;
import vbyte_compress_encoder;

module posting_field;

namespace infinity {

struct EncoderProvider {
    UniquePtr<Int32Encoder> int32_encoder_ = MakeUnique<Int32Encoder>();
    UniquePtr<Int16Encoder> int16_encoder_ = MakeUnique<Int16Encoder>();
    UniquePtr<NoCompressEncoder> no_compress_encoder_ = MakeUnique<NoCompressEncoder>();
    UniquePtr<VByteCompressEncoder> vbyte_compress_encoder_ = MakeUnique<VByteCompressEncoder>();

    static EncoderProvider *GetInstance() {
        static EncoderProvider instance;
        return &instance;
    }

    Int32Encoder *GetInt32Encoder() { return int32_encoder_.get(); }

    Int16Encoder *GetInt16Encoder() { return int16_encoder_.get(); }

    NoCompressEncoder *GetNoCompressEncoder() { return no_compress_encoder_.get(); }

    VByteCompressEncoder *GetVByteCompressEncoder() { return vbyte_compress_encoder_.get(); }
};

const Int32Encoder *GetDocIDEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

const Int32Encoder *GetTFEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

const Int16Encoder *GetDocPayloadEncoder() { return EncoderProvider::GetInstance()->GetInt16Encoder(); }

const Int16Encoder *GetTermPercentageEncoder() { return EncoderProvider::GetInstance()->GetInt16Encoder(); }

const Int32Encoder *GetSkipListEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

const Int32Encoder *GetPosListEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

} // namespace infinity