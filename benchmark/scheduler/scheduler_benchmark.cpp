//
// Created by jinhai on 23-5-7.
//

#include "common/types/internal_types.h"
#include "new_scheduler.h"
#include "fragment.h"
#include "concurrentqueue.h"
#include <iostream>

using namespace infinity;

void
execute_task(i64 id, Task* task) {
    printf("execute task by thread: %ld\n", id);
    NewScheduler::RunTask(task);
}

void
start_scheduler() {
    //    const HashSet<i64> cpu_mask{1, 3, 5, 7, 9, 11, 13, 15};
    const HashSet<i64> cpu_mask{1, 2, 3, 5, 6, 7, 9, 10, 11, 13, 14, 15};
//    const HashSet<i64> cpu_mask{1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 12, 13, 14, 15};
//    const HashSet<i64> cpu_mask{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
//    const HashSet<i64> cpu_mask{1, 3, 5, 7};
//    const HashSet<i64> cpu_mask;
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

    UniquePtr<Fragment> fragment = MakeUnique<Fragment>(id);

    UniquePtr<Source> source = MakeUnique<Source>(name, SourceType::kScan);
    fragment->AddSource(std::move(source));

    UniquePtr<Operator> op1 = MakeUnique<Operator>(name);
    fragment->AddOperator(std::move(op1));
    UniquePtr<Operator> op2 = MakeUnique<Operator>(name);
    fragment->AddOperator(std::move(op2));

    UniquePtr<Sink> sink = MakeUnique<Sink>(name);
    fragment->AddSink(std::move(sink));

    return fragment;
}

UniquePtr<Fragment>
build_fragment1(u64 id, const String& name) {
    // sink->op2->op1->exchange
    UniquePtr<Fragment> fragment = MakeUnique<Fragment>(id);

    UniquePtr<Source> source = MakeUnique<Source>(name, SourceType::kExchange);
    fragment->AddSource(std::move(source));

    UniquePtr<Operator> op1 = MakeUnique<Operator>(name);
    fragment->AddOperator(std::move(op1));
    UniquePtr<Operator> op2 = MakeUnique<Operator>(name);
    fragment->AddOperator(std::move(op2));

    UniquePtr<Sink> sink = MakeUnique<Sink>(name);
    fragment->AddSink(std::move(sink));

    return fragment;
}

void test_concurrent_queue() {
    ConcurrentQueue queue;
    ThreadPool p(2);
    DummyTask task1;
    DummyTask task2;
    queue.TryEnqueue(&task1);
    p.push([](i64 cpu_id, ConcurrentQueue* queue) {
        while(true) {
            Task* task{nullptr};
            if(queue->TryDequeue(task)) {
                if(task->type() == TaskType::kTerminate) {
                    printf("Terminated\n");
                    break;
                }
                task->Run(cpu_id);
            } else {
                printf("trying\n");
            }
        }
    }, &queue);
    sleep(1);
    queue.TryEnqueue(&task2);
    TerminateTask task3;
    queue.TryEnqueue(&task3);
    p.stop(true);
}

void test_waitfree_queue() {
    WaitFreeQueue queue;
    ThreadPool p(2);
    DummyTask task1;
    DummyTask task2;
    queue.TryEnqueue(&task1);
    p.push([](i64 cpu_id, WaitFreeQueue* queue) {
        while(true) {
            Task* task{nullptr};
            if(queue->TryDequeue(task)) {
                if(task->type() == TaskType::kTerminate) {
                    printf("Terminated\n");
                    break;
                }
                task->Run(cpu_id);
            } else {
                printf("trying\n");
            }
        }
    }, &queue);
    sleep(1);
    queue.TryEnqueue(&task2);
    TerminateTask task3;
    queue.TryEnqueue(&task3);
    p.stop(true);
}

auto
main () -> int {
#if 0
//    u64 parallel_size = std::thread::hardware_concurrency();
    u64 parallel_size = 2;

    start_scheduler();

    UniquePtr<Fragment> frag0 = build_fragment0(0, "test");

    SharedPtr<Task> root_task = frag0->BuildTask(parallel_size);

    ThreadPool p(2);
    p.push(execute_task, root_task.get());
//    p.push(first);
//    p.push(first);
//    p.push(first);

    // Create task according to fragment

    //
    sleep(1);
    stop_scheduler();
    return 0;
#else
//    test_concurrent_queue();
    test_waitfree_queue();
#endif
}