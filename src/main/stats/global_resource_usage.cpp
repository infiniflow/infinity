//
// Created by JinHai on 2022/11/25.
//
module;

import stl;

module global_resource_usage;

namespace infinity {

atomic_bool GlobalResourceUsage::initialized_ = false;

ai64 GlobalResourceUsage::object_count_ = 0;

ai64 GlobalResourceUsage::raw_memory_count_ = 0;

} // namespace infinity
