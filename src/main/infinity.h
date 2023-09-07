//
// Created by JinHai on 2022/7/19.
//

#pragma once

#include "common/singleton.h"
#include "common/types/internal_types.h"
#include "scheduler/scheduler.h"
#include "storage/storage.h"

namespace infinity {

class Config;
class Infinity : public Singleton<Infinity> {
public:
    [[nodiscard]] inline Scheduler*
    scheduler() noexcept {
        return scheduler_.get();
    }

    [[nodiscard]] inline Config*
    config() noexcept {
        return config_.get();
    }

    [[nodiscard]] inline Storage*
    storage() noexcept {
        return storage_.get();
    }

    void
    Init(const SharedPtr<String>& config_path);

    void
    UnInit();

private:
    friend class Singleton;
    Infinity() = default;

    UniquePtr<Scheduler> scheduler_{};
    UniquePtr<Config> config_{};
    UniquePtr<Storage> storage_{};

    bool initialized_ {false};
};

}

