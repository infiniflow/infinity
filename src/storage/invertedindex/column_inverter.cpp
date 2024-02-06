module;

module column_inverter;
import stl;
import column_vector;
import internal_types;

namespace infinity {

RefCount::RefCount() : lock_(), cv_(), ref_count_(0u) {}

RefCount::~RefCount() {}

void RefCount::Retain() noexcept {
    std::lock_guard<std::mutex> guard(lock_);
    ++ref_count_;
}

void RefCount::Release() noexcept {
    std::lock_guard<std::mutex> guard(lock_);
    --ref_count_;
    if (ref_count_ == 0u) {
        cv_.notify_all();
    }
}

void RefCount::WaitForZeroRefCount() {
    std::unique_lock<std::mutex> guard(lock_);
    cv_.wait(guard, [this] { return (ref_count_ == 0u); });
}

bool RefCount::ZeroRefCount() {
    std::unique_lock<std::mutex> guard(lock_);
    return (ref_count_ == 0u);
}

} // namespace infinity