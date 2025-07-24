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

#include <cpptrace/cpptrace.hpp>
#include <cstring>
#include <ctime>
#include <csignal>
#include <cstdio>
#include <cstdlib>

module crash_handler;

namespace infinity {

// Static member definitions
bool CrashHandler::enabled_ = true;
const char* CrashHandler::current_test_name_ = nullptr;
void (*CrashHandler::old_sigsegv_)(int) = nullptr;
void (*CrashHandler::old_sigabrt_)(int) = nullptr;
void (*CrashHandler::old_sigfpe_)(int) = nullptr;
void (*CrashHandler::old_sigill_)(int) = nullptr;
void (*CrashHandler::old_sigbus_)(int) = nullptr;

void CrashHandler::InstallForTest(const char* test_name) {
    if (!enabled_) return;
    
    current_test_name_ = test_name;
    
    // Install signal handlers
    old_sigsegv_ = std::signal(SIGSEGV, SignalHandler);
    old_sigabrt_ = std::signal(SIGABRT, SignalHandler);
    old_sigfpe_ = std::signal(SIGFPE, SignalHandler);
    old_sigill_ = std::signal(SIGILL, SignalHandler);
    old_sigbus_ = std::signal(SIGBUS, SignalHandler);
    
    fprintf(stderr, "[CrashHandler] Installed for test: %s\n", test_name);
}

void CrashHandler::Uninstall() {
    if (!enabled_) return;
    
    // Restore original handlers
    if (old_sigsegv_) std::signal(SIGSEGV, old_sigsegv_);
    if (old_sigabrt_) std::signal(SIGABRT, old_sigabrt_);
    if (old_sigfpe_) std::signal(SIGFPE, old_sigfpe_);
    if (old_sigill_) std::signal(SIGILL, old_sigill_);
    if (old_sigbus_) std::signal(SIGBUS, old_sigbus_);
    
    current_test_name_ = nullptr;
}

void CrashHandler::SignalHandler(int signal) {
    if (!enabled_) return;
    
    PrintCrashInfo(signal, current_test_name_);
    
    // Restore default handler and re-raise to generate core dump
    std::signal(signal, SIG_DFL);
    std::raise(signal);
}

void CrashHandler::PrintCrashInfo(int signal, const char* test_name) {
    const char* signal_name = "UNKNOWN";
    switch (signal) {
        case SIGSEGV: signal_name = "SIGSEGV (Segmentation fault)"; break;
        case SIGABRT: signal_name = "SIGABRT (Abort)"; break;
        case SIGFPE:  signal_name = "SIGFPE (Floating point exception)"; break;
        case SIGILL:  signal_name = "SIGILL (Illegal instruction)"; break;
        case SIGBUS:  signal_name = "SIGBUS (Bus error)"; break;
    }

    // Print crash header to stderr
    fprintf(stderr, "\n");
    fprintf(stderr, "=====================================\n");
    fprintf(stderr, "         CRASH DETECTED\n");
    fprintf(stderr, "=====================================\n");
    fprintf(stderr, "Signal: %s (%d)\n", signal_name, signal);
    if (test_name) {
        fprintf(stderr, "Test: %s\n", test_name);
    }

    // Get current time
    std::time_t now = std::time(nullptr);
    fprintf(stderr, "Time: %s", std::asctime(std::localtime(&now)));

    fprintf(stderr, "=====================================\n");
    fprintf(stderr, "         STACK TRACE\n");
    fprintf(stderr, "=====================================\n");

    // Use cpptrace directly (same as Infinity's PrintStacktrace)
    try {
        int trace_stack_depth = 256;
        std::string trace = cpptrace::generate_trace(0, trace_stack_depth).to_string();

        // Print error message first
        std::string crash_msg = std::string("Test crash with signal ") +
                               std::to_string(signal) + " (" + signal_name + ")";
        if (test_name) {
            crash_msg += " in test " + std::string(test_name);
        }
        fprintf(stderr, "Error: %s\n", crash_msg.c_str());

        // Print the stack trace
        fprintf(stderr, "%s\n", trace.c_str());

    } catch (...) {
        fprintf(stderr, "Failed to generate cpptrace stacktrace\n");
    }

    fprintf(stderr, "=====================================\n");
    fprintf(stderr, "       END CRASH REPORT\n");
    fprintf(stderr, "=====================================\n");
    fflush(stderr);
}

} // namespace infinity
