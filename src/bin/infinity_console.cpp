//
// Created by JinHai on 2022/9/7.
//

#include "compilation_config.h"

#include "linenoise.h"

#include <iostream>

namespace infinity {

class Console {
public:
    Console() = default;

    bool ReadLine();
};

}

int main(int argc, char** argv) {
    std::cout << "Startup Infinity database console, version: "
              << VERSION_MAJOR << "."
              << VERSION_MINOR << "."
              << VERSION_PATCH << " built on "
              << BUILD_TIME << " from branch: "
              << GIT_BRANCH_NAME << std::endl;

    // Use multiple line mode
    linenoiseSetMultiLine(1);

    // Store the command history into the file
    linenoiseHistoryLoad("command_history.log");

    char *line = nullptr;
    while((line = linenoise("infinity> ")) != NULL) {
        /* Do something with the string. */
        std::cout << line << std::endl;
//        if (line[0] != '\0' && line[0] != '/') {
//            printf("echo: '%s'\n", line);
//            linenoiseHistoryAdd(line); /* Add to the history. */
//            linenoiseHistorySave("history.txt"); /* Save the history on disk. */
//        } else if (!strncmp(line,"/historylen",11)) {
//            /* The "/historylen" command will change the history len. */
//            int len = atoi(line+11);
//            linenoiseHistorySetMaxLen(len);
//        } else if (!strncmp(line, "/mask", 5)) {
//            linenoiseMaskModeEnable();
//        } else if (!strncmp(line, "/unmask", 7)) {
//            linenoiseMaskModeDisable();
//        } else if (line[0] == '/') {
//            printf("Unreconized command: %s\n", line);
//        }
        free(line);
    }
}