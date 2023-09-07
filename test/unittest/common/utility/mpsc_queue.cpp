#include "common/utility/mpsc_queue.h"
#include <string>
#include <thread>
#include <set>
#include <algorithm>
#include <gtest/gtest.h>

namespace infinity {

TEST(TestMPSCQueue, EnqueueDequeue) {
    MPSCQueue<std::string> q;
    std::string s("test");
    q.enqueue(s);
    std::string s2;
    q.dequeue(s2);
    ASSERT_EQ(s, s2);
}

TEST(TestMPSCQueue, EnqueueDequeueMultithreaded) {
    MPSCQueue<int> q;
    std::vector<std::thread> threads;
    for (int t = 0; t < 5; t++) {
        threads.emplace_back(
        [t, &q]() {
            int start = t * 10000;
            int end = start + 10000;
            for (int i = start; i < end; i++) {
                q.enqueue(i);
            }
        });
    }

    std::for_each(threads.begin(), threads.end(), [](std::thread& t) {
        t.join();
    });

    std::set<int> elements;

    int s;
    while (q.dequeue(s)) {
        elements.insert(s);
    }

    ASSERT_EQ(50000UL, elements.size());

    for (int i = 0; i < 50000; i++) {
        ASSERT_NE(elements.end(), elements.find(i));
    }
}
}