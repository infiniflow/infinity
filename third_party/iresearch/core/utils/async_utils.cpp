////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2016 by EMC Corporation, All Rights Reserved
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is EMC Corporation
///
/// @author Andrey Abramov
/// @author Vasiliy Nabatchikov
////////////////////////////////////////////////////////////////////////////////

#include "utils/async_utils.hpp"

#include <functional>

#include "utils/assert.hpp"
#include "utils/log.hpp"
#include "utils/misc.hpp"
#include "utils/std.hpp"

#include "utils/rst/strings/str_cat.h"

using namespace std::chrono_literals;

namespace irs {
namespace async_utils {

void busywait_mutex::lock() noexcept {
  while (!try_lock()) {
    std::this_thread::yield();
  }
}

bool busywait_mutex::try_lock() noexcept {
  bool expected = false;
  return locked_.load(std::memory_order_relaxed) == expected &&
         locked_.compare_exchange_strong(expected, true,
                                         std::memory_order_acquire,
                                         std::memory_order_relaxed);
}

void busywait_mutex::unlock() noexcept {
  locked_.store(false, std::memory_order_release);
}

template<bool UsePriority>
thread_pool<UsePriority>::thread_pool(
  size_t max_threads /*= 0*/, size_t max_idle /*= 0*/,
  basic_string_view<native_char_t> worker_name /*= ""*/)
  : shared_state_(std::make_shared<shared_state>()),
    max_idle_(max_idle),
    max_threads_(max_threads),
    worker_name_(worker_name) {}

template<bool UsePriority>
thread_pool<UsePriority>::~thread_pool() {
  try {
    stop(true);
  } catch (...) {
  }
}

template<bool UsePriority>
size_t thread_pool<UsePriority>::max_idle() const {
  std::lock_guard lock{shared_state_->lock};

  return max_idle_;
}

template<bool UsePriority>
void thread_pool<UsePriority>::max_idle(size_t value) {
  auto& state = *shared_state_;

  {
    std::lock_guard lock{state.lock};

    max_idle_ = value;
  }

  state.cond.notify_all();  // wake any idle threads if they need termination
}

template<bool UsePriority>
void thread_pool<UsePriority>::max_idle_delta(int delta) {
  auto& state = *shared_state_;

  {
    std::lock_guard lock{state.lock};
    auto max_idle = max_idle_ + delta;

    if (delta > 0 && max_idle < max_idle_) {
      max_idle_ = std::numeric_limits<size_t>::max();
    } else if (delta < 0 && max_idle > max_idle_) {
      max_idle_ = std::numeric_limits<size_t>::min();
    } else {
      max_idle_ = max_idle;
    }
  }

  state.cond.notify_all();  // wake any idle threads if they need termination
}

template<bool UsePriority>
size_t thread_pool<UsePriority>::max_threads() const {
  std::lock_guard lock{shared_state_->lock};

  return max_threads_;
}

template<bool UsePriority>
void thread_pool<UsePriority>::max_threads(size_t value) {
  auto& state = *shared_state_;

  {
    std::lock_guard lock{shared_state_->lock};

    max_threads_ = value;

    if (State::ABORT != state.state.load()) {
      maybe_spawn_worker();
    }
  }

  state.cond.notify_all();  // wake any idle threads if they need termination
}

template<bool UsePriority>
void thread_pool<UsePriority>::max_threads_delta(int delta) {
  auto& state = *shared_state_;

  {
    std::lock_guard lock{state.lock};
    auto max_threads = max_threads_ + delta;

    if (delta > 0 && max_threads < max_threads_) {
      max_threads_ = std::numeric_limits<size_t>::max();
    } else if (delta < 0 && max_threads > max_threads_) {
      max_threads_ = std::numeric_limits<size_t>::min();
    } else {
      max_threads_ = max_threads;
    }

    if (State::ABORT != state.state.load()) {
      maybe_spawn_worker();
    }
  }

  state.cond.notify_all();  // wake any idle threads if they need termination
}

template<bool UsePriority>
bool thread_pool<UsePriority>::run(thread_pool<UsePriority>::func_t&& fn,
                                   clock_t::duration delay /*=0*/) {
  if (!fn) {
    return false;
  }

  auto& state = *shared_state_;

  {
    std::lock_guard lock{state.lock};

    if (State::RUN != state.state.load()) {
      return false;  // pool not active
    }
    if constexpr (UsePriority) {
      queue_.emplace(std::move(fn), clock_t::now() + delay);
    } else {
      queue_.emplace(std::move(fn));
    }

    try {
      maybe_spawn_worker();
    } catch (...) {
      if (0 == threads_.load()) {
        // failed to spawn a thread to execute a task
        queue_.pop();
        throw;
      }
    }
  }

  state.cond.notify_one();

  return true;
}

template<bool UsePriority>
void thread_pool<UsePriority>::stop(bool skip_pending /*= false*/) {
  shared_state_->state.store(skip_pending ? State::ABORT : State::FINISH);

  decltype(queue_) empty;
  {
    std::unique_lock lock{shared_state_->lock};

    // wait for all threads to terminate
    while (threads_.load()) {
      shared_state_->cond.notify_all();  // wake all threads
      shared_state_->cond.wait_for(lock, 50ms);
    }

    queue_.swap(empty);
  }
}

template<bool UsePriority>
void thread_pool<UsePriority>::limits(size_t max_threads, size_t max_idle) {
  auto& state = *shared_state_;

  {
    std::lock_guard lock{state.lock};

    max_threads_ = max_threads;
    max_idle_ = max_idle;

    if (State::ABORT != state.state.load()) {
      maybe_spawn_worker();
    }
  }

  state.cond.notify_all();  // wake any idle threads if they need termination
}

template<bool UsePriority>
bool thread_pool<UsePriority>::maybe_spawn_worker() {
  IRS_ASSERT(!shared_state_->lock.try_lock());  // lock must be held

  // create extra thread if all threads are busy and can grow pool
  const size_t pool_size = threads_.load();

  if (!queue_.empty() && active_ == pool_size && pool_size < max_threads_) {
    std::thread worker(&thread_pool::worker, this, shared_state_);
    worker.detach();

    threads_.fetch_add(1);

    return true;
  }

  return false;
}

template<bool UsePriority>
std::pair<size_t, size_t> thread_pool<UsePriority>::limits() const {
  std::lock_guard lock{shared_state_->lock};

  return {max_threads_, max_idle_};
}

template<bool UsePriority>
std::tuple<size_t, size_t, size_t> thread_pool<UsePriority>::stats() const {
  std::lock_guard lock{shared_state_->lock};

  return {active_, queue_.size(), threads_.load()};
}

template<bool UsePriority>
size_t thread_pool<UsePriority>::tasks_active() const {
  std::lock_guard lock{shared_state_->lock};

  return active_;
}

template<bool UsePriority>
size_t thread_pool<UsePriority>::tasks_pending() const {
  std::lock_guard lock{shared_state_->lock};

  return queue_.size();
}

template<bool UsePriority>
size_t thread_pool<UsePriority>::threads() const {
  std::lock_guard lock{shared_state_->lock};

  return threads_.load();
}

template<bool UsePriority>
void thread_pool<UsePriority>::worker(
  std::shared_ptr<shared_state> shared_state) noexcept {
  // hold a reference to 'shared_state_' ensure state is still alive
  if (!worker_name_.empty()) {
    set_thread_name(worker_name_.c_str());
  }

  {
    std::unique_lock lock{shared_state->lock, std::defer_lock};

    try {
      worker_impl(lock, shared_state);
    } catch (...) {
      // NOOP
    }

    threads_.fetch_sub(1);
  }

  if (State::RUN != shared_state->state.load()) {
    shared_state->cond.notify_all();  // wake up thread_pool::stop(...)
  }
}

template<bool UsePriority>
void thread_pool<UsePriority>::worker_impl(
  std::unique_lock<std::mutex>& lock,
  std::shared_ptr<shared_state> shared_state) {
  auto& state = shared_state->state;

  lock.lock();

  while (State::ABORT != state.load() && threads_.load() <= max_threads_) {
    IRS_ASSERT(lock.owns_lock());
    if (!queue_.empty()) {
      auto& top = next();
      bool proceed = true;
      if constexpr (UsePriority) {
        if (top.at > clock_t::now()) {
          proceed = false;
        }
      }
      if (proceed) {
        func_t fn = std::move(func(top));
        queue_.pop();
        ++active_;
        Finally decrement = [this]() noexcept { --active_; };
        // if have more tasks but no idle thread and can grow pool
        try {
          maybe_spawn_worker();
        } catch (const std::bad_alloc&) {
          fprintf(stderr, "Failed to allocate memory while spawning a worker");
        } catch (const std::exception& e) {
          IRS_LOG_ERROR(
            rst::StrCat({"Failed to grow pool, error '", e.what(), "'"}));
        } catch (...) {
          IRS_LOG_ERROR("Failed to grow pool");
        }

        lock.unlock();
        try {
          fn();
        } catch (const std::bad_alloc&) {
          fprintf(stderr, "Failed to allocate memory while executing task");
        } catch (const std::exception& e) {
          IRS_LOG_ERROR(
            rst::StrCat({"Failed to execute task, error '", e.what(), "'"}));
        } catch (...) {
          IRS_LOG_ERROR("Failed to execute task");
        }
        fn = nullptr;
        lock.lock();
        continue;
      }
    }
    IRS_ASSERT(active_ <= threads_.load());

    if (const auto idle = threads_.load() - active_;
        (idle <= max_idle_ || (!queue_.empty() && threads_.load() == 1))) {
      if (const auto run_state = state.load();
          !queue_.empty() && State::ABORT != run_state) {
        IRS_ASSERT(UsePriority);
        if constexpr (UsePriority) {
          const auto at = queue_.top().at;  // queue_ might be modified
          shared_state->cond.wait_until(lock, at);
        }
      } else if (State::RUN == run_state) {
        IRS_ASSERT(queue_.empty());
        shared_state->cond.wait(lock);
      } else {
        IRS_ASSERT(State::RUN != run_state);
        return;  // termination requested
      }
    } else {
      return;  // too many idle threads
    }
  }
}

template class thread_pool<true>;
template class thread_pool<false>;

}  // namespace async_utils
}  // namespace irs
