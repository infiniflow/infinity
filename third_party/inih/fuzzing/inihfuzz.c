/* This is a slightly tweaked copy of tests/unittest.c for fuzzing */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ini.h"

int User;
char Prev_section[50];

int dumper(void* user, const char* section, const char* name,
           const char* value)
{
    User = *((int*)user);
    if (!name || strcmp(section, Prev_section)) {
        printf("... [%s]\n", section);
        strncpy(Prev_section, section, sizeof(Prev_section));
        Prev_section[sizeof(Prev_section) - 1] = '\0';
    }
    if (!name) {
        return 1;
    }

    printf("... %s%s%s;\n", name, value ? "=" : "", value ? value : "");

    if (!value) {
        // Happens when INI_ALLOW_NO_VALUE=1 and line has no value (no '=' or ':')
        return 1;
    }

    return strcmp(name, "user")==0 && strcmp(value, "parse_error")==0 ? 0 : 1;
}

void parse(const char* fname) {
    static int u = 100;
    int e;

    *Prev_section = '\0';
    e = ini_parse(fname, dumper, &u);
    printf("%s: e=%d user=%d\n", fname, e, User);
    u++;
}

#ifndef FUZZING_BUILD_MODE_UNSAFE_FOR_PRODUCTION

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: inihfuzz file.ini\n");
        return 1;
    }
    parse(argv[1]);
    return 0;
}

#else

#define kMinInputLength 20
#define kMaxInputLength 1024

extern int LLVMFuzzerTestOneInput(const char *Data, size_t Size) {

    if (Size < kMinInputLength || Size > kMaxInputLength) {
        return 0;
    }

    int ret;
    *Prev_section = '\0';
    int u = 100;

    char *data = malloc(Size + 1);
    memcpy(data, Data, Size);
    data[Size] = '\0';

    ret = ini_parse(data, dumper, &u);

    free(data);

    return ret;
}

#endif
