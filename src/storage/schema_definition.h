//
// Created by JinHai on 2022/7/24.
//

#pragma once

#include <string>

class SchemaDefinition {
public:
    SchemaDefinition(std::string schema_name, bool if_exists)
        : name_(std::move(schema_name)), if_exists_(if_exists) {}
    [[nodiscard]] const std::string& name() const { return name_; }
    [[nodiscard]] bool if_exists() const { return if_exists_; }
private:
    std::string name_;
    bool if_exists_ = false;
};

