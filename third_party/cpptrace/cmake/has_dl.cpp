#include <dlfcn.h>

int main() {
    Dl_info info;
    dladdr(nullptr, &info);
}
