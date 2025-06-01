#include "utils/error.hpp"

namespace cpptrace {
namespace detail {
    internal_error::internal_error(std::string message) : msg("Cpptrace internal error: " + std::move(message)) {}

    constexpr const char* assert_actions[] = {"assertion", "verification", "panic"};
    constexpr const char* assert_names[] = {"ASSERT", "VERIFY", "PANIC"};

    void assert_fail(
        assert_type type,
        const char* expression,
        const char* signature,
        source_location location,
        const char* message
    ) {
        const char* action = assert_actions[static_cast<std::underlying_type<assert_type>::type>(type)];
        const char* name   = assert_names[static_cast<std::underlying_type<assert_type>::type>(type)];
        if(message == nullptr) {
            throw internal_error(
                "Cpptrace {} failed at {}:{}: {}\n"
                "    {}({});\n",
                action, location.file, location.line, signature,
                name, expression
            );
        } else {
            throw internal_error(
                "Cpptrace {} failed at {}:{}: {}: {}\n"
                "    {}({});\n",
                action, location.file, location.line, signature, message,
                name, expression
            );
        }
    }

    void panic(
        const char* signature,
        source_location location,
        string_view message
    ) {
        if(message == "") {
            throw internal_error(
                "Cpptrace panic {}:{}: {}\n",
                location.file, location.line, signature
            );
        } else {
            throw internal_error(
                "Cpptrace panic {}:{}: {}: {}\n",
                location.file, location.line, signature, message
            );
        }
    }
}
}
