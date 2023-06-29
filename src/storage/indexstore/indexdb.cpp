#include "indexdb.h"
#include "common/utility/infinity_assert.h"
#include "common/types/internal_types.h"

#include <leveldb/comparator.h>

namespace infinity {

using Iterator = leveldb::Iterator;
using Slice = leveldb::Slice;

IndexDB::IndexDB(const std::string& path)
    :db_(NULL)
    ,path_(path){
    cache_.reset(new PostingCache(256*1024*1024));
}

IndexDB::~IndexDB(){
    delete db_;
}

Status IndexDB::Open() {
    //options_.block_cache = leveldb::NewLRUCache(500 * 1048576);
    options_.write_buffer_size = 500 * 1048576;
    options_.filter_policy = leveldb::NewBloomFilterPolicy(16);
    options_.create_if_missing = true;
    options_.paranoid_checks = false;
    options_.compression = leveldb::kSnappyCompression;

    leveldb::Status status = leveldb::DB::Open(options_, path_, &db_);
    StorageAssert(status.ok(), "Failed to open leveldb database")

    return Status::OK();
}

Status IndexDB::Get(const std::string& key, std::shared_ptr<Roaring>& result){
    std::string value;
    result = cache_->Get(key);
    if(result){
        return Status::OK();
    }else{
        leveldb::Status s = db_->Get(leveldb::ReadOptions(), key, &value);
        if (s.ok()){
            result = MakeShared<Roaring>();
            *result = Roaring::read(value.c_str(), false); 
            cache_->Set(key,result);
            return Status::OK();
        }
        return Status::NotFound(key);        
    }
}

Status IndexDB::Put(const std::string&key, std::shared_ptr<Roaring>& data){
    size_t expected_size_in_bytes = data->getSizeInBytes();
    std::string buffer;
    buffer.resize(expected_size_in_bytes);
    data->write(buffer.data(), false);
    leveldb::Status s = db_->Put(leveldb::WriteOptions(), key, buffer);
    if (s.ok()){
        return Status::OK();
    }
    std::cerr << s.ToString() << std::endl;
    return Status::NotFound(key);
}

Status IndexDB::ScanForBSI(
    u64 column_id, 
    std::vector<std::shared_ptr<Roaring>>& results){

    u64 schema_id,table_id;//TODO
    std::string startKey,endKey;
    codec_.EncodeColumnKey(schema_id, table_id, column_id, "", startKey);
    codec_.EncodeColumnKey(schema_id, table_id, column_id+1, "", endKey);
    Slice endSlice(endKey);
    Iterator* iter = db_->NewIterator(leveldb::ReadOptions());
    for (iter->Seek(startKey); iter->Valid() && 
        options_.comparator->Compare(iter->key(), endSlice)<0; iter->Next()){
        if( !iter->value().empty() ) {
            std::shared_ptr<Roaring> r = MakeShared<Roaring>();
            *r = Roaring::read(iter->value().data(), false); 
            results.push_back(r);
        }

    }
    return Status::OK();
}

Tx* IndexDB::Begin(){
    UniquePtr<Tx> new_txn = MakeUnique<Tx>(this);
    Tx* res = new_txn.get();
    return res;
}

}