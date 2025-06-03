#include <cpptrace/cpptrace.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

#include <sys/wait.h>
#include <cstring>
#include <signal.h>
#include <csignal>
#include <unistd.h>

void trace() {
    *(volatile char*)0 = 2;
}

void bar() {
    trace();
}

void foo() {
    bar();
}

struct pipe_t {
    union {
        struct {
            int read_end;
            int write_end;
        };
        int data[2];
    };
};
static_assert(sizeof(pipe_t) == 2 * sizeof(int), "Unexpected struct packing");

void handler(int signo, siginfo_t* info, void* context) {
    const char* message = "SIGSEGV occurred:\n";
    write(STDERR_FILENO, message, strlen(message));
    cpptrace::frame_ptr buffer[100];
    std::size_t count = cpptrace::safe_generate_raw_trace(buffer, 100);
    pipe_t input_pipe;
    pipe(input_pipe.data);
    const pid_t pid = fork();
    if(pid == -1) {
        const char* fork_failure_message = "fork() failed\n";
        write(STDERR_FILENO, fork_failure_message, strlen(fork_failure_message));
        _exit(1);
    }
    if(pid == 0) { // child
        dup2(input_pipe.read_end, STDIN_FILENO);
        close(input_pipe.read_end);
        close(input_pipe.write_end);
        execl(
            "signal_tracer",
            "signal_tracer",
            nullptr
        );
        const char* exec_failure_message = "exec(signal_tracer) failed: Make sure the signal_tracer executable is in "
            "the current working directory and the binary's permissions are correct.\n";
        write(STDERR_FILENO, exec_failure_message, strlen(exec_failure_message));
        _exit(1);
    }
    for(std::size_t i = 0; i < count; i++) {
        cpptrace::safe_object_frame frame;
        cpptrace::get_safe_object_frame(buffer[i], &frame);
        write(input_pipe.write_end, &frame, sizeof(frame));
    }
    close(input_pipe.read_end);
    close(input_pipe.write_end);
    waitpid(pid, nullptr, 0);
    _exit(1);
}

void warmup_cpptrace() {
    cpptrace::frame_ptr buffer[10];
    std::size_t count = cpptrace::safe_generate_raw_trace(buffer, 10);
    cpptrace::safe_object_frame frame;
    cpptrace::get_safe_object_frame(buffer[0], &frame);
}

int main() {
    cpptrace::absorb_trace_exceptions(false);
    cpptrace::register_terminate_handler();
    warmup_cpptrace();

    struct sigaction action = { 0 };
    action.sa_flags = 0;
    action.sa_sigaction = &handler;
    if (sigaction(SIGSEGV, &action, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    foo();
}
