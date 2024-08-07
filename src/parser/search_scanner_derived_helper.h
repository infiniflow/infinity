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

#ifndef SearchScannerSuffix
#error "SearchScannerSuffix is not defined"
#endif
#define CONCAT_INNER(A, B) A##B
#define CONCAT(A, B) CONCAT_INNER(A, B)
#define SearchScannerDerived CONCAT(SearchScanner, SearchScannerSuffix)
#define yyFlexLexer CONCAT(SearchScannerDerived, FlexLexer)
#include <FlexLexer.h>
#include "search_scanner_derived.h"
#undef yyFlexLexer
#undef SearchScannerDerived
#undef CONCAT
#undef CONCAT_INNER
#undef SearchScannerSuffix
