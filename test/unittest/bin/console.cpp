//
// Created by JinHai on 2022/10/1.
//

#include <gtest/gtest.h>
#include "base_test.h"
#include "bin/console.h"
#include "common/utility/str.h"
#include "main/stats/global_resource_usage.h"
#include "main/infinity.h"

#include <filesystem>
#include <fstream>

class ConsoleTest : public BaseTest {
    void
    SetUp() override {
    }

    void
    TearDown() override {
    }
};

void
recurse_dir(const std::filesystem::path& dir_name, std::vector<std::filesystem::path>& file_names) {

    for(auto& p: std::filesystem::directory_iterator(dir_name)) {
        if(p.is_directory()) {
            recurse_dir(p.path(), file_names);
        } else {
            file_names.emplace_back(p.path());
        }
    }
}

void
execute_sql(infinity::Console& console, const std::string& filename) {
    std::string string_line;
    std::ifstream input_file(filename);
    while(std::getline(input_file, string_line)) {
        // Get the command line from head to '#'
        std::string cmd_line = string_line.substr(0, string_line.find_first_of('#'));

        // Trim the command line
        infinity::trim(cmd_line);
        if(cmd_line.empty()) {
            continue;;
        }
        try {
            std::cout << "[SQL]: " << cmd_line << std::endl;
            auto result = console.HandleCommand(cmd_line.c_str());
        } catch (std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
    std::cout << filename << std::endl;
}



// TEST_F(ConsoleTest, explain_unopt_logical) {
//     {
//         infinity::Console console;
//         console.Init();

//         std::filesystem::path current_path = std::filesystem::current_path();
//         std::filesystem::path test_data = current_path / "../../../test/unittest/bin/test_data";
//         std::vector<std::filesystem::path> file_names;
//         recurse_dir(test_data, file_names);
//         for(auto& path: file_names) {
//             execute_sql(console, path.string());
//         }
//         console.UnInit();
//     }

//     EXPECT_EQ(infinity::GlobalResourceUsage::GetObjectCount(), 0);
//     EXPECT_EQ(infinity::GlobalResourceUsage::GetRawMemoryCount(), 0);
// }
