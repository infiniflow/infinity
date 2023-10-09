//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "main/config.h"
#include "meta/catalog.h"
#include "txn/txn_manager.h"
#include <memory>
#include <string>

namespace infinity {

class Infinity;

class BufferManager;

class WalManager;

class Storage {
public:
    explicit Storage(const Config *config_ptr);

    [[nodiscard]] inline NewCatalog *catalog() noexcept { return new_catalog_.get(); }

    [[nodiscard]] inline BufferManager *buffer_manager() noexcept { return buffer_mgr_.get(); }

    [[nodiscard]] inline TxnManager *txn_manager() const noexcept { return txn_mgr_.get(); }

    void Init();

    void Uninit();

private:
    static SharedPtr<DirEntry> GetLatestCatalog(const String &dir);

    static void InitCatalog(NewCatalog *catalog, TxnManager *txn_mgr);

private:
    const Config *config_ptr_{};
    //    UniquePtr<Catalog> catalog_{};
    UniquePtr<NewCatalog> new_catalog_{};
    UniquePtr<BufferManager> buffer_mgr_{};
    UniquePtr<TxnManager> txn_mgr_{};
    SharedPtr<WalManager> wal_mgr_{};
};

} // namespace infinity
