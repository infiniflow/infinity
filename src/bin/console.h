//
// Created by JinHai on 2022/10/1.
//

#pragma once

#include <unordered_map>
#include <string>
#include <functional>

namespace infinity {

class Console {
public:
    explicit
    Console();

    void
    Init();

    void
    UnInit();

    std::string
    HandleCommand(const char *command);

private:
    void
    Execute(const std::string &command);

    void
    Register(const std::string &name, const std::function<void(const std::string &)> &func);

    // Command functions
    void
    Exit(const std::string &arguments);

    void
    Explain(const std::string &arguments);

    void
    Visualize(const std::string &arguments);

    void
    VerifyScript(const std::string &arguments);

    void
    RunScript(const std::string &arguments);

    void
    ExecuteSQL(const std::string &sql_text);

private:

    std::unordered_map<std::string, std::function<void(const std::string &)>> commands_;
};

}

