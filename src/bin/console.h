//
// Created by JinHai on 2022/10/1.
//

#pragma once

#include "common/types/alias/containers.h"
#include "common/types/alias/strings.h"

#include <functional>
#include <string>
#include <unordered_map>

namespace infinity {

class Console {
public:
    explicit Console();

    ~Console() = default;

    void Init();

    void UnInit();

    String HandleCommand(const char *command);

private:
    void Execute(const String &command);

    void Register(const String &name, const std::function<void(const String &)> &func);

    // Command functions
    void Exit(const String &arguments);

    void Explain(const String &arguments);

    void Visualize(const String &arguments);

    void VerifyScript(const String &arguments);

    void RunScript(const String &arguments);

    void ExecuteSQL(const String &sql_text);

private:
    HashMap<String, std::function<void(const String &)>> commands_;
};

} // namespace infinity
