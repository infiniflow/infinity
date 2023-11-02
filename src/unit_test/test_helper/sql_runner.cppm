//
// Created by jinhai on 23-3-8.
//

module;

import stl;
import data_table;

export module sql_runner;

namespace infinity {

export class SQLRunner {

public:
    static SharedPtr<DataTable> Run(const String &sql_text, bool print = true);
};

} // namespace infinity
