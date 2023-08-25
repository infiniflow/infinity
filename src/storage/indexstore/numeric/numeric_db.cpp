#include "numeric_db.h"
#include "context.h"
#include "page_manager.h"
#include "page_io.h"
#include "journal.h"
#include "btree_index.h"
#include "common/utility/infinity_assert.h"
#include "storage/indexstore/key_encoder.h"

#include <string>

#include <iostream>

namespace infinity {

namespace fs = std::filesystem;

const std::string kBTreePrefix = "segment-";

static inline BtreeHeader *
AsBtreeHeader(EnvHeader *header, int i) {
    BtreeHeader *base = (BtreeHeader *) (header->header_page_->Payload() + sizeof(EnvironmentHeader));
    return base + i;
}

NumericDB::NumericDB(std::filesystem::path db_path, uint64_t segment_id)
    : db_path_(db_path)
    , segment_id_(segment_id) {
    Initialize();
}

NumericDB::~NumericDB() {

}

void
NumericDB::Initialize() {
    if(!fs::exists(db_path_)) {
        fs::create_directories(db_path_);
    }
    fs::path file_path = db_path_/(kBTreePrefix + std::to_string(segment_id_));
    file_ = MakeShared<File>(file_path);
    //journal_ = MakeShared<Journal>();

    bool db_exists = fs::is_directory(db_path_) && fs::is_regular_file(file_path);

    return db_exists ? OpenEnv() : NewEnv();
}

void
NumericDB::NewEnv() {
    file_->Create();
    Page *page = new Page(file_.get());
    std::cout<<"new env "<<std::endl;
    page->Alloc(Page::kTypeHeader, Page::kSize);
    std::cout<<"after alloc "<<std::endl;
    ::memset(page->Data(), 0, Page::kSize);
    page->SetType(Page::kTypeHeader);
    page->SetDirty(true);

    header_.reset(new EnvHeader(page));
    header_->SetMagic('H', 'A', 'M', '\0');
    header_->SetVersion(BTREE_VERSION_MAJ, BTREE_VERSION_MIN, BTREE_VERSION_REV, BTREE_FILE_VERSION);
    header_->SetPageSize(Page::kSize);
    header_->SetMaxDatabases(BTREE_MAX_DATABASES);
    header_->header_page_->Flush();
    std::cout<<"header flush "<<std::endl;
    page_manager_ = MakeShared<PageManager>(file_.get(), header_.get());

}


void
NumericDB::OpenEnv() {
    Context context;
    file_->Open();
    // read the database header
    {
        Page *page = new Page(file_.get());
        page->Fetch(0);
        header_.reset(new EnvHeader(page));
        int st = 0;
        if (unlikely(!header_->VerifyMagic('H', 'A', 'M', '\0'))) {
            st = -1;
            goto fail_with_fake_cleansing;
        }

        // Check the database version; everything with a different file version
        // is incompatible.
        if (header_->Version(3) != BTREE_FILE_VERSION) {
            st = -1;
            goto fail_with_fake_cleansing;
        }


fail_with_fake_cleansing:
        if (unlikely(st != 0)) {
            if (file_->IsOpen())
                file_->Close();
            return;
        }

    }

    page_manager_ = MakeShared<PageManager>(file_.get(), header_.get());
    // load the state of the PageManager
    if (header_->PageManagerBlobid() != 0)
        page_manager_->Initialize(header_->PageManagerBlobid());
}


void
NumericDB::Close() {
    Context context;

    page_manager_->Close(&context);
    if (likely(header_ && header_->header_page_)) {
        Page *page = header_->header_page_;
        if (likely(page->Data() != 0))
            file_->FreePage(page);
    }
    if (likely(file_ && file_->IsOpen())) {
        file_->Flush();
        file_->Close();
    }
}

void
NumericDB::Create(SharedPtr<TableDef> table_def) {
    const Vector<SharedPtr<ColumnDef>>& colum_defs = table_def->columns();
    Context context;
    for(size_t i = 0 ;  i < colum_defs.size() ;  ++i) {
        SharedPtr<DataType> column_type = colum_defs[i]->type();
        if(column_type->IsNumeric()) {
            std::shared_ptr<BtreeIndex> db(new BtreeIndex(page_manager_.get()));
            uint32_t dbname = colum_defs[i]->id();
            size_t dbi = 0;
            for (; dbi < header_->MaxDatabases(); dbi++) {
                uint32_t name = AsBtreeHeader(header_.get(), dbi)->dbname_;
                if (!name) {
                    AsBtreeHeader(header_.get(), dbi)->dbname_ = dbname;
                    break;
                }
            }
            if (unlikely(dbi == header_->MaxDatabases())) {
                throw StorageException(fmt::format("max databases limit reached: {}", colum_defs[i]->name()));
            }
            AsBtreeHeader(header_.get(), dbi)->key_type_ = column_type->type();
            AsBtreeHeader(header_.get(), dbi)->key_size_ = GetKeyEncoder(column_type->type())->Size() + sizeof(uint32_t);
            AsBtreeHeader(header_.get(), dbi)->record_size_ = 0;
            db->Create(&context, AsBtreeHeader(header_.get(), dbi), column_type->type());
            btree_indices_[dbname] = db;
            context.changeset_.Flush(lsn_manager_.Next());
        }
    }
    Page *page = header_->header_page_;
    page->SetDirty(true);
    context.changeset_.Put(page);
}

void
NumericDB::Open(SharedPtr<TableDef> table_def) {
    const Vector<SharedPtr<ColumnDef>>& colum_defs = table_def->columns();
    Context context;
    for(size_t i = 0 ;  i < colum_defs.size() ;  ++i) {
        SharedPtr<DataType> column_type = colum_defs[i]->type();
        if(column_type->IsNumeric()) {
            std::shared_ptr<BtreeIndex> db(new BtreeIndex(page_manager_.get()));
            uint32_t dbname = colum_defs[i]->id();
            size_t dbi = 0;
            for (; dbi < header_->MaxDatabases(); dbi++) {
                uint32_t name = AsBtreeHeader(header_.get(), dbi)->dbname_;
                if (dbname == name) {
                    AsBtreeHeader(header_.get(), dbi)->dbname_ = dbname;
                    break;
                }
            }
            if (unlikely(dbi == header_->MaxDatabases())) {
                throw StorageException(fmt::format("database not found, column: {}", colum_defs[i]->name()));
            }
            db->Open(&context, AsBtreeHeader(header_.get(), dbi));
            btree_indices_[dbname] = db;
        }
    }
}

Status NumericDB::Put(const uint32_t column_id, const std::string& key, const uint32_t row_id) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);
    std::string encoded_key = key;
    encoded_key.append(row_id, sizeof(uint32_t));
    btree_key_t btree_key;
    btree_key.data_ = (void*)encoded_key.data();
    btree_key.size_ = key.length();
    btree_record_t btree_value;
    db->Insert(&context, &btree_key, &btree_value, 0);

    return Status::OK();
}

Status NumericDB::GetEqual(const uint32_t column_id, const std::string& key, std::shared_ptr<Roaring>& filter) {
    std::string encoded_start_key = key;
    encoded_start_key.append(0, sizeof(uint32_t));
    btree_key_t start_key;
    start_key.data_ = (void*)encoded_start_key.data();
    start_key.size_ = encoded_start_key.length();

    std::string encoded_end_key = key;
    encoded_end_key.append(std::numeric_limits<uint32_t>::max(), sizeof(uint32_t));
    btree_key_t end_key;
    end_key.data_ = (void*)encoded_end_key.data();
    end_key.size_ = encoded_end_key.length();

    return DoGetRange(column_id, &start_key, &end_key, filter);
}

Status NumericDB::GetRange(
    const uint32_t column_id,
    const std::string& start_key,
    const std::string& end_key,
    std::shared_ptr<Roaring>& filter) {

    std::string encoded_start_key = start_key;
    encoded_start_key.append(0, sizeof(uint32_t));
    btree_key_t btree_start_key;
    btree_start_key.data_ = (void*)encoded_start_key.data();
    btree_start_key.size_ = encoded_start_key.length();

    std::string encoded_end_key = end_key;
    encoded_end_key.append(std::numeric_limits<uint32_t>::max(), sizeof(uint32_t));
    btree_key_t btree_end_key;
    btree_end_key.data_ = (void*)encoded_end_key.data();
    btree_end_key.size_ = encoded_end_key.length();

    return DoGetRange(column_id, &btree_start_key, &btree_end_key, filter);
}

Status NumericDB::DoGetRange(
    const uint32_t column_id,
    btree_key_t* start_key,
    btree_key_t* end_key,
    std::shared_ptr<Roaring>& filter) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);

    int ret = db->Find(&context, start_key, end_key, 0, filter);
    return ret != -1 ? Status::OK() : Status::NotFound("not found");
}

Status NumericDB::Delete(const uint32_t column_id, const std::string& key, const uint32_t row_id) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);

    std::string encoded_key = key;
    encoded_key.append(row_id, sizeof(uint32_t));
    btree_key_t btree_key;
    btree_key.data_ = (void*)encoded_key.data();
    btree_key.size_ = key.length();
    btree_record_t btree_value;
    db->Erase(&context, &btree_key, 0, 0);


    return Status::OK();
}

}