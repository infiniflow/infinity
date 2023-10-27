//
// Created by jinhai on 23-5-7.
//


#include "task.h"
#include "fragment.h"
#include "base_profiler.h"
#include "ctpl.h"
#include <queue>

using namespace infinity;

static BaseProfiler profiler;
std::atomic_long long_atomic{0};

void
execute_task(int64_t id, Task* task, int64_t task_count) {
//    printf("execute task by thread: %ld\n", id);
    if(task->type() == TaskType::kPipeline) {
        PipelineTask* root_task = (PipelineTask*)(task);
        root_task->Init();

        std::queue<PipelineTask*> queue;
        queue.push(root_task);
        while(!queue.empty()) {
            PipelineTask* task_node = queue.front();
            queue.pop();
            if(task_node->children().empty()) {
                task_node->set_state(TaskState::kReady);
            } else {
                task_node->set_state(TaskState::kPending);
            }

            NewScheduler::RunTask(task_node);
            for(const auto& child_task: task_node->children()) {
                queue.push((PipelineTask*)child_task.get());
            }
        }

        root_task->GetResult();
        ++long_atomic;
        if(long_atomic > task_count) {
            printf("time cost: %ld ms\n", profiler.Elapsed() / 1000000);
        }
    }
}

void
start_scheduler() {
//    const std::unordered_set<int64_t> cpu_mask{1, 3, 5, 7, 9, 11, 13, 15};
    const std::unordered_set<int64_t> cpu_mask{1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15};
//    const std::unordered_set<int64_t> cpu_mask{1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15};
//    const std::unordered_set<int64_t> cpu_mask{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//    const std::unordered_set<int64_t> cpu_mask{1, 3, 5, 7};
//    const std::unordered_set<int64_t> cpu_mask;
//    total_query_count = 16;

    int64_t cpu_count = std::thread::hardware_concurrency();
    std::unordered_set<int64_t> cpu_set;
    for(int64_t idx = 0; idx < cpu_count; ++idx) {
        if(!cpu_mask.contains(idx)) {
            cpu_set.insert(idx);
        }
    }

    NewScheduler::Init(cpu_set);
}

void
stop_scheduler() {
    NewScheduler::Uninit();
}

std::unique_ptr<Fragment>
build_fragment0(uint64_t id, const std::string& name) {
    // sink->op2->op1->scan

    std::unique_ptr<Fragment> fragment = std::make_unique<Fragment>(id, FragmentType::kParallel);

    std::unique_ptr<Source> source = std::make_unique<Source>(name, SourceType::kScan);
    fragment->AddSource(std::move(source));

    std::unique_ptr<Operator> op1 = std::make_unique<Operator>(name);
    fragment->AddOperator(std::move(op1));
//    std::unique_ptr<Operator> op2 = std::make_unique<Operator>(name);
//    fragment->AddOperator(std::move(op2));

    std::unique_ptr<Sink> sink = std::make_unique<Sink>(name);
    fragment->AddSink(std::move(sink));

    return fragment;
}

std::unique_ptr<Fragment>
build_fragment1(uint64_t id, const std::string& name) {
    // sink->op2->op1->exchange
    std::unique_ptr<Fragment> root_fragment = std::make_unique<Fragment>(id, FragmentType::kSerial);

    {
        std::unique_ptr<Source> source = std::make_unique<Source>(name, SourceType::kExchange);
        root_fragment->AddSource(std::move(source));

        std::unique_ptr<Operator> op1 = std::make_unique<Operator>(name);
        root_fragment->AddOperator(std::move(op1));
        std::unique_ptr<Operator> op2 = std::make_unique<Operator>(name);
        root_fragment->AddOperator(std::move(op2));

        std::unique_ptr<Sink> sink = std::make_unique<Sink>(name);
        root_fragment->AddSink(std::move(sink));
    }

    std::unique_ptr<Fragment> child_fragment = std::make_unique<Fragment>(id, FragmentType::kParallel);

    {
        std::unique_ptr<Source> source = std::make_unique<Source>(name, SourceType::kScan);
        child_fragment->AddSource(std::move(source));

        std::unique_ptr<Operator> op1 = std::make_unique<Operator>(name);
        child_fragment->AddOperator(std::move(op1));
        std::unique_ptr<Operator> op2 = std::make_unique<Operator>(name);
        child_fragment->AddOperator(std::move(op2));

        std::unique_ptr<Sink> sink = std::make_unique<Sink>(name);
        child_fragment->AddSink(std::move(sink));
    }

    root_fragment->SetChild(std::move(child_fragment));

    return root_fragment;
}

auto
main() -> int {

//    uint64_t parallel_size = std::thread::hardware_concurrency();
    uint64_t parallel_size = 65536 * 50;
//    uint64_t parallel_size = 1;

    start_scheduler();

    ctpl::thread_pool pool(32);

    std::unique_ptr<Fragment> frag0 = build_fragment0(0, "test");
//    std::unique_ptr<Fragment> frag0 = build_fragment1(0, "test");
    std::vector<std::shared_ptr<Task>> root_tasks = frag0->BuildTask(parallel_size);
//    std::shared_ptr<Buffer> source_buffer_ = std::make_unique<Buffer>(BUFFER_SIZE);
#if 1
    profiler.Begin();
    for(const auto& task: root_tasks) {
        pool.push(execute_task, task.get(), parallel_size - 1);
    }
#endif
    sleep(7);
    stop_scheduler();
}
