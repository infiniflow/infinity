# ctrace  <!-- omit in toc -->

Cpptrace provides a C API under the name ctrace, documented below.

## Table of Contents  <!-- omit in toc -->

- [Documentation](#documentation)
  - [Stack Traces](#stack-traces)
  - [Object Traces](#object-traces)
  - [Raw Traces](#raw-traces)
  - [Utilities](#utilities)
  - [Utility types](#utility-types)
  - [Configuration](#configuration)
  - [Signal-Safe Tracing](#signal-safe-tracing)

## Documentation

All ctrace declarations are in the `ctrace.h` header:

```c
#include <ctrace/ctrace.h>
```

### Stack Traces

Generate stack traces with `ctrace_generate_trace()`. Often `skip = 0` and `max_depth = SIZE_MAX` is what you want for
the parameters.

`ctrace_stacktrace_to_string` and `ctrace_print_stacktrace` can then be used for output.

`ctrace_free_stacktrace` must be called when you are done with the trace.

```c
typedef struct ctrace_stacktrace ctrace_stacktrace;

struct ctrace_stacktrace_frame {
    ctrace_frame_ptr raw_address;
    ctrace_frame_ptr object_address;
    uint32_t line;
    uint32_t column;
    const char* filename;
    const char* symbol;
    ctrace_bool is_inline;
};

struct ctrace_stacktrace {
    ctrace_stacktrace_frame* frames;
    size_t count;
};

ctrace_stacktrace ctrace_generate_trace(size_t skip, size_t max_depth);
ctrace_owning_string ctrace_stacktrace_to_string(const ctrace_stacktrace* trace, ctrace_bool use_color);
void ctrace_print_stacktrace(const ctrace_stacktrace* trace, FILE* to, ctrace_bool use_color);
void ctrace_free_stacktrace(ctrace_stacktrace* trace);

// object_address is stored but if the object_path is needed this can be used
ctrace_object_frame ctrace_get_object_info(const ctrace_stacktrace_frame* frame);
```

### Object Traces

Object traces contain the most basic information needed to construct a stack trace outside the currently running
executable. It contains the raw address, the address in the binary (ASLR and the object file's memory space and whatnot
is resolved), and the path to the object the instruction pointer is located in.

`ctrace_free_object_trace` must be called when you are done with the trace.

```c
typedef struct ctrace_object_trace ctrace_object_trace;

struct ctrace_object_frame {
    ctrace_frame_ptr raw_address;
    ctrace_frame_ptr obj_address;
    const char* obj_path;
};

struct ctrace_object_trace {
    ctrace_object_frame* frames;
    size_t count;
};

ctrace_object_trace ctrace_generate_object_trace(size_t skip, size_t max_depth);
ctrace_stacktrace   ctrace_resolve_object_trace(const ctrace_object_trace* trace);
void                ctrace_free_object_trace(ctrace_object_trace* trace);
```

### Raw Traces

Raw traces are arrays of program counters. These are ideal for fast and cheap traces you want to resolve later.

Note it is important executables and shared libraries in memory aren't somehow unmapped otherwise libdl calls (and
`GetModuleFileName` in windows) will fail to figure out where the program counter corresponds to.

`ctrace_free_raw_trace` must be called when you are done with the trace.

```c
typedef struct ctrace_raw_trace ctrace_raw_trace;

ctrace_raw_trace  ctrace_generate_raw_trace(size_t skip, size_t max_depth);
ctrace_stacktrace ctrace_resolve_raw_trace(const ctrace_raw_trace* trace);
void              ctrace_free_raw_trace(ctrace_raw_trace* trace);
```

### Utilities

`ctrace_demangle`: Helper function for name demangling

`ctrace_stdin_fileno`, `ctrace_stderr_fileno`, `ctrace_stdout_fileno`: Returns the appropriate file descriptor for the
respective standard stream.

`ctrace_isatty`: Checks if a file descriptor corresponds to a tty device.

```c
ctrace_owning_string ctrace_demangle(const char* mangled);
int ctrace_stdin_fileno(void);
int ctrace_stderr_fileno(void);
int ctrace_stdout_fileno(void);
ctrace_bool ctrace_isatty(int fd);
```

### Utility types

For ABI reasons `ctrace_bool`s are used for bools. `ctrace_owning_string` is a wrapper type which indicates that a
string is owned and must be freed.

```c
typedef int8_t ctrace_bool;
typedef struct {
    const char* data;
} ctrace_owning_string;
ctrace_owning_string ctrace_generate_owning_string(const char* raw_string);
void ctrace_free_owning_string(ctrace_owning_string* string);
```

### Configuration

`experimental::set_cache_mode`: Control time-memory tradeoffs within the library. By default speed is prioritized. If
using this function, set the cache mode at the very start of your program before any traces are performed. Note: This
API is not set in stone yet and could change in the future.

`ctrace_enable_inlined_call_resolution`: Configure whether the library will attempt to resolve inlined call information for
release builds. Default is true.

```c
    typedef enum {
        /* Only minimal lookup tables */
        ctrace_prioritize_memory = 0,
        /* Build lookup tables but don't keep them around between trace calls */
        ctrace_hybrid = 1,
        /* Build lookup tables as needed */
        ctrace_prioritize_speed = 2
    } ctrace_cache_mode;
    void ctrace_set_cache_mode(ctrace_cache_mode mode);
    void ctrace_enable_inlined_call_resolution(ctrace_bool enable);
```

### Signal-Safe Tracing

For more details on the signal-safe tracing interface please refer to the README and the
[signal-safe-tracing.md](signal-safe-tracing.md) guide.

```c
typedef struct ctrace_safe_object_frame ctrace_safe_object_frame;
struct ctrace_safe_object_frame {
    ctrace_frame_ptr raw_address;
    ctrace_frame_ptr relative_obj_address;
    char object_path[CTRACE_PATH_MAX + 1];
};
size_t ctrace_safe_generate_raw_trace(ctrace_frame_ptr* buffer, size_t size, size_t skip, size_t max_depth);
void ctrace_get_safe_object_frame(ctrace_frame_ptr address, ctrace_safe_object_frame* out);
ctrace_bool ctrace_can_signal_safe_unwind();
ctrace_bool ctrace_can_get_safe_object_frame();
```
