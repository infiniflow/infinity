//
// Created by jinhai on 23-6-2.
//

#include "base_test.h"
#include "bin/compilation_config.h"
#include "main/infinity.h"
#include "simdjson.h"

class SimdJsonTest : public BaseTest {};

TEST_F(SimdJsonTest, test1) {
    using namespace infinity;
    String json_path = String(TEST_DATA_PATH) + "/json/twitter.json";
    //    std::cout << "JSON Path: " << json_path << std::endl;
    simdjson::ondemand::parser parser;
    simdjson::padded_string json = simdjson::padded_string::load(json_path);
    simdjson::ondemand::document tweets = parser.iterate(json);
    EXPECT_EQ(uint64_t(tweets["search_metadata"]["count"]), 100);
}