#pragma once

#include <memory>
#include <string>

namespace irs {
class filter;
} // namespace irs

namespace infinity {

class QueryParser;
class QueryScanner;

// Conducting the whole scanning and parsing of Lucene.
class QueryDriver {
public:
    QueryDriver();
    virtual ~QueryDriver();

    /**
     * parse a stream - read and parse line by line
     * @param ist - std::istream&, valid input stream
     */
    int ParseStream(std::istream &ist);

    /**
     * parse a single query
     * @param query - a single query string
     */
    int ParseSingle(const std::string &query);

    /**
     * parsing result
     */
    std::unique_ptr<irs::filter> result = nullptr;

private:
    int parse_helper(std::istream &stream);
    std::unique_ptr<QueryParser> parser = nullptr;
    std::unique_ptr<QueryScanner> scanner = nullptr;
};

} // namespace infinity
