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

export module infinity_core:ut.crash_handler;

export namespace infinity {

class CrashHandler {
public:
    // Install crash handlers for the current test
    static void InstallForTest(const char *test_name);

    // Remove crash handlers
    static void Uninstall();

    // Enable/disable crash detection
    static void SetEnabled(bool enabled) { enabled_ = enabled; }

private:
    static void SignalHandler(int signal);
    static void PrintCrashInfo(int signal, const char *test_name);

    static bool enabled_;
    static const char *current_test_name_;
    static void (*old_sigsegv_)(int);
    static void (*old_sigabrt_)(int);
    static void (*old_sigfpe_)(int);
    static void (*old_sigill_)(int);
    static void (*old_sigbus_)(int);
};

class CrashHandlerGuard {
public:
    explicit CrashHandlerGuard(const char *test_name = nullptr) { CrashHandler::InstallForTest(test_name); }

    ~CrashHandlerGuard() { CrashHandler::Uninstall(); }

    // Non-copyable
    CrashHandlerGuard(const CrashHandlerGuard &) = delete;
    CrashHandlerGuard &operator=(const CrashHandlerGuard &) = delete;
};

} // namespace infinity
