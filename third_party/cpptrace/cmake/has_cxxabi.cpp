#include <cxxabi.h>

int main() {
    int status;
    abi::__cxa_demangle("_Z3foov", nullptr, nullptr, &status);
}
