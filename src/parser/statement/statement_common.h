//
// Created by jinhai on 23-2-24.
//

#pragma once

#include <memory>
#include <string>

namespace infinity {

enum class CopyFileType {
    kCSV,
    kJSON,
    kFVECS,
};

inline std::shared_ptr<std::string> copy_file_to_str(CopyFileType copy_file_type) {
    switch (copy_file_type) {
        case CopyFileType::kCSV:
            return std::make_shared<std::string>("CSV");
        case CopyFileType::kJSON:
            return std::make_shared<std::string>("JSON");
        case CopyFileType::kFVECS:
            return std::make_shared<std::string>("FVECS");
    }
}

struct TableName {
    char *schema_name_ptr_{nullptr};
    char *table_name_ptr_{nullptr};
};

struct InitParameter {
    std::string para_name_{};
    std::string para_value_{};
};

struct IfExistsInfo {
    bool exists_{false};
    bool if_exists_{false};
    std::string info_{""};
};

struct IfNotExistsInfo {
    bool exists_{false};
    bool if_not_exists_{false};
    std::string info_{""};
};
} // namespace infinity
