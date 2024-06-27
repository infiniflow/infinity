// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "unit_test/base_test.h"

import stl;
import profiler;
import infinity_context;
import global_resource_usage;
import infinity_exception;

class QueryProfilerTest : public BaseTest {
    void SetUp() override {
        RemoveDbDirs();
#ifdef INFINITY_DEBUG
        infinity::GlobalResourceUsage::Init();
#endif
        std::shared_ptr<std::string> config_path = nullptr;
        infinity::InfinityContext::instance().Init(config_path);
    }

    void TearDown() override {
        infinity::InfinityContext::instance().UnInit();
#ifdef INFINITY_DEBUG
        EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
        EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
        infinity::GlobalResourceUsage::UnInit();
#endif
        BaseTest::TearDown();
    }
};

TEST_F(QueryProfilerTest, test1) {
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kParser), "Parser");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kLogicalPlan), "LogicalPlan");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kOptimizer), "Optimizer");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kPhysicalPlan), "PhysicalPlan");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kPipelineBuild), "PipelineBuild");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kTaskBuild), "TaskBuild");
    EXPECT_EQ(infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kExecution), "Execution");
    try {
        infinity::QueryProfiler::QueryPhaseToString(infinity::QueryPhase::kInvalid);
    } catch (std::exception &e) {
        std::string result(e.what());
        auto sub = infinity::GetErrorMsg(result);
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
        auto sub = infinity::GetErrorMsg(result);
        EXPECT_EQ(sub, "Executor Error: Query phase isn't started, yet");
    }

    try {
        profiler.StartPhase(infinity::QueryPhase::kParser);
        profiler.StartPhase(infinity::QueryPhase::kLogicalPlan);

    } catch (std::exception &e) {
        std::string result(e.what());
        auto sub = infinity::GetErrorMsg(result);
        EXPECT_EQ(sub, "Executor Error: Can't start new query phase before current phase(Parser) is finished");
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
    profiler.StartPhase(infinity::QueryPhase::kTaskBuild);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kTaskBuild);
    profiler.StartPhase(infinity::QueryPhase::kExecution);
    usleep(1000 * 1000);
    profiler.StopPhase(infinity::QueryPhase::kExecution);

    std::cout << profiler.ToString() << std::endl;
}