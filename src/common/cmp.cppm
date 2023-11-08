module;

#include <utility>

export module cmp;

export template <typename T1, typename T2>
struct CompareByFirst {
    using P = std::pair<T1, T2>;
    bool operator()(const P &lhs, const P &rhs) const { return lhs.first < rhs.first; }
};