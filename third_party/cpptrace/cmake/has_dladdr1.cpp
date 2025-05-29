#include <dlfcn.h>
#include <link.h>

int main() {
    Dl_info info;
    link_map* link_map_info;
    dladdr1(reinterpret_cast<void*>(&main), &info, reinterpret_cast<void**>(&link_map_info), RTLD_DL_LINKMAP);
}
