/* inih -- unit tests for ini_parse_string() */

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
    if (strcmp(section, Prev_section)) {
        printf("... [%s]\n", section);
        strncpy(Prev_section, section, sizeof(Prev_section));
        Prev_section[sizeof(Prev_section) - 1] = '\0';
    }
    printf("... %s=%s;\n", name, value);
    return 1;
}

void parse(const char* name, const char* string) {
    static int u = 100;
    int e;

    *Prev_section = '\0';
    e = ini_parse_string(string, dumper, &u);
    printf("%s: e=%d user=%d\n", name, e, User);
    u++;
}

int main(void)
{
    parse("empty string", "");
    parse("basic", "[section]\nfoo = bar\nbazz = buzz quxx");
    parse("crlf", "[section]\r\nhello = world\r\nforty_two = 42\r\n");
    parse("long line", "[sec]\nfoo = 01234567890123456789\nbar=4321\n");
    parse("long continued", "[sec]\nfoo = 0123456789012bix=1234\n");
    parse("error", "[s]\na=1\nb\nc=3");
    return 0;
}
