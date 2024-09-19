/* ini.h example that simply dumps an INI file without comments */

#include <stdio.h>
#include <string.h>
#include "../ini.h"

static int dumper(void* user, const char* section, const char* name,
                  const char* value)
{
    static char prev_section[50] = "";

    if (strcmp(section, prev_section)) {
        printf("%s[%s]\n", (prev_section[0] ? "\n" : ""), section);
        strncpy(prev_section, section, sizeof(prev_section));
        prev_section[sizeof(prev_section) - 1] = '\0';
    }
    printf("%s = %s\n", name, value);
    return 1;
}

int main(int argc, char* argv[])
{
    int error;

    if (argc <= 1) {
        printf("Usage: ini_dump filename.ini\n");
        return 1;
    }

    error = ini_parse(argv[1], dumper, NULL);
    if (error < 0) {
        printf("Can't read '%s'!\n", argv[1]);
        return 2;
    }
    else if (error) {
        printf("Bad config file (first error on line %d)!\n", error);
        return 3;
    }
    return 0;
}
