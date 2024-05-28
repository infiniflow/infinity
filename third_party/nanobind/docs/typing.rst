.. cpp:namespace:: nanobind

.. _typing:

Typing
======

This section covers three broad typing-related topics:

1. How to create rich type annotation in C++ bindings so that projects
   using them can be effectively type-checked.

2. How to :ref:`automatically generate stub files <stubs>` that are needed to
   enable static type checking and autocompletion in Python IDEs.

3. How to write :ref:`pattern files <pattern_files>` to handle advanced use
   cases requiring significant stub customization.

Signature customization
-----------------------

In larger binding projects, some customization of function or class signatures
is often needed so that static type checkers can effectively use the generated
stubs.

.. _typing_signature_functions:

Functions
^^^^^^^^^

Nanobind generates typed function signatures automatically, but these are not
always satisfactory. For example, the following function binding

.. code-block:: cpp

   nb::class_<Int>(m, "Int")
       .def(nb::self == nb::self);

is likely to be rejected because the default ``__eq__`` function signature

.. code-block:: text

   __eq__(self, arg: Int, /) -> bool

is more specific than that of the parent class ``object``:

.. code-block:: text

   __eq__(self, arg: object, /) -> bool

In this case, a static type checker like `MyPy
<https://github.com/python/mypy>`__ will report a failure:

.. code-block:: text

    error: Argument 1 of "__eq__" is incompatible with supertype "object"; supertype defines the argument type as "object"  [override]

To handle such cases, you can use the :cpp:class:`nb::sig <sig>`
attribute to overrides the function signature with a custom string.

.. code-block:: cpp

   nb::class_<Int>(m, "Int")
       .def(nb::self == nb::self,
            nb::sig("def __eq__(self, arg: object, /) -> bool"));

The argument must be a valid Python function signature of the form ``def
name(...) -> ...`` without trailing colon (``":"``) and newlines, where
``name`` must furthermore match the name given to the binding declaration. In
this case, the name is implicitly given by the operator. It must match
``"name"`` in the case of ``.def("name", ...)``-style bindings with an explicit
name. The signature can span multiple lines, e.g., to prefix one or more
decorators.

The modified signature is shown in generated stubs, docstrings, and error
messages (e.g., when a function receives incompatible arguments).

In cases where a custom signature is only needed to tweak how nanobind renders
the signature of a default argument, the more targeted
:cpp:func:`nb::arg("name").sig("signature") <arg::sig>` annotation is
preferable to :cpp:class:`nb::sig <sig>`.

.. _typing_signature_classes:

Classes
^^^^^^^

Signature customization is also available for class bindings, though only
stubs are affected in this case.

Consider the example below, which defines an iterable vector type storing
integers. Suppose that ``GeneralIterator`` iterates over arbitrary data and
does not provide a useful ``int``-typed signature.

.. code-block:: cpp

   using IntVec = std::vector<int>;

   nb::class_<IntVec>(m, "IntVec")
      .def("__iter__",
           [](const IntVec &v) -> GeneralIterator { ... })

It may be useful to inherit from ``collections.abc.Iterable[int]`` to
communicate more information to static type checkers, but such a Python → C++
inheritance chain is not permitted by nanobind.

.. _typing_liberties:

Stubs often take certain liberties in deviating somewhat from the precise type
signature of the underlying implementation, which is fine as long as this
improves the capabilities of the type checker (the stubs are only used by the
static type checking phase, which never imports the actual extension).

Here, we could specify

.. code-block:: cpp

   nb::class_<IntVec>(m, "IntVec",
                      nb::sig("class IntVec(collections.abc.Iterable[int])"));

This is technically a lie. Such shenanigans are worthwhile because they can
greatly improve the development experience (e.g. `VS Code
<https://code.visualstudio.com>`__ autocomplete) involving compiled extensions.

The supplied signature string must be a valid Python class signature of the
form ``class ClassName(...)`` excluding trailing colon (``":"``) and newline,
where ``ClassName`` must furthermore match the name provided in the main class
binding declaration.
The signature can span multiple lines, e.g., to prefix one or more decorators.

Generic types
-------------

.. _typing_generics_parameterizing:

Parameterizing generic types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Various standard Python types are `generic
<https://typing.readthedocs.io/en/latest/spec/generics.html>`__ can be
parameterized to improve the effectiveness of static type checkers such as
`MyPy <https://github.com/python/mypy>`__. In the presence of such a
specialization, a type checker can, e.g., infer that the variable ``a`` below
is of type ``int``.

.. code-block:: python

   def f() -> list[int]: ...

   a = f()[0]

This is even supported for *abstract types*---for example,
``collections.abc.Mapping[str, int]`` indicates an abstract mapping from
strings to integers.

nanobind provides the template class :cpp:class:`nb::typed\<T, Ts...\> <typed>`
to generate parameterized type annotations in C++ bindings. For example, the
argument and return value of the following function binding reproduces the
exact list and mapping types mentioned above.

.. code-block:: cpp

   m.def("f", [](nb::typed<nb::mapping, nb::str, int> arg)
               -> nb::typed<nb::list, int> { ... });

(Usually, :cpp:class:`nb::typed\<T, Ts...\> <typed>` would be applied to
:ref:`wrapper <wrappers>` types, though this is not a strict limitation.)

An important limitation of this feature is that it *only* affects function
signatures. Nanobind will (as always) ensure that ``f`` can only be called with
a ``nb::mapping``, but it will *not* insert additional runtime checks to verify that
``arg`` indeed maps strings to integers. It is the responsibility of the
function to perform these checks and, if needed, to raise a
:cpp:func:`nb::type_error <type_error>`.

The parameterized C++ type :cpp:class:`nb::typed\<T, Ts...\> <typed>`
subclasses the type ``T`` and can be used interchangeably with ``T``. The other
arguments (``Ts...``) are used to generate a Python type signature but have no
other effect (for example, parameterizing by ``str`` on the Python end can
alternatively be achieved by passing ``nb::str``, ``std::string``, or ``const
char*`` as part of the ``Ts..`` parameter pack).

.. _typing_generics_creating:

Creating generic types
^^^^^^^^^^^^^^^^^^^^^^

Python types inheriting from `types.Generic
<https://docs.python.org/3/library/typing.html#typing.Generic>`__ can be
*parameterized* by other types including generic `type variables
<https://docs.python.org/3/library/typing.html#typing.TypeVar>`__ that act as
placeholders. Such constructions enable more effective static type checking. In
the snippet below, tools like `MyPy <https://github.com/python/mypy>`__ or
`PyRight <https://github.com/microsoft/pyright>`__ can infer that ``x`` and
``y`` have types ``Wrapper[int]`` and ``int``, respectively.

.. code-block:: python

   import typing

   # 1. Instantiate a placeholder type ("type variable") used below
   T = typing.TypeVar("T")

   # 2. Create a generic type by inheriting from typing.Generic
   class Wrapper(typing.Generic[T]):
       # The constructor references the placeholder type
       def __init__(self, value: T):
           self.value = value

       # .. this type is then preserved in the getter
       def get(self) -> T:
           return self.value

   # Based on the typed constructor, MyPy knows that 'x' has type 'Wrapper[int]'
   x = Wrapper(3)

   # Based on the typed 'Wrapped.get' method, 'y' is inferred to have type 'int'
   y = x.get()

Note that parameterization of a generic type doesn't generate new code or
modify its functionality. It is not to be confused with C++ template
instantiation. The feature only exists to propagate fine-grained type
information and thereby aid static type checking.

Similar functionality can also be supported in nanobind-based binding projects.
This looks as follows:

.. code-block:: cpp

   #include <nanobind/typing.h> // needed by nb::type_var below

   struct Wrapper {
       nb::object value;
   };

   NB_MODULE(my_ext, m) {
       // 1. Instantiate a placeholder type ("type variable") used below
       m.attr("T") = nb::type_var("T");

       // 2. Create a generic type, and indicate in generated stubs
       //    that it derives from Generic[T]
       nb::class_<Wrapper> wrapper(m, "Wrapper", nb::is_generic(),
                                   nb::sig("class Wrapper(typing.Generic[T])"))
           .def(nb::init<nb::object>(),
                nb::sig("def __init__(self, arg: T, /) -> None"))
           .def("get", [](Wrapper &w) { return w.value; },
                nb::sig("def get(self, /) -> T"));
   }

This involves the following steps:

- The :cpp:func:`nb::type_var <type_var>` constructor generates a type variable
  analogous to the previous Python snippet and assigns it to the name ``"T"``
  within the module.

- If we were to follow the previous Python example, the next step would require
  defining ``Wrapper`` as a subclass of ``typing.Generic[T]``. However, this
  isn't possible because nanobind-based classes cannot derive from Python
  types.

- The solution to this problem takes the following :ref:`liberties
  <typing_liberties>`:

  - It passes the :cpp:class:`nb::is_generic <is_generic>` annotation to the
    :cpp:class:`nb::class_\<...\> <class_>` constructor, causing the addition
    of a ``__class_getattr__`` member that enables type parameterization.
    Following this step, an expression like ``Wrapper[int]`` becomes valid and
    returns a ``typing.TypeAlias`` (in other words, the behavior is *as if* we
    had derived from ``typing.Generic[T]``).

    However, `MyPy <https://github.com/python/mypy>`__ and similar tools don't
    quite know what to do with custom types overriding ``__class_getattr__``
    themselves, since the official parameterization mechanism is to subclass
    ``typing.Generic``.

  - Therefore, we *lie* about this in the stub and declare
    ``typing.Generic[T]`` as a base class. Only static type checkers will
    see this information, and it helps them to interpret how the type works.

  - That's it!

You may also extend parameterized forms of such generic types:

.. code-block:: cpp

   nb::class_<Subclass>(m, "Subclass", wrapper[nb::type<Foo>()]);

nanobind's stub generator will render this as ``class Subclass(Wrapper[Foo]):``.

Any-typed return values
^^^^^^^^^^^^^^^^^^^^^^^

The return value of a function can sometimes be unclear (dynamic), in which
case it can be helpful to declare ``typing.Any`` as a pragmatic return type
(this effectively disables analysis of the return value in static type
checkers). nanobind provides a :py:class:`nb::any <any>` wrapper type that is
equivalent to :py:class:`nb::object <object>` except that its type signature
renders as ``typing.Any`` to facilitate this.

.. _stubs:

Stub generation
---------------

A *stub file* provides a *typed* and potentially documented summary of a
module's class, function, and variable declarations. Stub files have the
extension ``.pyi`` and are often shipped along with Python extensions. They
are needed to enable autocompletion and static type checking in tools like
`Visual Studio Code <https://code.visualstudio.com>`__, `MyPy
<https://github.com/python/mypy>`__, `PyRight
<https://github.com/microsoft/pyright>`__ and `PyType
<https://github.com/google/pytype>`__.

Take for example the following function:

.. code-block:: python

   def square(x: int) -> int:
       '''Return the square of the input'''
       return x*x

The associated default stub removes the body, while retaining the docstring:

.. code-block:: python

   def square(x: int) -> int:
       '''Return the square of the input'''

An undocumented stub replaces the entire body with the Python ellipsis object
(``...``).

.. code-block:: python

   def square(x: int) -> int: ...

Complex default arguments are often also abbreviated with ``...`` to improve
the readability of signatures. You can read more about stub files in the
`typing documentation
<https://typing.readthedocs.io/en/latest/source/stubs.html>`__ and the `MyPy
documentation <https://mypy.readthedocs.io/en/stable/stubs.html>`__.

nanobind's ``stubgen`` tool automates the process of stub generation to turn
modules containing a mixture of ordinary Python code and C++ bindings into an
associated ``.pyi`` file.

The main challenge here is that C++ bindings are unlike ordinary Python
objects, which causes standard mechanisms to extract their signature to fail.
Existing tools like MyPy's `stubgen
<https://mypy.readthedocs.io/en/stable/stubgen.html>`__ and `pybind11-stubgen
<https://github.com/sizmailov/pybind11-stubgen>`__ must therefore parse
docstrings to infer function signatures, which is brittle and does not always
produce high-quality output.

nanobind functions expose a ``__nb_signature__`` property, which provides
structured information about typed function signatures, overload chains, and
default arguments. nanobind's ``stubgen`` leverages this information to
reliably generate high-quality stubs that are usable by static type checkers.

There are three ways to interface with the stub generator described in
the following subsections.

CMake interface
^^^^^^^^^^^^^^^

nanobind's CMake interface provides the :cmake:command:`nanobind_add_stub`
command for stub generation at build or install time. It generates a single
stub at a time--more complex cases involving large numbers of stubs are easily
handled using standard CMake constructs (e.g. a ``foreach()`` loop).

The command requires a target name (e.g., ``my_ext_stub``) that must be unique
but has no other significance. Once all dependencies (``DEPENDS`` parameter)
are met, it will invoke ``stubgen`` to turn a single module (``MODULE``
parameter) into a stub file (``OUTPUT`` parameter).

For this to work, the module must be importable. ``stubgen`` will add all paths
specified as part of the ``PYTHON_PATH`` parameter and then execute ``import
my_ext``, raising an error if this fails.

.. code-block:: cmake

   nanobind_add_stub(
     my_ext_stub
     MODULE my_ext
     OUTPUT my_ext.pyi
     PYTHON_PATH $<TARGET_FILE_DIR:my_ext>
     DEPENDS my_ext
   )

Typed extensions normally identify themselves via the presence of an empty file
named ``py.typed`` in each module directory. :cmake:command:`nanobind_add_stub`
can optionally generate this file as well.

.. code-block:: cmake

   nanobind_add_stub(
     ...
     MARKER_FILE py.typed
     ...
   )

CMake tracks the generated outputs in its dependency graph. The combination of
compiled extension module, stub, and marker file can subsequently be installed
by subsequent ``install()`` directives.

.. code-block:: cmake

  install(TARGETS my_ext DESTINATION ".")
  install(FILES py.typed my_ext.pyi DESTINATION ".")

In certain situations, it may be tricky to import an extension that is built
but not yet installed to its final destination. To handle such cases, specify
the ``INSTALL_TIME`` parameter to :cmake:command:`nanobind_add_stub` to delay
stub generation to the installation phase.

.. code-block:: cmake

    install(TARGETS my_ext DESTINATION ".")

    nanobind_add_stub(
      my_ext_stub
      INSTALL_TIME
      MODULE my_ext
      OUTPUT my_ext.pyi
      PYTHON_PATH "."
    )

This requires several changes:

1. ``PYTHON_PATH`` must be adjusted so that it references a location relative
   to ``CMAKE_INSTALL_PREFIX`` from which the installed module is importable.

2. The :cmake:command:`nanobind_add_stub` command should be preceded by
   ``install(TARGETS my_ext)`` and ``install(FILES`` commands that place all
   data (compiled extension files, plain Python code, etc.) needed to bring the
   module into an importable state.

   Place all relevant ``install()`` directives within the same
   ``CMakeLists.txt`` file to ensure that these steps are executed
   sequentially.

3. Dependencies (``DEPENDS``) no longer need to be listed. These are build-time
   constraints that do not apply in the installation phase.

4. The output file path (``OUTPUT``) is relative to ``CMAKE_INSTALL_PREFIX``
   and may need adjustments as well.

The :cmake:command:`nanobind_add_stub` command has a few other options, please
refer to its documentation for details.

Command line interface
^^^^^^^^^^^^^^^^^^^^^^

Alternatively, you can invoke ``stubgen`` on the command line. The nanobind
package must be installed for this to work, e.g., via ``pip install nanobind``.
The command line interface is also able to generate multiple stubs at once
(simply specify ``-m MODULE`` several times).

.. code-block:: bash

   $ python -m nanobind.stubgen -m my_ext -M py.typed
   Module "my_ext" ..
     - importing ..
     - analyzing ..
     - writing stub "my_ext.pyi" ..
     - writing marker file "py.typed" ..

Unless an output file (``-o``) or output directory (``-O``) is specified, this
places the ``.pyi`` files directly into the module. Existing stubs are
overwritten without warning.

The program has the following command line options:

.. code-block:: text

   usage: python -m nanobind.stubgen [-h] [-o FILE] [-O PATH] [-i PATH] [-m MODULE]
                                     [-r] [-M FILE] [-P] [-D] [-q]

   Generate stubs for nanobind-based extensions.

   options:
     -h, --help                    show this help message and exit
     -o FILE, --output-file FILE   write generated stubs to the specified file
     -O PATH, --output-dir PATH    write generated stubs to the specified directory
     -i PATH, --import PATH        add the directory to the Python import path (can
                                   specify multiple times)
     -m MODULE, --module MODULE    generate a stub for the specified module (can
                                   specify multiple times)
     -r, --recursive               recursively process submodules
     -M FILE, --marker-file FILE   generate a marker file (usually named 'py.typed')
     -p FILE, --pattern-file FILE  apply the given patterns to the generated stub
                                   (see the docs for syntax)
     -P, --include-private         include private members (with single leading or
                                   trailing underscore)
     -D, --exclude-docstrings      exclude docstrings from the generated stub
     -q, --quiet                   do not generate any output in the absence of failures


Python interface
^^^^^^^^^^^^^^^^

Finally, you can import ``stubgen`` into your own Python programs and use it to
programmatically generate stubs with a finer degree of control.

To do so, construct an instance of the ``StubGen`` class and repeatedly call
``.put()`` to register modules or contents within the modules (specific
methods, classes, etc.). Afterwards, the ``.get()`` method returns a string
containing the stub declarations.


.. code-block:: python

   from nanobind.stubgen import StubGen
   import my_module

   sg = StubGen()
   sg.put(my_module)
   print(sg.get())

Note that for now, the ``nanobind.stubgen.StubGen`` API is considered
experimental and not subject to the semantic versioning policy used by the
nanobind project.

.. _pattern_files:

Pattern files
-------------

In complex binding projects requiring static type checking, the previously
discussed mechanisms for controlling typed signatures (:cpp:class:`nb::sig
<sig>`, :cpp:class:`nb::typed <typed>`) may be insufficient. Two common reasons
are as follows:

- the ``@typing.overload`` chain associated with a function may sometimes
  require significant deviations from the actual overloads present on the C++
  side.

- Some members of a module could be inherited from existing Python packages or
  extension libraries, in which case patching their signature via
  :cpp:class:`nb::sig <sig>` is not even an option.

``stubgen`` supports *pattern files* as a last-resort solution to handle such
advanced needs. These are files written in a *domain-specific language* (DSL)
that specifies replacement patterns to dynamically rewrite stubs during
generation. To use one, simply add it to the :cmake:command:`nanobind_add_stub`
command.

.. code-block:: cmake

   nanobind_add_stub(
     ...
     PATTERN_FILE  <PATH>
     ...
   )

A pattern file contains sequence of patterns. Each pattern consists of a query
and an indented replacement block to be applied when the query matches.

.. code-block:: text

   # This is the first pattern
   query 1:
       replacement 1

   # And this is the second one
   query 2:
       replacement 2

Empty lines and lines beginning with ``#`` are ignored. The amount of
indentation is arbitrary: ``stubgen`` will re-indent the replacement as needed
based on where the query matched.

When the stub generator traverses the module, it computes the *fully qualified
name* of every type, function, property, etc. (for example:
``"my_ext.MyClass.my_function"``). The queries in a pattern file are checked
against these qualified names one by one until the first one matches.

For example, suppose that we had the following lackluster stub entry:

.. code-block:: python

   class MyClass:
       def my_function(arg: object) -> object: ...

The pattern below matches this function stub and inserts an alternative with
two typed overloads.

.. code-block:: text

   my_ext.MyClass.my_function:
       @overload
       def my_function(arg: int) -> int:
           """A helpful docstring"""

       @overload
       def my_function(arg: str) -> str: ...

Patterns can also *remove* entries, by simply not specifying a replacement
block. Also, queries don't have to match the entire qualified name. For
example, the following pattern deletes all occurrences of anything
containing the string ``secret`` somewhere in its name

.. code-block:: text

   secret:

In fact (you may have guessed it), the queries are *regular expressions*! The
query supports all features of Python's builtin `re
<https://docs.python.org/3/library/re.html>`__ library.

When the query uses *groups*, the replacement block may access the contents of
each numbered group using using the syntax ``\1``, ``\2``, etc. This permits
writing generic patterns that can be applied to a number of stub entries at
once:

.. code-block:: text

   __(eq|ne)__:
       def __\1__(self, arg, /) -> bool: ...

Named groups are also supported:

.. code-block:: text

   __(?P<op>eq|ne)__:
       def __\op__(self, arg, /) -> bool : ...

Finally, sometimes, it is desirable to rewrite only the signature of a function
in a stub but to keep its docstring so that it doesn't have to be copied into
the pattern file. The special escape code ``\doc`` references the previously
existing docstring.

.. code-block:: text

   my_ext.lookup:
       def lookup(array: Array[T], index: int) -> T:
           \doc

If your replacement rule requires additional types to work (e.g., from ``typing.*``),
you may use the special ``\from`` escape code to import them:

.. code-block:: text

   @overload
   my_ext.lookup:
       \from typing import Optional as _Opt, Literal
       def lookup(array: Array[T], index: Literal[0] = 0) -> _Opt[T]:
           \doc

You may also add free-form text the beginning or the end of the generated stub.
To do so, add an entry that matches on ``module_name.__prefix__`` or
``module_name.__suffix__``.
