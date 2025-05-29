#include "utils/string_view.hpp"

#include "utils/error.hpp"
#include "utils/microfmt.hpp"

#include <algorithm>
#include <cstring>

namespace cpptrace {
namespace detail {
    char string_view::operator[](size_t i) const {
        ASSERT(i < size());
        return ptr[i];
    }
    char string_view::at(size_t i) const {
        if(i >= size()) {
            throw std::runtime_error(microfmt::format("Out of bounds access {} >= {}", i, size()));
        }
        return ptr[i];
    }

    std::size_t string_view::find_last_of(string_view chars) const {
        if(empty() || chars.empty()) {
            return npos;
        }
        std::size_t pos = size();
        while(pos-- > 0) {
            if(std::find(chars.begin(), chars.end(), ptr[pos]) != chars.end()) {
                return pos;
            }
        }
        return npos;
    }

    bool operator==(string_view a, string_view b) {
        return a.size() == b.size() && std::memcmp(a.data(), b.data(), a.size()) == 0;
    }

    cstring_view cstring_view::substr(std::size_t pos) const {
        ASSERT(pos <= count);
        return {ptr + pos, count - pos};
    }

    void cstring_view::check_null() const {
        ASSERT(ptr[count] == 0);
    }
}
}
