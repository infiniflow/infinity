//
// Created by JinHai on 2022/7/18.
//

import compilation_config;
import stl;
import third_party;
import db_server;

namespace infinity {

void ParseArguments(int argc, char **argv, StartupParameter &parameters) {
    CxxOptions options("./infinity_main", "");

    options.add_options()("h,help", "Display this help and exit") // NOLINT
            ("f,config",
             "Specify the config file path. No default config file",
             cxx_value<String>()->default_value("")) // NOLINT
            ;

    ParseResult result = options.parse(argc, argv);

    if (result.count("help")) {
        Printf("{}", options.help());
        return;
    }

    String config_path = result["config"].as<String>();
    if (!config_path.empty()) {
        parameters.config_path = MakeShared<String>(config_path);
    }
}

}



auto main(int argc, char **argv) -> int {
    using namespace infinity;

    Printf(" __  .__   __.  _______  __  .__   __.  __  .___________.____    ____ \n"
           "|  | |  \\ |  | |   ____||  | |  \\ |  | |  | |           |\\   \\  /   / \n"
           "|  | |   \\|  | |  |__   |  | |   \\|  | |  | `---|  |----` \\   \\/   /  \n"
           "|  | |  . `  | |   __|  |  | |  . `  | |  |     |  |       \\_    _/   \n"
           "|  | |  |\\   | |  |     |  | |  |\\   | |  |     |  |         |  |     \n"
           "|__| |__| \\__| |__|     |__| |__| \\__| |__|     |__|         |__|     \n");

    Printf("Infinity, version: {}.{}.{} build on {} with {} mode from branch: {}, commit-id: {}\n",
           version_major(),
           version_minor(),
           version_patch(),
           current_system_time(),
           build_type(),
           git_branch_name(),
           git_commit_id());

    StartupParameter parameters;
    ParseArguments(argc, argv, parameters);

    infinity::DBServer db_server(parameters);
    db_server.Run();

    db_server.Shutdown();
    return 0;
}
