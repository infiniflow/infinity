// Copyright(C) 2023 InfiniFlow, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "location.hh"
#include "search_parser.h"

namespace infinity {

class SearchScanner : public yyFlexLexer {
public:
    SearchScanner(std::istream *in) : yyFlexLexer(in){};
    ~SearchScanner() override = default;

    // get rid of override virtual function warning
    using FlexLexer::yylex;

    virtual int yylex(SearchParser::semantic_type *const lval, SearchParser::location_type *location);
    // YY_DECL defined in lucene_lexer.l
    // Method body created by flex in lucene_lexer.y.cc

private:
    /* yylval ptr */
    SearchParser::semantic_type *yylval = nullptr;
};

} // namespace infinity
