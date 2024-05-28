.. _basics:

.. cpp:namespace:: nanobind

Creating your first extension
#############################

This section assumes that you have followed the instructions to :ref:`install
<installing>` nanobind and set up a basic :ref:`build system <building>`.

We are now ready to define a first basic extension that wraps a function
to add two numbers. Create a new file ``my_ext.cpp`` with the following
contents (the meaning of this code will be explained shortly):

.. code-block:: cpp

   #include <nanobind/nanobind.h>

   int add(int a, int b) { return a + b; }

   NB_MODULE(my_ext, m) {
       m.def("add", &add);
   }

Afterwards, you should be able to compile and run the extension.

Building using CMake
--------------------

Launch ``cmake`` in the project directory to set up a build system that will
write all output into a separate ``build`` subdirectory.

.. code-block:: bash

   cmake -S . -B build

.. note::

   If this step fails with an error message saying that Python cannot be
   found, you will need to install a suitable Python 3 development package.

   For example, on Ubuntu you would run:

   .. code-block:: bash

       apt install libpython3-dev

   On Windows, you we recommend downloading and running one of the `installers
   <https://www.python.org/downloads>`_ provided by the Python foundation.

.. note::

   If you have multiple versions of Python on your system, the CMake build
   system may not find the specific version you had in mind. This is
   problematic: extension built for one version of Python usually won't run on
   another version. You can provide a hint to the build system to help it find
   a specific version.

   In this case, delete the ``build`` folder (if you already created one) and
   re-run `cmake` while specifying the command line parameter ``-DPython_EXECUTABLE=<path to python executable>``.

   .. code-block:: bash

      rm -Rf build
      cmake -S . -B build -DPython_EXECUTABLE=<path to python executable>

Assuming the ``cmake`` ran without issues, you can now compile the extension using
the following command:

.. code-block:: bash

   cmake --build build

Finally, navigate into the ``build`` directory and launch an interactive Python
session:

.. code-block:: bash

   cd build
   python3

(The default build output directory is different on Windows: use ``cd build\Debug`` and ``python`` instead of the above.)

You should be able to import the extension and call the newly defined function ``my_ext.add()``.

.. code-block:: pycon

   Python 3.11.1 (main, Dec 23 2022, 09:28:24) [Clang 14.0.0 (clang-1400.0.29.202)] on darwin
   Type "help", "copyright", "credits" or "license" for more information.
   >>> import my_ext
   >>> my_ext.add(1, 2)
   3


Binding functions
-----------------

Let's step through the example binding code to understand what each line does.
The directive on the first line includes the core parts of nanobind:

.. code-block:: cpp

    #include <nanobind/nanobind.h>

nanobind also provides many optional add-on components that are aren't
included by default. They are discussed throughout this documentation along
with pointers to the header files that must be included when using them.

Next is the function to be exposed in Python, followed by the
mysterious-looking :c:macro:`NB_MODULE` macro.

.. code-block:: cpp

   int add(int a, int b) { return a + b; }

   NB_MODULE(my_ext, m) {
       m.def("add", &add);
   }

:c:macro:`NB_MODULE(my_ext, m) <NB_MODULE>` declares the extension with the
name ``my_ext``. This name **must** match the extension name provided to the
``nanobind_add_module()`` function in the CMake build system---otherwise,
importing the extension will fail with an obscure error about a missing
symbol. The second argument (``m``) names a variable of
type :cpp:class:`nanobind::module_` that represents the created module.

The part within curly braces (``{``, ``}``) consists of a sequence of
statements that initialize the desired function and class bindings. It is best
thought of as the ``main()`` function that will run when a user imports the
extension into a running Python session.

In this case, there is only one binding declaration that wraps the ``add``
referenced using the ampersand (``&``) operator. nanobind determines the
function's type signature and generates the necessary binding code. All of
this happens automatically at compile time.

.. note::

    Notice how little code was needed to expose our function to Python: all
    details regarding the function’s parameters and return value were
    automatically inferred using template metaprogramming. This overall
    approach and the used syntax go back to `Boost.Python
    <https://github.com/boostorg/python>`_, though the implementation in
    nanobind is very different.

.. _keyword_and_default_args:

Keyword and default arguments
-----------------------------

There are limits to what nanobind can determine at compile time. For example,
the argument names were lost and calling ``add()`` in Python using keyword
arguments fails:

.. code-block:: pycon

   >>> my_ext.add(a=1, b=2)
   TypeError: add(): incompatible function arguments. The following argument types are supported:
       1. add(arg0: int, arg1: int, /) -> int

   Invoked with types: kwargs = { a: int, b: int }

Let's improve the bindings to fix this. We will also add a docstring and a
default ``b`` argument so that ``add()`` increments when only one value is
provided. The modified binding code looks as follows:

.. code-block:: cpp

   #include <nanobind/nanobind.h>

   namespace nb = nanobind;
   using namespace nb::literals;

   int add(int a, int b = 1) { return a + b; }

   NB_MODULE(my_ext, m) {
       m.def("add", &add, "a"_a, "b"_a = 1,
             "This function adds two numbers and increments if only one is provided.");
   }

Let's go through all of the changed lines. The first sets up a short
namespace alias named ``nb``:

.. code-block:: cpp

   namespace nb = nanobind;

This is convenient because binding code usually ends up referencing many
classes and functions from this namespace. The subsequent ``using``
declaration is optional and enables a convenient syntax for annotating
function arguments:

.. code-block:: cpp

   using namespace nb::literals;

Without it, you would have to change every occurrence of the pattern ``"..."_a``
to the more verbose ``nb::arg("...")``.

The function binding declaration includes several changes. It is common to pile
on a few attributes and modifiers in :cpp:func:`.def(...) <module_::def()>`
binding declarations, which can be specified in any order.

.. code-block:: cpp

   m.def("add", &add, "a"_a, "b"_a = 1,
         "This function adds two numbers and increments if only one is provided.");

The string at the end is a `docstring <https://peps.python.org/pep-0257/>`_
that will later show up in generated documentation. The argument annotations
(``"a"_a, "b"_a``) associate parameters with names for keyword-based
argument passing.

Besides argument names, nanobind also cannot infer *default arguments*---you
*must repeat them* in the binding declaration. In the above snippet, the
``"b"_a = 1`` annotation informs nanobind about the value of the default
argument.

Exporting values
----------------

To export a value, use the :cpp:func:`attr() <nanobind::detail::api::attr>`
function to register it in the module as shown below. Bound classes and
built-in types are automatically converted when they are assigned in this
way.

.. code-block:: cpp

    m.attr("the_answer") = 42;

.. _docstrings:

Docstrings
----------

Let's add one more bit of flourish by assigning a docstring to the extension
module itself. Include the following line anywhere in the body of the
``NB_MODULE() {...}`` declaration:

.. code-block:: cpp

    m.doc() = "A simple example python extension";

After recompiling the extension, you should be able to view the associated
documentation using the ``help()`` builtin or the ``?`` operator in
IPython.

.. code-block:: pycon

   >>> import my_ext
   >>> help(my_ext)

   Help on module my_ext:

   NAME
       my_ext - A simple example python extension

   DATA
       add = <nanobind.nb_func object>
           add(a: int, b: int = 1) -> int

           This function adds two numbers and increments if only one is provided

       the_answer = 42

   FILE
       /Users/wjakob/my_ext/my_ext.cpython-311-darwin.so

The automatically generated documentation covers functions, classes,
parameter and return value type information, argument names, and default
arguments.


.. _binding_types:

Binding a custom type
---------------------

Let's now turn to an object oriented example. We will create bindings for a
simple C++ type named ``Dog`` defined as follows:

.. code-block:: cpp

   #include <string>

   struct Dog {
       std::string name;

       std::string bark() const {
           return name + ": woof!";
       }
   };

The ``Dog`` bindings look as follows:

.. code-block:: cpp

   #include <nanobind/nanobind.h>
   #include <nanobind/stl/string.h>

   namespace nb = nanobind;

   NB_MODULE(my_ext, m) {
       nb::class_<Dog>(m, "Dog")
           .def(nb::init<>())
           .def(nb::init<const std::string &>())
           .def("bark", &Dog::bark)
           .def_rw("name", &Dog::name);
   }

Let's look at selected lines of this example, starting with the added include directive:

.. code-block:: cpp

   #include <nanobind/stl/string.h>

nanobind has a minimal core and initially doesn't know how to deal with STL
types like ``std::string``. This line imports a *type caster* that realizes a
bidirectional conversion (C++ ``std::string`` ↔ Python ``str``) to make the
example usable. An upcoming :ref:`documentation section <type_casters>` will
provide more detail on type casters and other alternatives.

The class binding declaration :class:`nb::class_\<T\>() <class_>` supports both
``class`` and ``struct``-style data structures.

.. code-block:: cpp

   nb::class_<Dog>(m, "Dog")

Here, it associates the C++ type ``Dog`` with a new Python type named ``"Dog"``
and installs it in the :cpp:class:`nb::module_ <module_>` ``m``.

Initially, this type is completely empty---it has no members and cannot be
instantiated. The subsequent chain of binding declarations binds two
constructor overloads (via :cpp:class:`nb::init\<...\>() <init>`), a method,
and the mutable ``name`` field (via :cpp:func:`.def_rw(..)
<class_::def_rw>`, where ``rw`` stands for read/write access).

.. code-block:: cpp

   .def(nb::init<>())
   .def(nb::init<const std::string &>())
   .def("bark", &Dog::bark)
   .def_rw("name", &Dog::name);

An interactive Python session demonstrating this example is shown below:

.. code-block:: pycon

   Python 3.11.1 (main, Dec 23 2022, 09:28:24) [Clang 14.0.0 (clang-1400.0.29.202)] on darwin
   Type "help", "copyright", "credits" or "license" for more information.
   >>> import my_ext
   >>> d = my_ext.Dog('Max')
   >>> print(d)
   <my_ext.Dog object at 0x1044540f0>
   >>> d.name
   'Max'
   >>> d.name = 'Charlie'
   >>> d.bark()
   'Charlie: woof!'

The example showed how to bind constructors, methods, and mutable fields. Many
other things can be bound using analogous :cpp:class:`nb::class_\<...\>
<class_>` methods:

.. list-table::
  :widths: 40 60
  :header-rows: 1

  * - Type
    - Method
  * - Methods & constructors
    - :cpp:func:`.def() <class_::def>`
  * - Fields
    - :cpp:func:`.def_ro() <class_::def_ro>`,
      :cpp:func:`.def_rw() <class_::def_rw>`
  * - Properties
    - :cpp:func:`.def_prop_ro() <class_::def_prop_ro>`,
      :cpp:func:`.def_prop_rw() <class_::def_prop_rw>`
  * - Static methods
    - :cpp:func:`.def_static() <class_::def_static>`
  * - Static fields
    - :cpp:func:`.def_ro_static() <class_::def_ro_static>`,
      :cpp:func:`.def_rw_static() <class_::def_rw_static>`
  * - Static properties
    - :cpp:func:`.def_prop_ro_static() <class_::def_prop_ro_static>`,
      :cpp:func:`.def_prop_rw_static() <class_::def_prop_rw_static>`

.. note::

    All of these binding declarations support :ref:`docstrings <docstrings>`,
    :ref:`keyword, and default argument <keyword_and_default_args>` annotations
    as before.

.. _binding_lambdas:

Binding lambda functions
------------------------

Note how ``print(d)`` produced a rather useless summary in the example above:

.. code-block:: pycon

    >>> print(d)
    <my_ext.Dog object at 0x1044540f0>

To address this, we can add a special Python method named ``__repr__`` that
returns a human-readable summary. Unfortunately, a corresponding function with
such functionality does not currently exist in the C++ type, and it would be
nice if we did not have to modify it. We can bind a *lambda function* to
achieve both goals:

.. code-block:: cpp

   nb::class_<Dog>(m, "Dog")
       // ... skipped ...
       .def("__repr__",
            [](const Dog &p) { return "<my_ext.Dog named '" + p.name + "'>"; });

nanobind supports both stateless [#f1]_ and stateful lambda closures.

Higher order functions
----------------------

nanobind's support for higher-order functions [#f2]_ further blurs the language
boundary. The snippet below extends the ``Dog`` class with higher-order
function ``bark_later()`` that calls :cpp:func:`nb::cpp_function()
<cpp_function>` to convert and return a *stateful* C++ lambda function
(``callback``) as a Python function object.

.. code-block:: cpp

   nb::class_<Dog>(m, "Dog")
       // ... skipped ...
       .def("bark_later", [](const Dog &p) {
           auto callback = [name = p.name] {
               nb::print(nb::str("{}: woof!").format(name));
           };
           return nb::cpp_function(callback);
       });

The lambda function captures the ``Dog::name()`` property (a C++
``std::string``) and in turn calls Python functions (:cpp:func:`nb::print()
<print>`, :cpp:func:`nb::str::format() <str::format>`) to print onto the
console. Here is an example use of the binding in Python:

.. code-block:: pycon

   >>> f = d.bark_later()
   >>> f
   <nanobind.nb_func object at 0x10537c140>
   >>> f()
   Charlie: woof!

Wrap-up
-------

This concludes the basic part of the documentation, which provided a first
taste of nanobind and typical steps needed to create a custom extension.

The upcoming intermediate-level material covers performance and
safety-critical points:

- C++ and Python can exchange information in various different ways. 

  *Which one is best for a particular task?*

- A bound object can simultaneously exist in both C++ and Python.

  *Who owns it?*
  *When is it safe to delete it?*

Following these topics, the documentation revisits function and class
bindings in full detail.

.. [#f1] Stateless closures are those with an empty pair of brackets ``[]`` as
   the capture object.

.. [#f2] Higher-order functions are functions that take functions as arguments
   and/or return them.

