#include "codec.h"

namespace infinity {

void Codec::EncodeColumnKey(
    const u64 schema_id,
    const u64 table_id,
    const u64 column_id,
    const std::string& term,
    std::string& key) {

    AppendFixed64(key,schema_id);
    AppendFixed64(key,table_id);
    AppendFixed64(key,column_id);
    AppendBuffer(key, term.c_str(), term.size());
}

void Codec::DecodeColumnKey(
    const Slice& key,
    std::string& term) {

    Slice k(key);
    u64 schema_id,table_id,column_id;
    RemoveFixed64(k, schema_id);
    RemoveFixed64(k, table_id);
    RemoveFixed64(k, column_id);
    term = k.ToString();
}

void Codec::AppendFixed8(std::string& key, uint8_t i){
    key.push_back(i);
}

void Codec::AppendFixed64(std::string& key, uint64_t i){
    key.append((char*)&i, sizeof(i));
}

void Codec::AppendBuffer(std::string& key, const char* buffer, uint32_t size){
    key.append(buffer, size);
}

void Codec::RemoveFixed8(Slice& key, uint8_t& i) {
    i = *(uint8_t*)key.data();
    key.remove_prefix(sizeof(i));
}

void Codec::RemoveFixed64(Slice& key, uint64_t& i) { 
    key.remove_prefix(sizeof(i)); 
}

void Codec::RemoveBuffer(Slice& key, char* buffer, uint32_t size) {
    memcpy(buffer, key.data(), size);
    key.remove_prefix(size);
}

void Codec::AddInt(std::string& buf, u32 value) {
    u8 working[sizeof(value)];
    memcpy(working, &value, sizeof(value));
    buf.append(buf, sizeof(working));
}

void Codec::AddVInt(std::string& buf, u32 value) {
    u8 working[5];
    u8* ptr = EncodeVInt(working, value);
    buf.append(buf, ptr - working);
}

void Codec::AddLong(std::string& buf, u64 value) {
    u8 working[sizeof(value)];
    memcpy(working, &value, sizeof(value));
    buf.append(buf, sizeof(working));
}

void Codec::AddVLong(std::string& buf, u64 value) {
    u8 working[16];
    u8* ptr = EncodeVLong(working, value);
    buf.append(buf, ptr - working);
}

u32 DecodeInt(const uint8_t *ptr) {
    u32 result;
    memcpy(&result, ptr, sizeof(result));
    return result;
}

const u8 *GetVIntPtr(const u8 *p, u32* value) {
    u32 result = 0;
    for (u32 shift = 0; shift <= 28; shift += 7) {
        u32 byte = *p;
        p++;
        if (byte & 128) {
            result |= ((byte & 127) << shift);
        } else {
            result |= (byte << shift);
            *value = result;
            return p;
        }
    }
    return nullptr;
}
}

