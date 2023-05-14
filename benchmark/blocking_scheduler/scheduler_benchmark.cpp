//
// Created by jinhai on 23-5-7.
//

#include "common/types/internal_types.h"
#include "task.h"
#include "fragment.h"
#include "concurrentqueue.h"
#include "main/profiler/base_profiler.h"
#include <iostream>

using namespace infinity;


void test_concurrent_queue() {
    ConcurrentQueue queue;
    ThreadPool p(2);

    UniquePtr<Buffer> buffer1 = MakeUnique<Buffer>(BUFFER_SIZE);
    UniquePtr<Buffer> buffer2 = MakeUnique<Buffer>(BUFFER_SIZE);
    UniquePtr<Buffer> buffer3 = MakeUnique<Buffer>(BUFFER_SIZE);
    queue.TryEnqueue(std::move(buffer1));
    p.push([](i64 cpu_id, ConcurrentQueue* queue) {
        while(true) {
            SharedPtr<Buffer> buffer;
            if(queue->TryDequeue(buffer)) {
                if(buffer == nullptr) {
                    printf("Terminated\n");
                    break;
                } else {
                    printf("got a buffer\n");
                }
            } else {
                printf("trying\n");
            }
        }
    }, &queue);
//    sleep(1);
    queue.TryEnqueue(std::move(buffer2));
    queue.TryEnqueue(std::move(buffer3));
    UniquePtr<Buffer> buffer4{nullptr};
    queue.TryEnqueue(std::move(buffer4));
    p.stop(true);
}

void test_waitfree_queue() {
    WaitFreeQueue queue;
    ThreadPool p(2);
    SharedPtr<Buffer> buffer1 = MakeShared<Buffer>(BUFFER_SIZE);
    SharedPtr<Buffer> buffer2 = MakeShared<Buffer>(BUFFER_SIZE);
    SharedPtr<Buffer> buffer3 = MakeShared<Buffer>(BUFFER_SIZE);
    queue.TryEnqueue(buffer1);
    p.push([](i64 cpu_id, WaitFreeQueue* queue) {
        while(true) {
            SharedPtr<Buffer> buffer;
            if(queue->TryDequeue(buffer)) {
                if(buffer == nullptr) {
                    printf("Terminated\n");
                    break;
                } else {
                    printf("got a buffer\n");
                }
            } else {
                printf("trying\n");
            }
        }
    }, &queue);
//    sleep(1);
    queue.TryEnqueue(buffer2);
    queue.TryEnqueue(buffer3);
    SharedPtr<Buffer> buffer4{nullptr};
    queue.TryEnqueue(buffer4);
    p.stop(true);
}

static BaseProfiler profiler;
std::atomic_long long_atomic{0};

void
execute_task(i64 id, Task* task, i64 task_count) {
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
                NewScheduler::RunTask(task_node);
                continue;
            }
            for(const auto& child_task: task_node->children()) {
                queue.push((PipelineTask*)child_task.get());
            }
        }

        root_task->GetResult();
        ++ long_atomic;
        if(long_atomic > task_count) {
            printf("time cost: %ld ms\n", profiler.Elapsed() / 1000000);
        }
    }
}

void
direct_execute_task(i64 id, Task* task, i64 task_count) {
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
                NewScheduler::DispatchTask(id % 16, task_node);
//                NewScheduler::RunTask(task_node);
                continue;
            }
            for(const auto& child_task: task_node->children()) {
                queue.push((PipelineTask*)child_task.get());
            }
        }

        root_task->GetResult();
        ++ long_atomic;
        if(long_atomic > task_count) {
            printf("time cost: %ld ms\n", profiler.Elapsed() / 1000000);
        }
    }
}

void
start_scheduler() {
//        const HashSet<i64> cpu_mask{1, 3, 5, 7, 9, 11, 13, 15};
//    const HashSet<i64> cpu_mask{1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15};
//    const HashSet<i64> cpu_mask{1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15};
//    const HashSet<i64> cpu_mask{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//    const HashSet<i64> cpu_mask{1, 3, 5, 7};
    const HashSet<i64> cpu_mask;
//    total_query_count = 16;

    i64 cpu_count = std::thread::hardware_concurrency();
    HashSet<i64> cpu_set;
    for(i64 idx = 0; idx < cpu_count; ++ idx) {
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

UniquePtr<Fragment>
build_fragment0(u64 id, const String& name) {
    // sink->op2->op1->scan

    UniquePtr<Fragment> fragment = MakeUnique<Fragment>(id, FragmentType::kParallel);

    UniquePtr<Source> source = MakeUnique<Source>(name, SourceType::kScan);
    fragment->AddSource(std::move(source));

    UniquePtr<Operator> op1 = MakeUnique<Operator>(name);
    fragment->AddOperator(std::move(op1));
//    UniquePtr<Operator> op2 = MakeUnique<Operator>(name);
//    fragment->AddOperator(std::move(op2));

    UniquePtr<Sink> sink = MakeUnique<Sink>(name);
    fragment->AddSink(std::move(sink));

    return fragment;
}

UniquePtr<Fragment>
build_fragment1(u64 id, const String& name) {
    // sink->op2->op1->exchange
    UniquePtr<Fragment> root_fragment = MakeUnique<Fragment>(id, FragmentType::kSerial);

    {
        UniquePtr<Source> source = MakeUnique<Source>(name, SourceType::kExchange);
        root_fragment->AddSource(std::move(source));

        UniquePtr<Operator> op1 = MakeUnique<Operator>(name);
        root_fragment->AddOperator(std::move(op1));
        UniquePtr<Operator> op2 = MakeUnique<Operator>(name);
        root_fragment->AddOperator(std::move(op2));

        UniquePtr<Sink> sink = MakeUnique<Sink>(name);
        root_fragment->AddSink(std::move(sink));
    }

    UniquePtr<Fragment> child_fragment = MakeUnique<Fragment>(id, FragmentType::kParallel);

    {
        UniquePtr<Source> source = MakeUnique<Source>(name, SourceType::kScan);
        child_fragment->AddSource(std::move(source));

        UniquePtr<Operator> op1 = MakeUnique<Operator>(name);
        child_fragment->AddOperator(std::move(op1));
        UniquePtr<Operator> op2 = MakeUnique<Operator>(name);
        child_fragment->AddOperator(std::move(op2));

        UniquePtr<Sink> sink = MakeUnique<Sink>(name);
        child_fragment->AddSink(std::move(sink));
    }

    root_fragment->SetChild(std::move(child_fragment));

    return root_fragment;
}

auto
main () -> int {

//    u64 parallel_size = std::thread::hardware_concurrency();
//    u64 parallel_size = 65536;
    u64 parallel_size = 65536 * 50;

    start_scheduler();

    ThreadPool pool(16);

    UniquePtr<Fragment> frag0 = build_fragment0(0, "test");
//    UniquePtr<Fragment> frag0 = build_fragment1(0, "test");
    Vector<SharedPtr<Task>> root_tasks = frag0->BuildTask(parallel_size);
    SharedPtr<Buffer> source_buffer_ = MakeUnique<Buffer>(BUFFER_SIZE);

    profiler.Begin();
    for(const auto& task: root_tasks) {
//        pool.push(execute_task, task.get(), parallel_size - 1);
        pool.push(direct_execute_task, task.get(), parallel_size - 1);

    }
    sleep(5);
    stop_scheduler();
}