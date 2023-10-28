module;

import stl;
import int_encoder;

module posting_value;

namespace infinity {

struct EncoderProvider {
    UniquePtr<Int32Encoder> int32_encoder_;
    UniquePtr<Int16Encoder> int16_encoder_;

    static EncoderProvider *GetInstance() {
        static EncoderProvider instance;
        return &instance;
    }

    Int32Encoder *GetInt32Encoder() {
        if (!int32_encoder_.get())
            int32_encoder_.reset(new Int32Encoder);
        return int32_encoder_.get();
    }

    Int16Encoder *GetInt16Encoder() {
        if (!int16_encoder_.get())
            int16_encoder_.reset(new Int16Encoder);
        return int16_encoder_.get();
    }
};

const Int32Encoder *GetDocIDEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

const Int32Encoder *GetTFEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

const Int16Encoder *GetDocPayloadEncoder() { return EncoderProvider::GetInstance()->GetInt16Encoder(); }

const Int32Encoder *GetSkipListEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

const Int32Encoder *GetPosListEncoder() { return EncoderProvider::GetInstance()->GetInt32Encoder(); }

} // namespace infinity