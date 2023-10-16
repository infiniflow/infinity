//
// Created by jinhai on 23-8-21.
//

#include "base_test.h"
#include "json.hpp"
#include "bin/compilation_config.h"
#include "main/infinity.h"

class JsonTest : public BaseTest {};

TEST_F(JsonTest, test1) {
    using namespace infinity;
    using namespace nlohmann;

    String json_path = String(TEST_DATA_PATH) + "/json/twitter.json";
    std::cout << "JSON Path: " << json_path << std::endl;
    std::ifstream f(json_path);

    json data = json::parse(f);
    EXPECT_EQ(data["search_metadata"]["count"], 100);
}
