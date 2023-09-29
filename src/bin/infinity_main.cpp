//
// Created by JinHai on 2022/7/18.
//

//#include "common/utility/infinity_asserter.h"
#include "network/db_server.h"
#include "cxxopts.hpp"
#include "compilation_config.h"

#include <iostream>

void
ParseArguments(int argc, char** argv, infinity::StartupParameter& parameters) {
    cxxopts::Options options("./infinity_main", "");

    options.add_options()
            ("h,help", "Display this help and exit") // NOLINT
            ("f,config",
             "Specify the config file path. No default config file",
             cxxopts::value<std::string>()->default_value("")) // NOLINT
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);

    if(result.count("help")) {
        std::cout << options.help() << std::endl;
        return;
    }

    std::string config_path = result["config"].as<std::string>();
    if(!config_path.empty()) {
        parameters.config_path = std::make_shared<std::string>(config_path);
    }
}

int
main(int argc, char** argv) {
    std::cout << " __  .__   __.  _______  __  .__   __.  __  .___________.____    ____ \n"
                 "|  | |  \\ |  | |   ____||  | |  \\ |  | |  | |           |\\   \\  /   / \n"
                 "|  | |   \\|  | |  |__   |  | |   \\|  | |  | `---|  |----` \\   \\/   /  \n"
                 "|  | |  . `  | |   __|  |  | |  . `  | |  |     |  |       \\_    _/   \n"
                 "|  | |  |\\   | |  |     |  | |  |\\   | |  |     |  |         |  |     \n"
                 "|__| |__| \\__| |__|     |__| |__| \\__| |__|     |__|         |__|     \n";

    std::cout << "Infinity, version: "
              << VERSION_MAJOR << "."
              << VERSION_MINOR << "."
              << VERSION_PATCH << " built on "
              << BUILD_TIME << " with "
              << BUILD_TYPE << " mode from branch: "
              << GIT_BRANCH_NAME << ", commit-id: "
              << GIT_COMMIT_ID << std::endl;

    infinity::StartupParameter parameters;
    ParseArguments(argc, argv, parameters);

    infinity::DBServer db_server(parameters);
    db_server.Run();
    return 0;
}

