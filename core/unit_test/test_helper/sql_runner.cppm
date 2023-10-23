//
// Created by jinhai on 23-3-8.
//

module;

import stl;
import table;

export module sql_runner;

namespace infinity {

export class SQLRunner {

public:
    static SharedPtr<Table> Run(const String &sql_text, bool print = true);
};

} // namespace infinity
