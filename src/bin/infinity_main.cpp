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

#include <csignal>
#include <cstdlib>

import compilation_config;
import stl;
import third_party;
import db_server;

namespace {

infinity::DBServer db_server;

void SignalHandler(int signal_number, siginfo_t *signal_info, void *reserved) {
    switch (signal_number) {
        case SIGINT:
        case SIGQUIT:
        case SIGTERM: {
            db_server.Shutdown();
            break;
        }
        case SIGSEGV: {
            // Print back strace
            break;
        }
        default: {
            // Ignore
        }
    }
    exit(0);
}

void RegisterSignal() {
    struct sigaction sig_action;
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = SignalHandler;
    sigemptyset(&sig_action.sa_mask);
//    sigaction(SIGINT, &sig_action, NULL);
    sigaction(SIGQUIT, &sig_action, NULL);
    sigaction(SIGTERM, &sig_action, NULL);
}

} // namespace

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

} // namespace infinity

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

    db_server.Init(parameters);
    RegisterSignal();
    db_server.Run();

    return 0;
}
