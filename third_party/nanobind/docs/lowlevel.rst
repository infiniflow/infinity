.. _lowlevel:

.. cpp:namespace:: nanobind

Low-level interface
===================

nanobind exposes a low-level interface to provide fine-grained control over
the sequence of steps that instantiates a Python object wrapping a C++
instance. This is useful when writing generic binding code that manipulates
nanobind-based objects of various types.

Given a previous :cpp:class:`nb::class_\<...\> <class_>` binding declaration,
the :cpp:func:`nb::type\<T\>() <type>` template function can be used to look up
the Python type object associated with a C++ class named ``MyClass``.

.. code-block:: cpp

   nb::handle py_type = nb::type<MyClass>();

In the case of failure, this line will return a ``nullptr`` pointer, which
can be checked via ``py_type.is_valid()``. We can verify that the type
lookup succeeded, and that the returned instance indeed represents a
nanobind-owned type (via :cpp:func:`nb::type_check() <type_check>`, which is
redundant in this case):

.. code-block:: cpp

   assert(py_type.is_valid() && nb::type_check(py_type));

nanobind knows the size, alignment, and C++ RTTI ``std::type_info`` record of
all bound types. They can be queried on the fly via :cpp:func:`nb::type_size()
<type_size>`, :cpp:func:`nb::type_align() <type_size>`, and
:cpp:func:`nb::type_info() <type_size>` in situations where this is useful.

.. code-block:: cpp

   assert(nb::type_size(py_type) == sizeof(MyClass) &&
          nb::type_align(py_type) == alignof(MyClass) &&
          nb::type_info(py_type) == typeid(MyClass));

Given a type object representing a C++ type, we can create an uninitialized
instance via :cpp:func:`nb::inst_alloc() <inst_alloc>`. This is an ordinary
Python object that can, however, not (yet) be passed to bound C++ functions
to prevent undefined behavior. It must first be initialized.

.. code-block:: cpp

   nb::object py_inst = nb::inst_alloc(py_type);

We can confirm via :cpp:func:`nb::inst_check() <inst_check>` that this newly
created instance is managed by nanobind, that it has the correct type in
Python. Calling :cpp:func:`nb::inst_ready() <inst_ready>` reveals that the
*ready* flag of the instance is set to ``false`` (i.e., it is still
uninitialized).

.. code-block:: cpp

   assert(nb::inst_check(py_inst) &&
          py_inst.type().is(py_type) &&
          !nb::inst_ready(py_inst));

For simple *plain old data* (POD) types, the :cpp:func:`nb::inst_zero()
<inst_zero>` function can be used to *zero-initialize* the object and mark it
as ready.

.. code-block:: cpp

   nb::inst_zero(py_inst);
   assert(nb::inst_ready(py_inst));

We can destruct this default instance via :cpp:func:`nb::inst_destruct()
<inst_destruct>` and convert it back to non-ready status. This memory region
can then be reinitialized once more.

.. code-block:: cpp

   nb::inst_destruct(py_inst);
   assert(!nb::inst_ready(py_inst));

What follows is a more interesting example, where we use a lesser-known feature
of C++ (the "`placement new <https://en.wikipedia.org/wiki/Placement_syntax>`_"
operator) to construct an instance *in-place* into the memory region allocated
by nanobind.

.. code-block:: cpp

   // Get a C++ pointer to the uninitialized instance data
   MyClass *ptr = nb::inst_ptr<MyClass>(py_inst);

   // Perform an in-place construction of the C++ object at address 'ptr'
   new (ptr) MyClass(/* constructor arguments go here */);

Following this constructor call, we must inform nanobind that the instance
object is now fully constructed via :cpp:func:`nb::inst_mark_ready()
<inst_mark_ready>`. When its reference count reaches zero, nanobind will then
automatically call the in-place destructor (``MyClass::~MyClass``).

.. code-block:: cpp

   nb::inst_mark_ready(py_inst);
   assert(nb::inst_ready(py_inst));

Let’s destroy this instance once more manually (which will, again, call
the C++ destructor and mark the Python object as non-ready).

.. code-block:: cpp

   nb::inst_destruct(py_inst);

Another useful feature is that nanobind can copy- or move-construct ``py_inst``
from another instance of the same type via :cpp:func:`nb::inst_copy()
<inst_copy>` and :cpp:func:`nb::inst_move() <inst_move>`. These functions call
the C++ copy or move constructor and transition ``py_inst`` back to ``ready``
status. This is equivalent to calling an in-place version of these constructors
followed by a call to :cpp:func:`nb::inst_mark_ready() <inst_mark_ready>` but
compiles to more compact code (the :cpp:class:`nb::class_\<MyClass\> <class_>`
declaration had already created bindings for both constructors, and this simply
calls those bindings).

.. code-block:: cpp

   if (copy_instance)
       nb::inst_copy(/* dst = */ py_inst, /* src = */ some_other_instance);
   else
       nb::inst_move(/* dst = */ py_inst, /* src = */ some_other_instance);

Both functions assume that the destination object is uninitialized. Two
alternative versions :cpp:func:`nb::inst_replace_copy() <inst_replace_copy>`
and :cpp:func:`nb::inst_replace_move() <inst_replace_move>` destruct an
initialized instance and replace it with the contents of another by either
copying or moving.

.. code-block:: cpp

   if (copy_instance)
       nb::inst_replace_copy(/* dst = */ py_inst, /* src = */ some_other_instance);
   else
       nb::inst_replace_move(/* dst = */ py_inst, /* src = */ some_other_instance);

Note that these functions are all *unsafe* in the sense that they do not
verify that their input arguments are valid. This is done for
performance reasons, and such checks (if needed) are therefore the
responsibility of the caller. Functions labeled ``nb::type_*`` should
only be called with nanobind type objects, and functions labeled
``nb::inst_*`` should only be called with nanobind instance objects.

The functions :cpp:func:`nb::type_check() <type_check>` and
:cpp:func:`nb::inst_check() <inst_check>` are exceptions to this rule:
they accept any Python object and test whether something is a nanobind type or
instance object.

Two further functions :cpp:func:`nb::type_name() <type_name>` and
:cpp:func:`nb::inst_name() <inst_name>` determine the type name associated with
a type or instance thereof. These also accept non-nanobind types and instances.

Even lower-level interface
--------------------------

Every nanobind object has two important flags that control its behavior:

1. ``ready``: is the object fully constructed? If set to ``false``,
   nanobind will raise an exception when the object is passed to a bound
   C++ function.

2. ``destruct``: Should nanobind call the C++ destructor when the
   instance is garbage collected?

The functions :cpp:func:`nb::inst_zero() <inst_zero>`,
:cpp:func:`nb::inst_mark_ready() <inst_mark_ready>`, :cpp:func:`nb::inst_move()
<inst_move>`, and :cpp:func:`nb::inst_copy() <inst_copy>` set both of these
flags to ``true``, and :cpp:func:`nb::inst_destruct() <inst_destruct>` sets
both of them to ``false``.

In rare situations, the destructor should *not* be invoked when the instance is
garbage collected, for example when working with a nanobind instance
representing a field of a parent instance created using the
:cpp:enumerator:`nb::rv_policy::reference_internal
<rv_policy::reference_internal>` return value policy. The library therefore
exposes two more functions :cpp:func:`nb::inst_state() <inst_state>` and
:cpp:func:`nb::inst_set_state() <inst_set_state>` that can be used to access
them individually.

Referencing existing instances
------------------------------

The above examples used the function :cpp:func:`nb::inst_alloc() <inst_alloc>`
to allocate a Python object along with space to hold a C++ instance associated
with the binding ``py_type``.

.. code-block:: cpp

   nb::object py_inst = nb::inst_alloc(py_type);

   // Next, perform a C++ in-place construction into the
   // address given by nb::inst_ptr<MyClass>(py_inst)
   ... omitted, see the previous examples ...

What if the C++ instance already exists? nanobind also supports this case via
the :cpp:func:`nb::inst_reference() <inst_reference>` and
:cpp:func:`nb::inst_take_ownership() <inst_take_ownership>` functions—in this
case, the Python object references the existing memory region, which is
potentially (slightly) less efficient due to the need for an extra indirection.

.. code-block:: cpp

   MyClass *inst = new MyClass();

   // Transfer ownership of 'inst' to Python (which will use a delete
   // expression to free it when the Python instance is garbage collected)
   nb::object py_inst = nb::inst_take_ownership(py_type, inst);

   // We can also wrap C++ instances that should not be destructed since
   // they represent offsets into another data structure. In this case,
   // the optional 'parent' parameter ensures that 'py_inst' remains alive
   // while 'py_subinst' exists to prevent undefined behavior.
   nb::object py_subinst = nb::inst_reference(
       py_field_type, &inst->field, /* parent = */ py_inst);

.. _supplement:

Supplemental type data
----------------------

nanobind can stash supplemental data *inside* the type object of bound types.
This involves the :cpp:class:`nb::supplement\<T\>() <supplement>` class binding
annotation to reserve space and :cpp:func:`nb::type_supplement\<T\>()
<type_supplement>` to access the reserved memory region.

An example use of this fairly advanced feature are libraries that register
large numbers of different types (e.g. flavors of tensors). A single
generically implemented function can then query the supplemental data block to
handle each tensor type slightly differently.

Here is what this might look like in an implementation:

.. code-block:: cpp

  struct MyTensorMetadata {
      bool stored_on_gpu;
      // ..
      // should be a POD (plain old data) type
  };

  // Register a new type MyTensor, and reserve space for sizeof(MyTensorMedadata)
  nb::class_<MyTensor> cls(m, "MyTensor", nb::supplement<MyTensorMedadata>())

  /// Mutable reference to 'MyTensorMedadata' portion in Python type object
  MyTensorMedadata &supplement = nb::type_supplement<MyTensorMedadata>(cls);
  supplement.stored_on_gpu = true;

The :cpp:class:`nb::supplement\<T\>() <supplement>` annotation implicitly also
passes :cpp:class:`nb::is_final() <is_final>` to ensure that type objects with
supplemental data cannot be subclassed in Python.

nanobind requires that the specified type ``T`` be trivially default
constructible. It zero-initializes the supplement when the type is first
created but does not perform any further custom initialization or destruction.
You can fill the supplement with different contents following the type
creation, e.g., using the placement new operator.

The contents of the supplemental data are not directly visible to Python's
cyclic garbage collector, which creates challenges if you want to reference
Python objects. The recommended workaround is to store the Python objects
as attributes of the type object (in its ``__dict__``) and store a borrowed
``PyObject*`` reference in the supplemental data. If you use an attribute
name that begins with the symbol ``@``, then nanobind will prevent Python
code from rebinding or deleting the attribute after it has been set, making
the borrowed reference reasonably safe.
