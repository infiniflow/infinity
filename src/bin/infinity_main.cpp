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
import pg_server;
import infinity_exception;
import infinity_context;
import thrift_server;

namespace {

infinity::PGServer pg_server;

// infinity::Thread threaded_thrift_thread;
// infinity::ThreadedThriftServer threaded_thrift_server;

infinity::Thread pool_thrift_thread;
infinity::PoolThriftServer pool_thrift_server;
// infinity::NonBlockPoolThriftServer non_block_pool_thrift_server;

std::mutex server_mutex;
std::condition_variable server_cv;

bool server_running = false;

infinity::Thread shutdown_thread;

void ShutdownServer() {

    std::unique_lock<std::mutex> lock(server_mutex);
    server_running = true;
    server_cv.wait(lock, [&] { return !server_running; });

    //            threaded_thrift_server.Shutdown();
    //            threaded_thrift_thread.join();

    pool_thrift_server.Shutdown();
    pool_thrift_thread.join();

    //            non_block_pool_thrift_server.Shutdown();

    pg_server.Shutdown();

    infinity::InfinityContext::instance().UnInit();
    fmt::print("Shutdown infinity server successfully\n");
}

void SignalHandler(int signal_number, siginfo_t *, void *) {
    switch (signal_number) {
        case SIGUSR1: {
            fmt::print("Unrecoverable error issued, stop the server");
        }
        case SIGINT:
        case SIGQUIT:
        case SIGTERM: {
            fmt::print("Shutdown infinity server ...\n");

            std::unique_lock<std::mutex> lock(server_mutex);
            server_running = false;
            server_cv.notify_one();

            break;
        }
        case SIGSEGV: {
            infinity::PrintStacktrace("SEGMENT FAULTS");
            exit(0);
            break;
        }
        default: {
            // Ignore
            fmt::print("Other type of signal: %d\n", signal_number);
        }
    }
}

void RegisterSignal() {
    struct sigaction sig_action;
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = SignalHandler;
    sigemptyset(&sig_action.sa_mask);
    sigaction(SIGUSR1, &sig_action, NULL);
    sigaction(SIGINT, &sig_action, NULL);
    sigaction(SIGQUIT, &sig_action, NULL);
    sigaction(SIGTERM, &sig_action, NULL);
    sigaction(SIGSEGV, &sig_action, NULL);
}

} // namespace

auto main(int argc, char **argv) -> int {
    using namespace infinity;

    fmt::print(" __  .__   __.  _______  __  .__   __.  __  .___________.____    ____ \n"
               "|  | |  \\ |  | |   ____||  | |  \\ |  | |  | |           |\\   \\  /   / \n"
               "|  | |   \\|  | |  |__   |  | |   \\|  | |  | `---|  |----` \\   \\/   /  \n"
               "|  | |  . `  | |   __|  |  | |  . `  | |  |     |  |       \\_    _/   \n"
               "|  | |  |\\   | |  |     |  | |  |\\   | |  |     |  |         |  |     \n"
               "|__| |__| \\__| |__|     |__| |__| \\__| |__|     |__|         |__|     \n");

    StartupParameter parameters;
    CLI::App app{"infinity_main"};
    String config_path = "";
    app.add_option("-f,--config", config_path, "Specify the config file path. No default config file");
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }
    if (!config_path.empty()) {
        parameters.config_path = MakeShared<String>(config_path);
    }

    InfinityContext::instance().Init(parameters.config_path);

    RegisterSignal();

    InfinityContext::instance().config()->PrintAll();

    fmt::print("Release: {}.{}.{} build on {} with {} mode from branch: {}, commit-id: {}\n",
               version_major(),
               version_minor(),
               version_patch(),
               current_system_time(),
               build_type(),
               git_branch_name(),
               git_commit_id());
    //    threaded_thrift_server.Init(9090);
    //    threaded_thrift_thread = infinity::Thread([&]() { threaded_thrift_server.Start(); });
    u32 thrift_server_port = InfinityContext::instance().config()->sdk_port();
    i32 thrift_server_pool_size = InfinityContext::instance().config()->connection_limit();

    pool_thrift_server.Init(thrift_server_port, thrift_server_pool_size);
    pool_thrift_thread = infinity::Thread([&]() { pool_thrift_server.Start(); });

    //    non_block_pool_thrift_server.Init(9070, 64);
    //    non_block_pool_thrift_server.Start();
    shutdown_thread = infinity::Thread([&]() { ShutdownServer(); });
    pg_server.Run();

    shutdown_thread.join();

    fmt::print("Server is shutdown\n");
    return 0;
}
