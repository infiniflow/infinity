module;

import stl;
import array_queue;
export module task_executor;

namespace infinity {

export struct Runnable {
    using FuncT = std::function<int(Runnable &)>;

    static int DefaultInitFunction(Runnable &target);

    virtual void Run() = 0;

    virtual ~Runnable() {}
};

export using steady_time = std::chrono::steady_clock::time_point;
export using steady_clock = std::chrono::steady_clock;

export class ThreadIdleTracker {
private:
    steady_time idle_tag_ = steady_time::min();

public:
    bool IsIdle() const { return (idle_tag_ != steady_time::min()); }
    void SetIdle(steady_time t) {
        if (!IsIdle()) {
            idle_tag_ = t;
        }
    }
    NanoSeconds SetActive(steady_time t) {
        if (IsIdle()) {
            NanoSeconds how_long_idle = (t - idle_tag_);
            idle_tag_ = steady_time::min();
            return how_long_idle;
        } else {
            return NanoSeconds::zero();
        }
    }
    NanoSeconds Reset(steady_time t) {
        if (IsIdle()) {
            NanoSeconds how_long_idle = (t - idle_tag_);
            idle_tag_ = t;
            return how_long_idle;
        } else {
            return NanoSeconds::zero();
        }
    }
};

export class ExecutorIdleTracker {
private:
    steady_time start_;
    NanoSeconds total_idle_ = NanoSeconds::zero();

public:
    ExecutorIdleTracker(steady_time t) : start_(t) {}
    void WasIdle(NanoSeconds how_long_idle) { total_idle_ += how_long_idle; }
    double Reset(steady_time t, u32 num_threads);
};

export class TaskExecutor : public Runnable {
public:
    struct Task {
        using ptr = UniquePtr<Task>;
        virtual void Run() = 0;
        virtual ~Task() = default;
    };

    TaskExecutor(Runnable::FuncT func, u32 reserved_queue_size, u32 watermark, NanoSeconds reaction_time);

    virtual ~TaskExecutor();

    UniquePtr<Task> Execute(UniquePtr<Task> task);

    TaskExecutor &Sync();

    void WakeUp();

    u32 GetWaterMark() const;

    NanoSeconds GetReactionTime() const { return reaction_time_; }

    bool IsBlocking() const { return !overflow_; }

    void SetTaskLimit(u32 limit);

    u32 GetTaskLimit() const;

    TaskExecutor &ShutDown();

    void Run();

private:
    using Lock = std::unique_lock<std::mutex>;

    void Stop() { stopped_ = true; }

    bool IsStopped() const;

    void Drain(Lock &lock);

    void DrainTasks();

    void SleepProducer(Lock &guard, NanoSeconds max_wait_time, u64 wakeup_at);

    void RunTasksTill(u64 available);

    UniquePtr<Task> WaitForRoomOrPutInOverflowQueue(Lock &guard, UniquePtr<Task> task);

    u64 MoveToMainQueue(Lock &guard, UniquePtr<Task> task);

    void MoveOverflowToMainQueue();

    void MoveOverflowToMainQueue(Lock &guard);

    u64 Index(u64 counter) const;

    u64 NumTasks();

    u64 NumTasks(Lock &guard) const { return NumTasksInMainQueue() + NumTasksInOverflowQueue(guard); }

    u64 NumTasksInOverflowQueue(Lock &) const { return overflow_ ? overflow_->Size() : 0; }

    u64 NumTasksInMainQueue() const;

private:
    const double watermark_ratio_;
    Atomic<u32> task_limit_;
    Atomic<u32> wanted_task_limit_;
    Atomic<u64> rp_;
    UniquePtr<Task::ptr[]> tasks_;
    std::mutex mutex_;
    std::condition_variable consumer_condition_;
    std::condition_variable producer_condition_;
    Thread thread_;
    Atomic<bool> stopped_;
    ExecutorIdleTracker idle_tracker_;
    ThreadIdleTracker thread_idle_tracker_;
    u64 wakeup_count_;
    u64 last_accepted_;
    u32 queue_size_;
    Atomic<u64> wakeup_consumer_at_;
    Atomic<u64> producer_need_wakeup_at_;
    Atomic<u64> wp_;
    Atomic<u64> watermark_;
    const NanoSeconds reaction_time_;
    bool closed_;
    UniquePtr<ArrayQueue<UniquePtr<Task>>> overflow_;
};

export class SequencedTaskExecutor {
public:
    virtual ~SequencedTaskExecutor() = default;

    void Execute(u32 id, UniquePtr<TaskExecutor::Task> task);

    static UniquePtr<SequencedTaskExecutor> Create(Runnable::FuncT func, u32 threads, u32 task_limit);

    void SyncAll();

    void WakeUp();

    void SetTaskLimit(u32 task_limit);

    template <class Function>
    void ExecuteLambda(u32 id, Function &&function);

private:
    explicit SequencedTaskExecutor(Vector<UniquePtr<TaskExecutor>> executors);

    Vector<UniquePtr<TaskExecutor>> executors_;
};

template <typename Function>
class LambdaTask : public TaskExecutor::Task {
    Function func_;

public:
    LambdaTask(const Function &func) : func_(func) {}
    LambdaTask(Function &func) : func_(std::move(func)) {}
    LambdaTask(const LambdaTask &) = delete;
    LambdaTask &operator=(const LambdaTask &) = delete;
    ~LambdaTask() {}
    void Run() override { func_(); }
};

template <typename Function>
TaskExecutor::Task::ptr MakeLamdaTask(Function &&function) {
    return MakeUnique<LambdaTask<std::decay_t<Function>>>(std::forward<Function>(function));
}

template <class Function>
void SequencedTaskExecutor::ExecuteLambda(u32 id, Function &&function) {
    Execute(id, MakeLamdaTask(std::forward<Function>(function)));
}

} // namespace infinity