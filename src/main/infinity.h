//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "network/connection.h"
#include <memory>
#include "scheduler/naive_scheduler.h"
#include "storage/catalog.h"
#include "storage/storage.h"
#include "config.h"

namespace infinity {

class Infinity : public Singleton<Infinity> {
public:
    [[nodiscard]] inline std::unique_ptr<Scheduler>&
    scheduler() noexcept {
        return scheduler_;
    }

    [[nodiscard]] inline std::unique_ptr<Catalog>&
    catalog() noexcept {
        return storage_->catalog();
    }

    [[nodiscard]] std::unique_ptr<Config>&
    config() noexcept {
        return config_;
    }

    [[nodiscard]] inline std::unique_ptr<Storage>&
    storage() noexcept {
        return storage_;
    }

    void
    Init();

    void
    UnInit();

private:
    friend class Singleton;
    Infinity();

    std::unique_ptr<Scheduler> scheduler_;
    std::unique_ptr<Config> config_;
    std::unique_ptr<Storage> storage_;

    bool initialized_ {false};
};

}

