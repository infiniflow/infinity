#include "numeric_db.h"
#include "manifest.h"

namespace infinity{

namespace fs = std::filesystem;

const std::string kWALDirName = "log";
const std::string kManifestFileName = "MANIFEST";

NumericDB::NumericDB(std::filesystem::path db_path)
    : db_path_(db_path) {

    Initialize();
}

NumericDB::~NumericDB() {

}

void NumericDB::Initialize() {
    key_page_map_ = std::make_shared<KeyPageMap>();
    const bool db_exists = fs::is_directory(db_path_) && fs::is_regular_file(db_path_ / kManifestFileName);
    if(db_exists) {
        Status s;
        const auto manifest = Manifest::LoadFrom(db_path_ / kManifestFileName, s);
        if (!s.ok()) {}
        key_page_map_->ScanFilesAndInitialize(page_mgr_);
    } else {
        fs::create_directory(db_path_);
        key_page_map_->PreallocateAndInitialize(page_mgr_);

        const Status s = Manifest::Builder()
                         .WithNumPages(0)
                         .Build()
                         .WriteTo(db_path_ / kManifestFileName);
    }
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
    std::unique_ptr<Roaring>& filter){

    return Status::OK();
}

Status NumericDB::Delete(const Slice& key) {

    return Status::OK();
}
}