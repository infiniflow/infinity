#include <nanobench.h>
#include <thirdparty/doctest/doctest.h>

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

std::string readFile(std::string const& filename) {
    std::ifstream fin{filename};
    std::ostringstream buf;
    buf << fin.rdbuf();
    return buf.str();
}

// path where the template files are
std::string tplPath() {
    // not using std::filesystem becase we have to be compatible to c++11 :-/
    auto path = std::string{__FILE__};
    auto idx = path.find("unit_templates.cpp");
    return path.substr(0, idx) + "../docs/_generated/";
}

} // namespace

TEST_CASE("unit_templates_generate" * doctest::skip()) {
    {
        std::ofstream fout{tplPath() + "mustache.template.json"};
        fout << ankerl::nanobench::templates::json();
    }

    {
        std::ofstream fout{tplPath() + "mustache.template.html"};
        fout << ankerl::nanobench::templates::htmlBoxplot();
    }

    {
        std::ofstream fout{tplPath() + "mustache.template.csv"};
        fout << ankerl::nanobench::templates::csv();
    }

    {
        std::ofstream fout{tplPath() + "mustache.template.pyperf"};
        fout << ankerl::nanobench::templates::pyperf();
    }
}

TEST_CASE("unit_templates") {
    REQUIRE(readFile(tplPath() + "mustache.template.json") ==
            std::string{ankerl::nanobench::templates::json()});

    REQUIRE(readFile(tplPath() + "mustache.template.html") ==
            std::string{ankerl::nanobench::templates::htmlBoxplot()});

    REQUIRE(readFile(tplPath() + "mustache.template.csv") ==
            std::string{ankerl::nanobench::templates::csv()});

    REQUIRE(readFile(tplPath() + "mustache.template.pyperf") ==
            std::string{ankerl::nanobench::templates::pyperf()});
}
