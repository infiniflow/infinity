#include "codec.h"

namespace infinity {

void Codec::EncodeColumnKey(
    const u64 schema_id,
    const u64 table_id,
    const u64 column_id,
    const std::string& term,
    std::string& key) {

}

void Codec::DecodeColumnKey(
    const leveldb::Slice& key,
    std::string& term) {

}

}

