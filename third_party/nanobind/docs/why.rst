.. _why:

.. cpp:namespace:: nanobind

Why another binding library?
============================

I started the `pybind11 <http://github.com/pybind/pybind11>`_ project back in
2015 to generate better C++/Python bindings for a project I had been working
on. Thanks to many amazing contributions by others, pybind11 has since become a
core dependency of software used across the world including flagship projects
like `PyTorch <https://pytorch.org>`_ and `Tensorflow
<https://www.tensorflow.org>`_. Every day, it is downloaded over 400'000 times.
Hundreds of contributed extensions and generalizations address use cases of
this diverse audience. However, all of this success also came with costs: the
complexity of the library grew tremendously, which had a negative impact on
efficiency.

Curiously, the situation now is reminiscent of 2015: binding generation with
existing tools (`Boost.Python <https://github.com/boostorg/python>`_, `pybind11
<http://github.com/pybind/pybind11>`_) is slow and produces enormous binaries
with overheads on runtime performance. At the same time, key improvements in
C++17 and Python 3.8 provide opportunities for drastic simplifications.
Therefore, I am starting *another* binding project. This time, the scope is
intentionally limited so that this doesn't turn into an endless cycle.

So what is different?
---------------------

nanobind is highly related to pybind11 and inherits most of its conventions
and syntax. The main difference is a change in philosophy: pybind11 must
deal with *all of C++* to bind legacy codebases, while nanobind targets
a smaller C++ subset. *The codebase has to adapt to the binding tool and not
the other way around*, which allows nanobind to be simpler and faster. Pull
requests with extensions and generalizations to handle subtle fringe cases were
welcomed in pybind11, but they will likely be rejected in this project.

An overview of removed features is provided in a :ref:`separate section
<removed>`. Besides feature removal, the rewrite was also an opportunity to
address :ref:`long-standing performance issues <perf_improvements>` and add a
number of :ref:`major quality-of-life improvements <major_additions>` and
:ref:`smaller features <minor_additions>`.

.. _perf_improvements:

Performance improvements
------------------------

The :ref:`benchmark section <benchmarks>` evaluates the impact of the following
performance improvements:

- **Compact objects**: C++ objects are now co-located with the Python object
  whenever possible (less pointer chasing compared to pybind11). The
  per-instance overhead for wrapping a C++ type into a Python object shrinks by
  a factor of 2.3x. (pybind11: 56 bytes, nanobind: 24 bytes.)

- **Compact functions**: C++ function binding information is now co-located
  with the Python function object (less pointer chasing).

- **Compact types**: C++ type binding information is now co-located with the Python type object
  (less pointer chasing, fewer hashtable lookups).

- **Fast hash table**: nanobind upgrades several important internal
  associative data structures that previously used ``std::unordered_map`` to a
  more efficient alternative (`tsl::robin_map
  <https://github.com/Tessil/robin-map>`_, which is included as a git
  submodule).

- **Vector calls**: function calls from/to Python are realized using `PEP 590
  vector calls <https://www.python.org/dev/peps/pep-0590>`_, which gives a nice
  speed boost. The main function dispatch loop no longer allocates heap memory.

- **Library component**: pybind11 was designed as a header-only library, which
  is generally a good thing because it simplifies the compilation workflow.
  However, one major downside of this is that a large amount of redundant code
  has to be compiled in each binding file (e.g., the function dispatch loop and
  all of the related internal data structures). nanobind compiles a separate
  shared or static support library ("*libnanobind*") and links it against the
  binding code to avoid redundant compilation. The CMake interface
  :cmake:command:`nanobind_add_module()` fully automates these extra
  steps.

- **Smaller headers**: ``#include <pybind11/pybind11.h>`` pulls in a large
  portion of the STL (about 2.1 MiB of headers with Clang and libc++). nanobind
  minimizes STL usage to avoid this problem. Type casters even for for basic
  types like ``std::string`` require an explicit opt-in by including an extra
  header file (e.g. ``#include <nanobind/stl/string.h>``).

- **Simpler compilation**: pybind11 was dependent on *link time optimization*
  (LTO) to produce reasonably-sized bindings, which makes linking a build time
  bottleneck. With nanobind's split into a precompiled library and minimal
  metatemplating, LTO is no longer crucial and can be skipped.

- **Lifetime management**: nanobind maintains efficient internal data
  structures for lifetime management (needed for :cpp:class:`nb::keep_alive
  <keep_alive>`, :cpp:enumerator:`nb::rv_policy::reference_internal
  <rv_policy::reference_internal>`, the ``std::shared_ptr`` interface, etc.).
  With these changes, bound types no longer need to be weak-referenceable,
  which saves a pointer per instance.

.. _major_additions:

Major additions
---------------

nanobind includes a number of quality-of-life improvements for developers:

- **N-dimensional arrays**: nanobind can exchange data with modern array programming
  frameworks. It uses either `DLPack <https://github.com/dmlc/dlpack>`_ or the
  `buffer protocol <https://docs.python.org/3/c-api/buffer.html>`_ to achieve
  *zero-copy* CPU/GPU array exchange with frameworks like `NumPy
  <https://numpy.org>`_, `PyTorch <https://pytorch.org>`_, `TensorFlow
  <https://www.tensorflow.org>`_, `JAX <https://jax.readthedocs.io>`_, etc. See
  the :ref:`section on n-dimensional arrays <ndarrays>` for details.

- **Stable ABI**: nanobind can target Python's `stable ABI interface
  <https://docs.python.org/3/c-api/stable.html>`_ starting with Python 3.12.
  This means that extension modules will be compatible with future version of
  Python without having to compile separate binaries per interpreter. That
  vision is still relatively far out, however: it will require Python 3.12+ to
  be widely deployed.

- **Stub generation**: nanobind ships with a custom :ref:`stub generator
  <stubs>` and CMake integration to automatically create high quality stubs as
  part of the build process. `Stubs
  <https://typing.readthedocs.io/en/latest/source/stubs.html>`__ make compiled
  extension code compatible with visual autocomplete in editors like `Visual
  Studio Code <https://code.visualstudio.com>`__ and static type checkers like
  `MyPy <https://github.com/python/mypy>`__, `PyRight
  <https://github.com/microsoft/pyright>`__ and `PyType
  <https://github.com/google/pytype>`__.

- **Leak warnings**: When the Python interpreter shuts down, nanobind reports
  instance, type, and function leaks related to bindings, which is useful for
  tracking down reference counting issues.  If these warnings are undesired,
  call :cpp:func:`nb::set_leak_warnings(false) <set_leak_warnings>`. nanobind
  also fully deletes its internal data structures when the Python interpreter
  terminates, which avoids memory leak reports in tools like *valgrind*.

- **Better docstrings**: pybind11 pre-renders docstrings while the binding code
  runs. In other words, every call to ``.def(...)`` to bind a function
  immediately creates the underlying docstring. When a function takes a C++
  type as parameter that is not yet registered in pybind11, the docstring will
  include a C++ type name (e.g. ``std::vector<int, std::allocator<int>>``),
  which can look rather ugly. pybind11 binding declarations must be carefully
  arranged to work around this issue.

  nanobind avoids the issue altogether by not pre-rendering docstrings: they
  are created on the fly when queried. nanobind also has improved
  out-of-the-box compatibility with documentation generation tools like `Sphinx
  <https://www.sphinx-doc.org/en/master/>`__.

- **Low-level API**: nanobind exposes an optional low-level API to provide
  fine-grained control over diverse aspects including :ref:`instance creation
  <lowlevel>`, :ref:`type creation <typeslots>`, and it can store
  :ref:`supplemental data <supplement>` in types. The low-level API provides a
  useful escape hatch to pursue advanced projects that were not foreseen in
  the design of this library.

.. _minor_additions:

Minor additions
---------------

The following lists minor-but-useful additions relative to pybind11.

- **Finding Python objects associated with a C++ instance**: In addition to all
  of the return value policies supported by pybind11, nanobind provides one
  additional policy named :cpp:enumerator:`nb::rv_policy::none
  <rv_policy::none>` that *only* succeeds when the return value is already a
  known/registered Python object. In other words, this policy will never
  attempt to move, copy, or reference a C++ instance by constructing a new
  Python object.

  The new :cpp:func:`nb::find() <find>` function encapsulates this behavior. It
  resembles :cpp:func:`nb::cast() <cast>` in the sense that it returns the
  Python object associated with a C++ instance. But while :cpp:func:`nb::cast()
  <cast>` will create that Python object if it doesn't yet exist,
  :cpp:func:`nb::find() <find>` will return a ``nullptr`` object. This function
  is useful to interface with Python's :ref:`cyclic garbage collector
  <cyclic_gc>`.

- **Parameterized wrappers**: The :cpp:class:`nb::handle_t\<T\> <handle_t>` type
  behaves just like the :cpp:class:`nb::handle <handle>` class and wraps a
  ``PyObject *`` pointer. However, when binding a function that takes such an
  argument, nanobind will only call the associated function overload when the
  underlying Python object wraps a C++ instance of type ``T``.

  Similarly, the :cpp:class:`nb::type_object_t\<T\> <type_object_t>` type
  behaves just like the :cpp:class:`nb::type_object <type_object>` class and
  wraps a ``PyTypeObject *`` pointer. However, when binding a function that
  takes such an argument, nanobind will only call the associated function
  overload when the underlying Python type object is a subtype of the C++ type
  ``T``.

  Finally, the :cpp:class:`nb::typed\<T, Ts...\> <typed>` annotation can 
  parameterize any other type. The feature exists to improve the
  expressiveness of type signatures (e.g., to turn ``list`` into
  ``list[int]``). Note, however, that nanobind does not perform additional
  runtime checks in this case. Please see the section on :ref:`parameterizing
  generics <typing_generics_parameterizing>` for further details.

- **Signature overrides**: it may sometimes be necessary to tweak the
  type signature of a class or function to provide richer type information to
  static type checkers like `MyPy <https://github.com/python/mypy>`__ or
  `PyRight <https://github.com/microsoft/pyright>`__. In such cases, specify
  the :cpp:class:`nb::sig <signature>` attribute to override the default
  nanobind-provided signature.

  For example, the following function signature annotation creates an overload
  that should only be called with an ``1``-valued integer literal. While the
  function also includes a runtime check, a static type checker can now ensure
  that this error condition cannot possibly be triggered by a given piece of code.

  .. code-block:: cpp

     m.def("f",
           [](int arg) {
               if (arg != 1)
                  nb::raise("invalid input");
               return arg;
           },
           nb::sig("def f(arg: typing.Literal[1], /) -> int"));

  Please see the section on :ref:`customizing function signatures
  <typing_signature_functions>` and :ref:`class signatures
  <typing_signature_classes>` for further details.
