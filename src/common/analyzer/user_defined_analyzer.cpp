// Copyright(C) 2024 InfiniFlow, Inc. All rights reserved.
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

module;

#include "Python.h"

module user_defined_analyzer;

import stl;
import term;
import stemmer;
import analyzer;
import tokenizer;
import status;
import third_party;
import defer_op;

namespace infinity {

Tuple<Vector<String>, Status> UserDefinedAnalyzer::Analyze(const String &text) {
    Vector<String> return_list;

    std::filesystem::path path = analyzer_path_;

    if (!std::filesystem::exists(path)) {
        return {return_list, Status::FailToRunPython(fmt::format("{} doesn't exist!", analyzer_path_))};
    }

    String file_dir = path.parent_path();
    String file_name = path.filename();

    PyGILState_STATE gil_state = PyGILState_Ensure();
    DeferFn defer_fn1([&]() { PyGILState_Release(gil_state); });

    // Set module directory
    PyRun_SimpleString("import sys");
    String import_str = fmt::format("sys.path.append('{}')", file_dir);
    PyRun_SimpleString(import_str.c_str());

    // Import the module
    std::filesystem::path filePath(file_name);
    String main_filename = filePath.stem().string();
    PyObject *module = PyImport_ImportModule(main_filename.c_str());
    DeferFn defer_fn2([&]() { Py_XDECREF(module); });

    if (module == nullptr) {
        return {return_list, Status::FailToRunPython(fmt::format("Fail to load python module: {}", main_filename))};
    }

    // Load function: analyze
    PyObject *function = PyObject_GetAttrString(module, "analyze");
    DeferFn defer_fn3([&]() { Py_XDECREF(function); });

    if (function == nullptr || !PyCallable_Check(function)) {
        return {return_list, Status::FailToRunPython(fmt::format("Can't to load function: analyze"))};
    }

    PyObject *args = Py_BuildValue("(s)", text.c_str());
    DeferFn defer_fn4([&]() { Py_XDECREF(args); });

    PyObject *result = PyObject_CallObject(function, args);
    DeferFn defer_fn5([&]() { Py_XDECREF(result); });

    if (result == nullptr || !PyList_Check(result)) {
        return {return_list, Status::FailToRunPython(fmt::format("Failed to use {} to parse: {}", analyzer_path_, text))};
    }

    PyObject *python_list = nullptr;
    SizeT len = PyList_GET_SIZE(result);
    return_list.reserve(len);
    for (SizeT i = 0; i < len; ++i) {
        python_list = PyList_GetItem(result, i);

        char *result_ptr{nullptr};
        PyArg_Parse(python_list, "s", &result_ptr);
        return_list.push_back(result_ptr);

//        Py_DECREF(python_list);
//        python_list = nullptr;
    }

    return {return_list, Status::OK()};
}

} // namespace infinity