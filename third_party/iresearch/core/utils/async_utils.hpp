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

#pragma once

#include <atomic>
#include <condition_variable>
#include <function2/function2.hpp>
#include <functional>
#include <queue>
#include <thread>

#include "noncopyable.hpp"
#include "shared.hpp"
#include "string.hpp"
#include "thread_utils.hpp"

namespace irs {
namespace async_utils {

//////////////////////////////////////////////////////////////////////////////
/// @brief spinlock implementation for Win32 since std::mutex cannot be used
///        in calls going through dllmain()
//////////////////////////////////////////////////////////////////////////////
class busywait_mutex final {
 public:
  void lock() noexcept;
  bool try_lock() noexcept;
  void unlock() noexcept;

 private:
  std::atomic<bool> locked_{false};
};

template<bool UsePriority = true>
class thread_pool {
 public:
  using native_char_t = std::remove_pointer_t<thread_name_t>;
  using clock_t = std::chrono::steady_clock;
  using func_t = fu2::unique_function<void()>;

  explicit thread_pool(size_t max_threads = 0, size_t max_idle = 0,
                       basic_string_view<native_char_t> worker_name =
                         kEmptyStringView<native_char_t>);
  ~thread_pool();
  size_t max_idle() const;
  void max_idle(size_t value);
  void max_idle_delta(int delta);  // change value by delta
  size_t max_threads() const;
  void max_threads(size_t value);
  void max_threads_delta(int delta);  // change value by delta

  // 1st - max_threads(), 2nd - max_idle()
  std::pair<size_t, size_t> limits() const;
  void limits(size_t max_threads, size_t max_idle);

  bool run(func_t&& fn, [[maybe_unused]] clock_t::duration delay = {});
  void stop(bool skip_pending = false);  // always a blocking call
  size_t tasks_active() const;
  size_t tasks_pending() const;
  size_t threads() const;
  // 1st - tasks active(), 2nd - tasks pending(), 3rd - threads()
  std::tuple<size_t, size_t, size_t> stats() const;

 private:
  enum class State { ABORT, FINISH, RUN };

  auto& next() {
    if constexpr (UsePriority) {
      return queue_.top();
    } else {
      return queue_.front();
    }
  }

  template<typename T>
  static func_t& func(T& t) {
    if constexpr (UsePriority) {
      return const_cast<func_t&>(t.fn);
    } else {
      return const_cast<func_t&>(t);
    }
  }

  struct shared_state {
    std::mutex lock;
    std::condition_variable cond;
    std::atomic<State> state{State::RUN};
  };

  struct task {
    explicit task(func_t&& fn, clock_t::time_point at)
      : at(at), fn(std::move(fn)) {}

    clock_t::time_point at;
    func_t fn;

    bool operator<(const task& rhs) const noexcept { return rhs.at < at; }
  };

  void worker(std::shared_ptr<shared_state> shared_state) noexcept;
  void worker_impl(std::unique_lock<std::mutex>& lock,
                   std::shared_ptr<shared_state> shared_state);
  bool maybe_spawn_worker();

  std::shared_ptr<shared_state> shared_state_;
  size_t active_{0};
  std::atomic<size_t> threads_{0};
  size_t max_idle_;
  size_t max_threads_;
  std::conditional_t<UsePriority, std::priority_queue<task>, std::queue<func_t>>
    queue_;
  basic_string<native_char_t> worker_name_;
};

}  // namespace async_utils
}  // namespace irs
