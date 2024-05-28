Introduction
============

.. only:: not latex

   .. image:: https://github.com/wjakob/nanobind/raw/master/docs/images/logo.jpg
     :width: 800
     :align: center
     :alt: nanobind logo
     :class: only-light
   
   .. image:: https://rgl.s3.eu-central-1.amazonaws.com/media/uploads/wjakob/2023/03/28/nanobind_logo_dark.png
     :width: 800
     :align: center
     :alt: nanobind logo
     :class: only-dark

.. only:: latex

   .. image:: https://github.com/wjakob/nanobind/raw/master/docs/images/logo.jpg
     :width: 800
     :align: center
     :alt: nanobind logo
   
*nanobind* is a small binding library that exposes C++ types in Python and vice
versa. It is reminiscent of `Boost.Python
<https://www.boost.org/doc/libs/1_64_0/libs/python/doc/html>`_ and `pybind11
<http://github.com/pybind/pybind11>`_ and uses near-identical syntax. In
contrast to these existing tools, nanobind is *more efficient*: bindings
compile in a shorter amount of time, produce smaller binaries, and have better
runtime performance.

More concretely, :ref:`benchmarks <benchmarks>` show up to **~4× faster**
compile time, **~5× smaller** binaries, and **~10× lower** runtime overheads
compared to pybind11. nanobind also outperforms Cython in important metrics
(**3-12×** binary size reduction, **1.6-4×** compilation time reduction,
similar runtime performance).

.. only:: not latex

   Documentation formats
   ---------------------

   You are reading the HTML version of the documentation. An alternative `PDF
   version <https://nanobind.readthedocs.io/_/downloads/en/latest/pdf/>`__ is
   also available.

   Dependencies
   ------------

.. only:: latex

   .. rubric:: Dependencies

nanobinds depends on

- **Python 3.8+** or **PyPy 7.3.10+** (the *3.8* and *3.9* PyPy flavors are
  supported, though there are :ref:`some limitations <pypy_issues>`).
- **CMake 3.15+**.
- **A C++17 compiler**: Clang 7+, GCC 8+, and MSVC2019+ are officially
  supported. Others (MinGW, Intel, NVIDIA, ..) may work as well but will not
  receive support.

.. only:: not latex

   How to cite this project?
   -------------------------

.. only:: latex

   .. rubric:: How to cite this project?

Please use the following BibTeX template to cite nanobind in scientific
discourse:

.. code-block:: bibtex

    @misc{nanobind,
       author = {Wenzel Jakob},
       year = {2022},
       note = {https://github.com/wjakob/nanobind},
       title = {nanobind: tiny and efficient C++/Python bindings}
    }


The nanobind logo was designed by `AndoTwin Studio
<https://andotwinstudio.com>`__. High-resolution version are available `here
<https://rgl.s3.eu-central-1.amazonaws.com/media/uploads/wjakob/2023/03/27/nanobind_logo.jpg>`__
(light) and `here
<https://rgl.s3.eu-central-1.amazonaws.com/media/uploads/wjakob/2023/03/28/nanobind_logo_dark_1.png>`__
(dark).

.. only:: not latex

   Table of contents
   -----------------

.. toctree::
   :maxdepth: 1

   changelog
   why
   benchmark
   porting
   faq

.. toctree::
   :caption: Basics
   :maxdepth: 1

   installing
   building
   basics

.. toctree::
   :caption: Intermediate
   :maxdepth: 1

   exchanging
   ownership
   functions
   classes
   exceptions
   ndarray_index
   packaging
   typing
   utilities

.. toctree::
   :caption: Advanced
   :maxdepth: 1

   ownership_adv
   lowlevel
   typeslots

.. toctree::
   :caption: API Reference
   :maxdepth: 1

   api_core
   api_extra
   api_cmake
