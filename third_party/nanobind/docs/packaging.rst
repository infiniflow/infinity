.. _packaging:

Packaging
=========

A Python *wheel* is a self-contained binary file that bundles Python code and
extension libraries along with metadata such as versioned package dependencies.
Wheels are easy to download and install, and they are the recommended mechanism
for distributing extensions created using nanobind.

This section walks through the recommended sequence of steps to build wheels
and optionally automate this process to simultaneously target many platforms
(Linux, Windows, macOS) and processors (i386, x86_64, arm64) using the `GitHub
Actions <https://github.com/features/actions>`__ CI service.

Note that all of the recommended practices have already been implemented in the
`nanobind_example repository <https://github.com/wjakob/nanobind_example>`_,
which is a minimal C++ project with nanobind-based bindings. You may therefore
prefer to clone this repository and modify its contents.

Step 1: Overview
----------------

The example project has a simple directory structure:

.. code-block:: text

   ├── README.md
   ├── CMakeLists.txt
   ├── pyproject.toml
   └── src/
       ├── my_ext.cpp
       └── my_ext/
           └── __init__.py

The file ``CMakeLists.txt`` contains the C++-specifics part of the build
system, while ``pyproject.toml`` explains how to turn the example into a wheel.
The file ``README.md`` should ideally explain how to use the project in more
detail. Its contents are arbitrary, but the file must exist for the following
build system to work.

All source code is located in a ``src`` directory containing the Python package
as a subdirectory.

Compilation will turn ``my_ext.cpp`` into a shared library in the package
directory, which has an underscored platform-dependent name (e.g.,
``_my_ext_impl.cpython-311-darwin.so``) to indicate that it is an
implementation detail. The ``src/my_ext/__init__.py`` imports the extension and
exposes relevant functionality. In this small example project, it only contains
a single line:

.. code-block:: python

   from ._my_ext_impl import hello

The file ``src/my_ext.cpp`` contains minimal bindings for an example function:

.. code-block:: cpp

   #include <nanobind/nanobind.h>

   NB_MODULE(_my_ext_impl, m) {
       m.def("hello", []() { return "Hello world!"; });
   }

The next two steps will set up the infrastructure needed for wheel generation.

Step 2: Specify build dependencies and metadata
-----------------------------------------------

In the root directory of the project, create a file named ``pyproject.toml``
listing *build-time dependencies*. Note that runtime dependencies *do not* need
to be added here. The following core dependencies are required by nanobind:

.. code-block:: toml

   [build-system]
   requires = ["scikit-build-core >=0.4.3", "nanobind >=1.3.2"]
   build-backend = "scikit_build_core.build"

You may need to increase the minimum nanobind version in the above snippet if
you are using features from versions newer than 1.3.2.

Just below the list of build-time requirements, specify project metadata including:

- The project's name (which must be a valid package name)
- The version number
- A brief (1-line) description of the project
- The name of a more detailed README file
- The list of authors with email addresses.
- The software license
- The project web page
- Runtime dependencies, if applicable

An example is shown below:

.. code-block:: toml

   [project]
   name = "my_ext"
   version = "0.0.1"
   description = "A brief description of what this project does"
   readme = "README.md"
   requires-python = ">=3.8"
   authors = [
       { name = "Your Name", email = "your.email@address.com" },
   ]
   classifiers = [
       "License :: BSD",
   ]
   # Optional: runtime dependency specification
   # dependencies = [ "cryptography >=41.0" ]

   [project.urls]
   Homepage = "https://github.com/your/project"

We will use `scikit-build-core
<https://github.com/scikit-build/scikit-build-core>`__ to build wheels, and
this tool also has its own configuration block in ``pyproject.toml``. The
following defaults are recommended:

.. code-block:: toml

   [tool.scikit-build]
   # Protect the configuration against future changes in scikit-build-core
   minimum-version = "0.4"

   # Setuptools-style build caching in a local directory
   build-dir = "build/{wheel_tag}"

   # Build stable ABI wheels for CPython 3.12+
   wheel.py-api = "cp312"

Step 3: Set up a CMake build system
-----------------------------------

Next, we will set up a suitable ``CMakeLists.txt`` file in the root directory.
Since this build system is designed to be invoked through
``scikit-build-core``, it does not make sense to perform a standalone CMake
build. The message at the top warns users attempting to do this.

.. code-block:: cmake

   # Set the minimum CMake version and policies for highest tested version
   cmake_minimum_required(VERSION 3.15...3.27)

   # Set up the project and ensure there is a working C++ compiler
   project(my_ext LANGUAGES CXX)

   # Warn if the user invokes CMake directly
   if (NOT SKBUILD)
     message(WARNING "\
     This CMake file is meant to be executed using 'scikit-build-core'.
     Running it directly will almost certainly not produce the desired
     result. If you are a user trying to install this package, use the
     command below, which will install all necessary build dependencies,
     compile the package in an isolated environment, and then install it.
     =====================================================================
      $ pip install .
     =====================================================================
     If you are a software developer, and this is your own package, then
     it is usually much more efficient to install the build dependencies
     in your environment once and use the following command that avoids
     a costly creation of a new virtual environment at every compilation:
     =====================================================================
      $ pip install nanobind scikit-build-core[pyproject]
      $ pip install --no-build-isolation -ve .
     =====================================================================
     You may optionally add -Ceditable.rebuild=true to auto-rebuild when
     the package is imported. Otherwise, you need to rerun the above
     after editing C++ files.")
   endif()

Next, import Python and nanobind including the ``Development.SABIModule``
component that can be used to create `stable ABI
<https://docs.python.org/3/c-api/stable.html>`__ builds.

.. code-block:: cmake

   # Try to import all Python components potentially needed by nanobind
   find_package(Python 3.8
     REQUIRED COMPONENTS Interpreter Development.Module
     OPTIONAL_COMPONENTS Development.SABIModule)

   # Import nanobind through CMake's find_package mechanism
   find_package(nanobind CONFIG REQUIRED)

The last two steps build and install the actual extension

.. code-block:: cmake

    # We are now ready to compile the actual extension module
    nanobind_add_module(
      # Name of the extension
      _my_ext_impl

      # Target the stable ABI for Python 3.12+, which reduces
      # the number of binary wheels that must be built. This
      # does nothing on older Python versions
      STABLE_ABI

      # Source code goes here
      src/my_ext.cpp
    )

    # Install directive for scikit-build-core
    install(TARGETS _my_ext_impl LIBRARY DESTINATION my_ext)


Step 4: Install the package locally
-----------------------------------

To install the package, run

.. code-block:: bash

   $ cd <project-directory>
   $ pip install .

``pip`` will parse the ``pyproject.toml`` file and create a fresh environment
containing all needed dependencies. Following this, you should be able to
install and access the extension.

.. code-block:: python

   >>> import my_ext
   >>> my_ext.hello()
   'Hello world!'

Alternatively, you can use the following command to generate a ``.whl`` file
instead of installing the package.

.. code-block:: bash

   $ pip wheel .

Step 5: Incremental rebuilds
----------------------------

The ``pip install`` and ``pip wheel`` commands are extremely conservative to
ensure reproducible builds. They create a pristine virtual environment and
install build-time dependencies before compiling the extension *from scratch*.

It can be frustrating to wait for this lengthy sequence of steps after every
small change to a source file during the active development phase of a project.
To avoid this, first install the project's build dependencies, e.g.:

.. code-block:: bash

   $ pip install nanobind scikit-build-core[pyproject]

Next, install the project without build isolation to enable incremental builds:

.. code-block:: bash

   $ pip install --no-build-isolation -ve .

This command will need to be run after every change to reinstall the updated package.
For an even more interactive experience, use

.. code-block:: bash

   $ pip install --no-build-isolation -Ceditable.rebuild=true -ve .

This will automatically rebuild any code (if needed) whenever the ``my_ext``
package is imported into a Python session.

Step 6: Build wheels in the cloud
---------------------------------

On my machine, the ``pip wheel`` command produces a file named
``my_ext-0.0.1-cp311-cp311-macosx_13_0_arm64.whl`` that is specific to Python
3.11 running on an arm64 macOS machine. Other Python versions and operating
systems each require their own wheels, which leads to a dauntingly large build
matrix (though nanobind's stable ABI support will help to significantly reduce
the size of this matrix once Python 3.12 is more widespread). 

Rather than building these wheels manually on different machines, it is far
more efficient to use GitHub actions along with the powerful `cibuildwheel
<https://cibuildwheel.readthedocs.io/en/stable/>`__ package to fully automate
the process.

To do so, create a file named ``.github/workflows/wheels.yml`` containing
the contents of the `following file
<https://github.com/wjakob/nanobind_example/blob/master/.github/workflows/wheels.yml>`__.
You may want to remove the ``on: push:`` lines, otherwise, the action will run
after every commit, which is perhaps a bit excessive. In this case, you can
still trigger the action manually on the *Actions* tab of the GitHub project
page.

Furthermore, append the following ``cibuildwheel``-specific configuration to
``pyproject.toml``:

.. code-block:: toml

    [tool.cibuildwheel]
    # Necessary to see build output from the actual compilation
    build-verbosity = 1

    # Optional: run pytest to ensure that the package was correctly built
    # test-command = "pytest {project}/tests"
    # test-requires = "pytest"

    # Needed for full C++17 support on macOS
    [tool.cibuildwheel.macos.environment]
    MACOSX_DEPLOYMENT_TARGET = "10.14"

Following each run, the action provides a downloadable *build artifact*, which
is a ZIP file containing all the individual wheel files for each platform.

By default, ``cibuildwheel`` will launch a very large build matrix, and it is
possible that your extension is not compatible with every single configuration.
For example, suppose that the project depends on Python 3.9+ and a 64 bit
processor. In this case, add further entries to the ``[tool.cibuildwheel]``
block to remove incompatible configurations from the matrix:

.. code-block:: toml

    skip = ["cp38-*", "pp38-*"] # Skip CPython and PyPy 3.8
    archs = ["auto64"]          # Only target 64 bit architectures

The `cibuildwheel documentation
<https://cibuildwheel.readthedocs.io/en/stable/options/>`__ explains the
possible options.

If you set up a GitHub actions `secret
<https://docs.github.com/en/actions/security-guides/encrypted-secrets>`__ named
``pypi_password`` containing a PyPI authentication token, the action will
automatically upload the generated wheels to the `Python Package Index (PyPI)
<https://pypi.org>`__ when the action is triggered by a `software release event
<https://docs.github.com/en/repositories/releasing-projects-on-github/managing-releases-in-a-repository>`__.

