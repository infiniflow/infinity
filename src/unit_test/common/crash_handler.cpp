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

module;

#include <csignal>

module crash_handler;

import infinity_core;
import std.compat;

namespace infinity {

// Static member definitions
bool CrashHandler::enabled_ = true;
const char *CrashHandler::current_test_name_ = nullptr;
void (*CrashHandler::old_sigsegv_)(int) = nullptr;
void (*CrashHandler::old_sigabrt_)(int) = nullptr;
void (*CrashHandler::old_sigfpe_)(int) = nullptr;
void (*CrashHandler::old_sigill_)(int) = nullptr;
void (*CrashHandler::old_sigbus_)(int) = nullptr;

void CrashHandler::InstallForTest(const char *test_name) {
    if (!enabled_)
        return;

    current_test_name_ = test_name;

    // Install signal handlers
    old_sigsegv_ = std::signal(SIGSEGV, SignalHandler);
    old_sigabrt_ = std::signal(SIGABRT, SignalHandler);
    old_sigfpe_ = std::signal(SIGFPE, SignalHandler);
    old_sigill_ = std::signal(SIGILL, SignalHandler);
    old_sigbus_ = std::signal(SIGBUS, SignalHandler);

    fmt::print(stderr, "[CrashHandler] Installed for test: %s\n", test_name);
}

void CrashHandler::Uninstall() {
    if (!enabled_)
        return;

    // Restore original handlers
    if (old_sigsegv_)
        std::signal(SIGSEGV, old_sigsegv_);
    if (old_sigabrt_)
        std::signal(SIGABRT, old_sigabrt_);
    if (old_sigfpe_)
        std::signal(SIGFPE, old_sigfpe_);
    if (old_sigill_)
        std::signal(SIGILL, old_sigill_);
    if (old_sigbus_)
        std::signal(SIGBUS, old_sigbus_);

    current_test_name_ = nullptr;
}

void CrashHandler::SignalHandler(int signal) {
    if (!enabled_)
        return;

    PrintCrashInfo(signal, current_test_name_);

    // Restore default handler and re-raise to generate core dump
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

void CrashHandler::PrintCrashInfo(int signal, const char *test_name) {
    const char *signal_name = "UNKNOWN";
    switch (signal) {
        case SIGSEGV:
            signal_name = "SIGSEGV (Segmentation fault)";
            break;
        case SIGABRT:
            signal_name = "SIGABRT (Abort)";
            break;
        case SIGFPE:
            signal_name = "SIGFPE (Floating point exception)";
            break;
        case SIGILL:
            signal_name = "SIGILL (Illegal instruction)";
            break;
        case SIGBUS:
            signal_name = "SIGBUS (Bus error)";
            break;
    }

    // Print crash header to stderr
    fmt::print(stderr, "\n");
    fmt::print(stderr, "=====================================\n");
    fmt::print(stderr, "         CRASH DETECTED\n");
    fmt::print(stderr, "=====================================\n");
    fmt::print(stderr, "Signal: {} ({})\n", signal_name, signal);
    if (test_name) {
        fmt::print(stderr, "Test: {}\n", test_name);
    }

    // Get current time
    std::time_t now = std::time(nullptr);
    fmt::print(stderr, "Time: {}", std::asctime(std::localtime(&now)));

    fmt::print(stderr, "=====================================\n");
    fmt::print(stderr, "         STACK TRACE\n");
    fmt::print(stderr, "=====================================\n");

    // Use directly (same as Infinity's PrintStacktrace)
    try {
        std::string trace = to_string(std::stacktrace::current());

        // Print error message first
        std::string crash_msg = std::string("Test crash with signal ") + std::to_string(signal) + " (" + signal_name + ")";
        if (test_name) {
            crash_msg += " in test " + std::string(test_name);
        }
        fmt::print(stderr, "Error: {}\n", crash_msg);

        // Print the stack trace
        fmt::print(stderr, "{}", trace);

    } catch (...) {
        fmt::print(stderr, "Failed to generate stacktrace\n");
    }

    fmt::print(stderr, "=====================================\n");
    fmt::print(stderr, "       END CRASH REPORT\n");
    fmt::print(stderr, "=====================================\n");
}

} // namespace infinity
