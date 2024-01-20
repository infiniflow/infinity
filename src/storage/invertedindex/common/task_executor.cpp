module;

#include "array_queue.h"
#include <atomic>
#include <thread>

module task_executor;

import array_queue;
import stl;
import infinity_exception;

namespace infinity {

constexpr std::chrono::microseconds operator""_us(unsigned long long us) { return std::chrono::microseconds(std::chrono::milliseconds(us)); }

constexpr std::chrono::milliseconds operator""_ms(unsigned long long ms) { return std::chrono::milliseconds(ms); }

u32 RoundUp2(u32 val) {
    u32 res = 1;
    while (res < val)
        res <<= 1;
    return res;
}

std::thread Start(Runnable &runnable, Runnable::FuncT init_func) {
    return std::thread([&runnable, init_fun = std::move(init_func)]() { init_fun(runnable); });
}

constexpr i64 CountNs(NanoSeconds d) { return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count(); }

double ExecutorIdleTracker::Reset(steady_time t, u32 num_threads) {
    double idle = CountNs(total_idle_);
    double elapsed = std::max(idle, double(CountNs((t - start_) * num_threads)));
    start_ = t;
    total_idle_ = NanoSeconds::zero();
    return (elapsed > 0) ? (idle / elapsed) : 0.0;
}

TaskExecutor::TaskExecutor(Runnable::FuncT func, u32 reserved_queue_size, u32 watermark, NanoSeconds reaction_time)
    : watermark_ratio_(watermark < reserved_queue_size ? double(watermark) / reserved_queue_size : 1.0), task_limit_(RoundUp2(reserved_queue_size)),
      wanted_task_limit_(task_limit_.load()), rp_(0), tasks_(MakeUnique<TaskExecutor::Task::ptr[]>(task_limit_)), mutex_(), consumer_condition_(),
      producer_condition_(), thread_(), stopped_(false), idle_tracker_(steady_clock::now()), thread_idle_tracker_(), wakeup_count_(0),
      last_accepted_(0), queue_size_(), wakeup_consumer_at_(0), producer_need_wakeup_at_(0), wp_(0),
      watermark_(task_limit_.load() * watermark_ratio_), reaction_time_(reaction_time), closed_(false), overflow_() {
    thread_ = Start(*this, func);
    overflow_ = MakeUnique<ArrayQueue<TaskExecutor::Task::ptr>>();
}

TaskExecutor::~TaskExecutor() {
    ShutDown();
    Sync();
    Stop();
    consumer_condition_.notify_one();
    thread_.join();
}

bool TaskExecutor::IsStopped() const { return stopped_.load(std::memory_order_relaxed); }
u64 TaskExecutor::Index(u64 counter) const { return counter & (task_limit_.load(std::memory_order_relaxed) - 1); }
u64 TaskExecutor::NumTasksInMainQueue() const { return wp_.load(std::memory_order_relaxed) - rp_.load(std::memory_order_acquire); }
u32 TaskExecutor::GetTaskLimit() const { return task_limit_.load(std::memory_order_relaxed); }
u32 TaskExecutor::GetWaterMark() const { return watermark_.load(std::memory_order_relaxed); }

void TaskExecutor::SleepProducer(Lock &lock, NanoSeconds max_wait_time, u64 wakeup_at) {
    producer_need_wakeup_at_.store(wakeup_at, std::memory_order_relaxed);
    producer_condition_.wait_for(lock, max_wait_time);
    producer_need_wakeup_at_.store(0, std::memory_order_relaxed);
}

TaskExecutor::Task::ptr TaskExecutor::Execute(TaskExecutor::Task::ptr task) {
    u64 wp;
    {
        Lock guard(mutex_);
        if (closed_) {
            return task;
        }
        task = WaitForRoomOrPutInOverflowQueue(guard, std::move(task));
        if (task) {
            wp = MoveToMainQueue(guard, std::move(task));
        } else {
            wp = wp_.load(std::memory_order_relaxed) + NumTasksInOverflowQueue(guard);
        }
    }
    if (wp == wakeup_consumer_at_.load(std::memory_order_relaxed)) {
        consumer_condition_.notify_one();
    }
    return task;
}

u64 TaskExecutor::NumTasks() {
    if (overflow_) {
        Lock guard(mutex_);
        return NumTasksInMainQueue() + NumTasksInOverflowQueue(guard);
    } else {
        return NumTasksInMainQueue();
    }
}

u64 TaskExecutor::MoveToMainQueue(Lock &, TaskExecutor::Task::ptr task) {
    u64 wp = wp_.load(std::memory_order_relaxed);
    tasks_[Index(wp)] = std::move(task);
    wp_.store(wp + 1, std::memory_order_release);
    return wp;
}

void TaskExecutor::SetTaskLimit(uint32_t task_limit) { wanted_task_limit_ = RoundUp2(task_limit); }

void TaskExecutor::Drain(Lock &lock) {
    u64 wp = wp_.load(std::memory_order_relaxed);
    while (NumTasks(lock) > 0) {
        consumer_condition_.notify_one();
        SleepProducer(lock, 100_us, wp);
    }
}

void TaskExecutor::WakeUp() {
    if (NumTasks() > 0) {
        consumer_condition_.notify_one();
    }
}

TaskExecutor &TaskExecutor::Sync() {
    Lock lock(mutex_);
    u64 wp = wp_.load(std::memory_order_relaxed) + NumTasksInOverflowQueue(lock);
    while (wp > rp_.load(std::memory_order_acquire)) {
        consumer_condition_.notify_one();
        SleepProducer(lock, 100_us, wp);
    }
    return *this;
}

TaskExecutor &TaskExecutor::ShutDown() {
    Lock lock(mutex_);
    closed_ = true;
    return *this;
}

void TaskExecutor::Run() {
    while (!IsStopped()) {
        DrainTasks();
        producer_condition_.notify_all();
        wakeup_consumer_at_.store(wp_.load(std::memory_order_relaxed) + GetWaterMark(), std::memory_order_relaxed);
        Lock lock(mutex_);
        if (NumTasks(lock) <= 0) {
            steady_time now = steady_clock::now();
            thread_idle_tracker_.SetIdle(now);
            consumer_condition_.wait_until(lock, now + reaction_time_);
            idle_tracker_.WasIdle(thread_idle_tracker_.SetActive(steady_clock::now()));
            wakeup_count_++;
        }
        wakeup_consumer_at_.store(0, std::memory_order_relaxed);
    }
}

void TaskExecutor::DrainTasks() {
    while (NumTasks() > 0) {
        RunTasksTill(wp_.load(std::memory_order_acquire));
        MoveOverflowToMainQueue();
    }
}

void TaskExecutor::MoveOverflowToMainQueue() {
    if (!overflow_)
        return;
    Lock guard(mutex_);
    MoveOverflowToMainQueue(guard);
}
void TaskExecutor::MoveOverflowToMainQueue(Lock &guard) {
    while (!overflow_->Empty() && NumTasksInMainQueue() < task_limit_.load(std::memory_order_relaxed)) {
        MoveToMainQueue(guard, std::move(overflow_->Front()));
        overflow_->Pop();
    }
}

void TaskExecutor::RunTasksTill(u64 available) {
    u64 consumed = rp_.load(std::memory_order_relaxed);
    u64 wakeup_limit = producer_need_wakeup_at_.load(std::memory_order_relaxed);
    while (consumed < available) {
        TaskExecutor::Task::ptr task = std::move(tasks_[Index(consumed)]);
        task->Run();
        rp_.store(++consumed, std::memory_order_release);
        if (wakeup_limit == consumed) {
            producer_condition_.notify_all();
        }
    }
}

TaskExecutor::Task::ptr TaskExecutor::WaitForRoomOrPutInOverflowQueue(Lock &guard, TaskExecutor::Task::ptr task) {
    u64 wp = wp_.load(std::memory_order_relaxed);
    u64 task_limit = task_limit_.load(std::memory_order_relaxed);
    if (task_limit != wanted_task_limit_.load(std::memory_order_relaxed)) {
        Drain(guard);
        tasks_ = MakeUnique<TaskExecutor::Task::ptr[]>(wanted_task_limit_);
        task_limit_ = wanted_task_limit_.load();
        watermark_ = task_limit_ * watermark_ratio_;
    }
    u64 num_task_in_queue = NumTasks(guard);
    queue_size_ += num_task_in_queue;
    if (num_task_in_queue >= task_limit_.load(std::memory_order_relaxed)) {
        if (overflow_) {
            overflow_->Push(std::move(task));
        } else {
            while (NumTasks(guard) >= task_limit_.load(std::memory_order_relaxed)) {
                SleepProducer(guard, reaction_time_, wp - GetWaterMark());
            }
        }
    } else {
        if (overflow_ && !overflow_->Empty()) {
            overflow_->Push(std::move(task));
        }
    }
    if (overflow_ && !overflow_->Empty()) {
        MoveOverflowToMainQueue(guard);
    }
    return task;
}

SequencedTaskExecutor::SequencedTaskExecutor(Vector<UniquePtr<TaskExecutor>> executors) : executors_(std::move(executors)) {}

void SequencedTaskExecutor::Execute(u32 id, UniquePtr<TaskExecutor::Task> task) {
    auto rejected_task = executors_[id]->Execute(std::move(task));
    if (!rejected_task) {
        UnrecoverableError("SequencedTaskExecutor execute error ");
    }
}

UniquePtr<SequencedTaskExecutor> SequencedTaskExecutor::Create(Runnable::FuncT func, u32 threads, u32 task_limit) {
    auto executors = Vector<UniquePtr<TaskExecutor>>();
    for (u32 id = 0; id < threads; ++id) {
        u32 watermark = task_limit / 10;
        executors.push_back(MakeUnique<TaskExecutor>(func, task_limit, watermark, 100_ms));
    }
    return UniquePtr<SequencedTaskExecutor>(new SequencedTaskExecutor(std::move(executors)));
}

void SequencedTaskExecutor::SyncAll() {
    WakeUp();
    for (auto &executor : executors_) {
        executor->Sync();
    }
}

void SequencedTaskExecutor::WakeUp() {
    for (auto &executor : executors_) {
        executor->WakeUp();
    }
}

void SequencedTaskExecutor::SetTaskLimit(u32 task_limit) {
    for (const auto &executor : executors_) {
        executor->SetTaskLimit(task_limit);
    }
}
} // namespace infinity
