#include "numeric_db.h"
#include "context.h"
#include "page_manager.h"
#include "page_io.h"
#include "journal.h"
#include "btree_index.h"
#include "common/utility/infinity_assert.h"
#include "storage/indexstore/key_encoder.h"

#include <string>

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
    fs::path file_path = db_path_/(kBTreePrefix + std::to_string(segment_id_));
    file_ = MakeShared<File>(file_path);
    page_manager_ = MakeShared<PageManager>();
    journal_ = MakeShared<Journal>();

    bool db_exists = fs::is_directory(db_path_) && fs::is_regular_file(file_path);

    return db_exists ? OpenEnv() : NewEnv();
}

void
NumericDB::NewEnv() {
    file_->Create();
    Page *page = new Page(file_.get());
    page->Alloc(Page::kTypeHeader, Page::kSize);
    ::memset(page->Data(), 0, Page::kSize);
    page->SetType(Page::kTypeHeader);
    page->SetDirty(true);

    header_.reset(new EnvHeader(page));
    header_->SetMagic('H', 'A', 'M', '\0');
    header_->SetVersion(BTREE_VERSION_MAJ, BTREE_VERSION_MIN, BTREE_VERSION_REV, BTREE_FILE_VERSION);
    header_->SetPageSize(Page::kSize);
    header_->SetMaxDatabases(BTREE_MAX_DATABASES);
    header_->header_page_->Flush();
}


void
NumericDB::OpenEnv() {
    Context context;
    file_->Open();
    // read the database header
    {
        Page *page = 0;
        uint8_t hdrbuf[Page::kSize];
        file_->Read(0, hdrbuf, sizeof(hdrbuf));

        Page fakepage(file_.get());
        fakepage.SetData((PageData *)hdrbuf);
        header_.reset(new EnvHeader(&fakepage));

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

        fakepage.SetData(0);
        if (unlikely(st != 0)) {
            if (file_->IsOpen())
                file_->Close();
            return;
        }

        page = new Page(file_.get());
        page->Fetch(0);
        header_.reset(new EnvHeader(page));
    }

    // load the state of the PageManager
    if (header_->PageManagerBlobid() != 0)
        page_manager_->Initialize(header_->PageManagerBlobid());
}


void
NumericDB::Close() {
    Context context;

    page_manager_->Close(&context);
    if (likely(header_ && header_->header_page_)) {
        Page *page = header_->header_page_.get();
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
            std::shared_ptr<BtreeIndex> db(new BtreeIndex);
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
    Page *page = header_->header_page_.get();
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
            std::shared_ptr<BtreeIndex> db(new BtreeIndex);
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

Status NumericDB::Put(const uint32_t column_id, const Slice& key, const Slice& value) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);


    return Status::OK();
}

Status NumericDB::Get(const uint32_t column_id, const Slice& key, std::string& value) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);

    return Status::OK();
}

Status NumericDB::GetRange(
    const uint32_t column_id,
    const Slice& start_key,
    const Slice& end_key,
    std::unique_ptr<Roaring>& filter) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);

    return Status::OK();
}

Status NumericDB::Delete(const uint32_t column_id, const Slice& key) {
    std::shared_ptr<BtreeIndex> db = btree_indices_[column_id];
    if(!db.get()) throw StorageException(fmt::format("database not found, column: {}", column_id));
    Context context;
    page_manager_->PurgeCache(&context);

    return Status::OK();
}

}