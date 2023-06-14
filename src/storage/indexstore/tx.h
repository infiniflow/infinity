#pragma once

#include "common/types/internal_types.h"
#include "codec.h"

#include <roaring/roaring.hh>
#include <string>

namespace infinity{

using Roaring = roaring::Roaring;

class IndexDB;
class Tx{
public:
    Tx(IndexDB* db);

    ~Tx();

    bool CreateBitmap(const std::string& term); 

    void Add(const std::string& term, u64 id);

    void Commit();
private:
	IndexDB* db_ = nullptr;
	//TODO
	std::string key_;
	u64 schema_id_;
	u64 table_id_;
	u64 column_id_;
	std::shared_ptr<Roaring> handle_; 

	Codec codec_;
};

}
