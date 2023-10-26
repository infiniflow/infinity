//
// Created by jinhai on 23-8-27.
//

#include <iostream>
#include <string>
#include <filesystem>
#include "toml.hpp"
#include "toml_config.h"

auto
main() -> int {

    std::cout << std::filesystem::current_path() << std::endl;

    std::string filename = std::string(TEST_DATA_PATH) + "/config/infinity_conf.toml";

    auto config = toml::parse_file(filename);

    auto general_config = config["general"];
    std::cout << general_config["version"].value_or("") << std::endl;
    std::cout << general_config["timezone"].value_or("") << std::endl;
    std::cout << general_config["pg_port"] << std::endl;
    std::cout << general_config["http_port"] << std::endl;
    std::cout << general_config["grpc_port"] << std::endl;

    auto log_config = config["log"];
    std::cout << log_config["log_dir"].value_or("") << std::endl;
    std::cout << log_config["log_to_stdout"] << std::endl;
    std::cout << log_config["max_log_file_size"].value_or("") << std::endl;
    std::cout << log_config["log_level"].value_or("") << std::endl;


    auto storage_config = config["storage"];
    std::cout << storage_config["data_dir"].value_or("") << std::endl;
    std::cout << storage_config["wal_dir"].value_or("") << std::endl;

    auto buffer_config = config["buffer"];
    std::cout << buffer_config["buffer_pool_size"].value_or("") << std::endl;
    std::cout << buffer_config["temp_dir"].value_or("") << std::endl;
    return 0;
}
