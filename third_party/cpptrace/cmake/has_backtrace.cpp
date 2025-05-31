#ifdef CPPTRACE_BACKTRACE_PATH
#include CPPTRACE_BACKTRACE_PATH
#else
#include <backtrace.h>
#endif

int main() {
    backtrace_state* state = backtrace_create_state(nullptr, true, nullptr, nullptr);
}
