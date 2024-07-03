# nanobind: tiny and efficient C++/Python bindings

[![Documentation](https://img.shields.io/readthedocs/nanobind/latest)](https://nanobind.readthedocs.io/en/latest/)
[![Continuous Integration](https://img.shields.io/github/actions/workflow/status/wjakob/nanobind/ci.yml?label=tests)](https://github.com/wjakob/nanobind/actions/workflows/ci.yml)
[![](https://img.shields.io/pypi/v/nanobind.svg?color=brightgreen)](https://pypi.org/pypi/nanobind/)
![](https://img.shields.io/pypi/l/nanobind.svg?color=brightgreen)
[![](https://img.shields.io/badge/Example-Link-brightgreen)](https://github.com/wjakob/nanobind_example)
[![](https://img.shields.io/badge/Changelog-Link-brightgreen)](https://nanobind.readthedocs.io/en/latest/changelog.html)

<p align="center">
    <picture>
      <source media="(prefers-color-scheme: dark)" width="800" srcset="https://rgl.s3.eu-central-1.amazonaws.com/media/uploads/wjakob/2023/03/28/nanobind_logo_dark.png">
      <source media="(prefers-color-scheme: light)" width="800" srcset="https://github.com/wjakob/nanobind/raw/master/docs/images/logo.jpg">
      <img alt="nanobind logo" width="800" src="https://github.com/wjakob/nanobind/raw/master/docs/images/logo.jpg">
    </picture>
</p>

_nanobind_ is a small binding library that exposes C++ types in Python and vice
versa. It is reminiscent of
[Boost.Python](https://www.boost.org/doc/libs/1_64_0/libs/python/doc/html) and
[pybind11](https://github.com/pybind/pybind11) and uses near-identical syntax.
In contrast to these existing tools, nanobind is more efficient: bindings
compile in a shorter amount of time, produce smaller binaries, and have better
runtime performance.

More concretely,
[benchmarks](https://nanobind.readthedocs.io/en/latest/benchmark.html) show up
to **~4× faster** compile time, **~5× smaller** binaries, and **~10× lower**
runtime overheads compared to pybind11. nanobind also outperforms Cython in
important metrics (**3-12×** binary size reduction, **1.6-4×** compilation time
reduction, similar runtime performance).

## Documentation

Please see the following links for tutorial and reference documentation in
[HTML](https://nanobind.readthedocs.io/en/latest/) and
[PDF](https://nanobind.readthedocs.io/_/downloads/en/latest/pdf/) formats.

## License and attribution

All material in this repository is licensed under a three-clause [BSD
license](LICENSE).

Please use the following BibTeX template to cite nanobind in scientific
discourse:

```bibtex
@misc{nanobind,
   author = {Wenzel Jakob},
   year = {2022},
   note = {https://github.com/wjakob/nanobind},
   title = {nanobind: tiny and efficient C++/Python bindings}
}
```

The nanobind logo was designed by [AndoTwin Studio](https://andotwinstudio.com)
(high-resolution download:
[light](https://rgl.s3.eu-central-1.amazonaws.com/media/uploads/wjakob/2023/03/27/nanobind_logo.jpg),
[dark](https://rgl.s3.eu-central-1.amazonaws.com/media/uploads/wjakob/2023/03/28/nanobind_logo_dark_1.png)).
