//
// Created by jinhai on 23-10-16.
//

#include "unit_test/base_test.h"

import infinity_exception;
import infinity_assert;
import stl;
import profiler;

class BaseProfilerTest : public BaseTest {};

TEST_F(BaseProfilerTest, test1) {
    infinity::BaseProfiler prof;
    prof.Begin();
    prof.End();
    //    std::cout << prof.Elapsed() << std::endl;
    EXPECT_LT(prof.Elapsed(), 1000);
    EXPECT_NE(prof.ElapsedToString().find("ns"), std::string::npos);

    // Test the time duration larger than 1us
    prof.Begin();
    usleep(1);
    prof.End();
    EXPECT_GT(prof.Elapsed(), 1000);
    EXPECT_LT(prof.Elapsed(), 1000 * 1000);
    EXPECT_NE(prof.ElapsedToString().find("us"), std::string::npos);

    // Test the time duration larger than 1ms
    prof.Begin();
    usleep(1000);
    prof.End();
    EXPECT_GT(prof.Elapsed(), 1000 * 1000);
    EXPECT_LT(prof.Elapsed(), 1000 * 1000 * 1000);
    EXPECT_NE(prof.ElapsedToString().find("ms"), std::string::npos);

    // Test the time duration larger than 1s
    prof.Begin();
    usleep(1000 * 1000);
    prof.End();
    EXPECT_GT(prof.Elapsed(), 1000 * 1000 * 1000);
    EXPECT_LT(prof.Elapsed(), 1000 * 1000 * 1000 * 1000L);
    EXPECT_NE(prof.ElapsedToString().find('s'), std::string::npos);

    //    std::cout << prof.Elapsed() << std::endl;
    //    std::cout << prof.ElapsedToString() << std::endl;
}
