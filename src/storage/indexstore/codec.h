#pragma once

#include "common/types/internal_types.h"
#include <leveldb/slice.h>
#include <string>

namespace infinity{

using Slice = leveldb::Slice;

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
    	const Slice& key,
    	std::string& term);

private:
    void AppendFixed8(std::string& key, uint8_t i);

    void AppendFixed64(std::string& key, uint64_t i);

    void AppendBuffer(std::string& key, const char* buffer, uint32_t size);

    void RemoveFixed8(Slice& key, uint8_t& i);

    void RemoveFixed64(Slice& key, uint64_t& i);

    void RemoveBuffer(Slice& key, char* buffer, uint32_t size);


};

}