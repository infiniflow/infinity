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
    [[nodiscard]] inline UniquePtr<Scheduler>&
    scheduler() noexcept {
        return scheduler_;
    }

    [[nodiscard]] inline UniquePtr<Catalog>&
    catalog() noexcept {
        return storage_->catalog();
    }

    [[nodiscard]] UniquePtr<Config>&
    config() noexcept {
        return config_;
    }

    [[nodiscard]] inline UniquePtr<Storage>&
    storage() noexcept {
        return storage_;
    }

    void
    Init();

    void
    UnInit();

private:
    friend class Singleton;
    Infinity() = default;

    UniquePtr<Scheduler> scheduler_;
    UniquePtr<Config> config_;
    UniquePtr<Storage> storage_;

    bool initialized_ {false};
};

}

