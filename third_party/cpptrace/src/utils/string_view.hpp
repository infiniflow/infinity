#ifndef STRING_VIEW_HPP
#define STRING_VIEW_HPP

#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

#include <cpptrace/utils.hpp>

namespace cpptrace {
namespace detail {
    // Simple string view implementations
    // I haven't implemented all members because I don't need most of them currently, more may be added as needed
    // members exported for tests

    class string_view {
        const char* ptr;
        std::size_t count;

    public:
        using traits_type = std::char_traits<char>;
        using value_type = char;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = char*;
        using const_pointer = const char*;
        using reference = char&;
        using const_reference = const char&;
        using iterator = char*;
        using const_iterator = const char*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<reverse_iterator>;
        static constexpr std::size_t npos = std::string::npos;

        string_view() : ptr(nullptr), count(0) {}
        string_view(const char* str) : ptr(str), count(std::strlen(str)) {}
        string_view(const std::string& str) : ptr(str.c_str()), count(str.size()) {}
        string_view(const char* ptr, std::size_t count) : ptr(ptr), count(count) {}

        explicit operator std::string() {
            return std::string(ptr, ptr + count);
        }

        const char* data() const noexcept {
            return ptr;
        }
        std::size_t size() const noexcept {
            return count;
        }
        bool empty() const noexcept {
            return count == 0;
        }

        CPPTRACE_EXPORT char operator[](size_t i) const;
        CPPTRACE_EXPORT char at(size_t i) const;

        CPPTRACE_EXPORT std::size_t find_last_of(string_view chars) const;

        const_iterator begin() const noexcept {
            return ptr;
        }
        const_iterator end() const noexcept {
            return ptr + count;
        }
    };

    bool operator==(string_view, string_view);

    class cstring_view {
        const char* ptr;
        std::size_t count;

    public:
        using traits_type = std::char_traits<char>;
        using value_type = char;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = char*;
        using const_pointer = const char*;
        using reference = char&;
        using const_reference = const char&;
        using iterator = char*;
        using const_iterator = const char*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<reverse_iterator>;
        static constexpr std::size_t npos = string_view::npos;

        cstring_view() : ptr(nullptr), count(0) {}
        cstring_view(const char* str) : ptr(str), count(std::strlen(str)) {}
        cstring_view(const std::string& str) : ptr(str.c_str()), count(str.size()) {}
        cstring_view(const char* ptr, std::size_t count) : ptr(ptr), count(count) {
            check_null();
        }

        explicit operator std::string() {
            return std::string(ptr, ptr + count);
        }

        operator string_view() const noexcept {
            return string_view(ptr, count);
        }

        const char* data() const noexcept {
            return ptr;
        }
        const char* c_str() const noexcept {
            return ptr;
        }
        std::size_t size() const noexcept {
            return count;
        }
        bool empty() const noexcept {
            return count == 0;
        }

        char operator[](size_t i) const {
            return operator string_view().operator[](i);
        }
        char at(size_t i) const {
            return operator string_view().at(i);
        }

        std::size_t find_last_of(string_view chars) const {
            return operator string_view().find_last_of(chars);
        }

        CPPTRACE_EXPORT cstring_view substr(std::size_t pos) const;

        const_iterator begin() const noexcept {
            return ptr;
        }
        const_iterator end() const noexcept {
            return ptr + count;
        }
    private:
        CPPTRACE_EXPORT void check_null() const;
    };
}
}

#endif
