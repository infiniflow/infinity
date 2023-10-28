module;

import stl;
module codec;

namespace infinity {

void Codec::EncodeColumnKey(const u64 schema_id, const u64 table_id, const u64 column_id, const String &term, String &key) {

    AppendFixed64(key, schema_id);
    AppendFixed64(key, table_id);
    AppendFixed64(key, column_id);
    AppendBuffer(key, term.c_str(), term.size());
}

void Codec::DecodeColumnKey(const String &key, String &term) {
    term = key;
    u64 schema_id, table_id, column_id;
    RemoveFixed64(term, schema_id);
    RemoveFixed64(term, table_id);
    RemoveFixed64(term, column_id);
}

void Codec::AppendFixed8(String &key, u8 i) { key.push_back(i); }

void Codec::AppendFixed64(String &key, u64 i) { key.append((char *)&i, sizeof(i)); }

void Codec::AppendBuffer(String &key, const char *buffer, u32 size) { key.append(buffer, size); }

void Codec::RemoveFixed8(String &key, u8 &i) {
    i = *(u8 *)key.data();
    key.erase(0, sizeof(i));
}

void Codec::RemoveFixed64(String &key, u64 &i) { key.erase(0, sizeof(i)); }

void Codec::RemoveBuffer(String &key, char *buffer, u32 size) {
    Memcpy(buffer, key.data(), size);
    key.erase(size);
}

void Codec::AddInt(String &buf, u32 value) {
    u8 working[sizeof(value)];
    Memcpy(working, &value, sizeof(value));
    buf.append(buf, sizeof(working));
}

void Codec::AddVInt(String &buf, u32 value) {
    u8 working[5];
    u8 *ptr = EncodeVInt(working, value);
    buf.append(buf, ptr - working);
}

void Codec::AddLong(String &buf, u64 value) {
    u8 working[sizeof(value)];
    Memcpy(working, &value, sizeof(value));
    buf.append(buf, sizeof(working));
}

void Codec::AddVLong(String &buf, u64 value) {
    u8 working[16];
    u8 *ptr = EncodeVLong(working, value);
    buf.append(buf, ptr - working);
}

u32 DecodeInt(const u8 *ptr) {
    u32 result;
    Memcpy(&result, ptr, sizeof(result));
    return result;
}

const u8 *GetVIntPtr(const u8 *p, u32 *value) {
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

const u8 *GetVIntPtr(const u8 *p, u64 *value) {
    u64 result = 0;
    for (u32 shift = 0; shift <= 63; shift += 7) {
        u64 byte = *p;
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

} // namespace infinity
