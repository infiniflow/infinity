C++ API Reference (Extras)
==========================

.. cpp:namespace:: nanobind

Operator overloading
--------------------

The following optional include directive imports the special value :cpp:var:`self`.

.. code-block:: cpp

   #include <nanobind/operators.h>

The underlying type exposes various C++ operators that enable a shorthand
notation to bind operators to python. See the :ref:`operator overloading
<operator_overloading>` example in the main documentation for details.


.. cpp:class:: detail::self_t

   This is an internal class that should be accessed through the singleton
   :cpp:var:`self` value.

   It supports the overloaded operators listed below. Depending on whether
   :cpp:var:`self` is the left or right argument of a binary operation,
   the binding will map to different Python methods as shown below.

   .. list-table::
      :header-rows: 1
      :widths: 50 50

      * - C++ operator
        - Python method (left or right)
      * - ``operator-``
        - ``__sub__``, ``__rsub__``
      * - ``operator+``
        - ``__add__``, ``__radd__``
      * - ``operator*``
        - ``__mul__``, ``__rmul__``
      * - ``operator/``
        - ``__truediv__``, ``__rtruediv__``
      * - ``operator%``
        - ``__mod__``, ``__rmod__``
      * - ``operator<<``
        - ``__lshift__``, ``__rlshift__``
      * - ``operator>>``
        - ``__rshift__``, ``__rrshift__``
      * - ``operator&``
        - ``__and__``, ``__rand__``
      * - ``operator^``
        - ``__xor__``, ``__rxor__``
      * - ``operator|``
        - ``__or__``, ``__ror__``
      * - ``operator>``
        - ``__gt__``, ``__lt__``
      * - ``operator>=``
        - ``__ge__``, ``__le__``
      * - ``operator<``
        - ``__lt__``, ``__gt__``
      * - ``operator<=``
        - ``__le__``, ``__ge__``
      * - ``operator==``
        - ``__eq__``
      * - ``operator!=``
        - ``__ne__``
      * - ``operator+=``
        - ``__iadd__``
      * - ``operator-=``
        - ``__isub__``
      * - ``operator*=``
        - ``__mul__``
      * - ``operator/=``
        - ``__itruediv__``
      * - ``operator%=``
        - ``__imod__``
      * - ``operator<<=``
        - ``__ilrshift__``
      * - ``operator>>=``
        - ``__ilrshift__``
      * - ``operator&=``
        - ``__iand__``
      * - ``operator^=``
        - ``__ixor__``
      * - ``operator|=``
        - ``__ior__``
      * - ``operator-`` (unary)
        - ``__neg__``
      * - ``operator+`` (unary)
        - ``__pos__``
      * - ``operator~``  (unary)
        - ``__invert__``
      * - ``operator!``  (unary)
        - ``__bool__`` (with extra negation)
      * - ``nb::abs(..)``
        - ``__abs__``
      * - ``nb::hash(..)``
        - ``__hash__``

.. cpp:var:: detail::self_t self

Trampolines
-----------

The following macros to implement trampolines that forward virtual function
calls to Python require an additional include directive:

.. code-block:: cpp

   #include <nanobind/trampoline.h>

See the section on :ref:`trampolines <trampolines>` for further detail.

.. c:macro:: NB_TRAMPOLINE(base, size)

   Install a trampoline in an alias class to enable dispatching C++ virtual
   function calls to a Python implementation. Refer to the documentation on
   :ref:`trampolines <trampolines>` to see how this macro can be used.

.. c:macro:: NB_OVERRIDE(func, ...)

   Dispatch the call to a Python method named ``"func"`` if it is overloaded on
   the Python side, and forward the function arguments specified in the
   variable length argument ``...``. Otherwise, call the C++ implementation
   `func` in the base class.

   Refer to the documentation on :ref:`trampolines <trampolines>` to see how
   this macro can be used.

.. c:macro:: NB_OVERRIDE_PURE(func, ...)

   Dispatch the call to a Python method named ``"func"`` if it is overloaded on
   the Python side, and forward the function arguments specified in the
   variable length argument ``...``. Otherwise, raise an exception. This macro
   should be used when the C++ function is pure virtual.

   Refer to the documentation on :ref:`trampolines <trampolines>` to see how
   this macro can be used.

.. c:macro:: NB_OVERRIDE_NAME(name, func, ...)

   Dispatch the call to a Python method named ``name`` if it is overloaded on
   the Python side, and forward the function arguments specified in the
   variable length argument ``...``. Otherwise, call the C++ function `func` in
   the base class.

   This function differs from :c:macro:`NB_OVERRIDE() <NB_OVERRIDE>` in that
   C++ and Python functions can be named differently (e.g., ``operator+`` and
   ``__add__``). Refer to the documentation on :ref:`trampolines <trampolines>`
   to see how this macro can be used.

.. c:macro:: NB_OVERRIDE_PURE_NAME(name, func, ...)

   Dispatch the call to a Python method named ``name`` if it is overloaded on
   the Python side, and forward the function arguments specified in the
   variable length argument ``...``. Otherwise, raise an exception. This macro
   should be used when the C++ function is pure virtual.

   This function differs from :c:macro:`NB_OVERRIDE_PURE() <NB_OVERRIDE_PURE>`
   in that C++ and Python functions can be named differently (e.g.,
   ``operator+`` and ``__add__``). Although the C++ base implementation cannot
   be called, its name is still important since nanobind uses it to infer the
   return value type. Refer to the documentation on :ref:`trampolines
   <trampolines>` to see how this macro can be used.

.. _vector_bindings:

STL vector bindings
-------------------

The following function can be used to expose ``std::vector<...>`` variants
in Python. It is not part of the core nanobind API and requires an additional
include directive:

.. code-block:: cpp

   #include <nanobind/stl/bind_vector.h>

.. cpp:function:: template <typename Vector, rv_policy Policy = rv_policy::automatic_reference, typename... Args> class_<Vector> bind_vector(handle scope, const char * name, Args &&...args)

   Bind the STL vector-derived type `Vector` to the identifier `name` and
   place it in `scope` (e.g., a :cpp:class:`module_`). The variable argument
   list can be used to pass a docstring and other :ref:`class binding
   annotations <class_binding_annotations>`.

   The type includes the following methods resembling ``list``:

   .. list-table::
      :header-rows: 1
      :widths: 50 50

      * - Signature
        - Documentation
      * - ``__init__(self)``
        - Default constructor
      * - ``__init__(self, arg: Vector)``
        - Copy constructor
      * - ``__init__(self, arg: typing.Sequence)``
        - Construct from another sequence type
      * - ``__len__(self) -> int``
        - Return the number of elements
      * - ``__repr__(self) -> str``
        - Generate a string representation
      * - ``__contains__(self, arg: Value)``
        - Check if the vector contains ``arg``
      * - ``__eq__(self, arg: Vector)``
        - Check if the vector is equal to ``arg``
      * - ``__ne__(self, arg: Vector)``
        - Check if the vector is not equal to ``arg``
      * - ``__bool__(self) -> bool``
        - Check whether the vector is empty
      * - ``__iter__(self) -> iterator``
        - Instantiate an iterator to traverse the elements
      * - ``__getitem__(self, arg: int) -> Value``
        - Return an element from the list (supports negative indexing)
      * - ``__setitem__(self, arg0: int, arg1: Value)``
        - Assign an element in the list (supports negative indexing)
      * - ``__delitem__(self, arg: int)``
        - Delete an item from the list (supports negative indexing)
      * - ``__getitem__(self, arg: slice) -> Vector``
        - Slice-based getter
      * - ``__setitem__(self, arg0: slice, arg1: Value)``
        - Slice-based assignment
      * - ``__delitem__(self, arg: slice)``
        - Slice-based deletion
      * - ``clear(self)``
        - Remove all items from the list
      * - ``append(self, arg: Value)``
        - Append a list item
      * - ``insert(self, arg0: int, arg1: Value)``
        - Insert a list item (supports negative indexing)
      * - ``pop(self, index: int = -1)``
        - Pop an element at position ``index`` (the end by default)
      * - ``extend(self, arg: Vector)``
        - Extend ``self`` by appending elements from ``arg``.
      * - ``count(self, arg: Value)``
        - Count the number of times that ``arg`` is contained in the vector
      * - ``remove(self, arg: Value)``
        - Remove all occurrences of ``arg``.

   In contrast to ``std::vector<...>``, all bound functions perform range
   checks to avoid undefined behavior. When the type underlying the vector is
   not comparable or copy-assignable, some of these functions will not be
   generated.

   The binding operation is a no-op if the vector type has already been
   registered with nanobind.

   .. warning::

      While this function creates a type resembling a Python ``list``, it has a
      major caveat: the item accessor ``__getitem__`` copies the accessed
      element by default (the bottom of this paragraph explains how this copy
      can be avoided).

      Consequently, writes to elements may not propagate in the expected way.
      Consider the following C++ bindings:

      .. code-block:: cpp

         struct A {
             int value;
         };

         nb::class_<A>(m, "A")
             .def(nb::init<int>())
             .def_rw("value", &A::value);

         nb::bind_vector<std::vector<A>>(m, "VecA");

      On the Python end, they yield the following surprising behavior:

      .. code-block:: python

         from my_ext import A, VecA

         va = VecA()
         va.append(A(123))
         a[0].value = 456
         assert a[0].value == 456 # <-- assertion fails!

      To actually modify ``va``, another write is needed.

      .. code-block:: python

         v = a[0]
         v.value = 456
         a[0] = v

      This may seem like a strange design, so it is worth explaining why the
      implementation works in this way.

      The key issue is that any particular value (e.g., ``va[0]``) lies within
      a memory buffer managed by the ``std::vector``. It is not safe for
      nanobind to refer to objects within this buffer using their absolute or
      relative memory address. For example, inserting an element at position 0
      will rearrange the buffer's contents and shift all subsequent ``A``
      instances. If nanobind ``A`` objects could be "views" into the
      ``std::vector``, then an insertion would cause the contents of unrelated
      ``A`` Python objects to change unexpectedly. Insertion may also require
      reallocation of the buffer, invalidating all current addresses, and this
      could lead to undefined behavior (use-after-free) if nanobind did not
      make a copy.

      There are three situations in which the surprising behavior is avoided:

      1. If the modification of the array is performed using in-place
         operations like

         .. code-block:: python

            v[i] += 5

         In-place operators automatically perform an array assignment, causing
         the issue to disappear. This means that if you work with a vector type
         like ``std::vector<int>`` or ``std::vector<std::string>`` with an
         immutable element type like ``int`` or ``str`` on the Python end, it
         will behave completely naturally in Python.

      2. If the array contains STL shared pointers (e.g.,
         ``std::vector<std::shared_ptr<T>>``), the added
         indirection and ownership tracking removes the need for extra copies.

      3. If the array contains pointers to reference-counted objects (e.g.,
         ``std::vector<ref<T>>`` via the :cpp:class:`ref` wrapper) and ``T``
         uses the intrusive reference counting approach explained :ref:`here
         <intrusive>`, the added indirection and ownership tracking removes the
         need for extra copies.

         (It is usually unsafe to use this class to bind pointer-valued
         vectors ``std::vector<T*>`` when ``T`` does not use intrusive
         reference counting, because then there is nothing to prevent the Python
         objects returned by ``__getitem__`` from outliving the C++ ``T``
         objects that they point to. But if you are able to guarantee through
         other means that the ``T`` objects will live long enough, the intrusive
         reference counting is not strictly required.)

   .. note::

      Previous versions of nanobind (before 2.0) and pybind11 return Python
      objects from ``__getitem__`` that wrap *references* (i.e., views),
      meaning that they are only safe to use until the next insertion or
      deletion in the vector they were drawn from. As discussed above, any use
      after that point could **corrupt memory or crash your program**, which is
      why reference semantics are no longer the default.

      If you truly need the unsafe reference semantics, and if you
      can guarantee that all use of your bindings will respect
      the memory layout and reference-invalidation rules of the
      underlying C++ container type, you can request the old behavior
      by passing a second template argument of
      :cpp:enumerator:`rv_policy::reference_internal` to
      :cpp:func:`bind_vector`. This will override nanobind's usual
      choice of :cpp:enumerator:`rv_policy::copy` for ``__getitem__``.

      .. code-block:: cpp

         nb::bind_vector<std::vector<MyType>,
                         nb::rv_policy::reference_internal>(m, "ExampleVec");

      Again, please avoid this if at all possible.
      It is *very* easy to cause problems if you're not careful, as the
      following example demonstrates.

      .. code-block:: python

         def looks_fine_but_crashes(vec: ext.ExampleVec) -> None:
             # Trying to remove all the elements too much older than the last:
             last = vec[-1]
             # Even being careful to iterate backwards so we visit each
             # index only once...
             for idx in range(len(vec) - 2, -1, -1):
                 if last.timestamp - vec[idx].timestamp > 5:
                     del vec[idx]
                     # Oops! After the first deletion, 'last' now refers to
                     # uninitialized memory.


.. _map_bindings:

STL map bindings
----------------

The following function can be used to expose ``std::map<...>`` or
``std::unordered_map<...>`` variants in Python. It is not part of the core
nanobind API and requires an additional include directive:

.. code-block:: cpp

   #include <nanobind/stl/bind_map.h>

.. cpp:function:: template <typename Map, rv_policy Policy = rv_policy::automatic_reference, typename... Args> class_<Map> bind_map(handle scope, const char * name, Args &&...args)

   Bind the STL map-derived type `Map` (ordered or unordered) to the identifier
   `name` and place it in `scope` (e.g., a :cpp:class:`module_`). The variable
   argument list can be used to pass a docstring and other :ref:`class binding
   annotations <class_binding_annotations>`.

   The type includes the following methods resembling ``dict``:

   .. list-table::
      :header-rows: 1
      :widths: 50 50

      * - Signature
        - Documentation
      * - ``__init__(self)``
        - Default constructor
      * - ``__init__(self, arg: Map)``
        - Copy constructor
      * - ``__init__(self, arg: dict)``
        - Construct from a Python dictionary
      * - ``__len__(self) -> int``
        - Return the number of elements
      * - ``__repr__(self) -> str``
        - Generate a string representation
      * - ``__contains__(self, arg: Key)``
        - Check if the map contains ``arg``
      * - ``__eq__(self, arg: Map)``
        - Check if the map is equal to ``arg``
      * - ``__ne__(self, arg: Map)``
        - Check if the map is not equal to ``arg``
      * - ``__bool__(self) -> bool``
        - Check whether the map is empty
      * - ``__iter__(self) -> iterator``
        - Instantiate an iterator to traverse the set of map keys
      * - ``__getitem__(self, arg: Key) -> Value``
        - Return an element from the map
      * - ``__setitem__(self, arg0: Key, arg1: Value)``
        - Assign an element in the map
      * - ``__delitem__(self, arg: Key)``
        - Delete an item from the map
      * - ``clear(self)``
        - Remove all items from the list
      * - ``update(self, arg: Map)``
        - Update the map with elements from ``arg``.
      * - ``keys(self, arg: Map) -> Map.KeyView``
        - Returns an iterable view of the map's keys
      * - ``values(self, arg: Map) -> Map.ValueView``
        - Returns an iterable view of the map's values
      * - ``items(self, arg: Map) -> Map.ItemView``
        - Returns an iterable view of the map's items

   The binding operation is a no-op if the map type has already been
   registered with nanobind.

   The binding routine ideally expects the involved types to be:

   - copy-constructible
   - copy-assignable
   - equality-comparable

   If not all of these properties are available, then a subset of the above
   methods will be omitted. Please refer to ``bind_map.h`` for details on the
   logic.

   .. warning::

      While this function creates a type resembling a Python ``dict``, it has a
      major caveat: the item accessor ``__getitem__`` copies the accessed
      element by default.

      Please refer to the :ref:`STL vector bindings <vector_bindings>` for a
      discussion of the problem and possible solutions. Everything applies
      equally to the map case.

   .. note::

      Unlike ``std::vector``, the ``std::map`` and ``std::unordered_map``
      containers are *node-based*, meaning their elements do have a
      consistent address for as long as they're stored in the map.
      (Note that this is generally *not* true of third-party containers
      with similar interfaces, such as ``absl::flat_hash_map``.)

      If you are binding a node-based container type, and you want
      ``__getitem__`` to return a reference to the accessed element
      rather than copying it, it is *somewhat* safer than it would
      be with :cpp:func:`bind_vector` to use the unsafe workaround
      discussed there:

      .. code-block:: cpp

         nb::bind_map<std::map<std::string, SomeValue>,
                      nb::rv_policy::reference_internal>(m, "ExampleMap");

      With a node-based container, the only situation where a reference
      returned from ``__getitem__`` would be invalidated is if the individual
      element that it refers to were removed from the map. Unlike with
      ``std::vector``, additions and removals of *other* elements would
      not present a danger.

      It is still easy to cause problems if you're not careful, though:

      .. code-block:: python

         def unsafe_pop(map: ext.ExampleMap, key: str) -> ext.SomeValue:
             value = map[key]
             del map[key]
             # Oops! `value` now points to a dangling element. Anything you
             # do with it now is liable to crash the interpreter.
             return value  # uh-oh...


Unique pointer deleter
----------------------

The following *deleter* should be used to gain maximal flexibility in combination with
``std::unique_ptr<..>``. It requires the following additional include directive:

.. code-block:: cpp

   #include <nanobind/stl/unique_ptr.h>

See the two documentation sections on unique pointers for further detail
(:ref:`#1 <unique_ptr>`, :ref:`#2 <unique_ptr_adv>`).

.. cpp:struct:: template <typename T> deleter

   .. cpp:function:: deleter() = default

      Create a deleter that destroys the object using a ``delete`` expression.

   .. cpp:function:: deleter(handle h)

      Create a deleter that destroys the object by reducing the Python reference count.

   .. cpp:function:: bool owned_by_python() const

      Check if the object is owned by Python.

   .. cpp:function:: bool owned_by_cpp() const

      Check if the object is owned by C++.

   .. cpp:function:: void operator()(void * p) noexcept

      Destroy the object at address `p`.

.. _iterator_bindings:

Iterator bindings
-----------------

The following functions can be used to expose existing C++ iterators in
Python. They are not part of the core nanobind API and require an additional
include directive:

.. code-block:: cpp

   #include <nanobind/make_iterator.h>

.. cpp:function:: template <rv_policy Policy = rv_policy::automatic_reference, typename Iterator, typename... Extra> auto make_iterator(handle scope, const char * name, Iterator &&first, Iterator &&last, Extra &&...extra)

   Create a Python iterator wrapping the C++ iterator represented by the range
   ``[first, last)``. The `Extra` parameter can be used to pass additional
   function binding annotations.

   This function lazily creates a new Python iterator type identified by
   `name`, which is stored in the given `scope`. Usually, some kind of
   :cpp:class:`keep_alive` annotation is needed to tie the lifetime of the
   parent container to that of the iterator.

   The return value is a typed iterator (:cpp:class:`iterator` wrapped using
   :cpp:class:`typed`), whose template parameter is given by the type of
   ``*first``.

   Here is an example of what this might look like for a STL vector:

   .. code-block:: cpp

      using IntVec = std::vector<int>;

      nb::class_<IntVec>(m, "IntVec")
         .def("__iter__",
              [](const IntVec &v) {
                  return nb::make_iterator(nb::type<IntVec>(), "iterator",
                                           v.begin(), v.end());
              }, nb::keep_alive<0, 1>());

   .. note::

      Pre-2.0 versions of nanobind and pybind11 return *references* (views)
      into the underlying sequence.

      This is convenient when

      1. Iterated elements are used to modify the underlying container.

      2. Iterated elements should reflect separately made changes to
         the underlying container.

      But this strategy is *unsafe* if the allocated memory region or layout
      of the container could change (e.g., through insertion of removal of
      elements).

      Because of this, iterators now copy by default. There are two
      ways to still obtain references to the target elements:

      1. If the iterator is over STL shared pointers, the added indirection and
         ownership tracking removes the need for extra copies.

      2. If the iterator is over reference-counted objects (e.g., ``ref<T>``
         via the :cpp:class:`ref` wrapper) and ``T`` uses the intrusive
         reference counting approach explained :ref:`here <intrusive>`,
         the added indirection and ownership tracking removes the need
         for extra copies.

      If you truly need the unsafe reference semantics, and if you can
      guarantee that all use of your bindings will respect the memory layout
      and reference-invalidation rules of the underlying C++ container type,
      you can request the old behavior by passing
      :cpp:enumerator:`rv_policy::reference_internal` to the ``Policy``
      template argument of this function.


.. cpp:function:: template <rv_policy Policy = rv_policy::automatic_reference, typename Type, typename... Extra> auto make_iterator(handle scope, const char * name, Type &value, Extra &&...extra)

   This convenience wrapper calls the above :cpp:func:`make_iterator` variant with
   ``first`` and ``last`` set to ``std::begin(value)`` and ``std::end(value)``,
   respectively.

.. cpp:function:: template <rv_policy Policy = rv_policy::automatic_reference, typename Iterator, typename... Extra> iterator make_key_iterator(handle scope, const char * name, Iterator &&first, Iterator &&last, Extra &&...extra)

   :cpp:func:`make_iterator` specialization for C++ iterators that return
   key-value pairs. `make_key_iterator` returns the first pair element to
   iterate over keys.

   The return value is a typed iterator (:cpp:class:`iterator` wrapped using
   :cpp:class:`typed`), whose template parameter is given by the type of
   ``(*first).first``.


.. cpp:function:: template <rv_policy Policy = rv_policy::automatic_reference, typename Iterator, typename... Extra> iterator make_value_iterator(handle scope, const char * name, Iterator &&first, Iterator &&last, Extra &&...extra)

   :cpp:func:`make_iterator` specialization for C++ iterators that return
   key-value pairs. `make_value_iterator` returns the second pair element to
   iterate over values.

   The return value is a typed iterator (:cpp:class:`iterator` wrapped using
   :cpp:class:`typed`), whose template parameter is given by the type of
   ``(*first).second``.

N-dimensional array type
------------------------

The following type can be used to exchange n-dimension arrays with frameworks
like NumPy, PyTorch, Tensorflow, JAX, CuPy, and others. It requires an additional
include directive:

.. code-block:: cpp

   #include <nanobind/ndarray.h>

Detailed documentation including example code is provided in a :ref:`separate
section <ndarrays>`.

.. cpp:function:: bool ndarray_check(handle h) noexcept

   Test whether the Python object represents an ndarray.

   Objects with a ``__dlpack__`` attribute or objects that implement the buffer
   protocol are considered as ndarray objects. In addition, arrays from NumPy,
   PyTorch, TensorFlow and XLA are also regarded as ndarrays.

.. cpp:class:: template <typename... Args> ndarray

   .. cpp:function:: ndarray() = default

      Create an invalid array.

   .. cpp:function:: template <typename... Args2> explicit ndarray(const ndarray<Args2...> &other)

      Reinterpreting constructor that wraps an existing nd-array (parameterized
      by `Args`) into a new ndarray (parameterized by `Args2`).   No copy or
      conversion is made.

      Dropping parameters is always safe. For example, a function that
      returns different array types could call it to convert ``ndarray<T>`` to
      ``ndarray<>``.  When adding constraints, the constructor is only safe to
      use following a runtime check to ensure that newly created array actually
      possesses the advertised properties.

   .. cpp:function:: ndarray(const ndarray &)

      Copy constructor. Increases the reference count of the referenced array.

   .. cpp:function:: ndarray(ndarray &&)

      Move constructor. Steals the referenced array without changing reference counts.

   .. cpp:function:: ~ndarray()

      Decreases the reference count of the referenced array and potentially destroy it.

   .. cpp:function:: ndarray& operator=(const ndarray &)

      Copy assignment operator. Increases the reference count of the referenced array.
      Decreases the reference count of the previously referenced array and potentially destroy it.

   .. cpp:function:: ndarray& operator=(ndarray &&)

      Move assignment operator. Steals the referenced array without changing reference counts.
      Decreases the reference count of the previously referenced array and potentially destroy it.

   .. cpp:function:: ndarray(void * data, size_t ndim, const size_t * shape, handle owner = nanobind::handle(), const int64_t * strides = nullptr, dlpack::dtype dtype = nanobind::dtype<Scalar>(), int32_t device_type = device::cpu::value, int32_t device_id = 0)

      Create an array wrapping an existing memory allocation. The following
      parameters can be specified:

      - `data`: pointer address of the memory region. When the ndarray is
        parameterized by a constant scalar type to indicate read-only access, a
        const pointer must be passed instead.

      - `ndim`: the number of dimensions.

      - `shape`: specifies the size along each axis. The referenced array must
        must have `ndim` entries.

      - `owner`: if provided, the array will hold a reference to this object
        until it is destructed.

      - `strides` is optional; a value of ``nullptr`` implies C-style strides.

      - `dtype` describes the data type (floating point, signed/unsigned
        integer) and bit depth.

      - The `device_type` and `device_id` indicate the device and address
        space associated with the pointer `value`.

   .. cpp:function:: ndarray(void * data, const std::initializer_list<size_t> shape, handle owner = nanobind::handle(), std::initializer_list<int64_t> strides = { }, dlpack::dtype dtype = nanobind::dtype<Scalar>(), int32_t device_type = device::cpu::value, int32_t device_id = 0)

      Alternative form of the above constructor, which accepts the ``shape``
      and ``strides`` arguments using a ``std::initializer_list``. It
      automatically infers the value of ``ndim`` based on the size of
      ``shape``.

   .. cpp:function:: dlpack::dtype dtype() const

      Return the data type underlying the array

   .. cpp:function:: size_t ndim() const

      Return the number of dimensions.

   .. cpp:function:: size_t size() const

      Return the size of the array (i.e. the product of all dimensions).

   .. cpp:function:: size_t itemsize() const

      Return the size of a single array element in bytes. The returned value
      is rounded up to the next full byte in case of bit-level representations
      (query :cpp:member:`dtype::bits` for bit-level granularity).

   .. cpp:function:: size_t nbytes() const

      Return the size of the entire array bytes. The returned value is rounded
      up to the next full byte in case of bit-level representations.

   .. cpp:function:: size_t shape(size_t i) const

      Return the size of dimension `i`.

   .. cpp:function:: int64_t stride(size_t i) const

      Return the stride (in number of elements) of dimension `i`.

   .. cpp:function:: const int64_t* shape_ptr() const

      Return a pointer to the shape array. Note that the return type is
      ``const int64_t*``, which may be unexpected as the scalar version
      :cpp:func:`shape()` casts its result to a ``size_t``.

      This is a consequence of the DLPack tensor representation that uses
      signed 64-bit integers for all of these fields.

   .. cpp:function:: const int64_t* stride_ptr() const

      Return pointer to the stride array.

   .. cpp:function:: bool is_valid() const

      Check whether the array is in a valid state.

   .. cpp:function:: int32_t device_type() const

      ID denoting the type of device hosting the array. This will match the
      ``value`` field of a device class, such as :cpp:class:`device::cpu::value
      <device::cpu>` or :cpp:class:`device::cuda::value <device::cuda>`.

   .. cpp:function:: int32_t device_id() const

      In a multi-device/GPU setup, this function returns the ID of the device
      storing the array.

   .. cpp:function:: const Scalar * data() const

      Return a const pointer to the array data.

   .. cpp:function:: Scalar * data()

      Return a mutable pointer to the array data. Only enabled when `Scalar` is
      not itself ``const``.

   .. cpp:function:: template <typename... Extra> auto view()

      Returns an nd-array view that is optimized for fast array access on the
      CPU. You may optionally specify additional ndarray constraints via the
      `Extra` parameter (though a runtime check should first be performed to
      ensure that the array possesses these properties).

      The returned view provides the operations ``data()``, ``ndim()``,
      ``shape()``, ``stride()``, and ``operator()`` following the conventions
      of the `ndarray` type.

   .. cpp:function:: template <typename... Ts> auto& operator()(Ts... indices)

      Return a mutable reference to the element at stored at the provided
      index/indices. ``sizeof(Ts)`` must match :cpp:func:`ndim()`.

      This accessor is only available when the scalar type and array dimension
      were specified as template parameters.

Data types
^^^^^^^^^^

Nanobind uses the `DLPack <https://github.com/dmlc/dlpack>`_ ABI to represent
metadata describing n-dimensional arrays (even when they are exchanged using
the buffer protocol). Consequently, the set of possible dtypes is :ref:`more
restricted <dtype_restrictions>` than that of other nd-array libraries (e.g.,
NumPy). Relevant data structures are located in the ``nanobind::dlpack``
sub-namespace.


.. cpp:enum-class:: dlpack::dtype_code : uint8_t

   This enumeration characterizes the elementary array data type regardless of
   bit depth.

   .. cpp:enumerator:: Int = 0

      Signed integer format

   .. cpp:enumerator:: UInt = 1

      Unsigned integer format

   .. cpp:enumerator:: Float = 2

      IEEE-754 floating point format

   .. cpp:enumerator:: Bfloat = 4

      "Brain" floating point format

   .. cpp:enumerator:: Complex = 5

      Complex numbers parameterized by real and imaginary component

.. cpp:struct:: dlpack::dtype

   Represents the data type underlying an n-dimensional array. Use the
   :cpp:func:`dtype\<T\>() <::nanobind::dtype>` function to return a populated
   instance of this data structure given a scalar C++ arithmetic type.

   .. cpp:member:: uint8_t code = 0;

      This field must contain the value of one of the
      :cpp:enum:`dlpack::dtype_code` enumerants.

   .. cpp:member:: uint8_t bits = 0;

      Number of bits per entry (e.g., 32 for a C++ single precision ``float``)

   .. cpp:member:: uint16_t lanes = 0;

      Number of SIMD lanes (typically ``1``)

.. cpp:function:: template <typename T> dlpack::dtype dtype()

   Returns a populated instance of the :cpp:class:`dlpack::dtype` structure
   given a scalar C++ arithmetic type.

Array annotations
^^^^^^^^^^^^^^^^^

The :cpp:class:`ndarray\<..\> <ndarray>` class admits optional template
parameters. They constrain the type of array arguments that may be passed to a
function.

The following are supported:

Data type
+++++++++

The data type of the underlying scalar element. The following are supported.

- ``[u]int8_t`` up to ``[u]int64_t`` and other variations (``unsigned long long``, etc.)
- ``float``, ``double``
- ``bool``

Annotate the data type with ``const`` to indicate a read-only array. Note that
only the buffer protocol/NumPy interface considers ``const``-ness at the
moment; data exchange with other array libraries will ignore this annotation.

When the is unspecified (e.g., to accept arbitrary input arrays), the
:cpp:class:`ro` annotation can instead be used to denote read-only access:

.. cpp:class:: ro

   Indicate read-only access (use only when no data type is specified.)


nanobind does not support non-standard types as documented in the section on
:ref:`dtype limitations <dtype_restrictions>`.

Shape
+++++

.. cpp:class:: template <ssize_t... Is> shape

   Require the array to have ``sizeof...(Is)`` dimensions. Each entry of `Is`
   specifies a fixed size constraint for that specific dimension. An entry
   equal to ``-1`` indicates that *any* size should be accepted for this
   dimension.

   (An alias named ``nb::any`` representing ``-1`` was removed in nanobind 2).

.. cpp:class:: template <size_t N> ndim

   Alternative to the above that only constrains the array dimension.
   ``nb::ndim<2>`` is equivalent to ``nb::shape<-1, -1>``.

Contiguity
++++++++++

.. cpp:class:: c_contig

   Request that the array storage uses a C-contiguous representation.

.. cpp:class:: f_contig

   Request that the array storage uses a F (Fortran)-contiguous representation.

.. cpp:class:: any_contig

   Don't place any demands on array contiguity (the default).

Device type
+++++++++++

.. cpp:class:: device

   The following helper classes can be used to constrain the device and
   address space of an array. Each class has a ``static constexpr int32_t
   value`` field that will then match up with
   :cpp:func:`ndarray::device_id()`.

   .. cpp:class:: cpu

      CPU heap memory

   .. cpp:class:: cuda

      NVIDIA CUDA device memory

   .. cpp:class:: cuda_host

      NVIDIA CUDA host-pinned memory

   .. cpp:class:: cuda_managed

      NVIDIA CUDA managed memory

   .. cpp:class:: vulkan

      Vulkan device memory

   .. cpp:class:: metal

      Apple Metal device memory

   .. cpp:class:: rocm

      AMD ROCm device memory

   .. cpp:class:: rocm_host

      AMD ROCm host memory

   .. cpp:class:: oneapi

      Intel OneAPI device memory

Framework
+++++++++

Framework annotations cause :cpp:class:`nb::ndarray <ndarray>` objects to
convert into an equivalent representation in one of the following frameworks:

.. cpp:class:: numpy

.. cpp:class:: tensorflow

.. cpp:class:: pytorch

.. cpp:class:: jax

.. cpp:class:: cupy

Eigen convenience type aliases
------------------------------

The following helper type aliases require an additional include directive:

.. code-block:: cpp

   #include <nanobind/eigen/dense.h>

.. cpp:type:: DStride = Eigen::Stride<Eigen::Dynamic, Eigen::Dynamic>

   This type alias refers to an Eigen stride object that is sufficiently flexible
   so that can be easily called with NumPy arrays and array slices.

.. cpp:type:: template <typename T> DRef = Eigen::Ref<T, 0, DStride>

   This templated type alias creates an ``Eigen::Ref<..>`` with flexible strides for
   zero-copy data exchange between Eigen and NumPy.

.. cpp:type:: template <typename T> DMap = Eigen::Map<T, 0, DStride>

   This templated type alias creates an ``Eigen::Map<..>`` with flexible strides for
   zero-copy data exchange between Eigen and NumPy.

.. _chrono_conversions:

Timestamp and duration conversions
----------------------------------

nanobind supports bidirectional conversions of timestamps and
durations between their standard representations in Python
(:py:class:`datetime.datetime`, :py:class:`datetime.timedelta`) and in C++
(``std::chrono::time_point``, ``std::chrono::duration``).
A few unidirectional conversions from other Python types to these
C++ types are also provided and explained below.

These type casters require an additional include directive:

.. code-block:: cpp

   #include <nanobind/stl/chrono.h>

.. The rest of this section is adapted from pybind11/docs/advanced/cast/chrono.rst

An overview of clocks in C++11
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The C++11 standard defines three different clocks, and users can
define their own. Each ``std::chrono::time_point`` is defined relative
to a particular clock. When using the ``chrono`` type caster, you must be
aware that only ``std::chrono::system_clock`` is guaranteed to convert
to a Python :py:class:`~datetime.datetime` object; other clocks may convert to
:py:class:`~datetime.timedelta` if they don't represent calendar time.

The first clock defined by the standard is ``std::chrono::system_clock``.
This clock measures the current date and time, much like the Python
:py:func:`time.time` function. It can change abruptly due to
administrative actions, daylight savings time transitions, or
synchronization with an external time server. That makes this clock a
poor choice for timing purposes, but a good choice for wall-clock time.

The second clock defined by the standard is ``std::chrono::steady_clock``.
This clock ticks at a steady rate and is never adjusted, like
:py:func:`time.monotonic` in Python. That makes it excellent for timing
purposes, but the value in this clock does not correspond to the
current date and time. Often this clock will measure the amount of
time your system has been powered on. This clock will never be
the same clock as the system clock, because the system clock can
change but steady clocks cannot.

The third clock defined in the standard is ``std::chrono::high_resolution_clock``.
This clock is the clock that has the highest resolution out of all the
clocks in the system. It is normally an alias for either ``system_clock``
or ``steady_clock``, but can be its own independent clock. Due
to this uncertainty, conversions of time measured on the
``high_resolution_clock`` to Python produce platform-dependent types:
you'll get a :py:class:`~datetime.datetime` if ``high_resolution_clock`` is
an alias for ``system_clock`` on your system, or a :py:class:`~datetime.timedelta`
value otherwise.

Provided conversions
^^^^^^^^^^^^^^^^^^^^

The C++ types described in this section may be instantiated with any
precision. Conversions to a less-precise type will round towards zero.
Since Python's built-in date and time objects support only microsecond
precision, any precision beyond that on the C++ side will be lost when
converting to Python.

.. rubric:: C++ to Python

- ``std::chrono::system_clock::time_point`` → :py:class:`datetime.datetime`
    A system clock time will be converted to a Python
    :py:class:`~datetime.datetime` instance.  The result describes a time in the
    local timezone, but does not have any timezone information
    attached to it (it is a naive datetime object).

- ``std::chrono::duration`` → :py:class:`datetime.timedelta`
    A duration will be converted to a Python :py:class:`~datetime.timedelta`.
    Any precision beyond microseconds is lost by rounding towards zero.

- ``std::chrono::[other_clock]::time_point`` → :py:class:`datetime.timedelta`
    A time on any clock except the system clock will be converted to a Python
    :py:class:`~datetime.timedelta`, which measures the number of seconds between
    the clock's epoch and the time point of interest.

.. rubric:: Python to C++

- :py:class:`datetime.datetime` or :py:class:`datetime.date` or :py:class:`datetime.time` → ``std::chrono::system_clock::time_point``
    A Python date, time, or datetime object can be converted into a
    system clock timepoint.  A :py:class:`~datetime.time` with no date
    information is treated as that time on January 1, 1970. A
    :py:class:`~datetime.date` with no time information is treated as midnight
    on that date. **Any timezone information is ignored.**

- :py:class:`datetime.timedelta` → ``std::chrono::duration``
    A Python time delta object can be converted into a duration
    that describes the same number of seconds (modulo precision limitations).

- :py:class:`datetime.timedelta` → ``std::chrono::[other_clock]::time_point``
    A Python time delta object can be converted into a timepoint on a
    clock other than the system clock. The resulting timepoint will be
    that many seconds after the target clock's epoch time.

- ``float`` → ``std::chrono::duration``
    A floating-point value can be converted into a duration. The input is
    treated as a number of seconds, and fractional seconds are supported
    to the extent representable.

- ``float`` → ``std::chrono::[other_clock]::time_point``
    A floating-point value can be converted into a timepoint on a
    clock other than the system clock. The input is treated as a
    number of seconds, and fractional seconds are supported to the
    extent representable. The resulting timepoint will be that many
    seconds after the target clock's epoch time.


Evaluating Python expressions from strings
------------------------------------------

The following functions can be used to evaluate Python functions and
expressions. They require an additional include directive:

.. code-block:: cpp

   #include <nanobind/eval.h>

Detailed documentation including example code is provided in a :ref:`separate
section <utilities_eval>`.

.. cpp:enum-class:: eval_mode

   This enumeration specifies how the content of a string should be
   interpreted. Used in Py_CompileString().

   .. cpp:enumerator:: eval_expr = Py_eval_input

      Evaluate a string containing an isolated expression

   .. cpp:enumerator:: eval_single_statement = Py_single_input

      Evaluate a string containing a single statement. Returns \c None

   .. cpp:enumerator:: eval_statements = Py_file_input

      Evaluate a string containing a sequence of statement. Returns \c None

.. cpp:function:: template <eval_mode start = eval_expr, size_t N> object eval(const char (&s)[N], handle global = handle(), handle local = handle())

   Evaluate the given Python code in the given global/local scopes, and return
   the value.

.. cpp:function:: inline void exec(const str &expr, handle global = handle(), handle local = handle())

   Execute the given Python code in the given global/local scopes.

Intrusive reference counting helpers
------------------------------------

The following functions and classes can be used to augment user-provided
classes with intrusive reference counting that greatly simplifies shared
ownership in larger C++/Python binding projects.

This functionality requires the following include directives:

.. code-block:: cpp

   #include <nanobind/intrusive/counter.h>
   #include <nanobind/intrusive/ref.h>

These headers reference several functions, whose implementation must be
provided. You can do so by including the following file from a single ``.cpp``
file of your project:

.. code-block:: cpp

   #include <nanobind/intrusive/counter.inl>

The functionality in these files consist of the following classes and
functions:

.. cpp:class:: intrusive_counter

   Simple atomic reference counter that can optionally switch over to
   Python-based reference counting.

   The various copy/move assignment/constructors intentionally don't transfer
   the reference count. This is so that the contents of classes containing an
   ``intrusive_counter`` can be copied/moved without disturbing the reference
   counts of the associated instances.

   .. cpp:function:: intrusive_counter() noexcept = default

      Initialize with a reference count of zero.

   .. cpp:function:: intrusive_counter(const intrusive_counter &o)

      Copy constructor, which produces a zero-initialized counter.
      Does *not* copy the reference count from `o`.

   .. cpp:function:: intrusive_counter(intrusive_counter &&o)

      Move constructor, which produces a zero-initialized counter.
      Does *not* copy the reference count from `o`.

   .. cpp:function:: intrusive_counter &operator=(const intrusive_counter &o)

      Copy assignment operator. Does *not* copy the reference count from `o`.

   .. cpp:function:: intrusive_counter &operator=(intrusive_counter &&o)

      Move assignment operator. Does *not* copy the reference count from `o`.

   .. cpp:function:: void inc_ref() const noexcept

      Increase the reference count. When the counter references an object
      managed by Python, the operation calls ``Py_INCREF()`` to increase
      the reference count of the Python object instead.

      The :cpp:func:`inc_ref() <nanobind::inc_ref>` top-level function
      encapsulates this logic for subclasses of :cpp:class:`intrusive_base`.

   .. cpp:function:: bool dec_ref() const noexcept

      Decrease the reference count. When the counter references an object
      managed by Python, the operation calls ``Py_DECREF()`` to decrease
      the reference count of the Python object instead.

      When the C++-managed reference count reaches zero, the operation returns
      ``true`` to signal to the caller that it should use a *delete expression*
      to destroy the instance.

      The :cpp:func:`dec_ref() <nanobind::dec_ref>` top-level function
      encapsulates this logic for subclasses of :cpp:class:`intrusive_base`.

   .. cpp:function:: void set_self_py(PyObject * self)

      Set the Python object associated with this instance. This operation
      is usually called by nanobind when ownership is transferred to the
      Python side.

      Any references from prior calls to
      :cpp:func:`intrusive_counter::inc_ref()` are converted into Python
      references by calling ``Py_INCREF()`` repeatedly.

   .. cpp:function:: PyObject * self_py()

      Return the Python object associated with this instance (or ``nullptr``).

.. cpp:class:: intrusive_base

   Simple polymorphic base class for a intrusively reference-counted object
   hierarchy. The member functions expose corresponding functionality of
   :cpp:class:`intrusive_counter`.

   .. cpp:function:: void inc_ref() const noexcept

      See :cpp:func:`intrusive_counter::inc_ref()`.

   .. cpp:function:: bool dec_ref() const noexcept

      See :cpp:func:`intrusive_counter::dec_ref()`.

   .. cpp:function:: void set_self_py(PyObject * self)

      See :cpp:func:`intrusive_counter::set_self_py()`.

   .. cpp:function:: PyObject * self_py()

      See :cpp:func:`intrusive_counter::self_py()`.

.. cpp:function:: void intrusive_init(void (* intrusive_inc_ref_py)(PyObject * ) noexcept, void (* intrusive_dec_ref_py)(PyObject * ) noexcept)

   Function to register reference counting hooks with the intrusive reference
   counter class. This allows its implementation to not depend on Python.

   You would usually call this function as follows from the initialization
   routine of a Python extension:

   .. code-block:: cpp

      NB_MODULE(my_ext, m) {
          nb::intrusive_init(
              [](PyObject * o) noexcept {
                  nb::gil_scoped_acquire guard;
                  Py_INCREF(o);
              },
              [](PyObject * o) noexcept {
                  nb::gil_scoped_acquire guard;
                  Py_DECREF(o);
              });

          // ...
      }

.. cpp:function:: inline void inc_ref(intrusive_base * o) noexcept

   Reference counting helper function that calls ``o->inc_ref()`` if ``o`` is
   not equal to ``nullptr``.

.. cpp:function:: inline void dec_ref(intrusive_base * o) noexcept

   Reference counting helper function that calls ``o->dec_ref()`` if ``o`` is
   not equal to ``nullptr`` and ``delete o`` when the reference count reaches
   zero.

.. cpp:class:: template <typename T> ref

   RAII scoped reference counting helper class

   :cpp:class:`ref\<T\> <ref>` is a simple RAII wrapper class that encapsulates a
   pointer to an instance with intrusive reference counting.

   It takes care of increasing and decreasing the reference count as needed and
   deleting the instance when the count reaches zero.

   For this to work, compatible functions :cpp:func:`inc_ref()` and
   :cpp:func:`dec_ref()` must be defined before including the file
   ``nanobind/intrusive/ref.h``. Default implementations for subclasses of the
   type :cpp:class:`intrusive_base` are already provided as part of the file
   ``counter.h``.

   .. cpp:function:: ref() = default

      Create a null reference

   .. cpp:function:: ref(T * ptr)

      Create a reference from a pointer. Increases the reference count of the
      object (if not ``nullptr``).

   .. cpp:function:: ref(const ref &r)

      Copy a reference. Increase the reference count of the object (if not
      ``nullptr``).

   .. cpp:function:: ref(ref &&r) noexcept

      Move a reference. Object reference counts are unaffected by this operation.

   .. cpp:function:: ~ref()

      Destroy a reference. Decreases the reference count of the object (if not
      ``nullptr``).

   .. cpp:function:: ref& operator=(ref &&r) noexcept

      Move-assign another reference into this one.

   .. cpp:function:: ref& operator=(const ref &r)

      Copy-assign another reference into this one.

   .. cpp:function:: ref& operator=(const T * ptr)

      Overwrite this reference with a pointer to another object

   .. cpp:function:: void reset()

      Clear the reference and reduces the reference count of the object (if not
      ``nullptr``)

   .. cpp:function:: bool operator==(const ref &r) const

      Compare this reference with another reference (pointer equality)

   .. cpp:function:: bool operator!=(const ref &r) const

      Compare this reference with another reference (pointer inequality)

   .. cpp:function:: bool operator==(const T * ptr) const

      Compare this reference with another object (pointer equality)

   .. cpp:function:: bool operator!=(const T * ptr) const

      Compare this reference with another object (pointer inequality)

   .. cpp:function:: T * operator->()

      Access the object referenced by this reference

   .. cpp:function:: const T * operator->() const

      Access the object referenced by this reference (const version)

   .. cpp:function:: T& operator*()

      Return a C++ reference to the referenced object

   .. cpp:function:: const T& operator*() const

      Return a C++ reference to the referenced object (const version)

   .. cpp:function:: T* get()

      Return a C++ pointer to the referenced object

   .. cpp:function:: const T* get() const

      Return a C++ pointer to the referenced object (const version)

Typing
------

The following functions for typing-related functionality require an additional
include directive:

.. code-block:: cpp

   #include <nanobind/typing.h>

.. cpp:function:: template <typename... Args> object type_var(Args&&... args)

   Create a `type variable
   <https://docs.python.org/3/library/typing.html#typing.TypeVar>`__ (i.e., an
   instance of ``typing.TypeVar``). All arguments of the original Python
   construction are supported, e.g.:

   .. code-block:: cpp

        m.attr("T") = nb::type_var("T",
                                   "contravariant"_a = true,
                                   "covariant"_a = false,
                                   "bound"_a = nb::type<MyClass>());


.. cpp:function:: template <typename... Args> object type_var_tuple(Args&&... args)

   Analogousto :cpp:func:`type_var`, create a `type variable tuple
   <https://docs.python.org/3/library/typing.html#typing.TypeVarTuple>`__
   (i.e., an instance of ``typing.TypeVarTuple``).

.. cpp:function:: object any_type()

   Convenience wrapper, which returns ``typing.Any``.
