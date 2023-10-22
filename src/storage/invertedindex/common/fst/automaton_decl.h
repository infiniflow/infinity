////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2019 ArangoDB GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Andrey Abramov
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <cstddef>

namespace fst {

template <class Arc, class Allocator>
class VectorState;

template <class Arc, class State>
class VectorFst;

template <typename F, size_t CacheSize, bool MatchInput, bool ByteLabel>
class TableMatcher;

class BooleanWeight;

struct RangeLabel;

template <typename W>
struct Transition;

template <typename W>
using AutomatonState = VectorState<Transition<W>, std::allocator<Transition<W>>>;

template <typename W>
using Automaton = VectorFst<Transition<W>, AutomatonState<W>>;

} // namespace fst

namespace infinity {
using Automaton = fst::Automaton<fst::BooleanWeight>;
using RangeLabel = fst::RangeLabel;
using AutomatonTableMatcher = fst::TableMatcher<Automaton, 256, true, true>;

} // namespace infinity