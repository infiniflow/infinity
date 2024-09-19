/* Parse a configuration file into a struct using X-Macros */

#include <stdio.h>
#include <string.h>
#include "../ini.h"

/* define the config struct type */
typedef struct {
    #define CFG(s, n, default) char *s##_##n;
    #include "config.def"
} config;

/* create one and fill in its default values */
config Config = {
    #define CFG(s, n, default) default,
    #include "config.def"
};

/* process a line of the INI file, storing valid values into config struct */
int handler(void *user, const char *section, const char *name,
            const char *value)
{
    config *cfg = (config *)user;

    if (0) ;
    #define CFG(s, n, default) else if (strcmp(section, #s)==0 && \
        strcmp(name, #n)==0) cfg->s##_##n = strdup(value);
    #include "config.def"

    return 1;
}

/* print all the variables in the config, one per line */
void dump_config(config *cfg)
{
    #define CFG(s, n, default) printf("%s_%s = %s\n", #s, #n, cfg->s##_##n);
    #include "config.def"
}

int main(int argc, char* argv[])
{
    if (ini_parse("test.ini", handler, &Config) < 0)
        printf("Can't load 'test.ini', using defaults\n");
    dump_config(&Config);
    return 0;
}
