#pragma once

#include "common/types/internal_types.h"
#include <string>

namespace infinity{


class Codec{
public:
    Codec(){}

    ~Codec(){}

    void EncodeColumnKey(
    	const u64 schema_id, 
    	const u64 table_id, 
    	const u64 column_id, 
    	const std::string& term,
    	std::string& key);

    void DecodeColumnKey(
    	const std::string& key,
    	std::string& term);

    static void AddInt(std::string& buf, u32 value);

    static void AddVInt(std::string& buf, u32 value);

    static void AddLong(std::string& buf, u64 value);

    static void AddVLong(std::string& buf, u64 value);

    static u32 DecodeInt(const uint8_t *ptr);

    static u8* EncodeVLong(u8* dst, u64 v) {
        static const int B = 128;
        while (v >= B) {
            *(dst++) = (v & (B-1)) | B;
            v >>= 7;
        }
        *(dst++) = static_cast<u8>(v);
        return dst;
    }

    static inline u8 *EncodeVInt(u8 *dst, u32 v) {
        u8 *ptr = dst;
        static const int B = 128;
        if (v < (1<<7)) {
            *(ptr++) = v;
        } else if (v < (1<<14)) {
            *(ptr++) = v | B;
            *(ptr++) = v>>7;
        } else if (v < (1<<21)) {
            *(ptr++) = v | B;
            *(ptr++) = (v>>7) | B;
            *(ptr++) = v>>14;
        } else if (v < (1<<28)) {
            *(ptr++) = v | B;
            *(ptr++) = (v>>7) | B;
            *(ptr++) = (v>>14) | B;
            *(ptr++) = v>>21;
        } else {
            *(ptr++) = v | B;
            *(ptr++) = (v>>7) | B;
            *(ptr++) = (v>>14) | B;
            *(ptr++) = (v>>21) | B;
            *(ptr++) = v>>28;
        }
        return ptr;
    }

    static const u8 *GetVIntPtr(const u8 *p, u32* value);

    static const u8 *GetVLongPtr(const u8 *p, u64* value);

private:
    void AppendFixed8(std::string& key, uint8_t i);

    void AppendFixed64(std::string& key, uint64_t i);

    void AppendBuffer(std::string& key, const char* buffer, uint32_t size);

    void RemoveFixed8(std::string& key, uint8_t& i);

    void RemoveFixed64(std::string& key, uint64_t& i);

    void RemoveBuffer(std::string& key, char* buffer, uint32_t size);


};

}