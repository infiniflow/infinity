.. _ownership_adv:

.. cpp:namespace:: nanobind

Object ownership, continued
===========================

This section covers intrusive reference counting as an alternative to shared
pointers, and it explains the nitty-gritty details of how shared and unique
pointer conversion is implemented in nanobind.

.. _intrusive:

Intrusive reference counting
----------------------------

nanobind provides a custom intrusive reference counting solution that
completely solves the issue of shared C++/Python object ownership, while
avoiding the overheads and complexities of traditional C++ shared pointers
(``std::shared_ptr<T>``).

The main limitation is that it requires adapting the base class of an object
hierarchy according to the needs of nanobind, which may not always be possible.

Motivation
^^^^^^^^^^

Consider the following simple class with intrusive reference counting:

.. code-block:: cpp

   class Object {
   public:
       void inc_ref() const noexcept { ++m_ref_count; }

       void dec_ref() const noexcept {
           if (--m_ref_count == 0)
               delete this;
       }

   private:
       mutable std::atomic<size_t> m_ref_count { 0 };
   };

It contains an atomic counter that stores the number of references. When the
counter reaches zero, the object deallocates itself. Easy and efficient.

The advantage of over ``std::shared_ptr<T>`` is that no separate control block
must be allocated. Technical band-aids like ``std::enable_shared_from_this<T>``
can also be avoided, since the reference count is always found in the object
itself.

However, one issue that tends to arise when a type like ``Object`` is
wrapped using nanobind is that there are now *two* separate reference counts
referring to the same object: one in Python’s ``PyObject``, and one in
``Object``. This can lead to a problematic reference cycle:

- Python’s ``PyObject`` needs to keep the ``Object`` instance alive so that it
  can be safely passed to C++ functions.

- The C++ ``Object`` may in turn need to keep the ``PyObject`` alive. This
  is the case when a subclass uses *trampolines* (:c:macro:`NB_TRAMPOLINE`,
  :c:macro:`NB_OVERRIDE`) to catch C++ virtual function calls and
  potentially dispatch them to an overridden implementation in Python. In
  this case, the C++ instance needs to be able to perform a function call on
  its own Python object identity, which requires a reference.

The source of the problem is that there are *two* separate counters that try
to reason about the reference count of *one* instance, which leads to an
uncollectable inter-language reference cycle.

The solution
^^^^^^^^^^^^
We can solve the problem by using just one counter:

- if an instance lives purely on the C++ side, the ``m_ref_count``
  field is used to reason about the number of references.

- The first time that an instance is exposed to Python (by being
  created from Python, or by being returned from a bound C++ function),
  lifetime management switches over to Python.

The file `nanobind/intrusive/counter.h
<https://github.com/wjakob/nanobind/blob/master/include/nanobind/intrusive/counter.h>`_
includes an official sample implementation of this functionality. It contains an extra optimization to pack *either*
a reference counter or a pointer to a ``PyObject*`` into a single
``sizeof(void*)``-sized field.

The most basic interface, :cpp:class:`intrusive_counter` represents an atomic
counter that can be increased (via :cpp:func:`intrusive_counter::inc_ref()`) or
decreased (via :cpp:func:`intrusive_counter::dec_ref()`). When the counter
reaches zero, the object should be deleted, which ``dec_ref()`` indicates by
returning ``true``.

In addition to this simple counting mechanism, ownership of the object can also
be transferred to Python (via :cpp:func:`intrusive_counter::set_self_py()`). In
this case, subsequent calls to ``inc_ref()`` and ``dec_ref()`` modify the
reference count of the underlying Python object. 

To incorporate intrusive reference counting into your own project, you would
usually add an :cpp:class:`intrusive_counter`-typed member to the base class of an object
hierarchy and expose it as follows:

.. code-block:: cpp

   #include <nanobind/intrusive/counter.h>

   class Object {
   public:
       void inc_ref() noexcept { m_ref_count.inc_ref(); }
       bool dec_ref() noexcept { return m_ref_count.dec_ref(); }

       // Important: must declare virtual destructor
       virtual ~Object() = default;

       void set_self_py(PyObject *self) noexcept {
           m_ref_count.set_self_py(self);
       }

   private:
       nb::intrusive_counter m_ref_count;
   };

   // Convenience function for increasing the reference count of an instance
   inline void inc_ref(Object *o) noexcept {
       if (o)
          o->inc_ref();
   }

   // Convenience function for decreasing the reference count of an instance
   // and potentially deleting it when the count reaches zero
   inline void dec_ref(Object *o) noexcept {
       if (o && o->dec_ref())
           delete o;
   }

Alternatively, you could also inherit from :cpp:class:`intrusive_base`, which
obviates the need for all of the above declarations:

.. code-block:: cpp

   class Object : public nb::intrusive_base {
   public:
       // ...
   };

The main change in the bindings is that the base class must specify a
:cpp:class:`nb::intrusive_ptr <intrusive_ptr>` annotation to inform an instance
that lifetime management has been taken over by Python. This annotation is
automatically inherited by all subclasses. In the linked example, this is done
via the ``Object::set_self_py()`` method that we can now call from the class
binding annotation:

.. code-block:: cpp

   nb::class_<Object>(
     m, "Object",
     nb::intrusive_ptr<Object>(
         [](Object *o, PyObject *po) noexcept { o->set_self_py(po); }));

Also, somewhere in your binding initialization code, you must register Python
reference counting hooks with the intrusive reference counter class. This
allows its implementation of the code in ``nanobind/intrusive/counter.h`` to
*not* depend on Python (this means that it can be used in projects where Python
bindings are an optional component).

.. code-block:: cpp

   nb::intrusive_init(
       [](PyObject *o) noexcept {
           nb::gil_scoped_acquire guard;
           Py_INCREF(o);
       },
       [](PyObject *o) noexcept {
           nb::gil_scoped_acquire guard;
           Py_DECREF(o);
       });

These ``counter.h`` include file references several functions that must be
compiled somewhere inside the project, which can be accomplished by including
the following file from a single ``.cpp`` file.

.. code-block:: cpp

   #include <nanobind/intrusive/counter.inl>

Having to call :cpp:func:`inc_ref()` and :cpp:func:`dec_ref()` many times to
perform manual reference counting in project code can quickly become tedious.
Nanobind also ships with a :cpp:class:`ref\<T\> <ref>` RAII helper class to
help with this.

.. code-block:: cpp

   #include <nanobind/intrusive/ref.h>

   void foo() {
       /// Assignment to ref<T> automatically increases the object's reference count
       ref<MyObject> x = new MyObject();

       // ref<T> can be used like a normal pointer
       x->func();

   } // <-- ref::~ref() calls dec_ref(), which deletes the now-unreferenced instance

When the file ``nanobind/intrusive/ref.h`` is included following
``nanobind/nanobind.h``, it also exposes a custom type caster to bind functions
taking or returning ``ref<T>``-typed values.

That's it. If you use this approach, any potential issues involving shared
pointers, return value policies, reference leaks with trampolines, etc., can
be avoided from the beginning.

.. _shared_ptr_adv:

Shared pointers, continued
--------------------------

The following continues the :ref:`discussion of shared pointers <shared_ptr>`
in the introductory section on object ownership and provides detail on how
shared pointer conversion is *implemented* by nanobind.

When the user calls a C++ function taking an argument of type
``std::shared_ptr<T>`` from Python, ownership of that object must be
shared between C++ to Python. nanobind does this by increasing the reference
count of the ``PyObject`` and then creating a ``std::shared_ptr<T>`` with a new
control block containing a custom deleter that will in turn reduce the Python
reference count upon destruction of the shared pointer.

When a C++ function returns a ``std::shared_ptr<T>``, nanobind
checks if the instance already has a ``PyObject`` counterpart
(nothing needs to be done in this case). Otherwise, it indicates
shared ownership by creating a temporary ``std::shared_ptr<T>`` on
the heap that will be destructed when the ``PyObject`` is garbage
collected.

The approach in nanobind was chosen following on discussions with `Ralf
Grosse-Kunstleve <https://github.com/rwgk>`_; it is unusual in that multiple
``shared_ptr`` control blocks are potentially allocated for the same object,
which means that ``std::shared_ptr<T>::use_count()`` generally won’t show the
true global reference count.

.. _enable_shared_from_this:

enable_shared_from_this
^^^^^^^^^^^^^^^^^^^^^^^

The C++ standard library class ``std::enable_shared_from_this<T>``
allows an object that inherits from it to locate an existing
``std::shared_ptr<T>`` that manages that object. nanobind supports
types that inherit from ``enable_shared_from_this``, with some caveats
described in this section.

Background (not nanobind-specific): Suppose a type ``ST`` inherits
from ``std::enable_shared_from_this<ST>``. When a raw pointer ``ST
*obj`` or ``std::unique_ptr<ST> obj`` is wrapped in a shared pointer
using a constructor of the form ``std::shared_ptr<ST>(obj, ...)``, a
reference to the new ``shared_ptr``\'s control block is saved (as
``std::weak_ptr<ST>``) inside the object. This allows new
``shared_ptr``\s that share ownership with the existing one to be
obtained for the same object using ``obj->shared_from_this()`` or
``obj->weak_from_this()``.

nanobind's support for ``std::enable_shared_from_this`` consists of three
behaviors:

* If a raw pointer ``ST *obj`` is returned from C++ to Python, and
  there already exists an associated ``std::shared_ptr<ST>`` which
  ``obj->shared_from_this()`` can locate, then nanobind will produce a
  Python instance that shares ownership with it. The behavior is
  identical to what would happen if the C++ code did ``return
  obj->shared_from_this();`` (returning an explicit
  ``std::shared_ptr<ST>`` to Python) rather than ``return obj;``.
  The return value policy has limited effect in this case; you will get
  shared ownership on the Python side regardless of whether you used
  `rv_policy::take_ownership` or `rv_policy::reference`.
  (`rv_policy::copy` and `rv_policy::move` will still create a new
  object that has no ongoing relationship to the returned pointer.)

  * Note that this behavior occurs only if such a ``std::shared_ptr<ST>``
    already exists! If not, then nanobind behaves as it would without
    ``enable_shared_from_this``: a raw pointer will transfer exclusive
    ownership to Python by default, or will create a non-owning reference
    if you use `rv_policy::reference`.

* If a Python object is passed to C++ as ``std::shared_ptr<ST> obj``,
  and there already exists an associated ``std::shared_ptr<ST>`` which
  ``obj->shared_from_this()`` can locate, then nanobind will produce a
  ``std::shared_ptr<ST>`` that shares ownership with it: an additional
  reference to the same control block, rather than a new control block
  (as would occur without ``enable_shared_from_this``). This improves
  performance and makes the result of ``shared_ptr::use_count()`` more
  accurate.

* If a Python object is passed to C++ as ``std::shared_ptr<ST> obj``, and
  there is no associated ``std::shared_ptr<ST>`` that
  ``obj->shared_from_this()`` can locate, then nanobind will produce
  a ``std::shared_ptr<ST>`` as usual (with a new control block whose deleter
  drops a Python object reference), *and* will do so in a way that enables
  future calls to ``obj->shared_from_this()`` to find it as long
  as any ``shared_ptr`` that shares this control block is still alive on
  the C++ side.

  (Once all of the ``std::shared_ptr<ST>``\s that share this control block
  have been destroyed, the underlying PyObject reference being
  managed by the ``shared_ptr`` deleter will be dropped,
  and ``shared_from_this()`` will stop working. It can be reenabled by
  passing the Python object back to C++ as ``std::shared_ptr<ST>`` once more,
  which will create another control block.)

Bindings for a class that supports ``enable_shared_from_this`` will be
slightly larger than bindings for a class that doesn't, as nanobind
must produce type-specific code to implement the above behaviors.

.. warning:: The ``shared_from_this()`` method will only work when there
   is actually a ``std::shared_ptr`` managing the object. A nanobind
   instance constructed from Python will not have an associated
   ``std::shared_ptr`` yet, so ``shared_from_this()`` will throw an
   exception if you pass such an instance to C++ using a reference or
   raw pointer. ``shared_from_this()`` will only work when there exists
   a corresponding live ``std::shared_ptr`` on the C++ side.

   The only situation where nanobind will create the first
   ``std::shared_ptr`` for an object (thus enabling
   ``shared_from_this()``), even with ``enable_shared_from_this``, is
   when a Python instance is passed to C++ as the explicit type
   ``std::shared_ptr<T>``. If you don't do this, or if no such
   ``std::shared_ptr`` is still alive, then ``shared_from_this()`` will
   throw an exception. It also works to create the ``std::shared_ptr``
   on the C++ side, such as by using a factory function which always
   uses ``std::make_shared<T>(...)`` to construct the object, and
   returns the resulting ``std::shared_ptr<T>`` to Python.

If you need to enable ``shared_from_this`` immediately upon
regular Python-side object construction (i.e., ``SomeType(*args)``
rather than ``SomeType.some_fn(*args)``), you can bind a C++ function
that returns ``std::shared_ptr<T>`` as your class's ``__new__`` method.
See the documentation on :ref:`customizing object creation <custom_new>`.

.. warning:: C++ code that receives a raw pointer ``T *obj`` *must not*
   assume that it has exclusive ownership of ``obj``, or even that
   ``obj`` is allocated on the C++ heap (via ``operator new``);
   ``obj`` might instead be a subobject of a nanobind instance
   allocated from Python. This applies even if ``T`` supports
   ``shared_from_this()`` and there is no associated
   ``std::shared_ptr``. Lack of a ``shared_ptr`` does *not* imply
   exclusive ownership; it just means there's no way to share ownership
   with whoever the current owner is.

.. _unique_ptr_adv:

Unique pointers
---------------

The following continues the :ref:`discussion of unique pointers <unique_ptr>`
in the introductory section on object ownership and provides detail on how
unique pointer conversion is *implemented* by nanobind.

Whereas ``std::shared_ptr<..>`` could abstract over details concerning
storage and the deletion mechanism, this is not possible in the simpler
``std::unique_ptr<..>``, which means that some of those details leak into
the type signature.

When the user calls a C++ function taking an argument of type ``std::unique_ptr<T,
Deleter>`` from Python, ownership of that object must be transferred from C++ to Python.

- When ``Deleter`` is ``std::default_delete<T>`` (i.e., the default
  when no ``Deleter`` is specified), this ownership transfer is only
  possible when the instance was originally created by a *new expression*
  within C++ and nanobind has taken over ownership (i.e., it was created by
  a function returning a raw pointer ``T *value`` with
  :cpp:enumerator:`rv_policy::take_ownership`, or a function returning a
  ``std::unique_ptr<T>``). This limitation exists because the ``Deleter``
  will execute the statement ``delete value`` when the unique pointer
  expires, causing undefined behavior when the object was allocated within
  Python (the problem here is that nanobind uses the Python memory allocator
  and furthermore co-locates Python and C++ object storage. A *delete
  expression* cannot be used in such a case). nanobind detects this, refuses
  unsafe conversions with a ``TypeError`` and emits a separate warning.

- To enable ownership transfer under all conditions, nanobind
  provides a custom ``Deleter`` named :cpp:class:`nb::deleter\<T\>
  <deleter>` that uses reference counting to keep the underlying
  ``PyObject`` alive during the lifetime of the unique pointer. Following
  this route requires changing function signatures so that they use
  ``std::unique_ptr<T, nb::deleter<T>>`` instead of ``std::unique_ptr<T>``.
  This custom deleter supports ownership by both C++ and Python and can be
  used in all situations.

In both cases, a Python object may continue to exist after ownership was
transferred to C++ side. nanobind marks this object as *invalid*: any
operations involving it will fail with a ``TypeError``. Reverse ownership
transfer at a later point will make it usable again.

Binding functions that return a ``std::unique_ptr<T, Deleter>`` always
works: nanobind will then acquire or reacquire ownership of the object.

Deleters other than ``std::default_delete<T>`` or ``nb::deleter<T>`` are
*not supported*.
