//
// Created by jinhai on 23-2-22.
//

#include <iostream>
#include "sql_parser.h"

class test {
public:
    test() {
        std::cout << "CTOR" << std::endl;
    }
    ~test() {
        std::cout << "DTOR" << std::endl;
    }
};

auto
main(int argc, char* argv[]) -> int {
    using namespace infinity;
    Vector<String> inputs;
    inputs.emplace_back("create collection c1;");
    inputs.emplace_back("create schema s1;");
    inputs.emplace_back("create collection if not exists c2;");
    inputs.emplace_back("create schema if not exists s2;");

    inputs.emplace_back("drop collection c1;");
    inputs.emplace_back("drop schema s1;");
    inputs.emplace_back("drop collection if exists c2;");
    inputs.emplace_back("drop schema if exists s2;");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv);");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv, header);");
    inputs.emplace_back("copy t1 to '/usr/filename' with (format csv, header, delimiter '|');");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv);");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv, header);");
    inputs.emplace_back("copy t2 from '/usr/filename' with (format csv, header, delimiter '|');");

    inputs.emplace_back("copy t2 from '/usr/filename' with (format xml);");
    inputs.emplace_back("copy t2 to '/usr/filename' with (format xml);");
    inputs.emplace_back("show tables;");
    inputs.emplace_back("describe t1;");
    inputs.emplace_back("describe s1.t1;");

    SharedPtr<SQLParser> parser = MakeShared<SQLParser>();
    SharedPtr<ParserResult> result = MakeShared<ParserResult>();

    for(const String& input: inputs) {
        parser->Parse(input, result);
        std::cout << result->ToString() << std::endl;
        result->Reset();
    }
    return 0;
}
