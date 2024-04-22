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
#include <cstdio>
#include <cstdlib>
#ifdef ENABLE_JEMALLOC_PROF
#include <jemalloc/jemalloc.h>
#endif

import compilation_config;
import stl;
import third_party;
import pg_server;
import infinity_exception;
import infinity_context;
import thrift_server;
import http_server;

namespace {

infinity::PGServer pg_server;

#define THRIFT_SERVER_TYPE 0

#if THRIFT_SERVER_TYPE == 0

infinity::Thread pool_thrift_thread;
infinity::PoolThriftServer pool_thrift_server;

#elif THRIFT_SERVER_TYPE == 1

infinity::NonBlockPoolThriftServer non_block_pool_thrift_server;

#else

infinity::Thread threaded_thrift_thread;
infinity::ThreadedThriftServer threaded_thrift_server;

#endif

infinity::Thread http_server_thread;
infinity::HTTPServer http_server;


std::mutex server_mutex;
std::condition_variable server_cv;

bool server_running = false;

infinity::Thread shutdown_thread;

void ShutdownServer() {
    {
        std::unique_lock<std::mutex> lock(server_mutex);
        server_running = true;
        server_cv.wait(lock, [&] { return !server_running; });
    }

    http_server.Shutdown();
    http_server_thread.join();
    fmt::print("HTTP Server is shutdown.\n");

#if THRIFT_SERVER_TYPE == 0

    pool_thrift_server.Shutdown();
    pool_thrift_thread.join();

#elif THRIFT_SERVER_TYPE == 1

    non_block_pool_thrift_server.Shutdown();

#else

    threaded_thrift_server.Shutdown();
    threaded_thrift_thread.join();

#endif

    fmt::print("Thrift Server is shutdown.\n");

    pg_server.Shutdown();
    fmt::print("PG Server is shutdown.\n");
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
            // Print back strace
            infinity::PrintStacktrace("SEGMENT FAULTS");
            exit(0);
            break;
        }
#ifdef ENABLE_JEMALLOC_PROF
        case SIGUSR2: {
            // http://jemalloc.net/jemalloc.3.html
            int rc = mallctl("prof.dump", NULL, NULL, NULL, 0);
            printf("Dump memory profile %d\n", rc);
            break;
        }
#endif
        default: {
            // Ignore
            printf("Other type of signal: %d\n", signal_number);
        }
    }
    //    exit(0);
}

void RegisterSignal() {
    struct sigaction sig_action;
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = SignalHandler;
    sigemptyset(&sig_action.sa_mask);
    sigaction(SIGUSR1, &sig_action, NULL);
#ifdef ENABLE_JEMALLOC_PROF
    sigaction(SIGUSR2, &sig_action, NULL);
#endif
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

    http_server_thread = infinity::Thread([&]() { http_server.Start(InfinityContext::instance().config()->http_port()); });

    u32 thrift_server_port = InfinityContext::instance().config()->sdk_port();

#if THRIFT_SERVER_TYPE == 0

    i32 thrift_server_pool_size = InfinityContext::instance().config()->connection_limit();
    pool_thrift_server.Init(thrift_server_port, thrift_server_pool_size);
    pool_thrift_thread = infinity::Thread([&]() { pool_thrift_server.Start(); });

#elif THRIFT_SERVER_TYPE == 1

    i32 thrift_server_pool_size = InfinityContext::instance().config()->connection_limit();
    non_block_pool_thrift_server.Init(thrift_server_port, thrift_server_pool_size);
    non_block_pool_thrift_server.Start();

#else

    threaded_thrift_server.Init(thrift_server_port);
    threaded_thrift_thread = infinity::Thread([&]() { threaded_thrift_server.Start(); });

#endif

    shutdown_thread = infinity::Thread([&]() { ShutdownServer(); });
    pg_server.Run();

    shutdown_thread.join();

    fmt::print("Server is shutdown\n");
    return 0;
}
