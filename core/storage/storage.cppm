//
// Created by JinHai on 2022/9/14.
//

module;

import stl;
import config;
import new_catalog;
import txn_manager;
import buffer_manager;
import wal_manager;

export module storage;

namespace infinity {

export class Storage {
public:
    explicit Storage(const Config *config_ptr);

    [[nodiscard]] inline NewCatalog *catalog() noexcept { return new_catalog_.get(); }

    [[nodiscard]] inline BufferManager *buffer_manager() noexcept { return buffer_mgr_.get(); }

    [[nodiscard]] inline TxnManager *txn_manager() const noexcept { return txn_mgr_.get(); }

    void Init();

    void UnInit();

    static SharedPtr<DirEntry> GetLatestCatalog(const String &dir);

    static bool CatalogDirExists(const String &dir);

    void AttachCatalog(const Vector<String> &catalog_files);
    void InitNewCatalog();

private:
    static void InitCatalog(NewCatalog *catalog, TxnManager *txn_mgr);

private:
    const Config *config_ptr_{};
    UniquePtr<NewCatalog> new_catalog_{};
    UniquePtr<BufferManager> buffer_mgr_{};
    UniquePtr<TxnManager> txn_mgr_{};
    UniquePtr<WalManager> wal_mgr_{};
    bool exist_catalog_{false};
};

} // namespace infinity
