#include <cxxabi.h>

int main() {
    std::type_info* t = abi::__cxa_current_exception_type();
    (void*) t;
}
