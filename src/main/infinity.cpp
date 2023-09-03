//
// Created by JinHai on 2022/7/19.
//

#include "infinity.h"
#include "logger.h"

namespace infinity {

void
Infinity::Init(const SharedPtr<String>& config_path) {
    if(initialized_) {
        return ;
    } else {
        config_ = MakeUnique<Config>();
        config_->Init(config_path);

        Logger::Initialize(config_.get());

        SizeT memory_limit = 1024 * 1024 * 1024; // 1 Gib
        SharedPtr<String> temp_path = MakeShared<String>("/tmp/infinity/_tmp");
        SharedPtr<String> base_path = MakeShared<String>("/tmp/infinity/data");
        BufferManager buffer_mgr(memory_limit, base_path, temp_path);

        UniquePtr<String> dir = MakeUnique<String>("/tmp/infinity");
        NewCatalog new_catalog(std::move(dir));
        TxnManager txn_mgr(&new_catalog, &buffer_mgr);


        scheduler_ = MakeUnique<NaiveScheduler>();

        storage_ = MakeUnique<Storage>(config_.get());
        storage_->Init();

        initialized_ = true;
    }
}

void
Infinity::UnInit() {
    if(!initialized_) {
        return;
    }
    initialized_ = false;

    storage_->Uninit();
    storage_.reset();

    scheduler_.reset();

    Logger::Shutdown();

    config_.reset();
}

}
