#include <execinfo.h>

int main() {
    void* frames[10];
    backtrace(frames, 10);
}
