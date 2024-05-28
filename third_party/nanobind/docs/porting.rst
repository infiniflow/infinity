.. cpp:namespace:: nanobind

.. _porting:

Porting guide
=============

The API of nanobind is *extremely* similar to that of `pybind11
<https://pybind11.readthedocs.io/en/stable/>`_, which makes porting existing
projects easy. Parts of the nanobind documentation are almost verbatim copies
pybind11's documentation.

A number of noteworthy changes are documented below.

Namespace
---------

nanobind types and functions are located in the ``nanobind`` namespace. The
following shorthand alias is recommended and used throughout the documentation:

.. code-block:: cpp

   namespace nb = nanobind;

Name changes
------------

The following macros, types, and functions were renamed:

.. list-table::
  :widths: 42 48
  :header-rows: 1

  * - pybind11
    - nanobind

  * - ``PYBIND11_MODULE(..)``
    - :c:macro:`NB_MODULE(..) <NB_MODULE>`
  * - ``PYBIND11_OVERRIDE_*(..)``
    - :c:macro:`NB_OVERRIDE_*(..) <NB_OVERRIDE>`
  * - ``error_already_set``
    - :cpp:class:`python_error`
  * - ``type::of<T>()``
    - :cpp:func:`type\<T\>() <type>`
  * - ``type``
    - :cpp:class:`type_object`
  * - ``reinterpret_borrow<T>(x)``
    - :cpp:func:`borrow\<T\>(x) <borrow>`
  * - ``reinterpret_steal<T>(x)``
    - :cpp:func:`steal\<T\>(x) <steal>`
  * - ``.def_readwrite(..)``
    - :cpp:func:`.def_rw(..) <class_::def_rw>`
  * - ``.def_readonly(..)``
    - :cpp:func:`.def_ro(..) <class_::def_ro>`
  * - ``.def_property(..)``
    - :cpp:func:`.def_prop_rw(..) <class_::def_prop_rw>`
  * - ``.def_property_readonly(..)``
    - :cpp:func:`.def_prop_ro(..) <class_::def_prop_ro>`
  * - ``.def_readwrite_static(..)``
    - :cpp:func:`.def_rw_static(..) <class_::def_rw_static>`
  * - ``.def_readonly_static(..)``
    - :cpp:func:`.def_ro_static(..) <class_::def_ro_static>`
  * - ``.def_property_static(..)``
    - :cpp:func:`.def_prop_rw_static(..) <class_::def_prop_rw_static>`
  * - ``.def_property_readonly_static(..)``
    - :cpp:func:`.def_prop_ro_static(..) <class_::def_prop_ro_static>`
  * - ``register_exception<T>``
    - :cpp:class:`exception\<T\> <exception>`


None/null arguments
-------------------

In contrast to pybind11, nanobind does *not* permit ``None``-valued function
arguments by default. You must enable them explicitly using the
:cpp:func:`arg::none() <arg::none>` argument annotation, e.g.:

.. code-block:: cpp

   m.def("func", &func, "arg"_a.none());

It is also possible to set a ``None`` default value, in which case the
:cpp:func:`.none() <arg::none>` annotation can be omitted.

.. code-block:: cpp

   m.def("func", &func, "arg"_a = nb::none());

``None``-valued arguments are only supported by two of the three parameter
passing styles described in the section on :ref:`information exchange
<exchange>`. In particular, they are supported by :ref:`bindings <bindings>`
and :ref:`wrappers <wrappers>`, *but not* by :ref:`type casters
<type_casters>`.

Shared pointers and holders
---------------------------

When nanobind instantiates a C++ type within Python, the resulting instance
data is stored *within* the created Python object ("``PyObject``").
Alternatively, when an already existing C++ instance is transferred to Python
via a function return value and :cpp:enumerator:`rv_policy::reference`,
:cpp:enumerator:`rv_policy::reference_internal`, or
:cpp:enumerator:`rv_policy::take_ownership`, nanobind creates a smaller wrapper
``PyObject`` that only stores a pointer to the instance data.

This is *very different* from pybind11, where the instance ``PyObject``
contained a *holder type* (typically ``std::unique_ptr<T>``) storing a pointer
to the instance data. Dealing with holders caused inefficiencies and introduced
complexity; they were therefore removed in nanobind. This has implications on
object ownership, shared ownership, and interactions with C++ shared/unique
pointers. The :ref:`intermediate <ownership>` and :ref:`advanced
<ownership_adv>` sections on object ownership provide further detail.

The gist is that it is no longer necessary to specify holder types in the type
declaration:

*pybind11*:

.. code-block:: cpp

   py::class_<MyType, std::shared_ptr<MyType>>(m, "MyType")

*nanobind*:

.. code-block:: cpp

   nb::class_<MyType>(m, "MyType")

To bind functions that exchange shared/unique pointers, you must add one or
both of the following include directives to your code:

.. code-block:: cpp

   #include <nanobind/stl/unique_ptr.h>
   #include <nanobind/stl/shared_ptr.h>

Binding functions that take ``std::unique_ptr<T>`` arguments involves some
limitations that can be avoided by changing their signatures to
``std::unique_ptr<T, nb::deleter<T>>`` (:ref:`details <unique_ptr>`).

Use of ``std::enable_shared_from_this<T>`` is permitted, but since
nanobind does not use holder types, an object
constructed in Python will typically not have any associated
``std::shared_ptr<T>`` until it is passed to a C++ function that
accepts ``std::shared_ptr<T>``. That means a C++ function that accepts
a raw ``T*`` and calls ``shared_from_this()`` on it might stop working
when ported from pybind11 to nanobind. You can solve this problem
by always passing such objects across the Python/C++ boundary as
``std::shared_ptr<T>`` rather than as ``T*``. See the :ref:`advanced section
on object ownership <enable_shared_from_this>` for more details.

Custom constructors
-------------------
In pybind11, custom constructors (i.e. ones that do not already exist in the
C++ class) could be specified as a lambda function returning an instance of
the desired type.

.. code-block:: cpp

   py::class_<MyType>(m, "MyType")
       .def(py::init([](int) { return MyType(...); }));

Unfortunately, the implementation of this feature was quite complex and
often required further internal calls to the move or copy
constructor. nanobind instead reverts to how pybind11 originally
implemented this feature using in-place construction (`placement
new <https://en.wikipedia.org/wiki/Placement_syntax>`_):

.. code-block:: cpp

   nb::class_<MyType>(m, "MyType")
       .def("__init__", [](MyType *t) { new (t) MyType(...); });

The provided lambda function will be called with a pointer to uninitialized
memory that has already been allocated (this memory region is co-located
with the Python object for reasons of efficiency). The lambda function can
then either run an in-place constructor and return normally (in which case
the instance is assumed to be correctly constructed) or fail by raising an
exception.

To turn an existing factory function into a constructor, you will need to
combine the above pattern with an invocation of the move/copy-constructor,
e.g.:

.. code-block:: cpp

   nb::class_<MyType>(m, "MyType")
       .def("__init__", [](MyType *t) { new (t) MyType(MyType::create()); });

Implicit conversions
--------------------

In pybind11, implicit conversions were specified using a follow-up function
call. This also works in nanobind, but it is recommended that you already
specify them within the constructor declaration:

*pybind11*:

.. code-block:: cpp

   py::class_<MyType>(m, "MyType")
       .def(py::init<MyOtherType>());

   py::implicitly_convertible<MyOtherType, MyType>();

*nanobind*:

.. code-block:: cpp

   nb::class_<MyType>(m, "MyType")
       .def(nb::init_implicit<MyOtherType>());


Trampoline classes
------------------
Trampolines, i.e., polymorphic class implementations that forward virtual
function calls to Python, now require an extra :c:macro:`NB_TRAMPOLINE(parent,
size) <NB_TRAMPOLINE()>` declaration, where ``parent`` refers to the parent class
and ``size`` is at least as big as the number of :c:macro:`NB_OVERRIDE_*() <NB_OVERRIDE>`
calls. nanobind caches information to enable efficient function dispatch, for
which it must know the number of trampoline "slots".

The macro ``PYBIND11_OVERRIDE_*(..)`` required the base type and return value
as the first two arguments. This information is no longer needed in nanobind,
and the arguments should be removed in :c:macro:`NB_OVERRIDE_*()
<NB_OVERRIDE>`:

An example:

.. code-block:: cpp

   struct PyAnimal : Animal {
       NB_TRAMPOLINE(Animal, 1);

       std::string name() const override {
           NB_OVERRIDE(name);
       }
   };

Trampoline declarations with an insufficient size may eventually trigger a
Python ``RuntimeError`` exception with a descriptive label, e.g.:

.. code-block:: text

   nanobind::detail::get_trampoline('PyAnimal::what()'): the trampoline ran out of
   slots (you will need to increase the value provided to the NB_TRAMPOLINE() macro)

Iterator bindings
-----------------

Use of the :cpp:func:`nb::make_iterator() <make_iterator>`,
:cpp:func:`nb::make_key_iterator() <make_key_iterator>`, and
:cpp:func:`nb::make_value_iterator() <make_value_iterator>` functions requires
including the additional header file ``nanobind/make_iterator.h``. The
interface of these functions has also slightly changed: all take a Python scope
and a name as first and second arguments, which are used to permanently
"install" the iterator type (which is created on demand). See the `test suite
<https://github.com/wjakob/nanobind/blob/master/tests/test_make_iterator.cpp>`_
for a worked out example.

Type casters
------------

The API of custom type casters has changed *significantly*. The following
changes are needed:

- ``load()`` was renamed to ``from_python()``. The function now takes an extra
  ``uint8_t flags`` parameter (instead ``bool convert``, which is now
  represented by the flag ``nb::detail::cast_flags::convert``). A
  ``cleanup_list *`` pointer keeps track of Python temporaries that are created
  by the conversion, and which need to be deallocated after a function call has
  taken place.

  ``flags`` and ``cleanup`` should be passed to any recursive usage of
  ``type_caster::from_python()``. If casting fails due to a Python exception,
  the function should clear it (``PyErr_Clear()``) and return ``false``. If a
  severe error condition arises that should be reported, use Python warning API
  calls for this, e.g. ``PyErr_WarnFormat()``.

- ``cast()`` was renamed to ``from_cpp()``. The function takes a return value
  policy (as before) and a ``cleanup_list *`` pointer. If casting fails due to
  a Python exception, the function should *leave the error set* (note the
  asymmetry compared to ``from_python()``) and return ``nullptr``.

Note that the cleanup list is only available when ``from_python()`` or
``from_cpp()`` are called as part of function dispatch, while usage by
:cpp:func:`nb::cast() <cast>` may set ``cleanup`` to ``nullptr`` if implicit
conversions are not enabled. This case should be handled gracefully by refusing
the conversion if the cleanup list is absolutely required.

Type casters may not raise C++ exceptions. Both ``from_python()`` and
``from_cpp()`` must be annotated with ``noexcept``. Exceptions or failure
conditions caused by a conversion should instead be caught *within* the
function body and handled as follows:

- ``from_python()``: return ``false``. That's it. (Failed Python to C++
  conversion occur all the time while dispatching calls, causing nanobind
  to simply move to the next function overload. Dedicated error reporting would
  add undesirable overheads). In case of a severe internal error, use the
  CPython warning API (e.g., ``PyErr_Warn()``) to notify the user.

- ``from_cpp()``: a failure here is more serious, since a return value of a
  successfully evaluated cannot be converted, causing the call to fail. To
  provide further detail, use the CPython error API (e.g., ``PyErr_Format()``)
  and return an invalid handle (``return nb::handle();``).

The ``std::pair<T1, T2>`` type caster (`link
<https://github.com/wjakob/nanobind/blob/master/include/nanobind/stl/pair.h>`_)
may be useful as a starting point of custom implementations.

.. _removed:

Removed features
----------------

A number of pybind11 features are unavailable in nanobind. The list below
uses the following symbols:

- ○: This removal is a design choice. Use pybind11 if you need this feature.
- ●: Unclear, to be discussed.

Removed features include:

- ○ **Multiple inheritance**: this feature was a persistent source of
  complexity in pybind11 and it is one of the main casualties in creating
  nanobind.
- ○ **Holders**: nanobind instances co-locate instance data with a Python
  object instead of accessing it via a holder type. This is a major difference
  compared to pybind11 and will require changes to binding code that used
  custom holders (e.g. unique or shared pointers). The :ref:`intermediate
  <ownership>` and :ref:`advanced <ownership_adv>` sections on object ownership
  provide further detail.
- ○ **Multi-intepreter, Embedding**: The ability to embed Python in an
  executable or run several independent Python interpreters in the same process
  is unsupported. Nanobind caters to bindings only. Multi-interpreter support
  would require TLS lookups for nanobind data structures, which is undesirable.
- ○ **Function binding annotations**: The ``pos_only`` argument
  annotation was removed. However, the same behavior can be achieved by
  creating unnamed arguments; see the discussion in the section on
  :ref:`keyword-only arguments <kw_only>`.
- ○ **Metaclasses**: creating types with custom metaclasses is unsupported.
- ○ **Module-local bindings**: support was removed (both for types and exceptions).
- ○ **Custom allocation**: C++ classes with an overloaded or deleted ``operator
  new`` / ``operator delete`` are not supported.
- ○ **Compilation**: workarounds for buggy or non-standard-compliant
  compilers were removed and will not be reintroduced.
- ○ The ``options`` class for customizing docstring generation was removed.
- ○ The NumPy array class (``py::array``) was removed in exchange for a more
  powerful alternative (:cpp:class:`nb::ndarray\<..\> <nanobind::ndarray>`)
  that additionally supports CPU/GPU tensors produced by various frameworks
  (NumPy, PyTorch, TensorFlow, JAX, etc.). Its API is not compatible with
  pybind11, however.
- ● Buffer protocol binding (``.def_buffer()``) was removed in favor of
  :cpp:class:`nb::ndarray\<..\> <nanobind::ndarray>`.
- ● Support for evaluating Python files was removed.

Bullet points marked with ● may be reintroduced eventually, but this will
need to be done in a careful opt-in manner that does not affect code
complexity, binary size, and compilation/runtime performance of basic bindings
that don't depend on these features.
