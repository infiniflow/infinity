#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>

#define IS_CLANG 0
#define IS_GCC 0
#define IS_MSVC 0

#if defined(__clang__)
 #undef IS_CLANG
 #define IS_CLANG 1
#elif defined(__GNUC__) || defined(__GNUG__)
 #undef IS_GCC
 #define IS_GCC 1
#elif defined(_MSC_VER)
 #undef IS_MSVC
 #define IS_MSVC 1
#else
 #error "Unsupported compiler"
#endif

int main() {
    HANDLE proc = GetCurrentProcess();
    HANDLE thread = GetCurrentThread();
    // https://jpassing.com/2008/03/12/walking-the-stack-of-the-current-thread/

    // Get current thread context
    // GetThreadContext cannot be used on the current thread.
    // RtlCaptureContext doesn't work on i386
    CONTEXT context;
    #if defined(_M_IX86) || defined(__i386__)
    ZeroMemory(&context, sizeof(CONTEXT));
    context.ContextFlags = CONTEXT_CONTROL;
    #if IS_MSVC
    __asm {
        label:
        mov [context.Ebp], ebp;
        mov [context.Esp], esp;
        mov eax, [label];
        mov [context.Eip], eax;
    }
    #else
    asm(
        "label:\n\t"
        "mov{l %%ebp, %[cEbp] | %[cEbp], ebp};\n\t"
        "mov{l %%esp, %[cEsp] | %[cEsp], esp};\n\t"
        "mov{l $label, %%eax | eax, OFFSET label};\n\t"
        "mov{l %%eax, %[cEip] | %[cEip], eax};\n\t"
        : [cEbp] "=r" (context.Ebp),
            [cEsp] "=r" (context.Esp),
            [cEip] "=r" (context.Eip)
    );
    #endif
    #else
    RtlCaptureContext(&context);
    #endif
    // Setup current frame
    STACKFRAME64 frame;
    ZeroMemory(&frame, sizeof(STACKFRAME64));
    DWORD machine_type;
    #if defined(_M_IX86) || defined(__i386__)
    machine_type           = IMAGE_FILE_MACHINE_I386;
    frame.AddrPC.Offset    = context.Eip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context.Ebp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = context.Esp;
    frame.AddrStack.Mode   = AddrModeFlat;
    #elif defined(_M_X64) || defined(__x86_64__)
    machine_type           = IMAGE_FILE_MACHINE_AMD64;
    frame.AddrPC.Offset    = context.Rip;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context.Rsp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrStack.Offset = context.Rsp;
    frame.AddrStack.Mode   = AddrModeFlat;
    #elif defined(_M_IA64) || defined(__aarch64__)
    machine_type           = IMAGE_FILE_MACHINE_IA64;
    frame.AddrPC.Offset    = context.StIIP;
    frame.AddrPC.Mode      = AddrModeFlat;
    frame.AddrFrame.Offset = context.IntSp;
    frame.AddrFrame.Mode   = AddrModeFlat;
    frame.AddrBStore.Offset= context.RsBSP;
    frame.AddrBStore.Mode  = AddrModeFlat;
    frame.AddrStack.Offset = context.IntSp;
    frame.AddrStack.Mode   = AddrModeFlat;
    #else
    #error "Cpptrace: StackWalk64 not supported for this platform yet"
    #endif
    ZeroMemory(&context, sizeof(CONTEXT));
    StackWalk64(
        machine_type,
        proc,
        thread,
        &frame,
        machine_type == IMAGE_FILE_MACHINE_I386 ? NULL : &context,
        NULL,
        SymFunctionTableAccess64,
        SymGetModuleBase64,
        NULL
    );
}
