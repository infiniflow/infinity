#pragma once

#include "common/types/internal_types.h"
#include <leveldb/slice.h>
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
    	const leveldb::Slice& key,
    	std::string& term);

};

}