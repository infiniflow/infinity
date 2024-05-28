.. cpp:namespace:: nanobind

.. _exchange:

Exchanging information
======================

nanobind offers three fundamentally different ways of exchanging information
between Python and C++. Depending on the task at hand, one will usually be
preferable over the others, hence it is important to be aware of their
advantages and disadvantages.

.. _type_casters:

Option 1: Type Casters
----------------------

A *type caster* translates C++ objects into equivalent Python
objects and vice versa. The illustration below shows a translation between
C++ (blue) and Python (green) worlds, where a ``std::vector<int>`` instance
converts from/to a Python ``list`` containing ``int`` objects.

.. only:: not latex

   .. image:: images/caster-light.svg
     :width: 400
     :align: center
     :class: only-light

   .. image:: images/caster-dark.svg
     :width: 400
     :align: center
     :class: only-dark

.. only:: latex

   .. image:: images/caster-light.svg
     :width: 400
     :align: center

**Example**: The following function doubles the entries of an STL vector and
returns the result.

.. code-block:: cpp

    using IntVector = std::vector<int>;

    IntVector double_it(const IntVector &in) {
        IntVector out(in.size());
        for (size_t i = 0; i < in.size(); ++i)
            out[i] = in[i] * 2;
        return out;
    }

To expose it in Python, we can use the ``std::vector<...>`` type caster that
is located in an optional header file named ``nanobind/stl/vector.h``:

.. code-block:: cpp

    #include <nanobind/stl/vector.h>

    NB_MODULE(my_ext, m) {
        m.def("double_it", &double_it);
    }

That's all there is to it. The Python version of the function features
an automatically generated docstring, type checks, and (if needed) error reporting.

.. code-block:: pycon

    >>> import my_ext
    >>> my_ext.double_it([1, 2, 3])
    [2, 4, 6]

    >>> my_ext.double_it([1, 2, 'foo'])
    TypeError: double_it(): incompatible function arguments. The following argument types are supported:
        1. double_it(arg: list[int], /) -> list[int]

What are the implications of using type casters?

**Pro**: this approach is simple and convenient, especially when standard
(STL) types are involved. Usually, all that is needed is an ``#include``
directive to pull in the right header file. Complex nested types (e.g.
vectors of hash tables of strings) work automatically by combining type
casters recursively.

The following table lists the currently available type casters along with links
to external projects that provide further casters:

.. list-table::
  :widths: 42 48
  :header-rows: 1

  * - Type
    - Type caster header
  * - ``char``, ``char*``, ``void*``, ``nullptr_t``, ``bool``, ``int``,
      ``unsigned int``, ``long``, ``unsigned long``, ...
    - Built-in (no include file needed)
  * - ``std::array<..>``
    - ``#include <nanobind/stl/array.h>``
  * - ``std::chrono::duration<..>``, ``std::chrono::time_point<..>``
      (:ref:`more details <chrono_conversions>`)
    - ``#include <nanobind/stl/chrono.h>``
  * - ``std::complex<..>``
    - ``#include <nanobind/stl/complex.h>``
  * - ``std::filesystem::path``
    - ``#include <nanobind/stl/filesystem.h>``
  * - ``std::function<..>``
    - ``#include <nanobind/stl/function.h>``
  * - ``std::list<..>``
    - ``#include <nanobind/stl/list.h>``
  * - ``std::map<..>``
    - ``#include <nanobind/stl/map.h>``
  * - ``std::optional<..>``
    - ``#include <nanobind/stl/optional.h>``
  * - ``std::pair<..>``
    - ``#include <nanobind/stl/pair.h>``
  * - ``std::set<..>``
    - ``#include <nanobind/stl/set.h>``
  * - ``std::string``
    - ``#include <nanobind/stl/string.h>``
  * - ``std::string_view``
    - ``#include <nanobind/stl/string_view.h>``
  * - ``std::wstring``
    - ``#include <nanobind/stl/wstring.h>``
  * - ``std::tuple<..>``
    - ``#include <nanobind/stl/tuple.h>``
  * - ``std::shared_ptr<..>``
    - ``#include <nanobind/stl/shared_ptr.h>``
  * - ``std::unique_ptr<..>``
    - ``#include <nanobind/stl/unique_ptr.h>``
  * - ``std::unordered_set<..>``
    - ``#include <nanobind/stl/unordered_set.h>``
  * - ``std::unordered_map<..>``
    - ``#include <nanobind/stl/unordered_map.h>``
  * - ``std::variant<..>``
    - ``#include <nanobind/stl/variant.h>``
  * - ``std::vector<..>``
    - ``#include <nanobind/stl/vector.h>``
  * - ``nb::ndarray<..>``
    - ``#include <nanobind/ndarray.h>``
  * - ``Eigen::Matrix<..>``, ``Eigen::Array<..>``, ``Eigen::Ref<..>``, ``Eigen::Map<..>``
    - ``#include <nanobind/eigen/dense.h>``
  * - ``Eigen::SparseMatrix<..>``
    - ``#include <nanobind/eigen/sparse.h>``
  * - Apache Arrow types
    -  `https://github.com/maximiliank/nanobind_pyarrow <https://github.com/maximiliank/nanobind_pyarrow>`__
  * - ...
    - Please reach out if you have additions to this list.


**Con**: Every transition between the Python and C++ side will generally require a
conversion step (in this case, to re-create all list elements). This can be
wasteful when the other side only needs to access a small part of the data.
Conversely, the overhead should not be a problem when the data is fully
"consumed" following conversion.

A select few type casters (``std::unique_ptr<..>``, ``std::shared_ptr<..>``,
:cpp:class:`nb::ndarray <ndarray>`, and ``Eigen::*``) are special in the sense
that they can perform a type conversion *without* copying the underlying data.
Besides those few exceptions type casting always implies that a copy is made.

.. _type_caster_mutable:

Mutable reference issue
^^^^^^^^^^^^^^^^^^^^^^^

Another subtle limitation of type casters is that they
don't propagate updates through mutable references. Consider the
following alternative implementation of the ``double_it`` function:

.. code-block:: cpp

    void double_it(IntVector &in) {
        for (int &value : in)
            value *= 2;
    }

nanobind can wrap this function without problems, but it won't behave as
expected:

.. code-block:: pycon

    >>> x = [1, 2, 3]
    >>> my_ext.double_it(x)
    >>> x
    [1, 2, 3]  # <-- oops, unchanged!

*How could this happen?*
The reason is that type casters convert function arguments and return values once, but further
changes will not automatically propagate across the language barrier because
the representations are not intrinsically linked to each other. This problem is
not specific to STL types---for example, the following function will similarly
not update its argument once exposed in Python.

.. code-block:: cpp

    void double_it(int &in) { in *= 2; }

This is because builtin types like ``int``, ``str``, ``bool``, etc., are
all handled by type casters.

A simple alternative to propagate updates while retaining the convenience of
type casters is to bind a small wrapper lambda function that returns a tuple
with all output arguments. An example:

.. code-block:: cpp

    int foo(int &in) { in *= 2; return std::sqrt(in); }

And the binding code

.. code-block:: cpp

   m.def("foo", [](int i) { int rv = foo(i); return std::make_tuple(rv, i); });

In this case, a type caster (``#include <nanobind/stl/tuple.h``) must be
included to handle the ``std::tuple<int, int>`` return value.

.. _bindings:

Option 2: Bindings
------------------

*Bindings* expose C++ types in Python; the ability to create them is the
main feature of nanobind. In the list-of-integer example, they cause Python
to interpret ``std::vector<int>`` as a new Python type called
``my_ext.IntVector``.

.. only:: not latex

   .. image:: images/binding-light.svg
     :width: 400
     :align: center
     :class: only-light

   .. image:: images/binding-dark.svg
     :width: 400
     :align: center
     :class: only-dark

.. only:: latex

   .. image:: images/binding-light.svg
     :width: 400
     :align: center

**Example**: to switch the previous example to bindings, we first replace
the type caster header (`nanobind/stl/vector.h
<https://github.com/wjakob/nanobind/blob/master/include/nanobind/stl/vector.h>`_)
by its binding variant (`nanobind/stl/bind_vector.h
<https://github.com/wjakob/nanobind/blob/master/include/nanobind/stl/bind_vector.h>`_)
and then invoke the :cpp:func:`nb::bind_vector\<T\>() <bind_vector>` function to create a *new
Python type named* ``IntVector`` within the module ``m``.

.. code-block:: cpp
   :emphasize-lines: 1, 9

    #include <nanobind/stl/bind_vector.h>

    using IntVector = std::vector<int>;
    IntVector double_it(const IntVector &in) { /* .. omitted .. */ }

    namespace nb = nanobind;

    NB_MODULE(my_ext, m) {
        nb::bind_vector<IntVector>(m, "IntVector");
        m.def("double_it", &double_it);
    }

Any function taking or returning integer vectors will now use the type
binding. In the Python session below, nanobind performs an implicit
conversion from the Python list ``[1, 2, 3]`` to a ``my_ext.IntVector``
before calling the ``double_it`` function.

.. code-block:: pycon

    >>> import my_ext
    >>> my_ext.double_it([1, 2, 3])
    my_ext.IntVector([2, 4, 6])

    >>> my_ext.double_it.__doc__
    'double_it(arg: my_ext.IntVector, /) -> my_ext.IntVector'

Let's go through the implications of using bindings:

**Pro**: bindings don't require the costly conversion step when crossing the
language boundary. They also support mutable references, so the :ref:`issue
discussed in the context of type casters <type_caster_mutable>` does not
arise. Sometimes, binding is the only available option: when a C++ type does
not have an equivalent Python type, casting simply does not make sense.

**Con**: Creating good bindings that feel natural in Python requires some
additional work. We cheated in this example by relying on the
:cpp:func:`nb::bind_vector\<T\>() <bind_vector>` helper function that did
all the heavy lifting. Such helpers are currently only available for a few
special cases (vectors, ordered/unordered maps, iterators):

.. list-table::
  :widths: 42 48
  :header-rows: 1

  * - Type
    - Binding helper header
  * - ``std::vector<..>``
    - ``#include <nanobind/stl/bind_vector.h>``
      (:ref:`docs <vector_bindings>`)
  * - ``std::map<..>``
    - ``#include <nanobind/stl/bind_map.h>``
      (:ref:`docs <map_bindings>`)
  * - ``std::unordered_map<..>``
    - ``#include <nanobind/stl/bind_map.h>``
      (:ref:`docs <map_bindings>`)
  * - Forward iterators
    - ``#include <nanobind/make_iterator.h>``
      (:ref:`docs <iterator_bindings>`)
  * - Other types
    - See the previous example on :ref:`binding custom types <binding_types>`.

In general, you will need to write the binding code yourself. The previous
section on :ref:`binding custom types <binding_types>` showed an example of
such a type binding.

.. _wrappers:

Option 3: Wrappers
------------------

The last option is only rarely used, but it can be powerful alternative in
some cases. nanobind provides *wrapper* classes to use Python types within
C++. You can think of this as a kind of *reverse binding*. For example, a
Python list can be accessed through the :cpp:class:`nb::list <list>` type:

.. only:: not latex

   .. image:: images/wrapper-light.svg
     :width: 400
     :align: center
     :class: only-light

   .. image:: images/wrapper-dark.svg
     :width: 400
     :align: center
     :class: only-dark

.. only:: latex

   .. image:: images/wrapper-light.svg
     :width: 400
     :align: center

This is what the example looks like when expressed using
:cpp:class:`nb::list <list>` and :cpp:class:`nb::int_ <int_>`.

.. code-block:: cpp

    #include <nanobind/nanobind.h>

    namespace nb = nanobind;

    nb::list double_it(nb::list l) {
        nb::list result;
        for (nb::handle h: l)
            result.append(h * nb::int_(2));
        return result;
    }

    NB_MODULE(my_ext, m) {
        m.def("double_it", &double_it);
    }


The implications of using wrappers are:

**Pro**: Wrappers require no copying or type conversion. With them, C++ begins
to resemble dynamically typed Python code and can perform highly general
operations on Python objects. Wrappers are useful to tap into the powerful
Python software ecosystem (NumPy, Matplotlib, PyTorch, etc).

**Con**: Functions based on wrappers cannot run without Python. In contrast
to option 1 (:ref:`type casters <type_casters>`) and 2 (:ref:`bindings
<bindings>`), we can no longer reuse an existing function and process its
arguments and return value to interface the Python and C++ worlds: the
entire function must be rewritten using nanobind-specific wrapper types.
Every operation will translate into a corresponding Python C API call, which
means that wrappers aren't suitable for performance-critical loops or
multithreaded computations.

The following wrappers are available and require no additional include
directives:
:cpp:class:`any`,
:cpp:class:`bytes`, :cpp:class:`callable`, :cpp:class:`capsule`,
:cpp:class:`dict`, :cpp:class:`ellipsis`, :cpp:class:`handle`,
:cpp:class:`handle_t\<T\> <handle_t>`,
:cpp:class:`bool_`, :cpp:class:`int_`, :cpp:class:`float_`,
:cpp:class:`iterable`, :cpp:class:`iterator`,
:cpp:class:`list`, :cpp:class:`mapping`,
:cpp:class:`module_`, :cpp:class:`object`, :cpp:class:`set`, :cpp:class:`sequence`,
:cpp:class:`slice`, :cpp:class:`str`, :cpp:class:`tuple`,
:cpp:class:`weakref`,
:cpp:class:`type_object`, :cpp:class:`type_object_t\<T\> <handle_t>`,
:cpp:class:`args`, and :cpp:class:`kwargs`.

Discussion
----------

The choices outlined above are more fine-grained than they may appear. For
example, it is possible to use type casters, bindings, and wrappers to handle
multiple arguments of *a single function*.

They can also be combined *within* a single function argument. For example, you
can type cast a ``std::vector<T>`` containing bindings or wrappers.

In general, we recommend that you use

1. type casters for STL containers, and

2. bindings for other custom types.

If the former turn out to be a performance bottleneck, it is easy to replace
them with bindings or wrappers later on. Wrappers are only rarely useful;
you will usually know it when you need them.
