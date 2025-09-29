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
#include <cstring>

#ifdef ENABLE_JEMALLOC_PROF
#include <jemalloc/jemalloc.h>
#endif

import compilation_config;
import infinity_core;
import std.compat;

namespace {

#define THRIFT_SERVER_TYPE 0

#if THRIFT_SERVER_TYPE == 0

std::thread pool_thrift_thread;
infinity::PoolThriftServer pool_thrift_server;

#elif THRIFT_SERVER_TYPE == 1

std::thread non_block_pool_thrift_thread;
infinity::NonBlockPoolThriftServer non_block_pool_thrift_server;

#else

std::thread threaded_thrift_thread;
infinity::ThreadedThriftServer threaded_thrift_server;

#endif

std::thread pool_peer_thrift_thread;
infinity::PoolPeerThriftServer pool_peer_thrift_server;

std::thread http_server_thread;
infinity::HTTPServer http_server;

std::thread pg_thread;
infinity::PGServer pg_server;

// Used for server shutdown
std::mutex server_mutex;
std::condition_variable server_cv;

bool server_running = false;

std::thread shutdown_thread;

void StartThriftServer() {
    using namespace infinity;
    uint32_t thrift_server_port = InfinityContext::instance().config()->ClientPort();

#if THRIFT_SERVER_TYPE == 0

    int32_t thrift_server_pool_size = InfinityContext::instance().config()->ConnectionPoolSize();
    pool_thrift_server.Init(InfinityContext::instance().config()->ServerAddress(), thrift_server_port, thrift_server_pool_size);
    pool_thrift_thread = pool_thrift_server.Start();

#elif THRIFT_SERVER_TYPE == 1

    i32 thrift_server_pool_size = InfinityContext::instance().config()->ConnectionPoolSize();
    non_block_pool_thrift_server.Init(InfinityContext::instance().config()->ServerAddress(), thrift_server_port, thrift_server_pool_size);
    non_block_pool_thrift_thread = infinity::std::thread([&]() { non_block_pool_thrift_server.Start(); });

#else

    threaded_thrift_server.Init(InfinityContext::instance().config()->ServerAddress(), thrift_server_port);
    threaded_thrift_thread = infinity::std::thread([&]() { threaded_thrift_server.Start(); });

#endif
    LOG_INFO("Thrift server is started.");
}

void StopThriftServer() {
    using namespace infinity;
#if THRIFT_SERVER_TYPE == 0
    pool_thrift_server.Shutdown();
    pool_thrift_thread.join();
#elif THRIFT_SERVER_TYPE == 1
    non_block_pool_thrift_server.Shutdown();
    non_block_pool_thrift_server.join();
#else
    threaded_thrift_server.Shutdown();
    threaded_thrift_thread.join();
#endif
    LOG_INFO("Thrift server is shut down.");
}

void StartPeerServer() {
    using namespace infinity;
    uint32_t peer_server_port = InfinityContext::instance().config()->PeerServerPort();
    int32_t peer_server_connection_pool_size = InfinityContext::instance().config()->PeerServerConnectionPoolSize();
    pool_peer_thrift_server.Init(InfinityContext::instance().config()->PeerServerIP(), peer_server_port, peer_server_connection_pool_size);
    pool_peer_thrift_thread = pool_peer_thrift_server.Start();
    infinity::LOG_INFO("Peer server is started.");
}

void StopPeerServer() {
    using namespace infinity;
    pool_peer_thrift_server.Shutdown();
    pool_peer_thrift_thread.join();
    LOG_INFO("Peer server is shut down.");
}

void ShutdownServer() {
    {
        std::unique_lock<std::mutex> lock(server_mutex);
        server_running = true;
        server_cv.wait(lock, [&] { return !server_running; });
    }

    http_server.Shutdown();

    infinity::LOG_INFO("HTTP server is shut down.");

    pg_server.Shutdown();

    infinity::LOG_INFO("PG server is shut down.");

    infinity::InfinityContext::instance().UnInit();

    fmt::print("Shutdown infinity server successfully\n");
}

void SignalHandler(int signal_number, siginfo_t *, void *) {
    switch (signal_number) {
        case SIGUSR1:
        case SIGINT:
        case SIGQUIT:
        case SIGTERM: {
            infinity::LOG_INFO(fmt::format("Shutdown infinity server due to signal {}", strsignal(signal_number)));

            std::unique_lock<std::mutex> lock(server_mutex);
            server_running = false;
            server_cv.notify_one();

            break;
        }
        case SIGSEGV:
        case SIGABRT: {
            // Print back strace
            const char *signal_name = strsignal(signal_number);
            infinity::PrintTransactionHistory();
            infinity::PrintStacktrace(signal_name);

            // Restore default behavior and raise again to generate a coredump
            struct sigaction sa;
            sa.sa_handler = SIG_DFL;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(signal_number, &sa, nullptr);
            raise(signal_number);
            break;
        }
#if defined(ENABLE_JEMALLOC_PROF) && !defined(__APPLE__)
        case SIGUSR2: {
            // http://jemalloc.net/jemalloc.3.html
            malloc_stats_print(nullptr, nullptr, "admp");
            int rc = mallctl("prof.dump", nullptr, nullptr, nullptr, 0);
            infinity::LOG_INFO(fmt::format("Dump memory profile %d", rc));
            break;
        }
#endif
        default: {
            // Ignore
            infinity::LOG_INFO(fmt::format("Other type of signal: {}", strsignal(signal_number)));
        }
    }
    //    exit(0);
}

void RegisterSignal() {
    struct sigaction sig_action;
    sig_action.sa_flags = SA_SIGINFO;
    sig_action.sa_sigaction = SignalHandler;
    sigemptyset(&sig_action.sa_mask);
    sigaction(SIGUSR1, &sig_action, nullptr);
#ifdef ENABLE_JEMALLOC_PROF
    sigaction(SIGUSR2, &sig_action, nullptr);
#endif
    sigaction(SIGINT, &sig_action, nullptr);
    sigaction(SIGQUIT, &sig_action, nullptr);
    sigaction(SIGTERM, &sig_action, nullptr);
    sigaction(SIGSEGV, &sig_action, nullptr);
    sigaction(SIGABRT, &sig_action, nullptr);
}

void TerminateHandler() {
    std::string message = "TerminateHandler: ";
    try {
        std::exception_ptr eptr{std::current_exception()};
        if (eptr) {
            std::rethrow_exception(eptr);
        } else {
            message += "Exiting without exception";
        }
    } catch (const std::exception &ex) {
        message += "Unhandled Exception: ";
        message += ex.what();
    } catch (...) {
        message += "Unknown Unhandled Exception";
    }
    infinity::PrintTransactionHistory();
    infinity::PrintStacktrace(message);
    std::abort();
}

} // namespace

auto main(int argc, char **argv) -> int {
    using namespace infinity;

    fmt::print(" __  .__    __.  _______  __  .__    __.  __  .___________. ____     ____ \n"
               "|  | |   \\ |  | |   ____||  | |   \\ |  | |  | |           | \\    \\  /   / \n"
               "|  | |    \\|  | |  |__   |  | |    \\|  | |  | `---|  |----`  \\    \\/   /  \n"
               "|  | |  . `   | |   __|  |  | |  . `   | |  |     |  |        \\_    __/   \n"
               "|  | |  | \\   | |  |     |  | |  | \\   | |  |     |  |         |   |      \n"
               "|__| |__|  \\__| |__|     |__| |__|  \\__| |__|     |__|         |___|      \n");

    fmt::print("Release: {}.{}.{} build on {} with {} mode from branch: {}, commit-id: {}\n",
               version_major(),
               version_minor(),
               version_patch(),
               system_build_time(),
               build_type(),
               git_branch_name(),
               git_commit_id());

    fmt::print("Currently enabled SIMD support: {}\n", fmt::join(GetSupportedSimdTypesList(), ", "));

    CLI::App app{"infinity_main"};

    auto config_path = std::make_shared<std::string>();
    bool m_flag{false};
    app.add_option("-f,--config", *config_path, "Specify the config file path. No default config file");
    app.add_flag("-m,--maintenance", m_flag, "Start Infinity in maintenance mode");
    try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        return app.exit(e);
    }

    InfinityContext::instance().InitPhase1(config_path, nullptr);

    auto start_thrift_servers = [&]() {
        StartThriftServer();
        StartPeerServer();
    };

    auto stop_thrift_servers = [&]() {
        StopPeerServer();
        StopThriftServer();
    };

    InfinityContext::instance().AddThriftServerFn(start_thrift_servers, stop_thrift_servers);
    InfinityContext::instance().StartThriftServers();

    pg_thread = pg_server.Run();

    http_server_thread = http_server.Start(InfinityContext::instance().config()->ServerAddress(), InfinityContext::instance().config()->HTTPPort());

    shutdown_thread = std::thread([&]() { ShutdownServer(); });

    RegisterSignal();

    std::set_terminate(TerminateHandler);

    InfinityContext::instance().InitPhase2(m_flag);

    shutdown_thread.join();

    http_server_thread.join();

    pg_thread.join();

    fmt::print("Server is shut down\n");
    return 0;
}
