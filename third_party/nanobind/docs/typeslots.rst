.. _typeslots:

.. cpp:namespace:: nanobind

Customizing type creation
=========================

nanobind exposes a low-level interface to install custom *type slots*
(``PyType_Slot`` in the `CPython API
<https://docs.python.org/3/c-api/type.html#c.PyType_Slot>`_) in newly
constructed types. This provides an escape hatch to realize features that were
not foreseen in the design of this library.

To use this feature, specify the :cpp:class:`nb::type_slots() <type_slots>`
annotation when creating the type.

.. code-block:: cpp

   nb::class_<MyClass>(m, "MyClass", nb::type_slots(slots));

Here, ``slots`` should refer to an array of function pointers that are tagged
with a corresponding slot identifier. For example, here is an example
function that overrides the addition operator so that it behaves like a
multiplication.

.. code-block:: cpp

   PyObject *myclass_tp_add(PyObject *a, PyObject *b) {
       return PyNumber_Multiply(a, b);
   }

   PyType_Slot slots[] = {
       { Py_nb_add, (void *) myclass_tp_add },
       { 0, nullptr }
   };

The ``slots`` array specified in the previous
:cpp:class:`nb::class_\<MyClass\>() <class_>` declaration references the
function ``myclass_tp_add`` and is followed by a mandatory null terminator.
Information on type slots can be found in the CPython documentation sections
covering `type objects <https://docs.python.org/3/c-api/typeobj.html>`_ and
`type construction <https://docs.python.org/3/c-api/type.html>`_.

This example is contrived because it could have been accomplished using
builtin features:

.. code-block:: cpp

   nb::class_<MyClass>(m, "MyClass")
       .def("__add__",
            [](const MyClass &a, const MyClass &b) { return a * b; },
            nb::is_operator())

The next section introduces a more interesting use case.

.. _cyclic_gc:

Cyclic garbage collection
-------------------------

Python tracks the lifetime of objects using an approach known as *reference
counting*. An object can be safely deconstructed once it is no longer
referenced from elsewhere, which happens when its reference count reaches zero.

This mechanism is simple and efficient, but it breaks down when objects form
*reference cycles*. For example, consider the following data structure

.. code-block:: cpp

   struct Wrapper {
       std::shared_ptr<Wrapper> value;
   };

with associated bindings

.. code-block:: cpp

   nb::class_<Wrapper>(m, "Wrapper")
       .def(nb::init<>())
       .def_rw("value", &Wrapper::value);

If we instantiate this class with a cycle, it can never be reclaimed (even
when Python shuts down and is supposed to free up all memory):

.. code-block:: pycon

   >>> a = my_ext.Wrapper()
   >>> a.value = a
   >>> del a

nanobind will loudly complain about this when the Python interpreter shuts
down:

.. code-block:: pycon

   >>> exit()
   nanobind: leaked 1 instances!
   nanobind: leaked 1 types!
    - leaked type "my_ext.Wrapper"
   nanobind: leaked 3 functions!
    - leaked function "<anonymous>"
    - leaked function "__init__"
    - leaked function "<anonymous>"
   nanobind: this is likely caused by a reference counting issue in the binding code.

The leaked ``Wrapper`` instance ``a`` references the ``Wrapper`` type, which in
turn references function definitions, causing a longer sequence of warnings.

Python provides a *cyclic garbage collector* that can in principle solve
this problem. To operate correctly, it requires information about how
objects are connected to each other.

We can provide a ``tp_traverse`` type slot that walks through the object
graph to inform the cyclic GC, and a ``tp_clear`` slot to break any detected
reference cycles:

.. code-block:: cpp

   int wrapper_tp_traverse(PyObject *self, visitproc visit, void *arg) {
       // Retrieve a pointer to the C++ instance associated with 'self' (never fails)
       Wrapper *w = nb::inst_ptr<Wrapper>(self);

       // If w->value has an associated CPython object, return it.
       // If not, value.ptr() will equal NULL, which is also fine.
       nb::object value = nb::find(w->value);

       // Inform the Python GC about the instance (if non-NULL)
       Py_VISIT(value.ptr());

       return 0;
   }

   int wrapper_tp_clear(PyObject *self) {
       // Retrieve a pointer to the C++ instance associated with 'self' (never fails)
       Wrapper *w = nb::inst_ptr<Wrapper>(self);

       // Clear the cycle!
       w->value.reset();

       return 0;
   }

   // Slot data structure referencing the above two functions
   PyType_Slot slots[] = {
       { Py_tp_traverse, (void *) wrapper_tp_traverse },
       { Py_tp_clear, (void *) wrapper_tp_clear },
       { 0, nullptr }
   };

The type ``visitproc`` and macro ``Py_VISIT()`` are part of the Python C API.

The expression :cpp:func:`nb::inst_ptr\<Wrapper\>(self) <inst_ptr>` efficiently
returns the C++ instance associated with a Python object and is explained in
the documentation about nanobind's :cpp:ref:`low level interface <lowlevel>`.

Note the use of the :cpp:func:`nb::find() <find>` function, which behaves like
:cpp:func:`nb::cast() <cast>` by returning the Python object associated with a
C++ instance. The main difference is that :cpp:func:`nb::cast() <cast>` will
create the Python object if it doesn't exist, while :cpp:func:`nb::find()
<find>` returns a ``nullptr`` object in that case.

To activate this machinery, the ``Wrapper`` type bindings must be made aware of
these extra type slots:

.. code-block:: cpp

   nb::class_<Wrapper>(m, "Wrapper", nb::type_slots(slots))

With this change, the cycle can be garbage-collected, and the leak warnings
disappear.

Reference cycles involving functions
------------------------------------

What if our wrapper class from the previous example instead stored a function
object?

.. code-block:: cpp

   struct Wrapper {
       std::function<void(void)> value;
   };

It may not be immediately obvious, but functions are one of the main sources of
reference cycles! For example, in Python we could write

.. code-block:: pycon

   >>> a = my_ext.Wrapper()
   >>> a.value = lambda: print(a)

This function is actually a `function closure
<https://en.wikipedia.org/wiki/Closure_(computer_programming)>`_ because it
references external variable state (its body accesses ``a``). This creates
an inter-language cycle ``Wrapper`` → ``function`` (itself wrapped in
``std::function<void(void)>``) → ``Wrapper``.

Such cycles are extremely common when Python-based callbacks can be installed
in C++ classes. An example would be a callback handler triggered by a button
press in a GUI framework. It is important to detect and handle such cycles.

When given a ``std::function<>`` instance, :cpp:func:`nb::find() <find>`
retrieves the associated Python ``function`` object (if present), which means
that the previous ``wrapper_tp_traverse()`` traversal function continues to
work without changes. The ``tp_clear`` slot requires small touch-ups:

.. code-block:: cpp

   int wrapper_tp_clear(PyObject *self) {
       Wrapper *w = nb::inst_ptr<Wrapper>(self);
       w->value = nullptr;
       return 0;
   }

That's it!
