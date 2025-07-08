module;

// #include "storage/invertedindex/search/query_node.h"
// #include "search_scanner.h"

export module search_scanner;

import infinity_core;

namespace infinity {

export class SearchScanner {
public:
    virtual ~SearchScanner() = default;
    virtual int yylex(SearchParser::semantic_type *lval, SearchParser::location_type *location) = 0;
};

// export using infinity::SearchScanner;
};
