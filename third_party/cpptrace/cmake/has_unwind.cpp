#include <cstdint>

#include <unwind.h>

_Unwind_Reason_Code unwind_callback(_Unwind_Context* context, void* arg) {
    _Unwind_GetIP(context);
    int is_before_instruction = 0;
    uintptr_t ip = _Unwind_GetIPInfo(context, &is_before_instruction);
    return _URC_END_OF_STACK;
}

int main() {
    _Unwind_Backtrace(unwind_callback, nullptr);
}
