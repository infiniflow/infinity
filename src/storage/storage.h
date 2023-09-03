//
// Created by JinHai on 2022/9/14.
//

#pragma once

#include "catalog.h"
#include "meta/catalog.h"
#include "main/config.h"
#include "txn/txn_manager.h"
#include <string>


namespace infinity {

class Infinity;

class Storage {
public:
    explicit
    Storage(const Config* config_ptr);

    [[nodiscard]] inline UniquePtr<Catalog>&
    catalog() noexcept {
        return catalog_;
    }

    void
    Init();

    void
    Uninit();

private:
    static SharedPtr<DirEntry>
    GetLatestCatalog(const String& dir);

    static void
    InitCatalog(NewCatalog* catalog, TxnManager* txn_mgr);

private:
    const Config* config_ptr_{};
    UniquePtr<Catalog> catalog_{};
    UniquePtr<NewCatalog> new_catalog_{};

    UniquePtr<BufferManager> buffer_mgr_{};
    UniquePtr<TxnManager> txn_mgr_{};
};

}
