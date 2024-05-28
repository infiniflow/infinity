#!/usr/bin/env python3
#
# This script collects a list of symbols that are considered to be part of the
# CPython API. The result is used to inform the macOS linker that it's fine for
# those symbols to be undefined when an extension module is linked, as they
# will be provided when the extension module is loaded into the interpreter.

from urllib.request import urlopen
import re

funcs: "set[str]" = set()

for ver in ['3.7', '3.8', '3.9']:
  url = f'https://raw.githubusercontent.com/python/cpython/{ver}/PC/python3.def'
  output = urlopen(url).read().decode('utf-8')
  for match in re.findall(r"  (.*)=.*", output):
      funcs.add(match)

for ver in ['3.10', '3.11', 'main']:
    url = f'https://raw.githubusercontent.com/python/cpython/{ver}/PC/python3dll.c'
    output = urlopen(url).read().decode('utf-8')
    for match in re.findall(r"EXPORT_FUNC\((.*)\)", output):
        funcs.add(match)

funcs.remove('name')

# Add a few more functions that nanobind uses and which aren't in the above list
funcs |= {
    'PyFrame_GetBack',
    'PyGILState_Check',
    'PyObject_LengthHint',
    'Py_CompileStringExFlags',
    '_PyInterpreterState_Get',
    '_PyObject_MakeTpCall',
    '_PyObject_NextNotImplemented',
    '_Py_CheckFunctionResult',
    '_Py_RefTotal'
}

with open("darwin-ld-cpython.sym", "w") as f:
    for func in sorted(list(funcs)):
        f.write(f'-U _{func}\n')
