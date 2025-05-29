# Signal-Safe Stack Tracing <!-- omit in toc -->

- [Overview](#overview)
- [Big-Picture](#big-picture)
- [API](#api)
- [Strategy](#strategy)
- [Technical Requirements](#technical-requirements)
- [Signal-Safe Tracing With `fork()` + `exec()`](#signal-safe-tracing-with-fork--exec)
  - [In the main program](#in-the-main-program)
  - [In the tracer program](#in-the-tracer-program)

# Overview

Stack traces from signal handlers can provide very helpful information for debugging application crashes, e.g. from
SIGSEGV or SIGTRAP handlers. Signal handlers are really restrictive environments as your application could be
interrupted by a signal at any point, including in the middle of malloc or buffered IO or while holding a lock.
Doing a stack trace in a signal handler is possible but it requires a lot of care. This is difficult to do correctly
and most examples online do this incorrectly.

It is not possible to resolve debug symbols safely in the process from a signal handler without heroic effort. In order
to produce a full trace there are three options:
1. Carefully save the object trace information to be resolved at a later time outside the signal handler
2. Write the object trace information to a file to be resolved later
3. Spawn a new process, communicate object trace information to that process, and have that process do the trace
   resolution

For traces on segfaults, e.g., only options 2 and 3 are viable. The this guide will go over approach 3 and the cpptrace
safe API.

# Big-Picture

In order to do this full process safely the way to go is collecting basic information in the signal
handler and then either resolving later or handing that information to another process to resolve.

It's not as simple as calling `cpptrace::generate_trace().print()` but this is what is needed to
really do this safely as far as I can tell.

FAQ: What's the worst that could happen if you call `cpptrace::generate_trace().print()` from a
signal handler? In many cases you might be able to get away with it but you risk deadlocking or
memory corruption.

> [!IMPORTANT]
> Currently signal-safe stack unwinding is only possible with `libunwind`, more details later.

> [!IMPORTANT]
> `_dl_find_object` is required for signal-safe stack tracing. This is a relatively recent addition to glibc, added in
> glibc 2.35.

> [!CAUTION]
> Calls to shared objects can be lazy-loaded where the first call to the shared object invokes non-signal-safe functions
> such as `malloc()`. Because of this, the signal safe api must be "warmed up" ahead of a signal handler.

# API

Cpptrace provides APIs for generating raw trace information safely and then also safely resolving
those raw pointers to the most minimal object information needed to resolve later.

```cpp
namespace cpptrace {
    // signal-safe
    std::size_t safe_generate_raw_trace(frame_ptr* buffer, std::size_t size, std::size_t skip = 0);
    // signal-safe
    std::size_t safe_generate_raw_trace(frame_ptr* buffer, std::size_t size, std::size_t skip, std::size_t max_depth);

    struct safe_object_frame {
        frame_ptr raw_address;
        frame_ptr address_relative_to_object_start;
        char object_path[CPPTRACE_PATH_MAX + 1];
        object_frame resolve() const; // To be called outside a signal handler. Not signal safe.
    };

    // signal-safe
    void get_safe_object_frame(frame_ptr address, safe_object_frame* out);
    // signal-safe
    bool can_signal_safe_unwind();
    bool can_get_safe_object_frame();
}
```

`safe_generate_raw_trace` produces instruction pointers found while unwinding the stack. These addresses are sufficient
information to resolve a stack trace in the currently running process after a signal handler exits, but they aren't
sufficient for resolving outside of the currently running process, unless there is no position-independent code or
shared-library code.

To resolve outside the current process `safe_object_frame` information is needed. This contains the path to the
object where the address is located as well as the address before address randomization.

# Strategy

Signal-safe tracing can be done three ways:
- In a signal handler, call `safe_generate_raw_trace` and then outside a signal handler
  construct a `cpptrace:raw_trace` and resolve.
- In a signal handler, call `safe_generate_raw_trace`, then write `cpptrace::safe_object_frame`
  information to a file to be resolved later.
- In a signal handler, call `safe_generate_raw_trace`, `fork()` and `exec()` a process to handle the
  resolution, pass `cpptrace::safe_object_frame` information to that child through a pipe, and
  wait for the child to exit.

It's not as simple as calling `cpptrace::generate_trace().print()`, I know, but these are truly the
only ways to do this safely as far as I can tell.

# Technical Requirements

**Note:** Not all back-ends and platforms support these interfaces. If signal-safe unwinding isn't supported
`safe_generate_raw_trace` will just produce an empty trace and if object information can't be resolved in a signal-safe
way then `get_safe_object_frame` will not populate fields beyond the `raw_address`.

`cpptrace::can_signal_safe_unwind` and `cpptrace::can_get_safe_object_frame` can be used to check for safe tracing
support.

Currently the only back-end that can unwind safely is libunwind. Currently, the only way I know to get `dladdr`'s
information in a signal-safe manner is `_dl_find_object`, which doesn't exist on macos (or windows of course). If anyone
knows ways to do these safely on other platforms, I'd be much appreciative.

# Signal-Safe Tracing With `fork()` + `exec()`

Of the three strategies, `fork()` + `exec()`, is the most technically involved and the only way to resolve while the
signal handler is running. I think it's worthwhile to do a deep-dive into how to do this.

In the source code, [`signal_demo.cpp`](../test/signal_demo.cpp) and [`signal_tracer.cpp`](../test/signal_tracer.cpp)
provide a working example for what is described here.

## In the main program

The main program handles most of the complexity for tracing from signal handlers:
- Collecting a raw trace
- Spawning a child process
- Resolving raw frame pointers to minimal object frames
- Sending that info to the other process

Also note: A warmup for the library is done in main.

A basic implementation is as follows:

```cpp
#include <sys/wait.h>
#include <cstring>
#include <signal.h>
#include <unistd.h>

#include <cpptrace/cpptrace.hpp>

// This is just a utility I like, it makes the pipe API more expressive.
struct pipe_t {
    union {
        struct {
            int read_end;
            int write_end;
        };
        int data[2];
    };
};

void do_signal_safe_trace(cpptrace::frame_ptr* buffer, std::size_t count) {
    // Setup pipe and spawn child
    pipe_t input_pipe;
    pipe(input_pipe.data);
    const pid_t pid = fork();
    if(pid == -1) {
        const char* fork_failure_message = "fork() failed\n";
        write(STDERR_FILENO, fork_failure_message, strlen(fork_failure_message));
        return;
    }
    if(pid == 0) { // child
        dup2(input_pipe.read_end, STDIN_FILENO);
        close(input_pipe.read_end);
        close(input_pipe.write_end);
        execl("signal_tracer", "signal_tracer", nullptr);
        const char* exec_failure_message = "exec(signal_tracer) failed: Make sure the signal_tracer executable is in "
            "the current working directory and the binary's permissions are correct.\n";
        write(STDERR_FILENO, exec_failure_message, strlen(exec_failure_message));
        _exit(1);
    }
    // Resolve to safe_object_frames and write those to the pipe
    for(std::size_t i = 0; i < count; i++) {
        cpptrace::safe_object_frame frame;
        cpptrace::get_safe_object_frame(buffer[i], &frame);
        write(input_pipe.write_end, &frame, sizeof(frame));
    }
    close(input_pipe.read_end);
    close(input_pipe.write_end);
    // Wait for child
    waitpid(pid, nullptr, 0);
}

void handler(int signo, siginfo_t* info, void* context) {
    // Print basic message
    const char* message = "SIGSEGV occurred:\n";
    write(STDERR_FILENO, message, strlen(message));
    // Generate trace
    constexpr std::size_t N = 100;
    cpptrace::frame_ptr buffer[N];
    std::size_t count = cpptrace::safe_generate_raw_trace(buffer, N);
    do_signal_safe_trace(buffer, count);
    // Up to you if you want to exit or continue or whatever
    _exit(1);
}

void warmup_cpptrace() {
    // This is done for any dynamic-loading shenanigans
    cpptrace::frame_ptr buffer[10];
    std::size_t count = cpptrace::safe_generate_raw_trace(buffer, 10);
    cpptrace::safe_object_frame frame;
    cpptrace::get_safe_object_frame(buffer[0], &frame);
}

int main() {
    warmup_cpptrace();
    // Setup signal handler
    struct sigaction action = { 0 };
    action.sa_flags = 0;
    action.sa_sigaction = &handler;
    if (sigaction(SIGSEGV, &action, NULL) == -1) {
        perror("sigaction");
    }

    /// ...
}
```

## In the tracer program

The tracer program is quite simple. It just has to read `cpptrace::safe_object_frame`s from the pipe, resolve to
`cpptrace::object_frame`s, and resolve an `object_trace`.

```cpp
#include <cstdio>
#include <iostream>
#include <unistd.h>

#include <cpptrace/cpptrace.hpp>

int main() {
    cpptrace::object_trace trace;
    while(true) {
        cpptrace::safe_object_frame frame;
        // fread used over read because a read() from a pipe might not read the full frame
        std::size_t res = fread(&frame, sizeof(frame), 1, stdin);
        if(res == 0) {
            break;
        } else if(res != 1) {
            std::cerr<<"Something went wrong while reading from the pipe"<<res<<" "<<std::endl;
            break;
        } else {
            trace.frames.push_back(frame.resolve());
        }
    }
    trace.resolve().print();
}
```
