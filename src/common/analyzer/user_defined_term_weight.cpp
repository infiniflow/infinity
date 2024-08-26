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
#include <vector>

module user_defined_term_weight;

import stl;
import term;
import stemmer;
import analyzer;
import tokenizer;
import status;
import third_party;
import defer_op;

namespace infinity {

Status UserDefinedTermWeight::Init() {
    return Status::OK();
}

void UserDefinedTermWeight::UnInit() {
}

Tuple<HashMap<String, double>, Status> UserDefinedTermWeight::Run(const Vector<String> &text_array) {
    HashMap<String, double> return_map;

    std::filesystem::path path = tw_path_;

    if (!std::filesystem::exists(path)) {
        return {return_map, Status::FailToRunPython(fmt::format("{} doesn't exist!", tw_path_))};
    }

    String file_dir = path.parent_path();
    String file_name = path.filename();

//    Py_BEGIN_ALLOW_THREADS
//    auto check = PyGILState_Check();
//    PyGILState_STATE gil_state{};
//    if(!check) {
    PyGILState_STATE gil_state = PyGILState_Ensure();
    DeferFn defer_fn([&]() { PyGILState_Release(gil_state); });
//    }


    // Set module directory
    PyRun_SimpleString("import sys");
    String import_str = fmt::format("sys.path.append('{}')", file_dir);
    PyRun_SimpleString(import_str.c_str());

    // Import the module
    std::filesystem::path filePath(file_name);
    String main_filename = filePath.stem().string();
    module_ = PyImport_ImportModule(main_filename.c_str());
    if (module_ == nullptr) {
        return {return_map, Status::FailToRunPython(fmt::format("Fail to load python module: {}", main_filename))};
    }

    // Load function: analyze
    function_ = PyObject_GetAttrString(module_, "term_weight");
    if (function_ == nullptr || !PyCallable_Check(function_)) {
        return {return_map, Status::FailToRunPython(fmt::format("Can't to load function: analyze"))};
    }

    PyObject* pyParams = PyList_New(0);
    for(const String& text: text_array) {
        PyList_Append(pyParams, Py_BuildValue("s", text.c_str()));
    }

    PyObject* args = PyTuple_Pack(1, pyParams);
    PyObject* dict_result = PyObject_CallObject(function_, args);

    if (dict_result == nullptr || !PyDict_Check(dict_result)) {
        return {return_map, Status::FailToRunPython(fmt::format("Failed to use {} to parse text array", tw_path_))};
    }

    PyObject* pItems = PyDict_Items(dict_result);
    for (Py_ssize_t i = 0; i < PyList_Size(pItems); ++i) {
        PyObject* pItem = PyList_GetItem(pItems, i);
        PyObject* pKey = PyTuple_GetItem(pItem, 0);
        PyObject* pValue = PyTuple_GetItem(pItem, 1);

        return_map.emplace(PyUnicode_AsUTF8(pKey), PyFloat_AsDouble(pValue));
    }
    Py_XDECREF(pItems);
    Py_XDECREF(dict_result);
    Py_XDECREF(args);
    Py_XDECREF(pyParams);
    Py_XDECREF(function_);
    Py_XDECREF(module_);

//    Py_END_ALLOW_THREADS

    return {return_map, Status::OK()};
}

} // namespace infinity