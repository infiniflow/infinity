module;

#include <cassert>

export module infinity_core:mem_usage_change;

import :stl;

namespace infinity {

export struct MemUsageChange {
    bool is_add_{true};
    SizeT mem_{0};

    void Add(const MemUsageChange &other) {
        if (this->mem_ == 0) {
            this->mem_ = other.mem_;
            this->is_add_ = other.is_add_;
            return;
        }

        if (this->is_add_ == other.is_add_) {
            this->mem_ += other.mem_;
        } else {
            if (other.mem_ > this->mem_) {
                this->mem_ = other.mem_ - this->mem_;
                this->is_add_ = !this->is_add_;
            } else {
                this->mem_ -= other.mem_;
            }
        }
    }

    SizeT Apply(SizeT original) {
        if (is_add_) {
            return original + mem_;
        } else {
            assert(mem_ <= original);
            return original - mem_;
        }
    }
};

} // namespace infinity
