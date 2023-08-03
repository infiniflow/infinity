#include "numeric_db.h"

namespace infinity {

namespace fs = std::filesystem;

const std::string kWALDirName = "log";

NumericDB::NumericDB(std::filesystem::path db_path)
    : db_path_(db_path) {

    Initialize();
}

NumericDB::~NumericDB() {

}

void NumericDB::Initialize() {
}

Status NumericDB::Put(const Slice& key, const Slice& value) {

    return Status::OK();
}

Status NumericDB::Get(const Slice& key, std::string& value) {

    return Status::OK();
}

Status NumericDB::GetRange(
    const Slice& start_key,
    const Slice& end_key,
    std::unique_ptr<Roaring>& filter) {

    return Status::OK();
}

Status NumericDB::Delete(const Slice& key) {

    return Status::OK();
}
}