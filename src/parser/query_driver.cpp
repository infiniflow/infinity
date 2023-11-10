#include <cassert>
#include <istream>
#include <sstream>

#include "query_driver.h"
#include "query_parser.h"
#include "query_scanner.h"

namespace infinity {
QueryDriver::QueryDriver() {}
QueryDriver::~QueryDriver() {}

int QueryDriver::ParseStream(std::istream &ist) {
    // read and parse line by line, ignoring empty lines and comments
    std::string line;
    while (std::getline(ist, line)) {
        size_t firstNonBlank = line.find_first_not_of(" \t");
        if (firstNonBlank == std::string::npos || line[firstNonBlank] == '#') {
            continue;
        }
        line = line.substr(firstNonBlank);
        std::cerr << "---query: ###" << line << "###" << std::endl;
        int rc = ParseSingle(line);
        if (rc != 0) {
            std::cerr << "---failed" << std::endl;
            return rc;
        } else {
            std::cerr << "---accepted" << std::endl;
        }
    }
    return 0;
}

int QueryDriver::ParseSingle(const std::string &query) {
    std::istringstream iss(query);
    if (!iss.good() && iss.eof()) {
        return -1;
    }
    int rc = parse_helper(iss);
    return rc;
}

int QueryDriver::parse_helper(std::istream &stream) {
    try {
        scanner = std::make_unique<QueryScanner>(&stream);
        parser = std::make_unique<QueryParser>((*scanner) /* scanner */, (*this) /* driver */);
    } catch (std::bad_alloc &ba) {
        std::cerr << "Failed to allocate: (" << ba.what() << "), exiting!!\n";
        return -1;
    }

    const int accept(0);
    if (parser->parse() != accept) {
        return -1;
    }
    return 0;
}

} // namespace infinity
