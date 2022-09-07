//
// Created by JinHai on 2022/9/7.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "main/query_profiler.h"

class QueryProfilerTest : public BaseTest {

};

TEST_F(QueryProfilerTest, test1) {
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kParser), "Parser");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kLogicalPlan), "LogicalPlan");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kOptimizer), "Optimizer");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kPhysicalPlan), "PhysicalPlan");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kPipelineBuild), "PipelineBuild");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kExecution), "Execution");
    try {
        infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kInvalid);
    } catch (std::exception& e) {
        EXPECT_STRCASEEQ(e.what(), "Invalid query phase in query profiler @src/main/query_profiler.cpp:43");
    }

}

// Exception cases
TEST_F(QueryProfilerTest, test2) {
    infinity::QueryProfiler profiler;
    try {
        profiler.StopPhase(infinity::QueryPhase::kParser);
    } catch (std::exception& e) {
        EXPECT_STRCASEEQ(e.what(), "Query phase isn't started, yet @src/main/query_profiler.cpp:66");
    }

    try {
        profiler.StartPhase(infinity::QueryPhase::kParser);
        profiler.StartPhase(infinity::QueryPhase::kLogicalPlan);

    } catch (std::exception& e) {
        EXPECT_STRCASEEQ(e.what(), "Can't start new query phase before current phase(Parser) is finished @src/main/query_profiler.cpp:55");
    }
}


TEST_F(QueryProfilerTest, test3) {
    infinity::QueryProfiler profiler;
    profiler.StartPhase(infinity::QueryPhase::kParser);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kParser);
    profiler.StartPhase(infinity::QueryPhase::kLogicalPlan);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kLogicalPlan);
    profiler.StartPhase(infinity::QueryPhase::kOptimizer);
    profiler.optimizer().StartRule("OptimizerRule1");
    profiler.optimizer().StopRule();
    profiler.optimizer().StartRule("OptimizerRule2");
    profiler.optimizer().StopRule();
    profiler.optimizer().StartRule("OptimizerRule3");
    profiler.optimizer().StopRule();
    profiler.optimizer().StartRule("OptimizerRule4");
    profiler.optimizer().StopRule();
    profiler.StopPhase(infinity::QueryPhase::kOptimizer);
    profiler.StartPhase(infinity::QueryPhase::kPhysicalPlan);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kPhysicalPlan);
    profiler.StartPhase(infinity::QueryPhase::kPipelineBuild);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kPipelineBuild);
    profiler.StartPhase(infinity::QueryPhase::kExecution);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kExecution);

    std::cout << profiler.ToString() << std::endl;
}