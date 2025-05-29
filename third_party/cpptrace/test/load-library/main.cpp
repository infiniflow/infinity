#include "cpptrace/basic.hpp"
#include "cpptrace/utils.hpp"
#include "mydll.hpp"

#include <windows.h>

#include <iostream>

#include <cpptrace/from_current.hpp>

int main() {
    // generate a trace before LoadLibraryA to initialize dbghelp
    cpptrace::generate_trace().print();

    HMODULE lib = LoadLibraryA("mydll.dll");
    if (!lib) {
        std::cerr << "Failed to load DLL\n";
        return 1;
    }

    auto foo = reinterpret_cast<decltype(::foo)*>(GetProcAddress(lib, "foo"));
    if (!foo) {
        std::cerr << "Failed to get symbol\n";
        return 1;
    }

    cpptrace::load_symbols_for_file("mydll.dll");

    CPPTRACE_TRY {
        foo();
    } CPPTRACE_CATCH(...) {
        cpptrace::from_current_exception().print();
    }

    FreeLibrary(lib);
}
