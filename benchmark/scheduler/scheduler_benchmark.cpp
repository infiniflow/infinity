//
// Created by jinhai on 23-5-7.
//

#include "common/types/internal_types.h"
#include "embedding/scheduler.h"
#include <iostream>

using namespace infinity;

void
first(i64 id) {
    std::cout << "hello from " << id << ", function\n";
}

void
start_scheduler(Scheduler& scheduler) {
    //    const HashSet<i64> cpu_mask{1, 3, 5, 7, 9, 11, 13, 15};
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

    scheduler.Init(cpu_set);
}

void
stop_scheduler(Scheduler& scheduler) {
    scheduler.Uninit();
}

auto
main () -> int {

    Scheduler scheduler;
    start_scheduler(scheduler);
    stop_scheduler(scheduler);


//    ThreadPool p(2);
//    p.push(first);
//    p.push(first);
//    p.push(first);

    // Create task according to fragment

    //
    return 0;
}