#pragma once
#include <string>
#include <utility>
#include <vector>
namespace infinity {

class SearchParser {
public:
    static void ParseFields(const std::string &fields_str, std::vector<std::pair<std::string, float>> &fields);
    static std::string FieldsToString(const std::vector<std::pair<std::string, float>> &fields);
    static void ParseOptions(const std::string &options_str, std::vector<std::pair<std::string, std::string>> &options);
    static std::string OptionsToString(const std::vector<std::pair<std::string, std::string>> &options);

private:
    static std::pair<std::string, float> ParseField(const std::string_view &field);
    static std::pair<std::string, std::string> ParseOption(const std::string_view &option);
};

} // namespace infinity