#pragma once

#include "worker.h"
#include "common/utility/topology.h"

#include <array>

namespace infinity {

class WorkerManager {
public:
    WorkerManager() {}

    ~WorkerManager();

    void CreateWorkers(std::uint16_t max_cores);

    void Start();

    Worker* GetLeastLoadedWorker() const;

    Worker* GetMostLoadedWorker() const;

    Worker* GetWorker(std::uint16_t worker_id) const;

    const std::size_t GetNumWorkers() const {
        return max_cores_;
    }
private:
    std::uint16_t max_cores_;

    Topology cpu_topology_;

    std::array<Worker *, Topology::MaxCores> workers_;
};

}