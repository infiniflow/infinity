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

export module codec;

import stl;

namespace infinity {

export class Codec {
public:
    Codec() {}

    ~Codec() {}

    void EncodeColumnKey(const u64 schema_id, const u64 table_id, const u64 column_id, const String &term, String &key);

    void DecodeColumnKey(const String &key, String &term);

    static void AddInt(String &buf, u32 value);

    static void AddVInt(String &buf, u32 value);

    static void AddLong(String &buf, u64 value);

    static void AddVLong(String &buf, u64 value);

    static u32 DecodeInt(const u8 *ptr);

    static u8 *EncodeVLong(u8 *dst, u64 v) {
        static const int B = 128;
        while (v >= B) {
            *(dst++) = (v & (B - 1)) | B;
            v >>= 7;
        }
        *(dst++) = static_cast<u8>(v);
        return dst;
    }

    static inline u8 *EncodeVInt(u8 *dst, u32 v) {
        u8 *ptr = dst;
        static const int B = 128;
        if (v < (1 << 7)) {
            *(ptr++) = v;
        } else if (v < (1 << 14)) {
            *(ptr++) = v | B;
            *(ptr++) = v >> 7;
        } else if (v < (1 << 21)) {
            *(ptr++) = v | B;
            *(ptr++) = (v >> 7) | B;
            *(ptr++) = v >> 14;
        } else if (v < (1 << 28)) {
            *(ptr++) = v | B;
            *(ptr++) = (v >> 7) | B;
            *(ptr++) = (v >> 14) | B;
            *(ptr++) = v >> 21;
        } else {
            *(ptr++) = v | B;
            *(ptr++) = (v >> 7) | B;
            *(ptr++) = (v >> 14) | B;
            *(ptr++) = (v >> 21) | B;
            *(ptr++) = v >> 28;
        }
        return ptr;
    }

    static const u8 *GetVIntPtr(const u8 *p, u32 *value);

    static const u8 *GetVLongPtr(const u8 *p, u64 *value);

    static float U32ToFloat(u32 u_val) {
        union {
            float f_value;
            u32 u_value;
        } union_val;
        union_val.u_value = u_val;
        return union_val.f_value;
    }

    static u32 FloatToU32(float f_val) {
        union {
            float f_value;
            u32 u_value;
        } union_val;
        union_val.f_value = f_val;
        return union_val.f_value;
    }

    static double U64ToDouble(u64 u_val) {
        union {
            double d_value;
            u64 u_value;
        } union_val;
        union_val.u_value = u_val;
        return union_val.d_value;
    }

    static u64 DoubleToU64(double d_val) {
        union {
            double d_value;
            u64 u_value;
        } union_val;
        union_val.d_value = d_val;
        return union_val.u_value;
    }

private:
    void AppendFixed8(String &key, u8 i);

    void AppendFixed64(String &key, u64 i);

    void AppendBuffer(String &key, const char *buffer, u32 size);

    void RemoveFixed8(String &key, u8 &i);

    void RemoveFixed64(String &key, u64 &i);

    void RemoveBuffer(String &key, char *buffer, u32 size);
};

} // namespace infinity