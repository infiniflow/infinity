#include <dlfcn.h>

int main() {
    dl_find_object result;
    _dl_find_object(reinterpret_cast<void*>(main), &result);
}
