//
// Created by jinhai on 23-10-31.
//

module;

export module infinity_context;

import stl;
import config;
import resource_manager;
import fragment_scheduler;
import storage;
import singleton;

namespace infinity {

export class InfinityContext : public Singleton<InfinityContext> {
public:
    [[nodiscard]] inline FragmentScheduler*
    fragment_scheduler() noexcept { return fragment_scheduler_.get(); }

    [[nodiscard]] inline Config*
    config() noexcept { return config_.get(); }

    [[nodiscard]] inline Storage*
    storage() noexcept { return storage_.get(); }

    [[nodiscard]] inline ResourceManager*
    resource_manager() noexcept { return resource_manager_.get(); }

    void
    Init(const SharedPtr <String>& config_path);

    void
    UnInit();

private:
    friend class Singleton;

    InfinityContext() = default;

    UniquePtr <Config> config_{};
    UniquePtr <ResourceManager> resource_manager_{};
    UniquePtr <FragmentScheduler> fragment_scheduler_{};
    UniquePtr <Storage> storage_{};

    bool initialized_{false};
};

}

