.. _installing:

Installing the library
######################

The *nanobind* project is hosted at `wjakob/nanobind on GitHub
<https://github.com/wjakob/nanobind>`_. To use the library in your own
projects, it is usually easiest to install it using one of the following three
methods:

Install via Pip (recommended)
==============================

Run the following command in your terminal to install a package containing both
C++ and CMake source code needed to compile extension modules.

.. code-block:: bash

    python -m pip install nanobind

Install via Conda
=================

The following alternative installs an equivalent package through Conda. It is
provided for users that develop Conda-based extensions with a build-time
dependency on *nanobind*, in which case the PyPI package cannot be used.

.. code-block:: bash

    conda install -c conda-forge nanobind

Install as a Git submodule
==========================

If you prefer not to involve external package managers, and if your project
uses the Git control system, you may also directly reference *nanobind* as a
`Git submodule <https://git-scm.com/book/en/v2/Git-Tools-Submodules>`_. In the
main directory of your repository, run the following commands:

.. code-block:: bash

    git submodule add https://github.com/wjakob/nanobind ext/nanobind
    git submodule update --init --recursive

This assumes you are placing your dependencies in ``ext/``.

The :ref:`next section <building>` will explain how to set up a basic build
system that you can use to build your first extension module.
