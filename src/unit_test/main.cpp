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

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Get all test suites
    const testing::UnitTest& unit_test = *testing::UnitTest::GetInstance();
    for (int i = 0; i < unit_test.total_test_suite_count(); ++i) {
        const testing::TestSuite* test_suite = unit_test.GetTestSuite(i);
        std::cout << "Test Suite: " << test_suite->name() << "\n";
        
        // Get all tests in this suite
        for (int j = 0; j < test_suite->total_test_count(); ++j) {
            const testing::TestInfo* test_info = test_suite->GetTestInfo(j);
            std::cout << "  Test: " << test_info->name() << "\n";
        }
    }
    
    return RUN_ALL_TESTS();
}
