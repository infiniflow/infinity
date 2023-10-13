//
// Created by JinHai on 2022/9/7.
//

#include "base_test.h"
#include "main/infinity.h"
#include "main/profiler/query_profiler.h"

class QueryProfilerTest : public BaseTest {
    void SetUp() override {
        infinity::GlobalResourceUsage::Init();
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::Infinity::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::Infinity::instance().UnInit();
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
    }
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
    } catch (std::exception &e) {
        std::string result(e.what());
        std::string sub = result.substr(0, result.find_first_of('@', 0) - 1);
        EXPECT_EQ(sub, "Invalid query phase in query profiler");
    }
}

// Exception cases
TEST_F(QueryProfilerTest, test2) {
    infinity::QueryProfiler profiler;
    try {
        profiler.StopPhase(infinity::QueryPhase::kParser);
    } catch (std::exception &e) {
        std::string result(e.what());
        std::string sub = result.substr(0, result.find_first_of('@', 0) - 1);
        EXPECT_EQ(sub, "Query phase isn't started, yet");
    }

    try {
        profiler.StartPhase(infinity::QueryPhase::kParser);
        profiler.StartPhase(infinity::QueryPhase::kLogicalPlan);

    } catch (std::exception &e) {
        std::string result(e.what());
        std::string sub = result.substr(0, result.find_first_of('@', 0) - 1);
        EXPECT_EQ(sub, "Can't start new query phase before current phase(Parser) is finished");
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