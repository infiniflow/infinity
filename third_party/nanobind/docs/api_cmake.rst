.. _api_cmake:

CMake API Reference
===================

nanobind's CMake API simplifies the process of building python extension
modules. This is needed because quite a few steps are involved: nanobind must
build the module, a library component, link the two together, and add a
different set of compilation and linker flags depending on the target platform.

If you prefer another build system, then you have the following options:

- `Nicholas Junge <https://github.com/nicholasjng>`__ has created a `Bazel
  interface <https://github.com/nicholasjng/nanobind-Bazel>`__ to nanobind.
  Please report Bazel-specific issues there.

- You could create a new build system from scratch that takes care of these
  steps. See `this file
  <https://github.com/wjakob/nanobind/blob/master/src/nb_combined.cpp>`__ for
  inspiration on how to do this on Linux. Note that you will be on your own if
  you choose to go this route---I unfortunately do not have the time to respond
  to GitHub tickets related to custom build systems.

The section on :ref:`building extensions <building>` provided an introductory
example of how to set up a basic build system via the
:cmake:command:`nanobind_add_module` command, which is the :ref:`high level
<highlevel-cmake>` build interface. The defaults chosen by this function are
somewhat opinionated, however. For this reason, nanobind also provides an
alternative :ref:`low level <lowlevel-cmake>` interface that decomposes it into
smaller steps.

A later part of this section explains how a Git submodule dependency can be
:ref:`avoided <submodule_deps>` in exchange for a system-provided package.

Finally, the section ends with an explanation of the CMake convenience
interface for :ref:`stub generation <stub_generation_cmake>`.

.. _highlevel-cmake:

High-level interface
--------------------

The high-level interface consists of just one CMake command:

.. cmake:command:: nanobind_add_module

   Compile a nanobind extension module using the specified target name,
   optional flags, and source code files. Use it as follows:

   .. code-block:: cmake

      nanobind_add_module(
        my_ext                   # Target name
        NB_STATIC STABLE_ABI LTO # Optional flags (see below)
        my_ext.h                 # Source code files below
        my_ext.cpp)

   It supports the following optional parameters:

   .. list-table::

      * - ``STABLE_ABI``
        - Perform a `stable ABI
          <https://docs.python.org/3/c-api/stable.html>`_ build, making it
          possible to use a compiled extension across Python minor versions.
          The flag is ignored on Python versions older than < 3.12.
      * - ``NB_STATIC``
        - Compile the core nanobind library as a static library. This
          simplifies redistribution but can increase the combined binary
          storage footprint when a project contains many Python extensions
          (this is the default).
      * - ``NB_SHARED``
        - The opposite of ``NB_STATIC``: compile the core nanobind library
          as a shared library for use in projects that consist of multiple
          extensions.
      * - ``PROTECT_STACK``
        - Don't remove stack smashing-related protections.
      * - ``LTO``
        - Perform link time optimization.
      * - ``NOMINSIZE``
        - Don't perform optimizations to minimize binary size.
      * - ``NOSTRIP``
        - Don't strip unneded symbols and debug information from the compiled
          extension when performing release builds.
      * - ``NB_DOMAIN <name>``
        - Restrict the inter-extension type visibility to a named subdomain.
          See the associated :ref:`FAQ entry <type-visibility>` for details.
      * - ``MUSL_DYNAMIC_LIBCPP``
        - When `cibuildwheel
          <https://cibuildwheel.readthedocs.io/en/stable/>`__ is used to
          produce `musllinux <https://peps.python.org/pep-0656/>`__ wheels,
          don't statically link against ``libstdc++`` and ``libgcc`` (which is
          an optimization that nanobind does by default in this specific case).
          If this explanation sounds confusing, then you can ignore it. See the
          detailed description below for more information on this step.

   :cmake:command:`nanobind_add_module` performs the following
   steps to produce bindings.

   - It creates a CMake library via ``add_library(target_name MODULE ...)`` and
     enables the use of C++17 features during compilation.

   - It creates a CMake target for an internal library component required by
     nanobind (named ``nanobind-..`` where ``..`` depends on the compilation
     flags). This is only done once when compiling multiple extensions.

     This library component can either be a static or shared library depending
     on whether the optional ``NB_STATIC`` or ``NB_SHARED`` parameter was
     provided to ``nanobind_add_module()``. The default is a static build,
     which simplifies redistribution (only one shared library must be deployed).

     When a project contains many Python extensions, a shared build is
     preferable to avoid unnecessary binary size overheads that arise from
     redundant copies of the ``nanobind-...`` component.

   - It links the newly created library against the ``nanobind-..`` target.

   - It appends the library suffix (e.g., ``.cpython-39-darwin.so``) based
     on information provided by CMake’s ``FindPython`` module.

   - When requested via the optional ``STABLE_ABI`` parameter, the build system
     will create a `stable ABI <https://docs.python.org/3/c-api/stable.html>`_
     extension module with a different suffix (e.g., ``.abi3.so``).

     Once compiled, a stable ABI extension can be reused across Python minor
     versions. In contrast, ordinary builds are only compatible across patch
     versions. This feature requires Python >= 3.12 and is ignored on older
     versions. Note that use of the stable ABI come at a small performance cost
     since nanobind can no longer access the internals of various data
     structures directly. If in doubt, benchmark your code to see if the cost
     is acceptable.

   - In non-debug modes, it compiles with *size optimizations* (i.e.,
     ``-Os``). This is generally the mode that you will want to use for
     C++/Python bindings. Switching to ``-O3`` would enable further
     optimizations like vectorization, loop unrolling, etc., but these all
     increase compilation time and binary size with no real benefit for
     bindings.

     If your project contains portions that benefit from ``-O3``-level
     optimizations, then it’s better to run two separate compilation
     steps. An example is shown below:

     .. code:: cmake

        # Compile project code with current optimization mode configured in CMake
        add_library(example_lib STATIC source_1.cpp source_2.cpp)
        # Need position independent code (-fPIC) to link into 'example_ext' below
        set_target_properties(example_lib PROPERTIES POSITION_INDEPENDENT_CODE ON)

        # Compile extension module with size optimization and add 'example_lib'
        nanobind_add_module(example_ext common.h source_1.cpp source_2.cpp)
        target_link_libraries(example_ext PRIVATE example_lib)

     Size optimizations can be disabled by specifying the optional
     ``NOMINSIZE`` argument, though doing so is not recommended.

   - ``nanobind_add_module()`` also disables stack-smashing protections
     (i.e., it specifies ``-fno-stack-protector`` to Clang/GCC).
     Protecting against such vulnerabilities in a Python VM seems futile,
     and it adds non-negligible extra cost (+8% binary size in
     benchmarks). This behavior can be disabled by specifying the optional
     ``PROTECT_STACK`` flag. Either way, is not recommended that you use
     nanobind in a setting where it presents an attack surface.

   - It sets the default symbol visibility to ``hidden`` so that only functions
     and types specifically marked for export generate symbols in the resulting
     binary. This substantially reduces the size of the generated binary.

   - In release builds, it strips unreferenced functions and debug information
     names from the resulting binary. This can substantially reduce the size of
     the generated binary and can be disabled using the optional ``NOSTRIP``
     argument.

   - Link-time optimization (LTO) is *not active* by default; benefits compared
     to pybind11 are relatively low, and this can make linking a build
     bottleneck. That said, the optional ``LTO`` argument can be specified to
     enable LTO in release builds.

   - nanobind's CMake build system is often combined with `cibuildwheel
     <https://cibuildwheel.readthedocs.io/en/stable/>`__ to automate the
     generation of wheels for many different platforms. One such platform
     called `musllinux <https://peps.python.org/pep-0656/>`__ exists to create
     tiny self-contained binaries that are cheap to install in a container
     environment (Docker, etc.). An issue of the combination with nanobind is
     that ``musllinux`` doesn't include the ``libstdc++`` and ``libgcc``
     libraries which nanobind depends on. ``cibuildwheel`` then has to ship
     those along in each wheel, which actually increases their size rather
     dramatically (by a factor of >5x for small projects). To avoid this,
     nanobind prefers to link against these libraries *statically* when it
     detects a ``cibuildwheel`` build targeting ``musllinux``. Pass the
     ``MUSL_DYNAMIC_LIBCPP`` parameter to avoid this behavior.

   - If desired (via the optional ``NB_DOMAIN`` parameter), nanobind will
     restrict the visibility of symbols to a named subdomain to avoid conflicts
     between bindings. See the associated :ref:`FAQ entry <type-visibility>`
     for details.

.. _lowlevel-cmake:

Low-level interface
-------------------

Instead of :cmake:command:`nanobind_add_module` nanobind also exposes a more
fine-grained interface to the underlying operations.
The following

.. code-block:: cmake

    nanobind_add_module(my_ext NB_SHARED LTO my_ext.cpp)

is equivalent to

.. code-block:: cmake

    # Build the core parts of nanobind once
    nanobind_build_library(nanobind SHARED)

    # Compile an extension library
    add_library(my_ext MODULE my_ext.cpp)

    # .. and link it against the nanobind parts
    target_link_libraries(my_ext PRIVATE nanobind)

    # .. enable size optimizations
    nanobind_opt_size(my_ext)

    # .. enable link time optimization
    nanobind_lto(my_ext)

    # .. set the default symbol visibility to 'hidden'
    nanobind_set_visibility(my_ext)

    # .. strip unneeded symbols and debug info from the binary (only active in release builds)
    nanobind_strip(my_ext)

    # .. disable the stack protector
    nanobind_disable_stack_protector(my_ext)

    # .. set the Python extension suffix
    nanobind_extension(my_ext)

    # .. set important compilation flags
    nanobind_compile_options(my_ext)

    # .. set important linker flags
    nanobind_link_options(my_ext)

    # Statically link against libstdc++/libgcc when targeting musllinux
    nanobind_musl_static_libcpp(my_ext)

The various commands are described below:

.. cmake:command:: nanobind_build_library

   Compile the core nanobind library. The function expects only the target
   name and uses a slightly unusual parameter passing policy: its behavior
   changes based on whether or not one the following substrings is detected
   in the target name:

   .. list-table::
      :widths: 10 50

      * - ``-static``
        - Perform a static library build (without this suffix, a shared build is used)
      * - ``-abi3``
        - Perform a stable ABI build targeting Python v3.12+.

   .. code-block:: cmake

      # Normal shared library build
      nanobind_build_library(nanobind)

      # Static ABI3 build
      nanobind_build_library(nanobind-static-abi3)

.. cmake:command:: nanobind_opt_size

   This function enable size optimizations in ``Release``, ``MinSizeRel``,
   ``RelWithDebInfo`` builds. It expects a single target as argument, as in

   .. code-block:: cmake

      nanobind_opt_size(my_target)

.. cmake:command:: nanobind_set_visibility


   This function sets the default symbol visibility to ``hidden`` so that only
   functions and types specifically marked for export generate symbols in the
   resulting binary. It expects a single target as argument, as in

   .. code-block:: cmake

      nanobind_trim(my_target)

   This substantially reduces the size of the generated binary.

.. cmake:command:: nanobind_strip

   This function strips unused and debug symbols in ``Release`` and
   ``MinSizeRel`` builds on Linux and macOS. It expects a single target as
   argument, as in

   .. code-block:: cmake

      nanobind_strip(my_target)

.. cmake:command:: nanobind_disable_stack_protector

   The stack protector affects the binary size of bindings negatively (+8%
   on Linux in benchmarks). Protecting from stack smashing in a Python VM
   seems in any case futile, so this function disables it for the specified
   target when performing a build with optimizations. Use it as follows:

   .. code-block:: cmake

      nanobind_disable_stack_protector(my_target)

.. cmake:command:: nanobind_extension

   This function assigns an extension name to the compiled binding, e.g.,
   ``.cpython-311-darwin.so``. Use it as follows:

   .. code-block:: cmake

      nanobind_extension(my_target)

.. cmake:command:: nanobind_extension_abi3

   This function assigns a stable ABI extension name to the compiled binding,
   e.g., ``.abi3.so``. Use it as follows:

   .. code-block:: cmake

      nanobind_extension_abi3(my_target)


.. cmake:command:: nanobind_compile_options

   This function sets recommended compilation flags. Currently, it specifies
   ``/bigobj`` and ``/MP`` on MSVC builds, and it does nothing other platforms
   or compilers. Use it as follows:

   .. code-block:: cmake

      nanobind_compile_options(my_target)

.. cmake:command:: nanobind_link_options

   This function sets recommended linker flags. Currently, it controls link
   time handling of undefined symbols on Apple platforms related to Python C
   API calls, and it does nothing other platforms. Use it as follows:

   .. code-block:: cmake

      nanobind_link_options(my_target)

.. cmake:command:: nanobind_musl_static_libcpp

   This function passes the linker flags ``-static-libstdc++`` and
   ``-static-libgcc`` to ``gcc`` when the environment variable
   ``AUDITWHEEL_PLAT`` contains the string ``musllinux``, which indicates a
   cibuildwheel build targeting that platform.

   The function expects a single target as argument, as in

   .. code-block:: cmake

      nanobind_musl_static_libcpp(my_target)

.. _submodule_deps:

Submodule dependencies
----------------------

nanobind includes a dependency (a fast hash map named ``tsl::robin_map``) as a
Git submodule. If you prefer to use another (e.g., system-provided) version of
this dependency, set the ``NB_USE_SUBMODULE_DEPS`` variable before importing
nanobind into CMake. In this case, nanobind's CMake scripts will internally
invoke ``find_dependency(tsl-robin-map)`` to locate the associated header
files.

.. _stub_generation_cmake:

Stub generation
---------------

Nanobind's CMake tooling includes a convenience command to interface with the
``stubgen`` program explained in the section on :ref:`stub generation <stubs>`.

.. cmake:command:: nanobind_add_stub

   Import the specified module (``MODULE`` parameter), generate a stub, and
   write it to the specified file (``OUTPUT`` parameter). Here is an example
   use:

   .. code-block:: cmake

      nanobind_add_stub(
          my_ext_stub
          MODULE my_ext
          OUTPUT my_ext.pyi
          PYTHON_PATH $<TARGET_FILE_DIR:my_ext>
          DEPENDS my_ext
      )

   The target name (``my_ext_stub`` in this example) must be unique but has no
   other significance.

   ``stubgen`` will add all paths specified as part of the ``PYTHON_PATH``
   block and then execute ``import my_ext`` in a Python session. If the
   extension is not importable, this will cause stub generation to fail.

   This command supports the following parameters:

   .. list-table::

      * - ``INSTALL_TIME``
        - By default, stub generation takes place at build time following
          generation of all dependencies (see ``DEPENDS``). When this parameter
          is specified, stub generation is instead postponed to the
          installation phase.
      * - ``MODULE``
        - Specifies the name of the module that should be imported. Mandatory.
      * - ``OUTPUT``
        - Specifies the name of the stub file that should be written. The path
          is relative to ``CMAKE_CURRENT_BINARY_DIR`` for build-time stub
          generation and relative to ``CMAKE_INSTALL_PREFIX`` for install-time
          stub generation. Mandatory.
      * - ``PYTHON_PATH``
        - List of search paths that should be considered when importing the
          module. The paths are relative to ``CMAKE_CURRENT_BINARY_DIR`` for
          build-time stub generation and relative to ``CMAKE_INSTALL_PREFIX``
          for install-time stub generation. The current directory (``"."``) is
          always included and does not need to be specified. The parameter may
          contain CMake `generator expressions
          <https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html>`__
          when :cmake:command:`nanobind_add_stub` is used for build-time stub
          generation. Otherwise, generator expressions should not be used.
          Optional.
      * - ``DEPENDS``
        - Any targets listed here will be marked as a dependencies. This should
          generally be used to list the target names of one or more prior
          :cmake:command:`nanobind_add_module` declarations. Note that this
          parameter tracks *build-time* dependencies and does not need to be
          specified when stub generation occurs at install time (see
          ``INSTALL_TIME``). Optional.
      * - ``VERBOSE``
        - Show status messages generated by ``stubgen``.
      * - ``EXCLUDE_DOCSTRINGS``
        - Generate a stub containing only typed signatures without docstrings.
      * - ``INCLUDE_PRIVATE``
        - Also include private members, whose names begin or end with a single
          underscore.
      * - ``MARKER_FILE``
        - Typed extensions normally identify themselves via the presence of an
          empty file named ``py.typed`` in each module directory. When this
          parameter is specified, :cmake:command:`nanobind_add_stub` will
          automatically generate such an empty file as well.
      * - ``PATCH_FILE``
        - Specify a patch file used to replace declarations in the stub. The
          syntax is described in the section on :ref:`stub generation <stubs>`.
      * - ``COMPONENT``
        - Specify a component when ``INSTALL_TIME`` stub generation is used.
          This is analogous to ``install(..., COMPONENT [name])`` in other
          install targets.
      * - ``EXCLUDE_FROM_ALL``
        - If specified, the file is only installed as part of a
          component-specific installation when ``INSTALL_TIME`` stub generation
          is used. This is analogous to ``install(..., EXCLUDE_FROM_ALL)`` in
          other install targets.
