#include "indexdb.h"
#include "common/utility/infinity_assert.h"
#include "common/types/internal_types.h"

namespace infinity {

IndexDB::IndexDB(const std::string& path)
    :db_(NULL)
    ,path_(path){
    cache_.reset(new PostingCache(256*1024*1024));
}

IndexDB::~IndexDB(){
    delete db_;
}

Status IndexDB::Open() {
    leveldb::Options options;

    //options.block_cache = leveldb::NewLRUCache(500 * 1048576);
    options.write_buffer_size = 500 * 1048576;
    options.filter_policy = leveldb::NewBloomFilterPolicy(16);
    options.create_if_missing = true;
    options.paranoid_checks = false;
    options.compression = leveldb::kSnappyCompression;

    leveldb::Status status = leveldb::DB::Open(options, path_, &db_);
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
        return Status::NotFound();        
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
    return Status::NotFound();
}

Tx* IndexDB::Begin(){
    UniquePtr<Tx> new_txn = MakeUnique<Tx>(this);
    Tx* res = new_txn.get();
    return res;
}

}