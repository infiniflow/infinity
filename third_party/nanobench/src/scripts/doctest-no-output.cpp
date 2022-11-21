#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

int main(int argc, char** argv) {
    doctest::Context context;
    context.setOption("out", "/dev/null");
    context.setOption("no-version", true);
    context.applyCommandLine(argc, argv);
    return context.run();
}
