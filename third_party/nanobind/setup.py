from setuptools import setup
import re
import os
import shutil


VERSION_REGEX = re.compile(
    r"^\s*#\s*define\s+NB_VERSION_([A-Z]+)\s+(.*)$", re.MULTILINE)

this_directory = os.path.abspath(os.path.dirname(__file__))

with open(os.path.join(this_directory, "include/nanobind/nanobind.h")) as f:
    matches = dict(VERSION_REGEX.findall(f.read()))
    nanobind_version = "{MAJOR}.{MINOR}.{PATCH}".format(**matches)

long_description = '''
![nanobind logo](
https://github.com/wjakob/nanobind/raw/master/docs/images/logo.jpg?raw=True)

_nanobind_ is a small binding library that exposes C++ types in Python and
vice versa. It is reminiscent of
[Boost.Python](https://www.boost.org/doc/libs/1_64_0/libs/python/doc/html)
and [pybind11](http://github.com/pybind/pybind11) and uses near-identical
syntax. In contrast to these existing tools, nanobind is more efficient:
bindings compile in a shorter amount of time, produce smaller binaries, and
have better runtime performance.

More concretely,
[benchmarks](https://nanobind.readthedocs.io/en/latest/benchmark.html) show up
to **~4× faster** compile time, **~5× smaller** binaries, and **~10× lower**
runtime overheads compared to pybind11. nanobind also outperforms Cython in
important metrics (**3-12×** binary size reduction, **1.6-4×** compilation time
reduction, similar runtime performance).

Please see the following links for tutorial and reference documentation in
[HTML](https://nanobind.readthedocs.io/en/latest/) and
[PDF](https://nanobind.readthedocs.io/_/downloads/en/latest/pdf/) formats.
'''

from tempfile import TemporaryDirectory

with TemporaryDirectory() as temp_dir:
    base_dir = os.path.abspath(os.path.dirname(__file__))


    for name in ['include', 'ext', 'cmake', 'src']:
        shutil.copytree(os.path.join(base_dir, name),
                        os.path.join(temp_dir, name),
                        dirs_exist_ok=True)

    for fname in ['__init__.py', '__main__.py', 'stubgen.py']:
        shutil.move(os.path.join(temp_dir, 'src', fname),
                    os.path.join(temp_dir, fname))

    setup(
        name="nanobind",
        version=nanobind_version,
        author="Wenzel Jakob",
        author_email="wenzel.jakob@epfl.ch",
        description='nanobind: tiny and efficient C++/Python bindings',
        url="https://github.com/wjakob/nanobind",
        license="BSD",
        long_description=long_description,
        long_description_content_type='text/markdown',
        packages=['nanobind'],
        zip_safe=False,
        package_dir={'nanobind': temp_dir},
        package_data={'nanobind': [
            'include/nanobind/*.h',
            'include/nanobind/stl/*.h',
            'include/nanobind/stl/detail/*.h',
            'include/nanobind/eigen/*.h',
            'include/nanobind/intrusive/*.h',
            'include/nanobind/intrusive/*.inl',
            'ext/robin_map/include/tsl/robin_map.h',
            'ext/robin_map/include/tsl/robin_hash.h',
            'ext/robin_map/include/tsl/robin_growth_policy.h',
            'cmake/nanobind-config.cmake',
            'cmake/darwin-ld-cpython.sym',
            'cmake/darwin-ld-pypy.sym',
            'src/*.h',
            'src/*.cpp'
        ]}
    )
