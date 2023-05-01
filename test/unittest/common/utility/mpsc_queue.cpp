#include "common/utility/mpsc_queue.h"
#include <string>
#include <thread>
#include <set>
#include <gtest/gtest.h>

namespace infinity {

TEST(TestMPSCQueue, EnqueueDequeue) {
    MPSCQueue<std::string> q;
    std::string* s = new std::string("test");
    q.push_back(s);
    std::string* s2 = q.pop_front();
    ASSERT_EQ(s, s2);
    delete s2;
}

TEST(TestMPSCQueue, EnqueueDequeueMultithreaded) {
    MPSCQueue<int> q;
    std::vector<std::thread> threads;
    for (int t = 0; t < 5; t++) {
        threads.push_back(
        std::thread([t, &q]() {
            int start = t * 1000;
            int end = start + 1000;
            for (int i = start; i < end; i++) {
                q.push_back(new int(i));
            }
        }));
    }

    std::for_each(threads.begin(), threads.end(), [](std::thread& t) {
        t.join();
    });

    std::set<int> elements;

    int* s = nullptr;
    while ((s = q.pop_front()) != nullptr) {
        elements.insert(*s);
    }

    ASSERT_EQ(5000UL, elements.size());

    for (int i = 0; i < 5000; i++) {
        ASSERT_NE(elements.end(), elements.find(i));
    }
}
}