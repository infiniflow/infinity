.. cpp:namespace:: nanobind

.. _faq:

Frequently asked questions
==========================

Importing my module fails with an ``ImportError``
-------------------------------------------------

If importing the module fails as shown below, you have not specified a
matching module name in :cmake:command:`nanobind_add_module()` and
:c:macro:`NB_MODULE() <NB_MODULE>`.

.. code-block:: pycon

   >>> import my_ext
   ImportError: dynamic module does not define module export function (PyInit_my_ext)

Importing fails due to missing ``[lib]nanobind.{dylib,so,dll}``
---------------------------------------------------------------

If importing the module fails as shown below, the extension cannot find the
``nanobind`` shared library component.

.. code-block:: pycon

   >>> import my_ext
   ImportError: dlopen(my_ext.cpython-311-darwin.so, 0x0002):
   Library not loaded: '@rpath/libnanobind.dylib'

This is really more of a general C++/CMake/build system issue than one of
nanobind specifically. There are two solutions:

1. Build the library component statically by specifying the ``NB_STATIC`` flag
   in :cmake:command:`nanobind_add_module()` (this is the default starting with
   nanobind 0.2.0).

2. Ensure that the various shared libraries are installed in the right
   destination, and that their `rpath <https://en.wikipedia.org/wiki/Rpath>`_
   is set so that they can find each other.

   You can control the build output directory of the shared library component
   using the following CMake command:

   .. code-block:: pycon

      set_target_properties(nanobind
        PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY                <path>
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        <path>
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          <path>
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO <path>
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     <path>
      )

   Depending on the flags provided to :cmake:command:`nanobind_add_module()`,
   the shared library component may have a different name following the pattern
   ``nanobind[-abi3][-lto]``.

   The following CMake commands may be useful to adjust the build and install
   `rpath <https://en.wikipedia.org/wiki/Rpath>`_ of the extension:

   .. code-block:: cmake

      set_property(TARGET my_ext APPEND PROPERTY BUILD_RPATH "$<TARGET_FILE_DIR:nanobind>")
      set_property(TARGET my_ext APPEND PROPERTY INSTALL_RPATH ".. ?? ..")


Why are reference arguments not updated?
----------------------------------------

Functions like the following example can be exposed in Python, but they won't
propagate updates to mutable reference arguments.

.. code-block:: cpp

   void increment(int &i) {
       i++;
   }

This isn't specific to builtin types but also applies to STL collections and
other types when they are handled using :ref:`type casters <type_casters>`.
Please read the full section on :ref:`information exchange between C++ and
Python <exchange>` to understand the issue and alternatives.


Why am I getting errors about leaked functions and types?
---------------------------------------------------------

When the Python interpreter shuts down, it informs nanobind about this using a
``Py_AtExit()`` callback. If any nanobind-created instances, functions, or
types are still alive at this point, then *something went wrong* because they
should have been deleted by the garbage collector. Although this does not
always indicate a serious problem, the decision was made to have nanobind
complain rather noisily about the presence of such leaks.

Other binding tools (e.g., pybind11) are on the opposite of the spectrum:
because they never report leaks, it is quite easy to accidentally introduce
many of them until a developer eventually realizes that something is very
wrong.

Leaks mainly occur for four reasons:

- **Reference counting bugs**.  If you write raw Python C API code or use the
  nanobind wrappers including functions like ``Py_[X]INCREF()``,
  ``Py_[X]DECREF()``, :cpp:func:`nb::steal() <steal>`, :cpp:func:`nb::borrow()
  <borrow>`, :cpp:func:`.dec_ref() <detail::api::dec_ref>`,
  :cpp:func:`.inc_ref() <detail::api::inc_ref>`
  , etc., then incorrect
  use of such calls can cause a reference to leak that prevents the associated
  object from being deleted.

- **Reference cycles**. Python's garbage collector frees unused objects that are
  part of a circular reference chains (e.g., ``A->B->C->A``). This requires all
  types in the cycle to implement the ``tp_traverse`` *type slot*, and at least
  one of them to implement the ``tp_clear`` type slot. See the section on
  :ref:`cyclic garbage collection <cyclic_gc>` for details on how to do this with
  nanobind.

- **Interactions with other tools that leak references**. Python extension
  libraries---especially *huge* ones with C library components like PyTorch,
  Tensorflow, etc., have been observed to leak references to nanobind
  objects.

  Some of these frameworks cache JIT-compiled functions based on the arguments
  with which they were called, and such caching schemes could leak references
  to nanobind types if they aren't cleaned up by the responsible extensions
  (this is a hypothesis). In this case, the leak would be benign---even so, it
  should be fixed in the responsible framework so that leak warnings aren't
  cluttered with flukes and can be more broadly useful.

- **Older Python versions**: Very old Python versions (e.g., 3.8) don't
  do a good job cleaning up global references when the interpreter shuts down.
  The following code may leak a reference if it is a top-level statement in a
  Python file or the REPL.

  .. code-block:: python

     a = my_ext.MyObject()

  Such a warning is benign and does not indicate an actual leak. It simply
  highlights a flaws in the interpreter shutdown logic of old Python versions.
  Wrap your code into a function to address this issue even on such versions:

  .. code-block:: python

     def run():
         a = my_ext.MyObject()
         # ...

     if __name__ == '__main__':
         run()

- **Exceptions**. Some exceptions such as ``AttributeError`` have been observed
  to hold references, e.g. to the object which lacked the desired attribute. If
  the last exception raised by the program references a nanobind instance, then
  this may be reported as a leak since Python finalization appears not to
  release the exception object. See `issue #376
  <https://github.com/wjakob/nanobind/issues/376>`__ for a discussion.

If you find leak warnings to be a nuisance, then you can disable them in the
C++ binding code via the :cpp:func:`nb::set_leak_warnings() <set_leak_warnings>`
function.

.. code-block:: python

   nb::set_leak_warnings(false);

This is a *global flag* shared by all nanobind extension libraries in the same
ABI domain. If you do so, then please isolate your extension from others by
passing the ``NB_DOMAIN`` parameter to :cmake:command:`nanobind_add_module()`.

Compilation fails with a static assertion mentioning ``NB_MAKE_OPAQUE()``
-------------------------------------------------------------------------

If your compiler generates an error of the following sort, you are mixing type
casters and bindings in a way that has them competing for the same types:

.. code-block:: text

   nanobind/include/nanobind/nb_class.h:207:40: error: static assertion failed: ↵
   Attempted to create a constructor for a type that won't be  handled by the nanobind's ↵
   class type caster. Is it possible that you forgot to add NB_MAKE_OPAQUE() somewhere?

For example, the following won't work:

.. code-block:: cpp

   #include <nanobind/stl/vector.h>
   #include <nanobind/stl/bind_vector.h>

   namespace nb = nanobind;

   NB_MODULE(my_ext, m) {
       // The following line cannot be compiled
       nb::bind_vector<std::vector<int>>(m, "VectorInt");

       // This doesn't work either
       nb::class_<std::vector<int>>(m, "VectorInt");
   }

This is not specific to STL vectors and will happen whenever casters and
bindings target overlapping types.

:ref:`Type casters <type_casters>` employ a pattern matching technique known as
`partial template specialization
<https://en.wikipedia.org/wiki/Partial_template_specialization>`_. For example,
``nanobind/stl/vector.h`` installs a pattern that detects *any* use of
``std::vector<T, Allocator>``, which overlaps with the above binding of a specific
vector type.

The deeper reason for this conflict is that type casters enable a
*compile-time* transformation of nanobind code, which can conflict with
binding declarations that are a *runtime* construct.

To fix the conflict in this example, add the line :c:macro:`NB_MAKE_OPAQUE(T)
<NB_MAKE_OPAQUE>`, which adds another partial template specialization pattern
for ``T`` that says: "ignore ``T`` and don't use a type caster to handle it".

.. code-block:: cpp

   NB_MAKE_OPAQUE(std::vector<int>);

.. warning::

   If your extension consists of multiple source code files that involve
   overlapping use of type casters and bindings, you are *treading on thin
   ice*. It is easy to violate the *One Definition Rule* (ODR) [`details
   <https://en.wikipedia.org/wiki/One_Definition_Rule>`_] in such a case, which
   may lead to undefined behavior (miscompilations, etc.).

   Here is a hypothetical example of an ODR violation: an extension
   contains two source code files: ``src_1.cpp`` and ``src_2.cpp``.

   - ``src_1.cpp`` binds a function that returns an ``std::vector<int>`` using
     a :ref:`type caster <type_casters>` (``nanobind/stl/vector.h``).

   - ``src_2.cpp`` binds a function that returns an ``std::vector<int>`` using
     a :ref:`binding <bindings>` (``nanobind/stl/bind_vector.h``), and it also
     installs the needed type binding.

   The problem is that a partially specialized class in the nanobind
   implementation namespace (specifically,
   ``nanobind::detail::type_caster<std::vector<int>>``) now resolves to *two
   different implementations* in the two compilation units. It is unclear how
   such a conflict should be resolved at the linking stage, and you should
   consider code using such constructions broken.

   To avoid this issue altogether, we recommend that you create a single
   include file (e.g., ``binding_core.h``) containing all of the nanobind
   include files (binding, type casters), your own custom type casters (if
   present), and :c:macro:`NB_MAKE_OPAQUE(T) <NB_MAKE_OPAQUE>` declarations.
   Include this header consistently in all binding compilation units. The
   construction shown in the example (mixing type casters and bindings for the
   same type) is not allowed, and cannot occur when following the
   recommendation.

How can I preserve the ``const``-ness of values in bindings?
------------------------------------------------------------

This is a limitation of nanobind, which casts away ``const`` in function
arguments and return values. This is in line with the Python language, which
has no concept of const values. Additional care is therefore needed to avoid
bugs that would be caught by the type checker in a traditional C++ program.

How can I reduce build time?
----------------------------

Large binding projects should be partitioned into multiple files, as shown in
the following example:

:file:`example.cpp`:

.. code-block:: cpp

    void init_ex1(nb::module_ &);
    void init_ex2(nb::module_ &);
    /* ... */

    NB_MODULE(my_ext, m) {
        init_ex1(m);
        init_ex2(m);
        /* ... */
    }

:file:`ex1.cpp`:

.. code-block:: cpp

    void init_ex1(nb::module_ &m) {
        m.def("add", [](int a, int b) { return a + b; });
    }

:file:`ex2.cpp`:

.. code-block:: cpp

    void init_ex2(nb::module_ &m) {
        m.def("sub", [](int a, int b) { return a - b; });
    }

As shown above, the various ``init_ex`` functions should be contained in
separate files that can be compiled independently from one another, and then
linked together into the same final shared object.  Following this approach
will:

1. reduce memory requirements per compilation unit.

2. enable parallel builds (if desired).

3. allow for faster incremental builds. For instance, when a single class
   definition is changed, only a subset of the binding code will generally need
   to be recompiled.

.. _type-visibility:

How can I avoid conflicts with other projects using nanobind?
-------------------------------------------------------------

Suppose that a type binding in your project conflicts with another extension, for
example because both expose a common type (e.g., ``std::latch``). nanobind will
warn whenever it detects such a conflict:

.. code-block:: text

  RuntimeWarning: nanobind: type 'latch' was already registered!

In the worst case, this could actually break both packages (especially if the
bindings of the two packages expose an inconsistent/incompatible API).

The higher-level issue here is that nanobind will by default try to make type
bindings visible across extensions because this is helpful to partition large
binding projects into smaller parts. Such information exchange requires that
the extensions:

- use the same nanobind *ABI version* (see the :ref:`Changelog <changelog>` for details).
- use the same compiler (extensions built with GCC and Clang are isolated from each other).
- use ABI-compatible versions of the C++ library.
- use the stable ABI interface consistently (stable and unstable builds are isolated from each other).
- use debug/release mode consistently (debug and release builds are isolated from each other).

In addition, nanobind provides a feature to intentionally scope extensions to a
named domain to avoid conflicts with other extensions. To do so, specify the
``NB_DOMAIN`` parameter in CMake:

.. code-block:: cmake

   nanobind_add_module(my_ext
                       NB_DOMAIN my_project
                       my_ext.cpp)

In this case, inter-extension type visibility is furthermore restricted to
extensions in the ``"my_project"`` domain.

I'd like to use this project, but with $BUILD_SYSTEM instead of CMake
---------------------------------------------------------------------

A difficult aspect of C++ software development is the sheer number of competing
build systems, including

- `CMake <https://cmake.org>`__,
- `Meson <https://mesonbuild.com>`__,
- `xmake <https://xmake.io/#/>`__,
- `Premake <https://premake.github.io>`__,
- `Bazel <https://bazel.build>`__,
- `Conan <https://docs.conan.io/2/>`__,
- `Autotools <https://www.gnu.org/software/automake>`__,
- and many others.

The author of this project has some familiarity with CMake but lacks expertise
with this large space of alternative tools. Maintaining and shipping support for
other build systems is therefore considered beyond the scope of this *nano*
project (see also the :ref:`why? <why>` part of the documentation that explains
the rationale for being somewhat restrictive towards external contributions).

If you wish to create and maintain an alternative interface to nanobind, then
my request would be that you create and maintain separate repository (see,
e.g., `pybind11_bazel <https://github.com/pybind/pybind11_bazel>`__ as an
example how how this was handled in the case of pybind11). Please carefully
review the file `nanobind-config.cmake
<https://github.com/wjakob/nanobind/blob/master/cmake/nanobind-config.cmake>`__.
Besides getting things to compile, it specifies a number of platform-dependent
compiler and linker options that are needed to produce *optimal* (small and
efficient) binaries. Nanobind uses a `complicated and non-standard
<https://github.com/wjakob/nanobind/commit/2f29ec7d5fbebd5f55fb52da297c8d197279f659>`__
set of linker parameters on macOS, which is the result of a `lengthy
investigation
<https://github.com/python/cpython/issues/97524#issuecomment-1458855301>`__.
Other parameters like linker-level dead code elimination and size-based
optimization were similarly added following careful analysis. The CMake build
system provides the ability to compile ``libnanobind`` into either a shared or
a static library, to optionally target the stable ABI, and to isolate it from
other extensions via the ``NB_DOMAIN`` parameter. All of these are features
that would be nice to retain in an alternative build system. If you've made a
build system compatible with another tool that is sufficiently
feature-complete, then please file an issue and I am happy to reference it in
the documentation.

How to cite this project?
-------------------------

Please use the following BibTeX template to cite nanobind in scientific
discourse:

.. code-block:: bibtex

    @misc{nanobind,
       author = {Wenzel Jakob},
       year = {2022},
       note = {https://github.com/wjakob/nanobind},
       title = {nanobind: tiny and efficient C++/Python bindings}
    }
