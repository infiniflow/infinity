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

module python_instance;
#if 0
#include "Python.h"
namespace infinity {

static PyThreadState *saved_state = nullptr;

void PythonInstance::Init() {
    if (!Py_IsInitialized()) {
        Py_InitializeEx(0);
    }

    PyEval_InitThreads();
    saved_state = PyEval_SaveThread();
}

void PythonInstance::UnInit() {
    PyEval_RestoreThread(saved_state);
    saved_state = nullptr;

    if (Py_IsInitialized()) {
        Py_FinalizeEx();
    }

}

}
#endif