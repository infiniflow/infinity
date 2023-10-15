module;

#include "cxxopts.hpp"
#include "spdlog/fmt/fmt.h"
#include "spdlog/logger.h"
//#include "toml.hpp"

export module third_party;

namespace infinity {

// spdlog

export template <typename... T>
FMT_INLINE void Printf(fmt::format_string<T...> fmt, T &&...args) {
    const auto &vargs = fmt::make_format_args(args...);
    return fmt::detail::is_utf8() ? vprint(fmt, vargs) : fmt::detail::vprint_mojibake(stdout, fmt, vargs);
}

export template <typename... T>
FMT_NODISCARD FMT_INLINE auto Format(fmt::format_string<T...> fmt, T &&...args) -> std::string {
    return vformat(fmt, fmt::make_format_args(args...));
}

export using spdlogger = spdlog::logger;
export using spdlog_level = spdlog::level::level_enum;

// cxxopts
export using CxxOptions = cxxopts::Options;

export template <typename T>
std::shared_ptr<cxxopts::Value> cxx_value() {
    return std::make_shared<cxxopts::values::standard_value<T>>();
}

export using ParseResult = cxxopts::ParseResult;

// Toml parser
//using TomlParseResult = toml::parse_result;
//
//export TomlParseResult TomlParseFile(const std::string &file_path) { return toml::parse_file(file_path); }

} // namespace infinity
