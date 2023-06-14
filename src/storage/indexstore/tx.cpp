#include "tx.h"
#include "indexdb.h"

namespace infinity{

Tx::Tx(IndexDB* db):db_(db){
}

Tx::~Tx(){
}

bool Tx::CreateBitmap(const std::string& term){
    codec_.EncodeColumnKey(schema_id_, table_id_, column_id_, term, key_);

	return db_->Get(key_,handle_).ok();
}

void Tx::Add(const std::string& term, u64 id){
    handle_->add(id);
}

void Tx::Commit(){
	db_->Put(key_,handle_);
}


}