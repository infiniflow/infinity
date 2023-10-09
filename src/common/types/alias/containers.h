//
// Created by jinhai on 23-9-30.
//

#pragma once

#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace infinity {

template <typename T1, typename T2>
using Pair = std::pair<T1, T2>;

template <typename T, std::size_t N>
using Array = std::array<T, N>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
using List = std::list<T>;

template <typename S, typename T>
using Map = std::map<S, T>;

template <typename T>
using Set = std::set<T>;

template <typename S, typename T>
using HashMap = std::unordered_map<S, T>;

template <typename S>
using HashSet = std::unordered_set<S>;

} // namespace infinity
