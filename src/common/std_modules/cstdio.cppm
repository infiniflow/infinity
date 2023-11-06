module;
#include <cstdio>
# 3 __FILE__ 1 3 // Enter "faked" system files since std is reserved module name
export module std:cstdio;
export namespace std {
    using std::fopen;
    using std::fclose;
    using std::fgetc;
    using std::FILE;

    using std::fprintf;
    using std::fflush;
}

export {
    using ::stdin;
    using ::stdout;
    using ::stderr;
}
