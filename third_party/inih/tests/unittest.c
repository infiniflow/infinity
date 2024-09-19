/* inih -- unit tests

This works simply by dumping a bunch of info to standard output, which is
redirected to an output file (baseline_*.txt) and checked into the Subversion
repository. This baseline file is the test output, so the idea is to check it
once, and if it changes -- look at the diff and see which tests failed.

See unittest.bat and unittest.sh for how to run this (with tcc and gcc,
respectively).

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ini.h"

int User;
char Prev_section[50];

#if INI_HANDLER_LINENO
int dumper(void* user, const char* section, const char* name,
           const char* value, int lineno)
#else
int dumper(void* user, const char* section, const char* name,
           const char* value)
#endif
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

#if INI_HANDLER_LINENO
    printf("... %s%s%s;  line %d\n", name, value ? "=" : "", value ? value : "", lineno);
#else
    printf("... %s%s%s;\n", name, value ? "=" : "", value ? value : "");
#endif

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

int main(void)
{
    parse("no_file.ini");
    parse("normal.ini");
    parse("bad_section.ini");
    parse("bad_comment.ini");
    parse("user_error.ini");
    parse("multi_line.ini");
    parse("bad_multi.ini");
    parse("bom.ini");
    parse("duplicate_sections.ini");
    parse("no_value.ini");
    return 0;
}
