//
// Created by jinhai on 23-5-7.
//

#include "common/types/internal_types.h"
#include "new_scheduler.h"
#include "fragment.h"
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
build_fragment(u64 id, const String& name) {
    UniquePtr<Fragment> fragment = MakeUnique<Fragment>(id);

    UniquePtr<Source> source = MakeUnique<Source>(name);
    fragment->AddSource(std::move(source));

    UniquePtr<Sink> sink = MakeUnique<Sink>(name);
    fragment->AddSink(std::move(sink));

    return fragment;
}

auto
main () -> int {
    u64 parallel_size = std::thread::hardware_concurrency();

    start_scheduler();

    UniquePtr<Fragment> frag0 = build_fragment(0, "test");

    SharedPtr<Task> root_task = frag0->BuildTaskFromFragment(parallel_size);

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
}