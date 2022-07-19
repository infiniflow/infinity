//
// Created by JinHai on 2022/7/18.
//

#include <iostream>
#include "network/db_server.h"
#include "cxxopts.hpp"

void parse_arguments(int argc, char** argv, StartupParameter& parameters) {
    cxxopts::Options options("./infinity_server", "");

    options.add_options()
            ("h,help", "Display this help and exit") // NOLINT
            ("addr,address", "Specify the listening address. Default value: 0.0.0.0", cxxopts::value<std::string>()->default_value("0.0.0.0")) // NOLINT
            ("p,port", "Specify the listening port. 0 means randomly select an available one. Default value: 5432", cxxopts::value<uint16_t>()->default_value("5432")) // NOLINT
            ;

    cxxopts::ParseResult result = options.parse(argc, argv);

    if(result.count("help")) {
        std::cout << options.help() << std::endl;
        return ;
    }

    parameters.address = result["address"].as<std::string>();
    parameters.port = result["port"].as<uint16_t>();
}

int main(int argc, char** argv) {
    StartupParameter parameters;
    parse_arguments(argc, argv, parameters);

    DBServer db_server(parameters);
    db_server.run();
    return 0;
}

